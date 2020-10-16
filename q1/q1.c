#define _POSIX_C_SOURCE 199309L //required for clock
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <inttypes.h>
#include <math.h>

void swap(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int * shareMem(size_t size){
     key_t mem_key = IPC_PRIVATE;
     int shm_id = shmget(mem_key, size, IPC_CREAT | 0666);
     return (int*)shmat(shm_id, NULL, 0);
}

void merge(int *arr, int low, int high){
     int mid = low+(high-low)/2;
     int n1 = mid-low+1, n2 = high-mid;
     int L[n1], R[n2];

     for (int i = 0; i < n1; i++) L[i]=arr[i+low];
     for (int i = 0; i < n2; i++) R[i]=arr[i+mid+1];

     int i=0,j=0,c=low;
     while (i<n1 && j<n2)
     {
          if (L[i] <= R[j])  
               arr[c++] = L[i++]; 
          else 
               arr[c++] = R[j++]; 
     }
     while(i<n1) arr[c++] = L[i++];
     while(j<n2) arr[c++] = R[j++];
}

void normal_mergeSort(int *arr, int low, int high){

     if (low >= high) return;

     int mid = low+(high-low)/2;

     normal_mergeSort(arr, low, mid);
     normal_mergeSort(arr, mid+1, high);
     merge(arr,low,high);
}

void mergeSort(int *arr, int low, int high){
     if (low>=high) _exit(1);

     // selection sort
     if (high-low+1 < 5)
     {
          for (int i = low; i <= high-1; i++)
          {
               int mini = i;
               for (int j = i+1; j <= high; j++)
                    if (arr[j]<arr[mini])
                         mini = j;
               swap(&arr[i], &arr[mini]);
          }
          return;
     }

     int mid = low+(high-low)/2;
     int pid1=fork();
     if (pid1==0)
     {
          mergeSort(arr,low,mid);
          _exit(1);
     } 
     else {
          int pid2=fork();
          if (pid2==0)
          {
               mergeSort(arr,mid+1,high);
               _exit(1);
          }
          else {
               int status;
               waitpid(pid1,&status,0);
               waitpid(pid2,&status,0);
          }
     }
     merge(arr,low,high);
     return;
}

struct arg{
     int l;
     int r;
     int* arr;
};

void *threaded_mergeSort(void* a){
     //note that we are passing a struct to the threads for simplicity.
     struct arg *args = (struct arg*) a;

     int l = args->l;
     int r = args->r;
     int *arr = args->arr;
     if(l>r) return NULL;

     int m = l+(r-l)/2;
     // selection sort
     if (r-l+1 < 5)
     {
          for (int i = l; i <= r-1; i++)
          {
               int mini = i;
               for (int j = i+1; j <= r; j++)
                    if (arr[j]<arr[mini])
                         mini = j;
               swap(&arr[i], &arr[mini]);
          }
          return NULL;
     }


     //sort left half array
     struct arg a1;
     a1.l = l;
     a1.r = m;
     a1.arr = arr;
     pthread_t tid1;
     pthread_create(&tid1, NULL, threaded_mergeSort, &a1);

     //sort right half array
     struct arg a2;
     a2.l = m+1;
     a2.r = r;
     a2.arr = arr;
     pthread_t tid2;
     pthread_create(&tid2, NULL, threaded_mergeSort, &a2);

     //wait for the two halves to get sorted
     pthread_join(tid1, NULL);
     pthread_join(tid2, NULL);

     merge(arr,l,r);
}

void runSorts(long long int n){

     struct timespec ts;

     //getting shared memory
     int *arr = shareMem(sizeof(int)*(n+1));
     for(int i=0;i<n;i++) scanf("%d", arr+i);

     int brr[n+1], crr[n+1];
     for(int i=0;i<n;i++) 
     {
          brr[i] = arr[i];
          crr[i] = arr[i];
     }

     printf("Running concurrent_mergesort for n = %lld\n", n);
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     long double st = ts.tv_nsec/(1e9)+ts.tv_sec;

     //multiprocess mergesort ( sorts arr[] )------------------------------------------------------------
     mergeSort(arr, 0, n-1);
     for(int i=0; i<n; i++){
          printf("%d ",arr[i]);
     }
     printf("\n");
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     long double en = ts.tv_nsec/(1e9)+ts.tv_sec;
     printf("time = %Lf\n", en - st);
     long double t1 = en-st;

     pthread_t tid;
     struct arg a;
     a.l = 0;
     a.r = n-1;
     a.arr = brr;
     printf("Running threaded_concurrent_mergesort for n = %lld\n", n);
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     st = ts.tv_nsec/(1e9)+ts.tv_sec;

     //multithreaded mergesort ( sorts brr[] )----------------------------------------------------------
     pthread_create(&tid, NULL, threaded_mergeSort, &a);
     pthread_join(tid, NULL);
     for(int i=0; i<n; i++){
          printf("%d ",a.arr[i]);
     }
     printf("\n");
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     en = ts.tv_nsec/(1e9)+ts.tv_sec;
     printf("time = %Lf\n", en - st);
     long double t2 = en-st;

     printf("Running normal_mergesort for n = %lld\n", n);
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     st = ts.tv_nsec/(1e9)+ts.tv_sec;

     // normal mergesort ( sorts crr[] )----------------------------------------------------------------
     normal_mergeSort(crr, 0, n-1);
     for(int i=0; i<n; i++){
          printf("%d ",crr[i]);
     }
     printf("\n");
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     en = ts.tv_nsec/(1e9)+ts.tv_sec;
     printf("time = %Lf\n", en - st);
     long double t3 = en - st;

     printf("normal_mergesort ran:\n\t[ %Lf ] times faster than concurrent_mergesort\n\t[ %Lf ] times faster than threaded_concurrent_mergesort\n\n\n", t1/t3, t2/t3);
     shmdt(arr);
     return;
}

int main(){

     long long int n;
     scanf("%lld", &n);
     runSorts(n);
     return 0;
}
