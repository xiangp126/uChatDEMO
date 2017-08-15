#include <iostream>
#include <stdio.h>
#include "common.h"
#include "server.h"

using namespace std;
PEERSETTYPE  clientSet;
PEERSPUNCHED punchMap;

int main(int argc, const char *argv[]) {
    int  sockFd = -1;
    char message[IBUFSIZ];
    PeerInfo peer;
    PktInfo  packet;

    memset(message, 0, IBUFSIZ);
    memset(&peer, 0, sizeof(peer));
    sockFd = udpGetSocket();
    udpBindAddr(sockFd, BINDADDR, PORTNUM);
    sprintf(message, "Now listening on port %d...\n", PORTNUM);
    write(1, message, strlen(message));
    memset(message, 0, IBUFSIZ);

    createClientSet();
#if 1
    while (1) {
        onCalled(sockFd, clientSet, packet, peer);
    }

    close(sockFd);
#endif
    return 0;
}

