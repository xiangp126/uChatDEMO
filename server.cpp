#include <iostream>
#include "common.h"
#include "server.h"

static ofstream logFile(LOGNAME, ofstream::app);

ostream & operator<<(ostream &out, PEERSETTYPE &hashSet) {
    out << "----------->>> List" << endl;
    for (auto &peer : hashSet) {
        out << "  " << peer.ip << ":" << peer.port;
        out << endl;
    }
    out << "<<< ---------------" << endl;
    return out;
}

ostream & operator<<(ostream &out, PEERSPUNCHED &hashMap) {
    out << "----------->>> List" << endl;
    auto iter = hashMap.begin();
    for (; iter != hashMap.end(); ++iter) {
        out << iter->first << " Punched To " << iter->second << endl;
    }
    out << "<<< ---------------" << endl;
    return out;
}

void createClientSet() {
    // empty.
    return;
}

/* unordered_set remove duplicate items */
void addClient(PEERSETTYPE &hashSet, const PeerInfo &peer) {
    hashSet.insert(peer);
    return;
}

void delClient(PEERSETTYPE &hashSet, const PeerInfo &peer) {
    auto iter = hashSet.find(peer);    
    if (iter != hashSet.end()) {
        hashSet.erase(iter);
    } else {
        write2Log(logFile, "delete peer error: did not found.");
    }
    return;
}

void listInfo2Str(PEERSETTYPE &hashSet, PEERSPUNCHED &hashMap, char *msg) {
    ostringstream oss;
    oss << "\n-------------- *** Login Info\n";
    for (auto &peer : hashSet) {
        oss << "  " << peer.ip << " " << peer.port << "\n";
    }
    oss << "*** ------------------" << endl;

    oss << "\n-------------- *** Punch Info\n";
    auto iter = hashMap.begin();
    for (; iter != hashMap.end(); ++iter) {
        oss << iter->first << " ===>> " << iter->second << endl;
    }
    oss << "*** ------------------" << endl;

    memset(msg, 0, IBUFSIZ);
    strcpy(msg, oss.str().c_str());
    return;
}

void onCalled(int sockFd, PEERSETTYPE &hashSet, 
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
                    udpSendPkt(sockFd, punchMap[peer], packet);
                    peer = punchMap[peer];
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
                addClient(hashSet, peer);
                cout << peer << " login." << endl;
                break;
            }
        case PKTTYPE::LOGOUT: 
            {
                delClient(hashSet, peer);
                cout << peer << " logout." << endl;
                break;
            }
        case PKTTYPE::LIST: 
            {
                listInfo2Str(hashSet, punchMap, message);
                makePacket(message, packet, PKTTYPE::MESSAGE);
                udpSendPkt(sockFd, peer, packet);
                break;
            }
        case PKTTYPE::PUNCH: 
            {
                cout << "From " << peer << " To " << packet.getHead().peer
                                                                   << endl;
                PeerInfo tPeer  = packet.getHead().peer;
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

