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
/* declare external variables */
extern char message[IBUFSIZ];
extern PEERSPUNCHED punchMap;

ostream & operator<<(ostream &out, PEERSETTYPE &hashSet);
ostream & operator<<(ostream &out, PEERSPUNCHED &hashMap);
/* manipuate client set associated */
void createClientSet();
void addClient(PEERSETTYPE &hashSet, const PeerInfo &peer);
void delClient(PEERSETTYPE &hashSet, const PeerInfo &peer);
void listInfo2Str(PEERSETTYPE &hashSet, PEERSPUNCHED &hashMap, char *msg);
void onCalled(int sockFd, PEERSETTYPE &hashSet, 
              PktInfo &packet, PeerInfo &peer);

#endif
