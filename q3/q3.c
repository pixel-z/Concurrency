#include "def.h"

int randomInRange(int l, int r){
    return l + (rand()%(r-l+1));
}

void *initArtist(void *inp)
{
    Artist *artist = (Artist *)inp;
    sleep(artist->arrivalTime);
    printf(GREEN"%s %c arrived and waiting for stage\n"END,artist->name,artist->instrument);

    int timeTakenToPerform = randomInRange(t1,t2);
    int flag=0, time_exceed=0;

    clock_t t=clock();

        /****************** Increase 2 sec when singer joins ****************/
        /****************** Implement combinations of singer and musician ****************/
    while(flag==0)
    {
        clock_t timewaited = clock() - t;
        double times = ((double)timewaited)/CLOCKS_PER_SEC;
		if(times > ((double)waitTime))
		{
			time_exceed = 1; 
			break;
		}
        if (artist->instrument!='b')
        {
            for (int i = 0; i < acousticStageCount; i++)
            {
                pthread_mutex_lock(&artist->mutex);
                if (acousticStage[i].ready==1 && artist->instrument!='s')
                {
                    printf(BOLDBLUE"%s performing %c at acoustic stage for %d sec\n"END,artist->name,artist->instrument,timeTakenToPerform);
                    acousticStage[i].ready = 0;
                    acousticStage[i].musician_ready = 0;
                    pthread_mutex_unlock(&artist->mutex);

                    sleep(timeTakenToPerform);
                    printf(BLUE"%s performance at acoustic stage ended\n"END,artist->name);

                    pthread_mutex_lock(&artist->mutex);
                    acousticStage[i].ready = 1;
                    acousticStage[i].musician_ready = 1;
                    pthread_mutex_unlock(&artist->mutex);

                    flag=1;
                    break;
                }
                else if(acousticStage[i].singer_ready==1 && artist->instrument=='s')
                {
                    printf(BOLDBLUE"%s performing %c at acoustic stage for %d sec\n"END,artist->name,artist->instrument,timeTakenToPerform);
                    acousticStage[i].ready = 0;
                    acousticStage[i].singer_ready = 0;
                    pthread_mutex_unlock(&artist->mutex);

                    sleep(timeTakenToPerform);
                    printf(BLUE"%s performance at acoustic stage ended\n"END,artist->name);

                    pthread_mutex_lock(&artist->mutex);
                    if (acousticStage[i].musician_ready==1) // singer done (but musician)
                        acousticStage[i].ready = 1;
                    acousticStage[i].singer_ready = 1;
                    pthread_mutex_unlock(&artist->mutex);

                    flag=1;
                    break;
                }
                pthread_mutex_unlock(&artist->mutex);
            }
        }
        if (flag==1) break;

        if (artist->instrument!='v')
        {
        }
    }
    if (time_exceed==1)
    {
        printf(MAGENTA"%s %c left because of impatience\n"END,artist->name,artist->instrument);
        return NULL;
    }
    // more ig

}

int main() 
{
    srand(time(0));
    int flag=0;
     
    scanf("%d %d %d %d %d %d %d",&artistCount,&acousticStageCount,&electricStageCount,&coordCount,&t1,&t2,&waitTime);

    for (int i = 0; i < artistCount; i++)
        scanf("%s %c %d",artists[i].name,&artists[i].instrument,&artists[i].arrivalTime);

    for (int i = 0; i < acousticStageCount; i++)
    {
        acousticStage[i].id = i+1;
        acousticStage[i].ready = 1;
        acousticStage[i].singer_ready = 1;
    }
    for (int i = 0; i < electricStageCount; i++)
    {
        electricStage[i].id = i+1;
        electricStage[i].ready = 1;
        electricStage[i].singer_ready = 1;
    }

    for (int i = 0; i < artistCount; i++)
    {
        artists[i].id = i+1;
        pthread_mutex_init(&artists[i].mutex,NULL);
        pthread_create(&artists[i].tid, NULL,initArtist,&artists[i]);
    }
    // for (int i = 0; i < coordCount; i++)
    // {
    //     coordinators[i].id = i+1;
    //     pthread_mutex_init(&coordinators[i].mutex, NULL);
    //     pthread_create(&coordinators[i].tid,NULL,initCoordinator,&coordinators[i]);
    // }
    
    
    for (int i = 0; i < artistCount; i++)
    {
        pthread_join(artists[i].tid,NULL);
    }
    printf(RED"\nFinished\n"END);

    return 0;
}