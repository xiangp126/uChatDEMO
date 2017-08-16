#ifndef SERVER_H_
#define SERVER_H_

#include <iostream>
#include <sstream>
#include "common.h"

using std::ostream;
using std::ostringstream;
using std::cout;
using std::endl;

#define LOGNAME "./p2p_server.log"
#define TICKS_INI   10  // count down from TICKS to 0, then timeout.
/* declare external variables */
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
void listInfo2Str(PEERTICKTYPE &clientMap, PEERPUNCHEDTYPE &punchMap, char *msg);
void onCalled(int sockFd, PEERTICKTYPE &clientMap, 
              PktInfo &packet, PeerInfo &peer);
void *handleTicks(void *arg);
void setReentrant(pthread_mutex_t &lock, pthread_mutexattr_t &attr);

#endif
