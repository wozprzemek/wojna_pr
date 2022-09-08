#include "main.h"
#include "main_thread.h"
#include "com_thread.h"
#include <pthread.h>
#include <mpi.h> 

#define FIELDNO 3

/* sem_init sem_destroy sem_post sem_wait */
//#include <semaphore.h>
/* flagi dla open */
//#include <fcntl.h>

state_t stan = InRun;
int size, rank, money;
MPI_Datatype MPI_PACKET_T;
pthread_t comThread;

pthread_mutex_t stateMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t moneyMutex = PTHREAD_MUTEX_INITIALIZER;

void check_thread_support(int provided)
{
    printf("THREAD SUPPORT: requested: %d\n", provided);
    switch (provided)
    {
    case MPI_THREAD_SINGLE:
        printf("No thread support, exiting.\n");
        /* Nie ma co, trzeba wychodzić */
        fprintf(stderr, "No thread support, exiting.\n");
        MPI_Finalize();
        exit(-1);
        break;
    case MPI_THREAD_FUNNELED:
        printf("tylko te wątki, ktore wykonaly mpi_init_thread mogą wykonać wołania do biblioteki mpi\n");
        break;
    case MPI_THREAD_SERIALIZED:
        /* Potrzebne zamki wokół wywołań biblioteki MPI */
        printf("tylko jeden watek naraz może wykonać wołania do biblioteki MPI\n");
        break;
    case MPI_THREAD_MULTIPLE:
        printf("Full thread support.\n"); /* tego chcemy. Wszystkie inne powodują problemy */
        break;
    default:
        printf("Nikt nic nie wie\n");
    }
}

void initCustomType() {
    const int nItems = FIELDNO;
    int blockLengths[FIELDNO] = {1, 1, 1};
    MPI_Datatype types[FIELDNO] = {MPI_INT, MPI_INT, MPI_INT};

    MPI_Aint offsets[nItems];
    offsets[0] = offsetof(packet_t, ts);
    offsets[1] = offsetof(packet_t, src);
    offsets[2] = offsetof(packet_t, data);

    MPI_Type_create_struct(nItems, blockleblockLengthsngths, offsets, types, &MPI_PACKET_T);
    MPI_Type_commit(&MPI_PACKET_T);
}

void init(int *argc, char ***argv) {
    printf("Init started.");
    int provided;
    MPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, &provided);
    check_thread_support(provided);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(rank);

    pthread_create(&comThread, NULL, startKomWatek, 0);
    printf("Init done.");
}

void finalize() {
    printf("Finalizing.");
    pthread_mutex_destroy(&stateMutex);
    pthread_join(comThread, NULL);
    MPI_Type_free(&MPI_PACKET_T);
    MPI_Finalize();
}

int main(int argc, char **argv)
{
    init(&argc, &argv);     // create the communication thread
    mainLoop();             // from main_thread.cpp
    finalize();

    return 0;
}

// /* opis patrz main.h */
// void sendPacket(packet_t *pkt, int destination, int tag)
// {
//     int freepkt = 0;
//     if (pkt == 0)
//     {
//         pkt = malloc(sizeof(packet_t));
//         freepkt = 1;
//     }
//     pkt->src = rank;
//     MPI_Send(pkt, 1, MPI_PACKET_T, destination, tag, MPI_COMM_WORLD);
//     if (freepkt)
//         free(pkt);
// }

// void changeMoney(int newMoney)
// {
//     pthread_mutex_lock(&moneyMut);
//     if (stan == InFinish)
//     {
//         pthread_mutex_unlock(&moneyMut);
//         return;
//     }
//     debug("przed: %d", money)
//         money += newMoney;
//     debug("po: %d", money)
//         pthread_mutex_unlock(&moneyMut);
// }

// void enterDock()
// {
//     pthread_mutex_lock(&moneyMut);
//     if (stan == InFinish)
//     {
//         pthread_mutex_unlock(&moneyMut);
//         return;
//     }
//     debug("przed: %d", money)
//         money += newMoney;
//     debug("po: %d", money)
//         pthread_mutex_unlock(&moneyMut);
// }

// void getMechanics(int newMoney)
// {
//     pthread_mutex_lock(&moneyMut);
//     if (stan == InFinish)
//     {
//         pthread_mutex_unlock(&moneyMut);
//         return;
//     }
//     debug("przed: %d", money)
//         money += newMoney;
//     debug("po: %d", money)
//         pthread_mutex_unlock(&moneyMut);
// }

// void changeState(state_t newState)
// {
//     pthread_mutex_lock(&stateMut);
//     if (stan == InFinish)
//     {
//         pthread_mutex_unlock(&stateMut);
//         return;
//     }
//     stan = newState;
//     pthread_mutex_unlock(&stateMut);
// }
