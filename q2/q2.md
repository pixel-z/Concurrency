pharmaCount <= 1000 (prob array size)

### Instructions to compile
```bash
    make
    ./a.out
```

### q2.c
- Functions used:
    ```cpp
    int randomInRange(int l, int r);    // returns random number in [l,r]
    int main();                         
    ```
- `srand(time(0))` is used to seed the random number generator with current time
- 3 for loops in which all the threads are called.
- All company, student, vaccination zone are threads.
- Struct variables and mutexes are initialized.
- Simulation ends when all student threads gone through drive.

### student.c
- All student threads are send to `initStudent()` function, which allocates     random arrival time for each student and when student arrives then sends it `waitingStud()` function.
    ```cpp
    void* initStud(void *inp)
    {
        Student *stud = (Student *)inp;
        stud->arrivalTime = randomInRange(1,5);
        printf( STUDENT_COLOR "Student %d\033[0m\t initialized with arrivalTime %d\n",stud->id,stud->arrivalTime);
        sleep(stud->arrivalTime);
        waitingStud(stud);
        return NULL;
    }
    ```
- In `waitingStud()` each student thread continuously checks for available vaccine zone slot concurrently (applying mutex locks), if it is available then send to `assignedStud()` function. Then student check in the end is for student who test negative for antibody test, the thread recurses `waitingStud()` function again.

    ```cpp
    void waitingStud(Student *stud)
    {
        stud->vacc_count++;
        if (stud->vacc_count>3)
        {
            // send home 
            return;
        } 
        int flag=0;
        while (flag==0)
        {
            for (int i = 0; i < vaccCount; i++)
            {
                pthread_mutex_lock(&zones[i].mutex);
                if (zones[i].ready==1 && zones[i].slot_left>0)
                {
                    assignedStud(stud, &zones[i]);
                    sleep(1);
                    zones[i].slot_left--;
                    flag=1;
                    pthread_mutex_unlock(&zones[i].mutex);
                    break;
                }
                pthread_mutex_unlock(&zones[i].mutex);
            }
        }
        if (stud->id!=-1)
            waitingStud(stud);
    }
    ```

- In `assignedStud()`, we multiply the probability of success with RAND_MAX. If this value is less than `RAND_MAX*(success_prob)` then the student has tested positive for antibody and the id becomes -1 to stop recursing it in `waitingStud()`.
    ```cpp
    void assignedStud(Student *stud, Zone *z)
    {
        printf(STUDENT_COLOR "Student %d\033[0m \t assigned slot on Vaccination " ZONE_COLOR "Zone %d\033[0m and waiting to be vaccinated\n",stud->id,z->id);
        printf(STUDENT_COLOR "Student %d\033[0m \t on Vaccination " ZONE_COLOR "Zone %d\033[0m has been vaccinated with success_prob %lf\n",stud->id,z->id,z->prob);
        
        /* Antibody test */
        int rand = randomInRange(1,RAND_MAX);
        if (rand <= (RAND_MAX*z->prob))
        {
            printf(STUDENT_COLOR "Student %d\033[0m\t has tested positive for antibodies\n",stud->id);
            stud->id = -1;
            studentLeft--;
            if (studentLeft == 0)
            {
                printf("\nSimulation Over\n");
                _exit(1);
            }
        }
        else
            printf(STUDENT_COLOR "Student %d\033[0m\t has tested negative for antibodies\n",stud->id);
    }
    ```

### company.c
- Thread from `int main()` is send to `initComp()` which redirects to `make_drugs()` function.
- The pharma company makes drugs in and then waits for its batches to be all emptied. As soon as it is emptied it resumes production.
- Busy waiting is implemented in empty while loop which waits for batches to be finished.

    ```cpp
    void make_drugs(Company *comp)
    {
        int timeTaken = randomInRange(2,5);
        int batches = randomInRange(1,5);
        int capacity = randomInRange(10,20);

        pthread_mutex_lock(&comp->mutex);
        printf(COMPANY_COLOR "PharmaCompany %d\033[0m\t preparing %d batches of %d vaccines each, with success_prob %lf\n", comp->id,batches,capacity,comp->prob);

        sleep(timeTaken);

        printf(COMPANY_COLOR "PharmaCompany %d\033[0m\t prepared %d batches of %d vaccines each, with success_prob %lf\n", comp->id,batches,capacity,comp->prob);
        comp->vaccInBatch=capacity;
        comp->batches = batches;
        pthread_mutex_unlock(&comp->mutex);

        /* Drugs ready & waiting to be delivered */
        while (comp->batches>0)
        {
            // chill (busy waiting)
        }

        /* all batches in company finished */
        if (comp->batches==0)
        {
            printf(COMPANY_COLOR"PharmaCompany %d\033[0m\t All vaccines prepared are emptied. Resuming production now\n",comp->id);
        }
        make_drugs(comp);
    }
    ```

### vacczone.c

- `initZone()` is called which continuosly checks for company which can deliver it a batch of vaccines. As soon as it finds one `vacc_phase()` is called.
    ```cpp
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
                if (companies[i].batches>0)
                {
                    printf(ZONE_COLOR"Zone %d\033[0m\t\t "COMPANY_COLOR"PharmaCompany %d\033[0m delivering a vaccine batch which has success_prob %lf\n",zone->id,companies[i].id,companies[i].prob);
                    printf(ZONE_COLOR"Zone %d\033[0m\t\t "COMPANY_COLOR"PharmaCompany %d\033[0m delivered vaccines, resuming vaccinations now\n",zone->id,companies[i].id);
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
    ```

- `vacc_phase()` is function which implements vaccination phase. Busy waiting is also implemented here.

    ```cpp
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
    ```