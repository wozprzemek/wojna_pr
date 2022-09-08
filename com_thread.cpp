#include "main.h"
#include "watek_komunikacyjny.h"

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *startKomWatek(void *ptr)
{
    MPI_Status status;
    packet_t packet;
    while (true) {
        MPI_Recv( &packet, 1, MPI_packet_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        switch ( status.MPI_TAG ) {
            case FINISH: 
                    changeState(InFinish);
            break;

            default:
            break;
        }
    }
}
