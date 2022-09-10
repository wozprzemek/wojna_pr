#ifndef GLOBALH
#define GLOBALH
#define _GNU_SOURCE

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <vector>
#include <algorithm>
#include <functional>

/* odkomentować, jeżeli się chce DEBUGI */
//#define DEBUG 

#define SEC_IN_STATE 1
#define MIN_DAMAGE 1
#define MAX_DAMAGE 10	
#define N_DOCK 3
#define N_MECH 10

/* States */
typedef enum {FIGHTING, WAITING_DOCK, WAITING_MECH, REPAIRING} state_t;

/* Messages */
enum message_t {DOCK_REQ, DOCK_ACK, DOCK_REL, MECH_REQ, MECH_ACK, MECH_REL};

/* Global process variables */
extern state_t state;
extern int rank;
extern int size;
extern int lamportTime;
extern int damage;
extern std::vector<int> dockACK;
extern std::vector<int> mechACK;
extern std::vector<int> dockStatus; // 0 - free, 1 - taken
extern std::vector<int> mechStatus; // 0 - free, 1 or more - taken
extern std::vector<std::pair<int, int>> dockRequestQueue;
extern std::vector<std::pair<int, int>> mechRequestQueue;

/* Packet structure */
#define FIELDNO 3
typedef struct {
    int lamportTime;        // lamport timestamp
    int inDock;             // is the ship in dock
    int mechanicsTaken;     // number of mechanics taken
} packet_t;

extern MPI_Datatype MPI_PACKET_T;

void updateLamportTime(int recv);
bool priority(std::vector<int> &ACKList, std::vector<std::pair<int, int>> &requestQueue);
void addToRequestQueue(std::pair<int, int> req, std::vector<std::pair<int, int>> &requestQueue);

void lockMutex();
void unlockMutex();

/* Print formatting */
#ifdef DEBUG
#define debug(FORMAT,...) printf("%c[%d;%dm [%d]: " FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, rank, ##__VA_ARGS__, 27,0,37);
#else
#define debug(...) ;
#endif

#define P_WHITE printf("%c[%d;%dm",27,1,37);
#define P_BLACK printf("%c[%d;%dm",27,1,30);
#define P_RED printf("%c[%d;%dm",27,1,31);
#define P_GREEN printf("%c[%d;%dm",27,1,33);
#define P_BLUE printf("%c[%d;%dm",27,1,34);
#define P_MAGENTA printf("%c[%d;%dm",27,1,35);
#define P_CYAN printf("%c[%d;%d;%dm",27,1,36);
#define P_SET(X) printf("%c[%d;%dm",27,1,31+(6+X)%7);
#define P_CLR printf("%c[%d;%dm",27,0,37);

/* printf ale z kolorkami i automatycznym wyświetlaniem RANK. Patrz debug wyżej po szczegóły, jak działa ustawianie kolorków */
#define println(FORMAT, ...) printf("%c[%d;%dm [%d]: " FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, rank, ##__VA_ARGS__, 27,0,37);

#endif
