#include "main.h"
#include "main_thread.h"

/* Main loop; takes care of handling states.*/
void mainLoop()
{
    srandom(rank);
    while (true) {
        switch (state) {
            case state_t::FIGHTING:
                int fightingTime = rand() % 10;
                sleep(fightingTime); // fight for a random amount of time

                lockMutex();
                    damage = (rand() % (MAX_DAMAGE - MIN_DAMAGE + 1)) 
                    + MIN_DAMAGE; // add random damage to the ship
                    state = state_t::WAITING_DOCK; // change the state after fighting
                unlockMutex();

                packet_t *packet = (packet_t *) malloc(sizeof(packet_t)); // allocate packet

                lockMutex();
                    updateLamportTime(0); // update timestamp
                unlockMutex(); 

                /* Broadcast the dock request messages. */
                for (int i = 0; i < size; i++) {
                    if (i != rank){ // to all except the process itself
                        lockMutex();
                            packet->lamportTime = lamportTime;
                            packet->inDock = 0;
                            packet->mechanicsTaken = 0;
                            MPI_Send(packet, 1, MPI_PACKET_T, i, message_t::DOCK_REQ, MPI_COMM_WORLD); // send dock request
                        unlockMutex(); 
                    }
                }

                addToDockRequestQueue(std::make_pair(lamportTime, rank)); // add the request to the queue

                free(packet);
                break;

            case state_t::WAITING_DOCK:
                lockMutex();
                int freeDocks = 0;
                // check if there are free docks
                for (int i=0; i<size; i++) {
                    if (dockStatus[i] == 0) {
                        freeDocks++;
                    }
                }
                
                if (freeDocks >= 1 && priority()) { // there are free docks and the process received larger timestamps from all others (TODO)
                    if (dockRequestQueue.size() > 0) {
                        if (dockRequestQueue.back().second == rank) { // process is at the top of the queue

                            dockStatus[rank] = 1; // add the dock to the dock status list
                            printf("DOCKING");
                            state = state_t::WAITING_MECH; // change the state once docked

                            updateLamportTime(0); // update timestamp

                            /* Broadcast the mechanic request messages. */
                            for (int i = 0; i < size; i++) {
                                if (i != rank){ // to all except the process itself
                                    packet->lamportTime = lamportTime;
                                    packet->inDock = 1;
                                    packet->mechanicsTaken = 0;
                                    MPI_Send(packet, 1, MPI_PACKET_T, i, message_t::MECH_REQ, MPI_COMM_WORLD); // send mechanics request
                                }
                            }
                            dockRequestQueue.pop_back(); // remove the request
                        }
                    }
                    unlockMutex();
                }
                else {
                    unlockMutex();
                    continue;
                };
                
                break;
                
            case state_t::WAITING_MECH:
                lockMutex();
                int freeMechanics = 0;
                // check if there are enough free mechanics
                for (int i=0; i<size; i++) {
                    if (mechStatus[i] == 0) {
                        freeMechanics++;
                    }
                }

                if (freeMechanics >= damage && priority()) { // there are enough free mechanics based on the damage and the process received larger timestamps from all others
                    if (mechRequestQueue.size() > 0) {
                        if (mechRequestQueue.back().second == rank) { // process is at the top of the queue
                            mechStatus[rank] = damage; // add the taken mechanics to the mechanic status list
                            state = state_t::IN_REPAIR; // change the state once access is granted

                            mechRequestQueue.pop_back(); // remove the request
                        }
                    }
                    unlockMutex();
                }
                else {
                    unlockMutex();
                    continue;
                };

                break;

            case state_t::IN_REPAIR:
                sleep(damage);
                lockMutex();

                    damage = 0;
                    dockStatus[rank] = 0;
                    mechStatus[rank] = 0;
                    state = state_t::FIGHTING; // go back to fighting after repair

                    updateLamportTime(0); // update timestamp

                unlockMutex();

                packet_t *packet = (packet_t *) malloc(sizeof(packet_t));

                // lockMutex();

                /* Broadcast the dock and mechanic release messages. */
                for (int i = 0; i < size; i++) {
                    if (i != rank){ // to all except the process itself
                        packet->lamportTime = lamportTime;
                        packet->inDock = 0;
                        packet->mechanicsTaken = 0;
                        MPI_Send(packet, 1, MPI_PACKET_T, i, message_t::DOCK_REL, MPI_COMM_WORLD); // send dock release
                        MPI_Send(packet, 1, MPI_PACKET_T, i, message_t::MECH_REL, MPI_COMM_WORLD); // send mechanics release
                    }
                }  
                // unlockMutex();
                free(packet);
                break;

            default:
            break;
        }
    }
}
