#include "main.h"
#include "main_thread.h"

/* Main loop; takes care of handling states.*/
void mainLoop()
{
    srand(rank);
    while (true) {
        switch (state) {
            case state_t::FIGHTING: {
                debug("Started fighting.");
                int fightingTime = (rand() % (MAX_FIGHTING_TIME - MIN_FIGHTING_TIME + 1)) 
                    + MIN_FIGHTING_TIME;
                sleep(fightingTime); // fight for a random amount of time

                lockMutex();
                    damage = (rand() % (MAX_DAMAGE - MIN_DAMAGE + 1)) 
                    + MIN_DAMAGE; // add random damage to the ship
                    state = state_t::WAITING_DOCK; // change the state after fighting
                    debug("Got %d damage.", damage);
                unlockMutex();

                packet_t *packet = (packet_t *) malloc(sizeof(packet_t)); // allocate packet

                lockMutex();
                    updateLamportTime(0); // update timestamp
                unlockMutex(); 

                /* Broadcast the dock request messages. */
                packet->lamportTimestamp = lamportTime;
                packet->inDock = 0;
                packet->mechanicsTaken = 0;
                for (int i = 0; i < size; i++) {
                    if (i != rank){ // to all except the process itself
                        // lockMutex();
                        MPI_Send(packet, 1, MPI_PACKET_T, i, message_t::DOCK_REQ, MPI_COMM_WORLD); // send dock request
                        debug("Sent DOCK_REQ to %d", i);
                        // unlockMutex();
                    }
                }

                lockMutex();
                    addToRequestQueue(std::make_pair(lamportTime, rank), dockRequestQueue); // add the request to the queue
                    debug("Added (%d, %d) to dock request queue.", lamportTime, rank);
                unlockMutex();

                free(packet);
                debug("Finished fighting.");

                } break;

            case state_t::WAITING_DOCK: {
                lockMutex();
                    int freeDocks = 0;
                    // check if there are free docks
                    for (int i=0; i<size; i++) {
                        if (dockStatus[i] == 0) {
                            freeDocks++;
                        }
                    }
                    
                    if (freeDocks >= 1 && priority(dockACK, dockRequestQueue)) { // there are free docks and the process received larger timestamps from all others (TODO)
                        // dock critical section
                        dockStatus[rank] = 1; // claim a dock
                        debug("DOCKING");
                        state = state_t::WAITING_MECH; // change the state once docked

                        updateLamportTime(0); // update timestamp

                        packet_t *packet = (packet_t *) malloc(sizeof(packet_t)); // allocate packet

                        /* Broadcast the mechanic request messages. */
                        packet->lamportTimestamp = lamportTime;
                        packet->inDock = 1;
                        packet->mechanicsTaken = 0;

                        for (int i = 0; i < size; i++) {
                            if (i != rank){ // to all except the process itself
                                // lockMutex();
                                MPI_Send(packet, 1, MPI_PACKET_T, i, message_t::MECH_REQ, MPI_COMM_WORLD); // send mechanics request
                                debug("Sent MECH_REQ to %d", i);
                                // unlockMutex();
                            }
                        }

                        lockMutex();
                            addToRequestQueue(std::make_pair(lamportTime, rank), mechRequestQueue); // add the request to the queue
                            debug("Added (%d, %d) to mech request queue.", lamportTime, rank);
                        unlockMutex();

                        dockRequestQueue.pop_back(); // remove the request
                        free(packet);
                    unlockMutex();
                } else {
                    unlockMutex();
                    continue; // warning
                };
                
                } break;
                
            case state_t::WAITING_MECH: {
                lockMutex();
                    int freeMechanics = N_MECH;

                    // check if there are enough free mechanics
                    for (int i=0; i<size; i++) {
                        freeMechanics -= mechStatus[i];
                    }

                    // debug("Damage: %d, free mechanics: %d, priority: %d - %d", damage, freeMechanics, priority(mechACK, mechRequestQueue), freeMechanics >= damage);

                    if (freeMechanics >= damage && priority(mechACK, mechRequestQueue)) { // there are enough free mechanics based on the damage and the process received larger timestamps from all others
                        // mechanics critical section
                        debug("REPAIRING");
                        mechStatus[rank] = damage; // add the taken mechanics to the mechanic status list
                        state = state_t::REPAIRING; // change the state once access is granted

                        mechRequestQueue.pop_back(); // remove the request
                    unlockMutex();
                }
                else {
                    unlockMutex();
                    continue;
                };

                } break;

            case state_t::REPAIRING: {
                sleep(1);
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
                packet->lamportTimestamp = lamportTime;
                packet->inDock = 0;
                packet->mechanicsTaken = 0;
                for (int i = 0; i < size; i++) {
                    if (i != rank){ // to all except the process itself
                        MPI_Send(packet, 1, MPI_PACKET_T, i, message_t::DOCK_REL, MPI_COMM_WORLD); // send dock release
                        debug("Sent DOCK_REL to %d", i);
                        MPI_Send(packet, 1, MPI_PACKET_T, i, message_t::MECH_REL, MPI_COMM_WORLD); // send mechanics release
                        debug("Sent MECH_REL to %d", i);
                    }
                }  
                // unlockMutex();
                free(packet);
                 }break;

            default: {
                printf("Unknown state.");
            }break;
        }
    }
}
