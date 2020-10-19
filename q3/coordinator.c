#include "def.h"

void gotoCoordinator(Artist* artist)
{
    sem_wait(&semaphore);
    sleep(2);
    printf(MAGENTA"%s collecting t-shirt\n"END,artist->name);
    sem_post(&semaphore);
}