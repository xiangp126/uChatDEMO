#include <iostream>
#include <stdio.h>
#include "client.h"

using std::cout;
using std::endl;
static const int TIMEOUT = 1800; // 30 * 60 seconds

int main(int argc, const char *argv[]) {
    struct   hostent *hp;
    struct   timeval timeout;
    struct   HeartParm heartBServer;
    struct   HeartParm heartBClient;
    int      retVal = -1, rc;
    int      maxFd  = 0;
    int      sockFd  = -1;
    int      readFd1 = 0;
    fd_set   readFds;
    PeerInfo server;
    PeerInfo peer;
    PktInfo  packet;
    pthread_t tids[MAXTID];

    memset(&server, 0, sizeof(server));
    memset(&peer, 0, sizeof(peer));
    memset(&packet, 0, sizeof(packet));
    sockFd = udpGetSocket();
    maxFd  = MAX(readFd1, sockFd) + 1;
    hp = gethostbyname(argv[1]);
    if (hp == NULL) {
        oops("DNS lookup error");
    }
    struct in_addr decAddr;
    memcpy(&decAddr, hp->h_addr, hp->h_length);
    strcpy(server.ip, inet_ntoa(decAddr));
    // memcpy(server.ip, inet_ntoa(hp->h_addr), hp->h_length);
    server.port = atoi(argv[2]);

    /* initial heart beat thread to server.
     */
    memset(&heartBServer, 0, sizeof(heartBServer));
    heartBServer.sockFd = sockFd;
    heartBServer.sleep  = 2000 * 1000; // 2000 ms 
    heartBServer.peer   = &server;
    heartBServer.tsType = THREADSWITCH::ISSERVER;
    rc = pthread_create(&tids[SERVERTID], NULL, sendHeartBeat, 
                                          (void *)&heartBServer);
    if (rc != 0) {
        oops("Create Heart Beat pthread_create to server error.");
    }

    /* initial heart beat thread to peer.
     */
    memset(&heartBClient, 0, sizeof(heartBClient));
    heartBClient.sockFd = sockFd;
    heartBClient.sleep  = 2000 * 1000; // 2000 ms 
    heartBClient.peer   = &server;
    heartBClient.tsType = THREADSWITCH::ISCLIENT;
    rc = pthread_create(&tids[CLIENTTID], NULL, sendHeartBeat, 
                                          (void *)&heartBClient);
    if (rc != 0) {
        oops("Create Heart Beat pthread_create to peer error.");
    }

    promptInput();
    while (1) {
        FD_ZERO(&readFds);
        FD_SET(readFd1, &readFds);
        FD_SET(sockFd, &readFds);
        /* set timeout structure */
        timeout.tv_sec  = 30;
        timeout.tv_usec = 0;
        retVal = select(maxFd, &readFds, NULL, NULL, &timeout);
        /* -1 error, 0 timeout */
        if (retVal == -1) {
            oops("select error");
        }
        if (retVal > 0) {
            if (FD_ISSET(readFd1, &readFds)) {
                handleInput(sockFd, server, packet);
            }
            if (FD_ISSET(sockFd, &readFds)) {
                handleNet(sockFd, peer, packet);
            }
            promptInput();
        } else {
            //fprintf(stderr, "No Input after %d seconds\n", TIMEOUT);
        }
    }
    close(sockFd);
    return 0;
}

