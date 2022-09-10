#include "main.h"
#include "com_thread.h"

/* Communication thread; takes care of receiving and reacting to messages*/
void *comLoop(void *ptr) {
    MPI_Status status;
    packet_t packet;

    while (true) {
        MPI_Recv(&packet, 1, MPI_PACKET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        switch (status.MPI_TAG) {
            case message_t::DOCK_REQ: 
                lockMutex();
                    updateLamportTime(packet.lamportTime); // update timestamp
                unlockMutex();

                addToRequestQueue(std::make_pair(packet.lamportTime, status.MPI_SOURCE), dockRequestQueue); // add the request to the queue

                packet.lamportTime = lamportTime;
                packet.inDock = dockStatus[rank];
                packet.mechanicsTaken = mechStatus[rank];

                MPI_Send(&packet, 1, MPI_PACKET_T, status.MPI_SOURCE, message_t::DOCK_ACK, MPI_COMM_WORLD); // send dock ACK

            break;

            case message_t::DOCK_ACK: 
                lockMutex();
                    updateLamportTime(packet.lamportTime); // update timestamp
                    dockACK[status.MPI_SOURCE] = 1; // add the ACK to the dock ACK list
                unlockMutex();
                
            break;

            case message_t::DOCK_REL: 
                lockMutex();
                    updateLamportTime(packet.lamportTime); // update timestamp
                    dockStatus[status.MPI_SOURCE] = 0; // remove the ship from the dock status list
                    for(int i = 0; i < dockRequestQueue.size(); i++) {
                        if (dockRequestQueue[i].second == status.MPI_SOURCE) {
                            dockRequestQueue.erase(dockRequestQueue.begin() + i); // remove the ship from the dock request queue
                            break;
                        }
                    }
                unlockMutex();
                
            break;

            case message_t::MECH_REQ: 
                lockMutex();
                    updateLamportTime(packet.lamportTime); // update timestamp
                unlockMutex();

                addToRequestQueue(std::make_pair(packet.lamportTime, status.MPI_SOURCE), mechRequestQueue); // add the request to the queue

                packet.lamportTime = lamportTime;
                packet.inDock = dockStatus[rank];
                packet.mechanicsTaken = mechStatus[rank];

                MPI_Send(&packet, 1, MPI_PACKET_T, status.MPI_SOURCE, message_t::MECH_ACK, MPI_COMM_WORLD); // send mech ACK
            
            break;

            case message_t::MECH_ACK: 
                lockMutex();
                    updateLamportTime(packet.lamportTime); // update timestamp
                    mechACK[status.MPI_SOURCE] = 1; // add the ACK to the mech ACK list
                unlockMutex();

            break;

            case message_t::MECH_REL: 
                lockMutex();
                    updateLamportTime(packet.lamportTime); // update timestamp
                    mechStatus[status.MPI_SOURCE] = 0; // remove the ship from the mech status list
                    for(int i = 0; i < mechRequestQueue.size(); i++) {
                        if (mechRequestQueue[i].second == status.MPI_SOURCE) {
                            mechRequestQueue.erase(mechRequestQueue.begin() + i); // remove the ship from the mech request queue
                            break;
                        }
                    }
                unlockMutex();
               
            break;

            default:
                printf("Unknown message type received: %d", status.MPI_TAG);
            break;
        }
    }
}
