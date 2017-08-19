#include <iostream>
#include "common.h"
#include "server.h"

pthread_mutex_t ticksLock;
pthread_mutexattr_t tickLockAttr;

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
    pthread_mutex_lock(&ticksLock);
    clientMap[peer] = TICKS_INI;
    pthread_mutex_unlock(&ticksLock);

    return;
}

void delClient(PEERTICKTYPE &clientMap, const PeerInfo &peer) {
    pthread_mutex_lock(&ticksLock);
    auto iterFind = clientMap.find(peer);
    if (iterFind != clientMap.end()) {
        clientMap.erase(iterFind);
    } else {
        write2Log(logFile, "delete peer error: did not found.");
    }
    pthread_mutex_unlock(&ticksLock);

    return;
}

/* till now, did not use this function. */
void setReentrant(pthread_mutex_t &lock, pthread_mutexattr_t &attr) {
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&lock, &attr);
    
    return;
}

void listInfo2Str(PEERTICKTYPE &clientMap, PEERPUNCHEDTYPE &punchMap,
                                           char *msg) {
    ostringstream oss;
    oss << "\n-------------------------- *** Login Info\n";

    pthread_mutex_lock(&ticksLock);
    auto iter1 = clientMap.begin();
    for (; iter1 != clientMap.end(); ++iter1) {
        oss << "  " << iter1->first.ip << " " << iter1->first.port 
            << "  " << " ===>> " << "  TTL: " << iter1->second << "\n";
    }
    oss << "*** ------------------------------" << endl;

    oss << "\n-------------------------- *** Punch Info\n";
    auto iter2 = punchMap.begin();
    for (; iter2 != punchMap.end(); ++iter2) {
        oss << "  " << iter2->first.ip << " " << iter2->first.port 
            << "  " << " ===>> " 
            << "  " << iter2->second.ip << " " << iter2->second.port
            << endl;
    }
    pthread_mutex_unlock(&ticksLock);
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
                /* check if peer has punched pair. */
                cout << "Message From " << peer << ". " << endl; 
                pthread_mutex_lock(&ticksLock);
                auto iterFind = punchMap.find(peer);
                if (iterFind != punchMap.end()) {
                    /* TYPE SYN inform peer to fetch getHead().peer info 
                     * from NET packet in addition with peer info. */
                    packet.getHead().type = PKTTYPE::SYN;
                    packet.getHead().peer = peer;
                    udpSendPkt(sockFd, punchMap[peer], packet);
                }
                pthread_mutex_unlock(&ticksLock);
                break;
            }
        case PKTTYPE::HEARTBEAT: 
            {
                pthread_mutex_lock(&ticksLock);

                clientMap[peer] = TICKS_INI;
        /* fix bug: under some uncertein circumstance handleTicks() 
         * will stop minus ticks, so use upper code replacing below,
         * seems work good.
         */
#if 0
                auto iterFind = clientMap.find(peer);
                if (iterFind != clientMap.end()) {
                    iterFind->second = TICKS_INI;
                } else {
                    /* reentrant lock. */
                    addClient(clientMap, peer);
                }
#endif
                pthread_mutex_unlock(&ticksLock);
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
#if 1
                /* check if punchMap still has this timeout info. */
                pthread_mutex_lock(&ticksLock);
                cout << "############################### Delete It" << endl;
                auto iterFind = punchMap.find(peer);
                if (iterFind != punchMap.end()) {
                    cout << "Found To Delete Peer: " << peer << endl;
                    cout << "#######################################" << endl;
                    punchMap.erase(iterFind);
                }
                pthread_mutex_unlock(&ticksLock);
#endif
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
                PeerInfo tPeer = packet.getHead().peer;
                cout << "From " << peer << " To " << tPeer << endl;

                /* check if both peer and tPeer has logined. */
                pthread_mutex_lock(&ticksLock);
                auto iterFind = clientMap.find(peer);
                auto pFind = clientMap.find(tPeer);
                if ((iterFind == clientMap.end()) 
                                  || (pFind == clientMap.end())) {
                    strcpy(message, "First, You Two Must All Be Logined.\
                                            \nJust Type 'list' to See Info.");
                    makePacket(message, packet, PKTTYPE::ERROR);
                    udpSendPkt(sockFd, peer, packet);
                    break;
                }

                packet.getHead().peer = peer;
                /* add to punchMap */
                punchMap[peer]  = tPeer;
                punchMap[tPeer] = peer;
                cout << punchMap << endl;
                pthread_mutex_unlock(&ticksLock);

                /* notice the punched peer. */
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

void *handleTicks(void *arg) {
    PEERTICKTYPE *hashMap = (PEERTICKTYPE *)arg;
    while (1) {
        pthread_mutex_lock(&ticksLock);
        /* Erasing an element of a map invalidates iterators pointing
         * to that element (after all that element has been deleted).
         * You shouldn't reuse that iterator, instead, advance the 
         * iterator to the next element before the deletion takes place.
         */
        auto iter = hashMap->begin();
#if 0
        cout << "########## Enter --iter->second" << endl;
#endif
        while (iter != hashMap->end()) {
            --iter->second;
            if (iter->second < 0) {
                PeerInfo peer = iter->first;
                hashMap->erase(iter++);
#if 1
                /* check if punchMap still has this timeout info. */
                cout << "############################### Delete It" << endl;
                auto iterFind = punchMap.find(peer);
                if (iterFind != punchMap.end()) {
                    cout << "Found Timeout Peer: " << peer << endl;
                    cout << "#######################################" << endl;
                    punchMap.erase(iterFind);
                }
#endif
            } else {
                ++iter;
            }
        }

        pthread_mutex_unlock(&ticksLock);

        /* sleep 1 s before next lock action. sleep some time is must.*/
        sleep(1);
    }
    return NULL;
}

