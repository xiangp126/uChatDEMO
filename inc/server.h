#ifndef SERVER_H_
#define SERVER_H_

#include <iostream>
#include <sstream>
#include <iomanip>
#include "common.h"

using std::ostream;
using std::ostringstream;
using std::cout;
using std::endl;

#define LOGNAME     "./p2p_server.log"
#define MAXHOSTLEN  15
/* typedef variables and structure. */
class TickInfo {
public:
    int  tick;
    char hostname[MAXHOSTLEN];
public:
    TickInfo(int _tick = 0, char *_hostname = "Annoymous") {
        tick = _tick;
        strncpy(hostname, _hostname, MAXHOSTLEN - 1);
        hostname[MAXHOSTLEN - 1] = '\0';
    }
};
typedef unordered_map<PeerInfo, TickInfo, HashFunc> PEERTICKTYPE;
typedef unordered_map<PeerInfo, PeerInfo, HashFunc> PEERPUNCHEDTYPE;

/* declare external variables. */
extern char message[IBUFSIZ];
extern PEERPUNCHEDTYPE punchMap;
extern pthread_mutex_t     ticksLock;
extern pthread_mutexattr_t tickLockAttr;

ostream & operator<<(ostream &out, PEERTICKTYPE &clientMap);
ostream & operator<<(ostream &out, PEERPUNCHEDTYPE &punchMap);
/* manipuate client set associated */
void createClientSet();
void addClient(PEERTICKTYPE &clientMap, const PeerInfo &peer);
void delClient(PEERTICKTYPE &clientMap, const PeerInfo &peer);
void listInfo2Str(PEERTICKTYPE &clientMap, PEERPUNCHEDTYPE &punchMap,char *msg);
void onCalled(int sockFd, PEERTICKTYPE &clientMap, 
              PktInfo &packet, PeerInfo &peer);
void *handleTicks(void *arg);
void setReentrant(pthread_mutex_t &lock, pthread_mutexattr_t &attr);
void getSetHostName(PEERTICKTYPE &hashMap, PeerInfo &peer, char *payload);

#endif
