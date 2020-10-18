#include "def.h"

int randomInRange(int l, int r){
    return l + (rand()%(r-l+1));
}

void *initArtist(void *inp)
{
    Artist *artist = (Artist *)inp;
    sleep(artist->arrivalTime);
    printf(GREEN"%s %c arrived\n"END,artist->name,artist->instrument);

    int timeTaken = randomInRange(t1,t2);
    int flag=0;
    while(flag==0)
    {
        if (artist->instrument!='b')
        {
            for (int i = 0; i < acousticStageCount; i++)
            {
                
            }
        }
        if (flag==1) break;

        if (artist->instrument!='v')
        {
        }
    }

}

// void *initCoordinator(void *inp)
// {
//     Coordinator *coordinator = (Coordinator *)inp;
// }

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