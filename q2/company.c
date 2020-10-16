#include "def.h"

void make_drugs(Company *comp)
{
    int timeTaken = randomInRange(2,5);
    comp->batches = randomInRange(1,5);
    int capacity = randomInRange(10,20);

    printf(COMPANY_COLOR "PharmaCompany %d\033[0m\t preparing %d batches of %d vaccines each, with success_prob %lf\n", comp->id,comp->batches,capacity,comp->prob);
    comp->ready=0;

    sleep(timeTaken);

    // "Pharmaceutical Company J has prepared N batches of vaccines which have success probability Xj.
    // Waiting for all the vaccines to be used to resume production"
    printf(COMPANY_COLOR "PharmaCompany %d\033[0m\t prepared %d batches of vaccines with success_prob %lf\n", comp->id,comp->batches,comp->prob);

    /* Drugs ready & waiting to be delivered */
    while (comp->batches>0 && studentCount>0)
    {
        // busy waiting
    }

    /* all batches in company finished */
    if (studentCount>0 && comp->batches==0)
    {
        // "All the vaccines prepared by Pharmaceutical Company J are emptied. Resuming production now."
        printf("PharmaCompany %d\033[0m\t All vaccines prepared are emptied. Resuming production now",comp->id);
    }
    make_drugs(comp);
}

void* initComp(void *inp)
{
    Company *comp = (Company *)inp;
    
    make_drugs(comp);

    return NULL;
}