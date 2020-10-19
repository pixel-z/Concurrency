#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define RED "\033[0;31m"        // finished
#define BLUE "\033[0;34m"       // performance ended
#define BOLDBLUE "\033[1;34m"   // performing
#define GREEN "\033[0;32m"      // arrived
#define YELLOW "\033[0;33m"
#define MAGENTA "\033[0;35m"    // collecting
#define CYAN "\033[0;36m"
#define END "\033[0m"

typedef struct Artist{
    int id;
    char name[1000];
    char instrument;          // p=piano,s=singer,etc
    int arrivalTime;
    pthread_t tid;
    pthread_mutex_t mutex;
} Artist;

typedef struct Coordinator{
    int id; // is this needed?
    pthread_t tid;
    pthread_mutex_t mutex;
} Coordinator;

typedef struct Stage{
    int id; // is this needed?
    int ready;              // if someone performing=0
    int musician_ready;     
    int singer_ready;       // 1=singer can go on stage
} Stage;

Artist artists[1000];
Coordinator coordinators[1000];
Stage acousticStage[1000], electricStage[1000];

int artistCount, coordCount;
int acousticStageCount, electricStageCount;
int waitTime;   // max wait time musician waits
int t1,t2;

sem_t semaphore;

int randomInRange(int l, int r);
