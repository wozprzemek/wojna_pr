#include "main.h"
#include "com_thread.h"

/* Communication thread; takes care of receiving and reacting to messages*/
void *comLoop(void *ptr) {
    MPI_Status status;
    packet_t packet;

    while (true) {
        MPI_Recv( &packet, 1, MPI_packet_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        switch ( status.MPI_TAG ) {
            case message_t::DOCK_REQ: 
                // TODO
            break;

            case message_t::DOCK_ACK: 
                // TODO
            break;

            case message_t::DOCK_REL: 
                // TODO
            break;

            case message_t::MECH_REQ: 
                // TODO
            break;

            case message_t::MECH_ACK: 
                // TODO
            break;

            case message_t::MECH_REL: 
                // TODO
            break;

            default:
            
            break;
        }
    }
}
