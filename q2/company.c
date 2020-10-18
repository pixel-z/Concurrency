#include "def.h"

void make_drugs(Company *comp)
{
    int timeTaken = randomInRange(2,5);
    int batches = randomInRange(1,5);
    int capacity = randomInRange(10,20);

    pthread_mutex_lock(&comp->mutex);
    printf(COMPANY_COLOR "PharmaCompany %d\033[0m\t preparing %d batches of %d vaccines each, with success_prob %lf\n", comp->id,batches,capacity,comp->prob);
    // pthread_mutex_lock(&comp->mutex);
    // comp->ready=0;
    // pthread_mutex_unlock(&comp->mutex);

    sleep(timeTaken);

    printf(COMPANY_COLOR "PharmaCompany %d\033[0m\t prepared %d batches of vaccines with success_prob %lf\n", comp->id,batches,comp->prob);
    // pthread_mutex_unlock(&comp->mutex);
    // pthread_mutex_lock(&comp->mutex);
    // comp->ready=1;
    comp->vaccInBatch=capacity;
    comp->batches = batches;
    pthread_mutex_unlock(&comp->mutex);

    /* Drugs ready & waiting to be delivered */
    while (comp->batches>0)
    {
        // chill (busy waiting)
    }

    /* all batches in company finished */
    if (comp->batches==0)
    {
        printf(COMPANY_COLOR"PharmaCompany %d\033[0m\t All vaccines prepared are emptied. Resuming production now\n",comp->id);
    }
    make_drugs(comp);
}

void* initComp(void *inp)
{
    Company *comp = (Company *)inp;
    
    make_drugs(comp);

    return NULL;
}