#include "main.h"
#include "com_thread.h"

/* Communication thread; takes care of receiving and reacting to messages*/
void *comLoop(void *ptr) {
    MPI_Status status;
    packet_t packet;

    while (true) {
        // println("Waiting for message. [println]");
        MPI_Recv(&packet, 1, MPI_PACKET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        switch (status.MPI_TAG) {
            case message_t::DOCK_REQ: 
            debug("Received DOCK_REQ message from %d", status.MPI_SOURCE);
                lockMutex();
                    updateLamportTime(packet.lamportTimestamp); // update timestamp
                    addToRequestQueue(std::make_pair(packet.lamportTimestamp, status.MPI_SOURCE), dockRequestQueue); // add the request to the queue
                    debug("Added (%d, %d) to dock request queue.", packet.lamportTimestamp, status.MPI_SOURCE);
                unlockMutex();

                packet.lamportTimestamp = lamportTime;
                packet.inDock = dockStatus[rank];
                packet.mechanicsTaken = mechStatus[rank];

                MPI_Send(&packet, 1, MPI_PACKET_T, status.MPI_SOURCE, message_t::DOCK_ACK, MPI_COMM_WORLD); // send dock ACK
                debug("Sent DOCK_ACK to %d", status.MPI_SOURCE);

            break;

            case message_t::DOCK_ACK: 
            debug("Received DOCK_ACK message from %d", status.MPI_SOURCE);
                lockMutex();
                    updateLamportTime(packet.lamportTimestamp); // update timestamp
                    dockACK[status.MPI_SOURCE] = 1; // add the ACK to the dock ACK list
                    // maybe a pause mechanism here?
                unlockMutex();
                if(state == state_t::WAITING_DOCK){
                    signal();
                }
                
            break;

            case message_t::DOCK_REL: 
            debug("Received DOCK_REL message from %d", status.MPI_SOURCE);
                lockMutex();
                    updateLamportTime(packet.lamportTimestamp); // update timestamp
                    dockStatus[status.MPI_SOURCE] = 0; // remove the ship from the dock status list
                    for(int i = 0; i < dockRequestQueue.size(); i++) {
                        if (dockRequestQueue[i].second == status.MPI_SOURCE) {
                            dockRequestQueue.erase(dockRequestQueue.begin() + i); // remove the ship from the dock request queue
                            break;
                        }
                    }
                unlockMutex();
                if(state == state_t::WAITING_DOCK){
                    signal();
                }
                
            break;

            case message_t::MECH_REQ: 
            debug("Received MECH_REQ message from %d", status.MPI_SOURCE);
                lockMutex();
                    updateLamportTime(packet.lamportTimestamp); // update timestamp
                    addToRequestQueue(std::make_pair(packet.lamportTimestamp, status.MPI_SOURCE), mechRequestQueue); // add the request to the queue
                    debug("Added (%d, %d) to mech request queue.", packet.lamportTimestamp, status.MPI_SOURCE);
                unlockMutex();

                packet.lamportTimestamp = lamportTime;
                packet.inDock = dockStatus[rank];
                packet.mechanicsTaken = mechStatus[rank];

                MPI_Send(&packet, 1, MPI_PACKET_T, status.MPI_SOURCE, message_t::MECH_ACK, MPI_COMM_WORLD); // send mech ACK
                debug("Sent MECH_ACK to %d", status.MPI_SOURCE);
            
            break;

            case message_t::MECH_ACK: 
            debug("Received MECH_ACK message from %d", status.MPI_SOURCE);
                lockMutex();
                    updateLamportTime(packet.lamportTimestamp); // update timestamp
                    mechACK[status.MPI_SOURCE] = 1; // add the ACK to the mech ACK list
                unlockMutex();
                if(state == state_t::WAITING_MECH){
                    signal();
                }

            break;

            case message_t::MECH_REL: 
            debug("Received MECH_REL message from %d", status.MPI_SOURCE);
                lockMutex();
                    updateLamportTime(packet.lamportTimestamp); // update timestamp
                    mechStatus[status.MPI_SOURCE] = 0; // remove the ship from the mech status list
                    for(int i = 0; i < mechRequestQueue.size(); i++) {
                        if (mechRequestQueue[i].second == status.MPI_SOURCE) {
                            mechRequestQueue.erase(mechRequestQueue.begin() + i); // remove the ship from the mech request queue
                            break;
                        }
                    }
                unlockMutex();
                if(state == state_t::WAITING_MECH){
                    signal();
                }
               
            break;

            default:
                printf("Unknown message type received: %d", status.MPI_TAG);
            break;
        }
    }
}
