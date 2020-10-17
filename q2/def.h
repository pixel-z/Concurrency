#ifndef DEF_H
#define DEF_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define MAXN 100005

#define COMPANY_COLOR "\033[0;36m"
#define STUDENT_COLOR "\033[0;32m"
#define ZONE_COLOR "\033[0;35m"

typedef struct Company{
    int id;
    double prob;       // prob of success of vaccine
    int batches;       // no of batches company makes (changes every time)
    int vaccInBatch;   // no of vaccines in each batch
    int ready;         // 0=preparing drugs, 1=completed(idol)
    pthread_t tid;
    pthread_mutex_t mutex;
} Company;

typedef struct Zone{
    int id;
    int ready;          // 1=ready for new phase of students, else 0
    int slot_left;      // initially some random val (add student = slot_left-1)
    pthread_t tid;
    pthread_mutex_t mutex;
} Zone;

typedef struct Student{
    int id;
    int vacc_count;     // no of times vaccinated
    int arrivalTime;
    pthread_t tid;
} Student;


Company companies[MAXN];
Zone zones[MAXN];
Student students[MAXN];

int pharmaCount, vaccCount, studentCount;
int randomInRange(int l, int r);



#endif