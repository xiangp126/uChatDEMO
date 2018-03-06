#include <iostream>
#include <stdio.h>
#include "common.h"
#include "server.h"

using namespace std;
PEERTICKTYPE    clientMap;
PEERPUNCHEDTYPE punchMap;

int main(int argc, const char *argv[]) {
    int  sockFd = -1, rc = -1;
    char message[IBUFSIZ];
    PeerInfo peer;
    PktInfo  packet;
    pthread_t keepAliveTid;

    memset(message, 0, IBUFSIZ);
    memset(&peer, 0, sizeof(peer));
    sockFd = udpGetSocket();
    udpBindAddr(sockFd, BINDADDR, PORTNUM);
    sprintf(message, "Now listening on port %d...\n", PORTNUM);
    write(1, message, strlen(message));
    memset(message, 0, IBUFSIZ);

    /* handle keep alive thread on server. */
    rc = pthread_create(&keepAliveTid, NULL, handleTicks, (void *)&clientMap);
    if (rc != 0) {
        oops("Create Keep Alive Thread Error.");
    }

    /* Set Attribute of Lock to be Reentrant. onCalled -> ::LOGIN
     * will reentrant the lock. */
    pthread_mutexattr_init(&tickLockAttr);
    pthread_mutexattr_settype(&tickLockAttr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&ticksLock, &tickLockAttr);

    while (1) {
        onCalled(sockFd, clientMap, packet, peer);
    }

    /* destroy attr of type pthread_mutexattr_t. */
    pthread_mutexattr_destroy(&tickLockAttr);
    close(sockFd);

    return 0;
}

