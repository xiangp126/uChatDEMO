#include <iostream>
#include "common.h"

using namespace std;

const int  SLEEPTIME  = 1000 * 1000;  // 1000 ms
const int  PKTHEADLEN = sizeof(PktHead);
const char *CMDS[] = {
    "MESSAGE",
    "HEARTBEAT",
    "LOGIN",
    "LOGOUT",
    "LIST",
    "PUNCH",
    "SYN",
    "ACK",
    "WHOAMI",
    "ERROR",
    "EXIT"
};
pthread_mutex_t pLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  pCond = PTHREAD_COND_INITIALIZER;
int pGlobal = THREADSWITCH::ALLOFF;

/* implement class PktInfo */
PktInfo::PktInfo() {
    memset(&head, 0, sizeof(head));
    memset(payload, 0, IBUFSIZ);
}

ostream & operator<< (ostream &out, PktInfo &packet) {
    out << packet.getHead();
    out << "\nPacket Payload: " << packet.getPayload();
    fflush(stdout);
    return out;
}

ostream & operator<< (ostream &out, PktHead &head) {
    out << "Head Type = " << CMDS[head.type] << endl;
    out << "Payload length = " << head.length << 
                            " (Had + '\\0') " << endl;
    out << "Head PeerInfo = " << head.peer;
    fflush(stdout);
    return out;
}

/* implement class PeerInfo */
PeerInfo::PeerInfo(char *_ip, int _port) {
    strcpy(ip, _ip);
    port = _port;
}

PeerInfo::PeerInfo(const PeerInfo &peer) {
    strcpy(ip, peer.ip);
    port = peer.port;
}

bool PeerInfo::operator== (const PeerInfo &peer) const{
    if (strcmp(this->ip, peer.ip) == 0 && this->port == peer.port) {
        return true;
    } else {
        return false;
    }
}

void PeerInfo::operator= (const PeerInfo &peer) {
    strcpy(ip, peer.ip);
    port = peer.port;
    return;
}

void write2Log(ofstream &out, char *err) {
    out << err << endl;
    return;
}

ostream & operator<<(ostream &out, const PeerInfo &peer) {
    out << "(" << peer.ip << " " << peer.port << ")";
    fflush(stdout);
    return out;
}

/* implement class HashFunc */
size_t HashFunc::operator()(const PeerInfo &peer) const {
    ssize_t hashCode;
    hashCode = (hash<string>()(peer.ip) ^
                (hash<int>()(peer.port) >> 1));
    hashCode = ABS(hashCode);
#if 0
    cout << "ip hash = " << hash<string>()(peer.ip) << endl;
    cout << "port hash = " << hash<int>()(peer.port) << endl;
    cout << "total output: " << hashCode << endl;
#endif
    return hashCode;
}

/* implement common API functions. */
int udpGetSocket() {
    /* step 1: ask kernel for a socket. 
     * just as get a line of the phone. program is the phone.
     */
    int sockFd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockFd == -1) {
        oops("socket error");
    }
    return sockFd;
}

void udpBindAddr(int sockFd, const char *bindAddr, int port) {
#if 1
    struct sockaddr_in servAddr;
    /* step 2: bind address to socket. Address is host & port.
     * just as bind phone number to the phone.
     */
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_addr.s_addr = inet_addr(bindAddr);
    servAddr.sin_family      = AF_INET;
    servAddr.sin_port        = htons(PORTNUM); /* host to network short(16b) */
    if (bind(sockFd, (struct sockaddr *)&servAddr, sizeof(servAddr)) != 0) {
        oops("bind error");
    }
#endif
    return;
}

void makePacket(char *msg, PktInfo &pkt, PKTTYPE type) {
    memset(&pkt, 0, sizeof(pkt));
    char *payload = pkt.getPayload();
    ssize_t len   = 0;
    if (msg != NULL) {
        len = strlen(msg);  
        /* truncate payload if it is too large. */
        len %= (IBUFSIZ - 1);
        memcpy(payload, msg, len); 
    } else  {
        cerr << "Warnning: msg == NULL" << endl;
    }
    /* I use memcpy did not copy terminator '\0', I manual add
     * one at the end of char *payload.
     */
    payload[len] = '\0';
    pkt.getHead().type   = type;
    pkt.getHead().length = len + 1; 
    PeerInfo tPeer;
    pkt.getHead().peer = tPeer;

    return;
}

ssize_t udpSendPkt(int sockFd, const PeerInfo &msgTo, PktInfo &packet) {
    struct sockaddr_in peerAddr;
    memset(&peerAddr, 0, sizeof(peerAddr));
    peerAddr.sin_family      = AF_INET;
    peerAddr.sin_port        = htons(msgTo.port);
    peerAddr.sin_addr.s_addr = inet_addr(msgTo.ip);

    /* packet actual length, did not waste bandwidth. */
    size_t len = packet.getHead().length + PKTHEADLEN;
#if 0
    cout << "Just Before sendto: " << "Packet Info: " << endl;
    cout << packet << endl;
    cout << "Peer Info: " << endl;
    cout << msgTo << endl;
#endif
    /* To fix a fatal sendto bug. I found that sometimes first 'login'
     * before any message sent,  will got sendto error. if oops() exit
     * then the program was terminated. 
     * I want to check this sendSize for more than one times, if all
     * failed, then exit program.     Comment NO.1
     */
    ssize_t sendSize = -1;
    const int MAXTRYTIMES = 10;
    int tryCnt = 0;
    while (tryCnt < MAXTRYTIMES) {
        sendSize = sendto(sockFd, 
                    &packet,
                    len,
                    MSG_DONTWAIT, 
                    (const struct sockaddr *)&peerAddr, 
                    sizeof(peerAddr));
        if (sendSize == -1) {
            /* As Comment NO.1, cannot figure out why, but is was 
             * trully needed add this sleep for command 'login' 
             * stay away from send to error.
             */
            usleep(100);
            ++tryCnt;
#if 0
            cerr << "Send To " << tryCnt << " Times Failed." << endl;
#endif
        } else {
            break;
        }
    }
    if (tryCnt >= MAXTRYTIMES) {
        oops("Fatal sendto error.");
    }

    return sendSize;
}

ssize_t udpRecvPkt(int sockFd, PeerInfo &msgFrom, PktInfo &packet) {
    struct sockaddr_in peerAddr;
    /* take care! sLen must be initialized before use. */
    socklen_t sLen = sizeof(peerAddr); 
    memset(&peerAddr, 0, sizeof(peerAddr));
    memset(&packet, 0, sizeof(packet));

    ssize_t recvSize = recvfrom(sockFd,
                                &packet,
                                sizeof(packet),
                                0,
                                (struct sockaddr *)&peerAddr, 
                                &sLen);
    if (recvSize == -1) {
        oops("recvfrom error");
    } else {
        const char *ip    = inet_ntoa(peerAddr.sin_addr);
        int        port   = peerAddr.sin_port;
        msgFrom.port      = ntohs(port);
        memcpy(msgFrom.ip, ip, INET_ADDRSTRLEN); // 16 bits
    }
    return recvSize;
}

ssize_t udpSendTo(int sockFd, const PeerInfo &msgTo, const char *msg) {
    struct sockaddr_in peerAddr;
    memset(&peerAddr, 0, sizeof(peerAddr));
    peerAddr.sin_family      = AF_INET;
    peerAddr.sin_port        = htons(msgTo.port);
    peerAddr.sin_addr.s_addr = inet_addr(msgTo.ip);
    ssize_t sendSize = sendto(sockFd, 
                              msg, 
                              strlen(msg), 
                              MSG_DONTWAIT, 
                              (const struct sockaddr *)&peerAddr, 
                              sizeof(peerAddr));
    if (sendSize == -1) {
        oops("sendto error");
    }
    return sendSize;
}

ssize_t udpRecvFrom(int sockFd, PeerInfo &msgFrom, char *msg) {
    struct sockaddr_in peerAddr;
    socklen_t          sLen;
    sLen = sizeof(peerAddr); /* take care! it must be initialized before use. */
    memset(&peerAddr, 0, sizeof(peerAddr));
    memset(msg, 0, IBUFSIZ);
    ssize_t recvSize = recvfrom(sockFd,
                                msg,
                                IBUFSIZ,
                                0,
                                (struct sockaddr *)&peerAddr, 
                                &sLen);
    if (recvSize == -1) {
        oops("recvfrom error");
    } else {
        const char *ip    = inet_ntoa(peerAddr.sin_addr);
        int        port   = peerAddr.sin_port;
        msgFrom.port      = ntohs(port);
        strncpy(msgFrom.ip, ip, INET_ADDRSTRLEN); // 16bits
    }
    return recvSize;
}

void promptInput() {
    fprintf(stdout, ">>> ");
    fflush(stdout);
    return;
}

void checkFirstWord(char *msg, char *cmd) {
    memset(msg, 0, FWORDLEN);
    memcpy(msg, cmd, FWORDLEN);
    msg[FWORDLEN - 1] = '\0';
    
    const char *pFast = msg;
    char *pSlow = msg;
    while (*pFast == ' ') {
        ++pFast; /* skip pre blank space. */
    }
    /* read from stdin, the carriage return was read to string also,
     * which will cause strlen(string) plus one. This problem can be
     * tackled by several methods.
     */
    while (isalpha(*pFast)) {
        *pSlow++ = *pFast++;
    }
    *pSlow = '\0';

    return;
}

void *sendHeartBeat(void *heartBeat) {
    struct HeartParm *info = (struct HeartParm *)heartBeat;
    THREADSWITCH toType = info->tsType;
    int sleepTime = info->sleep;

    /* receive Heart Beat Packet only need check type field,
     * it did not care what the payload was.
     */
    if (toType == THREADSWITCH::ISSERVER) {
        PktInfo packet;
        while (1) {
            pthread_mutex_lock(&pLock);
            while ((pGlobal == THREADSWITCH::SERVEROFF) ||
                        (pGlobal == THREADSWITCH::ALLOFF)) {
                pthread_cond_wait(&pCond, &pLock);
            }
            makePacket("HI Server.", packet, PKTTYPE::HEARTBEAT);
#if 0
            cout << "In Send HB: cout << packet: " << endl;
            cout << packet << endl;
#endif
            udpSendPkt(info->sockFd, *(info->peer), packet);
            usleep(sleepTime);
            pthread_mutex_unlock(&pLock);
            /* Strongly Notice here: must sleep some time after 
             * pthread_mutex_unlock before next pthread_mutex_lock. 
             * If not, other func changing 'condition' can not 
             * acquire the lock, thus always blocked at pthread_
             * mutex_lock. In one word, there must be some gap 
             * time between unlock & next lock action.
             */
            usleep(200);
        }
    } else if (toType == THREADSWITCH::ISCLIENT) {
        PktInfo packet;
        makePacket("HI Client.", packet, PKTTYPE::HEARTBEAT);
        while (1) {
            pthread_mutex_lock(&pLock);
            while ((pGlobal == THREADSWITCH::CLIENTOFF) ||
                        (pGlobal == THREADSWITCH::ALLOFF)) {
                pthread_cond_wait(&pCond, &pLock);
            }
            udpSendPkt(info->sockFd, *(info->peer), packet);
            usleep(sleepTime);
            pthread_mutex_unlock(&pLock);
            usleep(200);
        }
    }

    return NULL;
}

