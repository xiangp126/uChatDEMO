#include <iostream>
#include "common.h"
#include "server.h"

static ofstream logFile(LOGNAME, ofstream::app);

ostream & operator<<(ostream &out, PEERTICKTYPE &clientMap) {
    out << "----------->>> List" << endl;
    auto iter = clientMap.begin();
    for (; iter != clientMap.end(); ++iter) {
        out << "  " << iter->first.ip << ":" << iter->first.port;
        out << endl;
    }

    out << "<<< ---------------" << endl;
    return out;
}

ostream & operator<<(ostream &out, PEERPUNCHEDTYPE &hashMap) {
    out << "----------->>> List" << endl;
    auto iter = hashMap.begin();
    for (; iter != hashMap.end(); ++iter) {
    }
    out << "<<< ---------------" << endl;
    return out;
}

/* unordered_map remove duplicate items */
void addClient(PEERTICKTYPE &clientMap, const PeerInfo &peer) {
    clientMap[peer] = TICKS;
    return;
}

void delClient(PEERTICKTYPE &clientMap, const PeerInfo &peer) {
    auto iterFind = clientMap.find(peer);
    if (iterFind != clientMap.end()) {
        clientMap.erase(iterFind);
    } else {
        write2Log(logFile, "delete peer error: did not found.");
    }

    return;
}

void listInfo2Str(PEERTICKTYPE &clientMap, PEERPUNCHEDTYPE &punchMap, char *msg) {
    ostringstream oss;
    oss << "\n-------------- *** Login Info\n";

    auto iter1 = clientMap.begin();
    for (; iter1 != clientMap.end(); ++iter1) {
        oss << "  " << iter1->first.ip << " " << iter1->first.port << "\n";
    }
    oss << "*** ------------------" << endl;

    oss << "\n-------------------------- *** Punch Info\n";
    auto iter2 = punchMap.begin();
    for (; iter2 != punchMap.end(); ++iter2) {
        oss << iter2->first << " ===>> " << iter2->second << endl;
    }
    oss << "*** --------------------------------------" << endl;

    memset(msg, 0, IBUFSIZ);
    strcpy(msg, oss.str().c_str());
    return;
}

void onCalled(int sockFd, PEERTICKTYPE &clientMap, 
                          PktInfo &packet, PeerInfo &peer) {
    char message[IBUFSIZ];
    memset(message, 0, IBUFSIZ);

    ssize_t recvSize = udpRecvPkt(sockFd, peer, packet);
    PKTTYPE type = packet.getHead().type;
    switch (type) {
        case PKTTYPE::MESSAGE: 
            {
                /* check if peer has puncued pair. */
                cout << "Message From " << peer << ": " << 
                                        packet.getPayload() << endl;
                auto iterFind = punchMap.find(peer);
                if (iterFind != punchMap.end()) {
                    /* type SYN to tell peer to fetch peerinfo from 
                     * NET packet. */
                    packet.getHead().type = PKTTYPE::SYN;
                    packet.getHead().peer = peer;
                    udpSendPkt(sockFd, punchMap[peer], packet);
                }
                break;
            }
        case PKTTYPE::HEARTBEAT: 
            {
                cout << "Heart Beat Received From " << peer << endl;
                break;
            }
        case PKTTYPE::LOGIN: 
            {
                addClient(clientMap, peer);
                cout << peer << " login." << endl;
                break;
            }
        case PKTTYPE::LOGOUT: 
            {
                delClient(clientMap, peer);
                cout << peer << " logout." << endl;
                break;
            }
        case PKTTYPE::LIST: 
            {
                listInfo2Str(clientMap, punchMap, message);
                makePacket(message, packet, PKTTYPE::MESSAGE);
                udpSendPkt(sockFd, peer, packet);
                break;
            }
        case PKTTYPE::PUNCH: 
            {
                cout << "From " << peer << " To " << packet.getHead().peer
                                                                   << endl;
                PeerInfo tPeer  = packet.getHead().peer;
                packet.getHead().peer = peer;
                /* add to punchMap */
                punchMap[peer]  = tPeer;
                punchMap[tPeer] = peer;
                cout << punchMap << endl;

                /* notice be punched peer. */
                udpSendPkt(sockFd, tPeer, packet);
                break;
            }
        case PKTTYPE::SYN: 
            {
                cout << "From " << peer << endl;
                break;
            }
        case PKTTYPE::ACK: 
            {
                break;
            }
        case PKTTYPE::WHOAMI: 
            {
                makePacket(message, packet, PKTTYPE::WHOAMI);
                packet.getHead().peer = peer;
                udpSendPkt(sockFd, peer, packet);
                break;
            }
        default:
            break;
    }
#if 1
    cout << packet << "\n" << endl;
#endif
       return;
}

