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

                pthread_mutex_lock(&stateMutex);
                    damage = (rand() % (MAX_DAMAGE - MIN_DAMAGE + 1)) 
                    + MIN_DAMAGE; // add random damage to the ship
                    state = state_t::WAITING_DOCK // change the state after fighting
                pthread_mutex_unlock(&stateMutex);

                packet_t *packet = (packet_t *) malloc(sizeof(packet_t));

                pthread_mutex_lock(&stateMutex);
                    updateLamportTime(0);
                pthread_mutex_unlock(&stateMutex); 

                /* Broadcast the request messages. */
                for (int i = 0; i < size; i++) {
                    if (i != rank){
                        pthread_mutex_lock(&stateMutex);
                            packet->lamportTime = lamportTime;
                            packet->src = 0;
                            packet->data = 0;

                            MPI_Send(packet, 1, MPI_PACKET_T, i, Message::REQ_D, MPI_COMM_WORLD);
                        pthread_mutex_unlock(&stateMutex); 
                    }
                }
                free(packet);
                break;

            case state_t::WAITING_DOCK:
                int free = 0;
                // check if there are free docks
                for (int i=0; i<size; i++) {
                    if (dockStatus[i] == 0) {
                        free++;
                    }
                }
                
                if (free > 0 && priority()) { // there are free docks and the process received larger timestamps from all others
                    if (requestQueue.size > 0) {
                        if (requestQueue[0].second == rank) { // process is at the top of the queue
                            // TODO critical section
                        }
                    }
                }
                else {
                    continue;
                };
                
                break;
                
            case state_t::WAITING_MECH:
                int free = 0;
                // check if there are enough free mechanics
                for (int i=0; i<size; i++) {
                    if (mechStatus[i] == 0) {
                        free++;
                    }
                }

                if (free > 0 && priority()) { // there are enough free mechanics and the process received larger timestamps from all others
                    if (requestQueue.size > 0) {
                        if (requestQueue[0].second == rank) { // process is at the top of the queue
                            // TODO critical section
                        }
                    }
                }
                else {
                    continue;
                };

                break;

            case state_t::IN_REPAIR:

                break;

            default:

            break;
        }
    }
}
