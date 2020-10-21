max allowed artists and coordinators <= 1000

### Instructions to compile
```bash
make
./a.out
```

### def.h
- Everything is initialized, all libraries, colors, structs, semaphore, etc.
- 


### q3.c

- Initially we take artistCount, acousticStageCount, electricStageCount, coordinator_Count, t1, t2, waitTime as input.
- All musicians and singers are threads.
- They are send to `initArtist()` function.

```cpp
int main() 
{
    srand(time(0));
    scanf("%d %d %d %d %d %d %d",&artistCount,&acousticStageCount,&electricStageCount,&coordCount,&t1,&t2,&waitTime);
    for (int i = 0; i < artistCount; i++)
        scanf("%s %c %d",artists[i].name,&artists[i].instrument,&artists[i].arrivalTime);

    acousticStagesEmpty = acousticStageCount;
    electricStagesEmpty = electricStageCount;
    sem_init(&semaphore, 0, coordCount);

    for (int i = 0; i < acousticStageCount; i++)
    {
        acousticStage[i].ready = 1;
        acousticStage[i].singer_ready = 1;
    }
    for (int i = 0; i < electricStageCount; i++)
    {
        electricStage[i].ready = 1;
        electricStage[i].singer_ready = 1;
    }
    for (int i = 0; i < artistCount; i++)
    {
        pthread_mutex_init(&artists[i].mutex,NULL);
        pthread_create(&artists[i].tid, NULL,initArtist,&artists[i]);
    }

    for (int i = 0; i < artistCount; i++)
    {
        pthread_join(artists[i].tid,NULL);
    }
    printf(BOLDRED"\nFinished\n"END);

    return 0;
}
```

### artist.c
- `initArtist()` then calls `allotStage()` function.
```cpp
void *initArtist(void *inp)
{
    Artist *artist = (Artist *)inp;
    sleep(artist->arrivalTime);
    printf(GREEN"%s %c arrived\n"END,artist->name,artist->instrument);
    artist->waiting = 1;

    // int timeTakenToPerform = randomInRange(t1,t2);
    artist->st = getTimeStruct(artist->arrivalTime+waitTime);
    
    allotStage(artist);
    return NULL;
}
```

- `allotStage()` uses conditional timewait to avoid busy waiting. Conditional wait signals whenever any stage becomes empty and then it calls `findStage()` function.
```cpp
void allotStage(Artist* artist)
{
    int flag = 0;
    pthread_mutex_lock(&artist->mutex);
    if (acousticStagesEmpty>0)
        flag = findStage(artist);
    if (flag == 0)
    {
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
```
- `findStage()` is the most important and longest function.

```cpp
int findStage(Artist* artist)
{
    int timeTakenToPerform = randomInRange(t1,t2);
    if (artist->instrument!='b')    // can search acoustic stages
    {
        for (int i = 0; i < acousticStageCount; i++)
        {
            if (acousticStage[i].ready==1 && artist->instrument!='s')   // musician gets empty stage
            {
            }
            else if(artist->instrument=='s') 
            {
                if (acousticStage[i].ready==1) // only singer performing
                {
                }
                else if (acousticStage[i].singer_ready==1) // singer joins musician
                {
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
            }
            else if(artist->instrument=='s') 
            {
                if (electricStage[i].ready==1) // only singer performing
                {
                }
                else if (electricStage[i].singer_ready==1) // singer joins musician
                {
                }
            }
        }
    }
    return 0;
}

```
