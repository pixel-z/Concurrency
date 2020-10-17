#include "def.h"

// If a student is assigned a vaczone
void assignedStud(Student *stud, Zone *z)
{
    // *************************** decrement studentCount when positive antibody test **************************
    printf(STUDENT_COLOR "Student %d\033[0m \t assigned slot on Vaccination " ZONE_COLOR "Zone %d\e[0m and waiting to be vaccinated\n",stud->id,z->id);
    
}

// continuously check if zone is there to be allocated for waiting student
void waitingStud(Student *stud)
{
    printf( STUDENT_COLOR "Student %d\033[0m\t is waiting to be allocated a slot on a Vaccination Zone\n",stud->id);
    int flag=0;
    while (flag==0)
    {
        for (int i = 0; i < vaccCount; i++)
        {
            pthread_mutex_lock(&zones[i].mutex);
            if (zones[i].ready==1 && zones[i].slot_left>0)
            {
                assignedStud(stud, &zones[i]);
                zones[i].slot_left--;
                flag=1;
                pthread_mutex_unlock(&zones[i].mutex);
                break;
            }
            pthread_mutex_unlock(&zones[i].mutex);
        }

    }
}

// initialize student
void* initStud(void *inp)
{
    Student *stud = (Student *)inp;
    stud->arrivalTime = randomInRange(1,5);
    stud->vacc_count++;
    
    if (stud->vacc_count>3) return NULL;

    printf( STUDENT_COLOR "Student %d\033[0m\t initialized with arrivalTime %d\n",stud->id,stud->arrivalTime);
    sleep(stud->arrivalTime);
    printf( STUDENT_COLOR "Student %d\033[0m\t arrived for its %d round of Vaccination\n",stud->id,stud->vacc_count);

    waitingStud(stud);

    return NULL;
}