#include "def.h"
#include "company.c"
#include "vacczone.c"
#include "student.c"

// returns random number between [l,r]
int randomInRange(int l, int r){
    return l + (rand()%(r-l+1));
}

int main()
{
    srand(time(0)); // changing random var every session
    printf("Enter pharmaCount, vaccZoneCount, studentCount: ");
    scanf("%d%d%d",&pharmaCount,&vaccCount,&studentCount);
    studentLeft=studentCount;

    if ((pharmaCount<0 || vaccCount<0 || studentCount<0)|| ((pharmaCount == 0 || vaccCount == 0) && studentCount > 0)) {
        printf("Incompatible inputs\n");
        return 1;
    }

    printf("Enter %d probabilities for each company: ",pharmaCount);
    for (int i = 0; i < pharmaCount; i++)
    {
        scanf("%lf",&companies[i].prob);
        if (companies[i].prob > 1)
        {
            printf("\033[1;31mProbability should be < 1\nExiting...\033[0m\n");
            return 1;
        }
        
    }
    
    // Company
    for (int i = 0; i < pharmaCount; i++)
    {
        companies[i].id = i+1;
        // companies[i].ready = 1;
        pthread_mutex_init(&companies[i].mutex, NULL);
        pthread_create(&companies[i].tid,NULL,initComp,&companies[i]);
    }
    // Vaccination zones
    for (int i = 0; i < vaccCount; i++)
    {
        zones[i].id = i+1;
        zones[i].ready = 1;
        pthread_mutex_init(&zones[i].mutex, NULL);
        pthread_create(&zones[i].tid,NULL,initZone,&zones[i]);
    }
    // Student
    for (int i = 0; i < studentCount; i++)
    {
        students[i].id = i+1;
        students[i].vacc_count = 0;
        pthread_create(&students[i].tid,NULL,initStud,&students[i]);
    }
    


    // Stop the simulation when all the students have gone through the drive
    for (int i = 0; i < studentCount; i++)
        pthread_join(students[i].tid,NULL);

    /********************** remove this **********************/
    // for (int i = 0; i < pharmaCount; i++)
    //     pthread_join(companies[i].tid,NULL);
    // for (int i = 0; i < vaccCount; i++)
    //     pthread_join(zones[i].tid,NULL);

    printf("\nSimulation Over\n");

    return 0;
}