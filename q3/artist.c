#include "def.h"

// used for conditional wait (gives waitTime till which to wait)
struct timespec* getTimeStructSinceEpoch(int extraTime) {
    struct timespec* st = (struct timespec*)malloc(sizeof(struct timespec*));

    clock_gettime(CLOCK_REALTIME, st);
    st->tv_sec += extraTime;

    return st;
}

int findStage(Artist* artist)
{
    int timeTakenToPerform = randomInRange(t1,t2);
   
    if (artist->instrument!='b')    // can search acoustic stages
    {
        for (int i = 0; i < acousticStageCount; i++)
        {
            if (acousticStage[i].ready==1 && artist->instrument!='s')   // musician gets empty stage
            {
                printf(BOLDBLUE"%s performing %c at acoustic stage for %d sec\n"END,artist->name,artist->instrument,timeTakenToPerform);
                acousticStage[i].ready = 0;
                // acousticStage[i].musician_ready = 0;
                acousticStage[i].musician = artist;

                sleep(timeTakenToPerform);
                if (acousticStage[i].singer_ready==0)   //singer joins in between (extend by 2 sec)
                { 
                    sleep(2);
                    acousticStage[i].singer_ready = 1;
                }  
                
                printf(BLUE"%s performance at acoustic stage ended\n"END,artist->name);
                // searching for next performer
                artist->waiting = 0;
                for (int j = 0; j < artistCount; j++)
                {
                    if(artist[j].waiting==1){
                        // printf(YELLOW"%s is next"END,artist[j].name);
                        pthread_cond_signal(&artist[j].cond);
                        break;
                    }
                }
                
                acousticStage[i].ready = 1;
                // acousticStage[i].musician_ready = 1;
                return 1;
            }
            else if(artist->instrument=='s') 
            {
                if (acousticStage[i].ready==1) // only singer performing
                {
                    printf(BOLDBLUE"%s performing %c at acoustic stage for %d sec\n"END,artist->name,artist->instrument,timeTakenToPerform);
                    acousticStage[i].ready = 0;
                    acousticStage[i].singer_ready = 0;
                    // pthread_mutex_unlock(&artist->mutex);

                    sleep(timeTakenToPerform);
                    printf(BLUE"%s performance at acoustic stage ended\n"END,artist->name);

                    artist->waiting = 0;
                    for (int j = 0; j < artistCount; j++)
                    {
                        if(artist[j].waiting==1){
                            printf(YELLOW"%s is next"END,artist[j].name);
                            pthread_cond_signal(&artist[j].cond);
                            break;
                        }
                    }
                    acousticStage[i].ready = 1;
                    acousticStage[i].singer_ready = 1;
                    return 1;
                }
                else if (acousticStage[i].singer_ready==1) // singer joins musician
                {
                    printf(CYAN"%s joined %s performance, performance extended by 2 secs\n"END,artist->name,acousticStage[i].musician->name);
                    acousticStage[i].singer_ready = 0;
                    return 1;
                }
                
            }
        }
    }
    if (artist->instrument!='v')    // can search electricStage
    {
        for (int i = 0; i < electricStageCount; i++)
        {
            if (electricStage[i].ready==1 && artist->instrument!='s')   // musician gets empty stage
            {
                printf(BOLDBLUE"%s performing %c at electric stage for %d sec\n"END,artist->name,artist->instrument,timeTakenToPerform);
                electricStage[i].ready = 0;
                // electricStage[i].musician_ready = 0;
                electricStage[i].musician = artist;

                sleep(timeTakenToPerform);
                if (electricStage[i].singer_ready==0)   //singer joins in between (extend by 2 sec)
                { 
                    sleep(2);
                    electricStage[i].singer_ready = 1;
                }  
                
                printf(BLUE"%s performance at electric stage ended\n"END,artist->name);
                artist->waiting = 0;
                for (int j = 0; j < artistCount; j++)
                {
                    if(artist[j].waiting==1){
                        pthread_cond_signal(&artist[j].cond);
                        break;
                    }
                }
                electricStage[i].ready = 1;
                // electricStage[i].musician_ready = 1;
                return 1;
            }
            else if(artist->instrument=='s') 
            {
                if (electricStage[i].ready==1) // only singer performing
                {
                    printf(BOLDBLUE"%s performing %c at electric stage for %d sec\n"END,artist->name,artist->instrument,timeTakenToPerform);
                    electricStage[i].ready = 0;
                    electricStage[i].singer_ready = 0;
                    // pthread_mutex_unlock(&artist->mutex);

                    sleep(timeTakenToPerform);
                    printf(BLUE"%s performance at electric stage ended\n"END,artist->name);
                    artist->waiting = 0;
                    for (int j = 0; j < artistCount; j++)
                    {
                        if(artist[j].waiting==1){
                            pthread_cond_signal(&artist[j].cond);
                            break;
                        }
                    }

                    electricStage[i].ready = 1;
                    electricStage[i].singer_ready = 1;
                    return 1;
                }
                else if (electricStage[i].singer_ready==1) // singer joins musician
                {
                    printf(CYAN"%s joined %s performance, performance extended by 2 secs\n"END,artist->name,electricStage[i].musician->name);
                    electricStage[i].singer_ready = 0;
                    return 1;
                }
                
            }
        }
    }
    

    return 0;
}

void allotStage(Artist* artist)
{
    int flag = 0;
    pthread_mutex_lock(&artist->mutex);

    if (acousticStagesEmpty>0)
        flag = findStage(artist);
    if (flag == 0)
    {
        // printf(CYAN"%s is waiting to perform\n"END,artist->name);
        int ret = pthread_cond_timedwait(&artist->cond,&artist->mutex,artist->st);
        pthread_mutex_unlock(&artist->mutex);

        if (ret == 0)   // got signal that stage available
        {
            allotStage(artist);
            return;
        }
        else
        {
            printf(RED"%s left because of impatience\n"END,artist->name);
            artist->waiting = 0;
            return;
        }
    }
    
    pthread_mutex_unlock(&artist->mutex);
    artist->waiting = 0;
    if (artist->instrument!='s')
        gotoCoordinator(artist);
    
}

void *initArtist(void *inp)
{
    Artist *artist = (Artist *)inp;
    sleep(artist->arrivalTime);
    printf(GREEN"%s %c arrived\n"END,artist->name,artist->instrument);
    artist->waiting = 1;

    // int timeTakenToPerform = randomInRange(t1,t2);
    artist->st = getTimeStructSinceEpoch(artist->arrivalTime+waitTime);
    
    allotStage(artist);
    return NULL;
}