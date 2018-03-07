#ifndef COMMON_H_
#define COMMON_H_

#include <iostream>
#include <string>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <assert.h>
#include "config.h"

using std::cout;
using std::endl;
using std::cerr;
using std::ostream;
using std::ofstream;
using std::size_t;
using std::unordered_set;
using std::unordered_map;
using std::hash;
using std::string;

#define FWORDLEN   10  /* first word length. */
#define BINDADDR   "0"
#define DEBUG      1
#define MAX(a, b)  (a > b ? a : b)
#define ABS(a)     (a >= 0 ? a : -a)
#define oops(msg) { fprintf(stdout, "%s at:", msg); \
            printf ("%s->%s:%u\n", __FILE__, __FUNCTION__, __LINE__); \
            exit(1); }
#define LOG_TRACE(format, args...) fprintf(stdout, format"\n", ##args)
#define LOG_ERR(format, args...)   fprintf(stderr, format"\n", ##args)

namespace myspace {
    enum PKTTYPE {
        MESSAGE = 0,
        HEARTBEAT,
        LOGIN,
        LOGOUT,
        LIST,
        PUNCH,
        SYN,
        ACK,
        WHOAMI,
        ERROR,
        HELP,
        SETNAME,
        EXIT,
        MAXPKTTYPE
    };
    enum THREADSWITCH {
        SERVERON = 0,
        SERVEROFF,
        CLIENTON,
        CLIENTOFF,
        ALLON,
        ALLOFF,
        ISSERVER,
        ISCLIENT,
        MAXSWITCH
    };
}

using myspace::PKTTYPE;
using myspace::THREADSWITCH;

/* declare external variables */
extern const int  SLEEPTIME;
extern const int  PKTHEADLEN;
extern const char *CMDS[PKTTYPE::MAXPKTTYPE];
extern pthread_mutex_t pLock;
extern pthread_cond_t  pCond;
extern int pGlobal;

#if 1
/* PeerInfo is a self-defined structure, so
 * std::map did not has hash function whose para is type PeerInfo
 * and when add PeerInfo element into std::map, std::map did not
 * know how to hash it, which will end with compile error.
 * so need define new HashFunc and overload its 'call' func
 * with PeerInfo as its parameter.
 */
class PeerInfo {
public:
    char ip[INET_ADDRSTRLEN];  // 255.255.255.255 => 15 + 1 = 16
    int  port;
public:
    /* if need sort, must overload operator<  */
    PeerInfo(char *_ip = "0.0.0.0", int _port = 0);
    PeerInfo(const PeerInfo &);
    ~PeerInfo() {}
    bool operator== (const PeerInfo &) const;
    void operator= (const PeerInfo &);
    friend ostream & operator<<(ostream &, const PeerInfo &);
};
#endif

class PktHead {
public:
    PKTTYPE type;       // default public viewable.
    size_t  length;     // only packet payload len.
    PeerInfo peer;
//    union {             // annonymous union, punch command will need.
//        PeerInfo peer;
//        char unused[sizeof(PeerInfo)];
//    };
public:
    friend ostream & operator<< (ostream &, PktHead &);
};  // sizeof(PktHead)

class PktInfo {
private:
    PktHead head; // fix length of thpe head.
    char    payload[IBUFSIZ];
public:
    PktInfo();
    PktHead & getHead() {return head;}
    char *getPayload() {return payload;}
    friend ostream & operator<< (ostream &, PktInfo &);
}; // sizeof PktInfo == (4 + 4 + 20) + 8192

class HashFunc {
public:
    /* overload 'call' operator */
    size_t operator()(const PeerInfo &) const;
};
/* only useful for transferring into heart beat thread func */
struct HeartParm {
    int sockFd;
    int sleep;
    PeerInfo *peer;
    THREADSWITCH tsType;
};

/* declaration of API functions. */
int  udpGetSocket();
void promptInput();
void checkFirstWord(char *msg, char *cmd);
void *sendHeartBeat(void *heartBeat);
void write2Log(ofstream &out, char *err);
void udpBindAddr(int sockFd, const char *bindAddr, int port);
void makePacket(char *msg, PktInfo &pkt,
                 PKTTYPE type = PKTTYPE::MESSAGE);
ssize_t udpSendPkt(int sockFd, const PeerInfo &msgTo, PktInfo &packet);
ssize_t udpRecvPkt(int sockFd, PeerInfo &msgFrom, PktInfo &packet);
ssize_t udpSendTo(int sockId, const PeerInfo &msgTo, const char *msg);
ssize_t udpRecvFrom(int sockId, PeerInfo &msgFrom, char *msg);

// program is the phone, kernel is the ISP. Note BUFSIZ == 8192.
/*
 * step 1: get a line                       socket
 * step 2: assign phone number              bind
 * step 3: allow call coming                listen
 *         networking cables & power on
 * step 4: call coming					    accept
 * step 5: speak with each other            read/write
 * step 6: end the call                     close
 */

#endif
