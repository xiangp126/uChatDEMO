#ifndef CLIENT_H_
#define CLIENT_H_

#include "common.h"
/* if want to send cmd packet, type cmd before any
 * words you typed. And if you only want to type word
 * 'cmd', type one blank space right before word 'cmd'.
 */
//#ine CMDMODE "cmd"
enum {
    SERVERTID,
    CLIENTTID,
    MAXTID
};

PKTTYPE checkCmd(char *cmd, PktInfo &packet);
void promptInput();
void handleInput(int sockFd, PeerInfo &peer, PktInfo &);
void handleNet(int sockFd, PeerInfo &peer, PktInfo &pkt);
int  setPunchInfo(char *cmd, PktInfo &packet);

#endif
