#include "def.h"

void make_drugs(Company *comp)
{
    int timeTaken = randomInRange(2,5);
    int no_batches = randomInRange(1,5);
    int capacity = randomInRange(10,20);

    printf(COMPANY_COLOR "PharmaCompany %d\033[0m\t preparing %d batches of %d vaccines each, with success_prob %lf\n", comp->id,no_batches,capacity,comp->prob);
    comp->ready=0;

    sleep(timeTaken);

    // "Pharmaceutical Company J has prepared N batches of vaccines which have success probability Xj.
    // Waiting for all the vaccines to be used to resume production"
    printf(COMPANY_COLOR "PharmaCompany %d\033[0m\t prepared %d batches of vaccines with success_prob %lf\n", comp->id,no_batches,comp->prob);

    /* Drugs ready (EZ), waiting to be delivered */
    while (1)
    {
        
    }
        
}

void* initComp(void *inp)
{
    Company *comp = (Company *)inp;
    
    make_drugs(comp);

    return NULL;
}