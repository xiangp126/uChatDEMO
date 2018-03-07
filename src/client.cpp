#include <iostream>
#include "client.h"

using namespace std;

void usage(void) {
    fprintf(stderr,
"NAME\n"
"    p2pclient --- Client End of Peer to Peer communication Through NAT\n\n"

"USAGE\n"
"    >>> COMMAND [PARAM]\n"
"    type COMMANDS before any word you typed. If you want to type COMMANDS\n"
"    as a word itself, just leave a blank before it. Default you chat with\n"
"    the server, after punched, you chat directly with the peer you punched.\n\n"

"COMMAND:\n"
"    help\n"
"    list\n"
"    whoami\n"
"    punch [ip] [port]\n"
"    login\n"
"    logout\n"
"    setname [hostname]\n"
"    exit\n\n"

"DESCRIPTION\n"
"    HELP:    print this help info, you can type 'help' when needed.\n"
"    LIST:    print logined and punched info. It's useful before 'punch'.\n"
"    WHOAMI:  show who you are, namely the ip address & port through NAT.\n"
"    PUNCH:   establish connection between you and the peer you want to talk with.\n"
"             After punched, you talked directly to punched peer.\n"
"             punch [ip] [port], such as: punch [64.0.1.5] [12400]\n"
"    LOGIN:   login you existence to the server, may type 'list' when logined.\n"
"    LOGOUT:  clear your login info and stop sending heartbeat to the server.\n"
"    SETNAME: set hostname to make peer more identified. When logined, setname will\n"
"             take effect at once. The hostname will be back to Annoymous after\n"
"             logout. Format: setname [hostname], suck as: setname corsair\n"
"             That is: setname only take effect after logined.\n"
"    EXIT:    exit this program on your machine, same as CTRL + D.\n\n"

"AUTHORS\n"
"       p2pclient is a DEMO for either further study or research purpose, you\n"
"       can modify or redistribute it whatever you want.\n"
    );

    return;
}

PKTTYPE checkCmd(char *cmd, PktInfo &packet) {
    PKTTYPE type = PKTTYPE::MESSAGE;
    /* distinguish 'login' and ' login', the former is command
     * while the latter not. Notice space before login.
     */
    if ((cmd == NULL) || ! isalpha(cmd[0])) {
        return type;
    }

    char fWord[FWORDLEN];
    checkFirstWord(fWord, cmd);
#if 0
    cout << "fWord = " << fWord << endl;
    cout << "strlen(fWord) = " << strlen(fWord) << endl;
#endif
    /* case ignorance string compare. */
    if (strcasecmp(fWord, CMDS[PKTTYPE::LOGIN]) == 0) {
        type = PKTTYPE::LOGIN;
    } else if (strcasecmp(fWord, CMDS[PKTTYPE::LOGOUT]) == 0) {
        type = PKTTYPE::LOGOUT;
    } else if (strcasecmp(fWord, CMDS[PKTTYPE::LIST]) == 0) {
        type = PKTTYPE::LIST;
    } else if (strcasecmp(fWord, CMDS[PKTTYPE::PUNCH]) == 0) {
        type = PKTTYPE::PUNCH;
    } else if (strcasecmp(fWord, CMDS[PKTTYPE::SYN]) == 0) {
        type = PKTTYPE::SYN;
    } else if (strcasecmp(fWord, CMDS[PKTTYPE::ACK]) == 0) {
        type = PKTTYPE::ACK;
    } else if (strcasecmp(fWord, CMDS[PKTTYPE::WHOAMI]) == 0) {
        type = PKTTYPE::WHOAMI;
    } else if (strcasecmp(fWord, CMDS[PKTTYPE::HELP]) == 0) {
        type = PKTTYPE::HELP;
    } else if (strcasecmp(fWord, CMDS[PKTTYPE::SETNAME]) == 0) {
        type = PKTTYPE::SETNAME;
    } else if (strcasecmp(fWord, CMDS[PKTTYPE::EXIT]) == 0) {
        type = PKTTYPE::EXIT;
        exit(0);
    }

    return type;  // return this type for further use.
}

void handleInput(int sockFd, PeerInfo &peer, PktInfo &packet) {
    int  rdFd = 0, rc;
    char msg[IBUFSIZ];
    pthread_t tids[MAXTID];
    PKTTYPE   type;
    struct HeartParm heartBClient;

    memset(msg, 0, IBUFSIZ);
    ssize_t rdSize = read(rdFd, msg, IBUFSIZ);
    if (rdSize == -1) {
        oops("read from stdin error");
    } else {
        /* Ctrl + D did not send a signal, it means EOF, causing 'read'
         * reads 0 bytes. so I can use it for quit.
         */
        if (rdSize == 0) {
            cout << endl;
            exit(1);
        }
    }
    /* 'read' from stdin, the 'carriage return' was read to string also,
     * which will cause strlen(string) plus one. This problem can be
     * tackled by several methods. Below method is safe, except from
     * EOF discussed above, 'read' at least reads '\n' from stdin, thus
     * rdSize is at least 1 if successfully read and not EOF.
     */
    msg[rdSize - 1] = '\0';   /* remove last '\n' */

    /* make packet of relative type, default is PKTTYPE::MESSAGE */
    type = checkCmd(msg, packet);
    makePacket(msg, packet, type);

    /* 'read' from stdin, the 'carriage return' was read to string also,
     * which will cause strlen(string) plus one. This problem can be
     * tackled by several methods. Below method is safe, except from
     * EOF discussed above, 'read' at least reads '\n' from stdin, thus
     * rdSize is at least 1 if successfully read and not EOF.
     */
    switch (type) {
        case PKTTYPE::MESSAGE:
            //cout << "Message To   " << peer << ": " << msg << endl;
            break;
        case PKTTYPE::LOGIN:
            {
                /* start thread of sending Heart Beat to server */
                pthread_mutex_lock(&pLock);
                pGlobal = THREADSWITCH::SERVERON;
                pthread_mutex_unlock(&pLock);
                pthread_cond_broadcast(&pCond);
                break;
            }
        case PKTTYPE::LOGOUT:
            {
                /* suspend thread of sending Heart Beat to server */
                pthread_mutex_lock(&pLock);
                pGlobal = THREADSWITCH::ALLOFF;
                pthread_mutex_unlock(&pLock);
                pthread_cond_broadcast(&pCond);
                break;
            }
        case PKTTYPE::PUNCH:
            {
                rc = setPunchInfo(msg, packet);
                if (rc == -1) {
                    cout << "Usage Format: punch 127.0.0.1 18974" << endl;
                    cout << "Notice Not Leave any blank before 'punch'.\n";
                    return;
                }
                int cnt = 0, MAXTRY = 1;
                /* change packet type to PUNCH. */
                packet.getHead().type = PKTTYPE::PUNCH;
                while (cnt < MAXTRY) {
                    cout << "Sending PUNCH Packet To Server..." << endl;
                    /* send to server, for which to mid-transfer. */
                    // udpSendPkt(sockFd, peer, packet);
                    ++cnt;
                }
                break;
            }
        case PKTTYPE::SYN:
            break;
        case PKTTYPE::HELP:
            {
                usage();
                return;
            }
        case PKTTYPE::SETNAME:
            {
                /* change packet type to SETNAME. */
                packet.getHead().type = PKTTYPE::SETNAME;
                break;
            }
        default:
            break;
    }

    ssize_t sendSize = udpSendPkt(sockFd, peer, packet);

    return;
}

void handleNet(int sockFd, PeerInfo &peer, PktInfo &packet) {
    ssize_t recvSize = udpRecvPkt(sockFd, peer, packet);
    if (recvSize == -1) {
        oops("read error");
    }
    PKTTYPE type = packet.getHead().type;

    switch (type) {
        case PKTTYPE::LOGIN:
            break;
        case PKTTYPE::LOGOUT:
            break;
        case PKTTYPE::ACK:
            {
                break;
            }
        case PKTTYPE::WHOAMI:
            cout << "You Are: " << packet.getHead().peer << endl;
            break;
        case PKTTYPE::PUNCH:
            {
                PeerInfo peer = packet.getHead().peer;
                cout << "Peer " << peer << " Want To Chat With You." << endl;
                cout << "By Default, Auto Send Accept." << endl;
                break;
            }
        case PKTTYPE::ERROR:
            {
                cout << "NOTICE: " << packet.getPayload() << endl;
                break;
            }
        case PKTTYPE::SYN:
            {
                peer = packet.getHead().peer;
                cout << "Message From " << peer << ": " << packet.getPayload()
                                                        << endl;
                break;
            }
        default:
            cout << "Message From " << peer << ": " << packet.getPayload()
                                                    << endl;
            break;
    }

    return;
}

int setPunchInfo(char *cmd, PktInfo &packet) {
    /* only get first 3 words interested. */
    PeerInfo peer;
    char *p1, *p2;
    int  PUNCHSTRLEN = 28;
    char punStr[PUNCHSTRLEN + 1];

    p1 = cmd;
    /* skip first key word 'punch' */
    while (*p1 != ' ' && *p1 != '\0') {
        ++p1;
    }

    int loopTime = 2, cpCnt = 0;
    for (int i = 0; i < loopTime; ++i) {
        p2 = p1 + 1;
        while (*p2 != ' ' && *p2 != '\0') {
            ++p2;
        }
        cpCnt = p2 - (p1 + 1);
        if (cpCnt > 0 && cpCnt < INET_ADDRSTRLEN) {
            memset(punStr, 0, cpCnt);
            memcpy(punStr, p1 + 1, cpCnt);
            punStr[cpCnt] = '\0';
        } else {
            return -1;
        }
        if (i == 0) {
            strcpy(peer.ip, punStr);
        } else {
            peer.port = atoi(punStr);
        }
        p1 = p2;
    }

    cout << "Want To Punch " << peer << endl;
    packet.getHead().peer = peer;

    return 0;
}
