#include "def.h"

int min(int a, int b){
    return a < b ? a : b;
}

void vacc_phase(Zone *zone, Company *comp)
{
    printf(ZONE_COLOR"Zone %d\033[0m\t is entering Vaccination phase\n",comp->id);
    int vaccInBatch = comp->vaccInBatch;

    while(vaccInBatch && studentCount)
    {
        int mini = min(8, min(vaccInBatch,studentCount));
        int slots = randomInRange(1,mini);
        pthread_mutex_lock(&zone->mutex);
        zone->slot_left = slots;
        pthread_mutex_unlock(&zone->mutex);

        vaccInBatch -= slots;
        
        printf(ZONE_COLOR"Zone %d\033[0m\t is ready to vaccinate with %d slots\n",comp->id,slots);
    }
    printf(ZONE_COLOR"Zone %d\033[0m\t has run out of vaccines",zone->id);
    pthread_mutex_lock(&zone->mutex);
    zone->ready = 1;
    pthread_mutex_unlock(&zone->mutex);
}

// searches for company
void* initZone(void* inp)
{
    Zone* zone = (Zone*) inp;

    while (studentCount>0)
    {
        for (int i = 0; i < pharmaCount; i++)
        {
            pthread_mutex_lock(&companies[i].mutex);
            pthread_mutex_lock(&zone[i].mutex);
            if (companies[i].batches>0 && companies[i].ready==1 && zone->ready==1)
            {
                companies[i].batches--;
                zone->ready = 0;
                // printf(COMPANY_COLOR "PharmaCompany %d\033[0m\t delivering a vaccine batch to Vaccination "
                // ZONE_COLOR"Zone %d\033[0m which has success_prob %lf\n",companies[i].id,zone->id,companies[i].prob);
                printf(ZONE_COLOR"Zone %d\033[0m\t PharmaCompany delivering a vaccine batch which has success_prob %lf\n",zone->id,companies[i].prob);
                printf(ZONE_COLOR"Zone %d\033[0m\t PharmaCompany delivered vaccines, resuming vaccinations now\n",zone->id);
                vacc_phase(zone,&companies[i]);
                pthread_mutex_unlock(&companies[i].mutex);
                pthread_mutex_unlock(&zone[i].mutex);
                break;
            }
            pthread_mutex_unlock(&companies[i].mutex);
            pthread_mutex_unlock(&zone[i].mutex);
        }
        
    }

}