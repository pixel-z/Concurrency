#include "def.h"

int min(int a, int b){
    return a < b ? a : b;
}

void vacc_phase(Zone *zone, Company *comp)
{
    printf(ZONE_COLOR"Zone %d\033[0m\t\t is entering Vaccination phase\n",comp->id);
    int vaccInBatch = comp->vaccInBatch;

    while(vaccInBatch && studentLeft)
    {
        int mini = min(8, min(vaccInBatch,studentLeft));
        int slots = randomInRange(1,mini);
        pthread_mutex_lock(&zone->mutex);
        zone->slot_left = slots;
        pthread_mutex_unlock(&zone->mutex);

        vaccInBatch -= slots;
        
        printf(ZONE_COLOR"Zone %d\033[0m\t\t is ready to vaccinate with %d slots\n",comp->id,slots);
        while (zone->slot_left != 0);
        {
            // chill
        }
    }
    printf(ZONE_COLOR"Zone %d\033[0m\t\t has run out of vaccines\n",zone->id);
    pthread_mutex_lock(&zone->mutex);
    zone->ready = 0;
    pthread_mutex_unlock(&zone->mutex);
}

// searches for company
void* initZone(void* inp)
{
    Zone* zone = (Zone*) inp;
    printf(ZONE_COLOR"Zone %d\033[0m\t\t is initialized\n",zone->id);

    while (studentLeft>0)
    {
        for (int i = 0; i < pharmaCount; i++)
        {
            pthread_mutex_lock(&companies[i].mutex);
            pthread_mutex_lock(&zone[i].mutex);
            if (companies[i].batches>0 /*&& companies[i].ready==1 && zone->ready==0*/)
            {
                printf(ZONE_COLOR"Zone %d\033[0m\t\t "COMPANY_COLOR"PharmaCompany %d\033[0m delivering a vaccine batch which has success_prob %lf\n",zone->id,companies[i].id,companies[i].prob);
                printf(ZONE_COLOR"Zone %d\033[0m\t\t "COMPANY_COLOR"PharmaCompany %d\033[0m delivered vaccines, resuming vaccinations now\n",companies[i].id,zone->id);
                companies[i].batches--;
                zone->ready = 1;
                zone->prob = companies[i].prob;

                pthread_mutex_unlock(&companies[i].mutex);
                pthread_mutex_unlock(&zone[i].mutex);
                vacc_phase(zone,&companies[i]);
                break;
            }
            pthread_mutex_unlock(&companies[i].mutex);
            pthread_mutex_unlock(&zone[i].mutex);
        }
        
    }

}