### Overview
The code shows which method (normal version, concurrent version, multi-threaded version) is fastest and by how much.

This is same as given in the tutorial. Just some minor changes.

### Instructions to compile
```bash
    gcc q1.c -lpthread
    ./a.out
```

### Methods

1. Normal Mergesort:    
    - Functions used:   
        ```cpp
            normal_mergeSort()
            merge()
        ``` 
    - Find the middle point to divide the array into two halves:  
        mid = low+(high-low)/2
    - Call mergeSort for first half: Call `mergeSort(arr, l, m)`
    - Call mergeSort for second half: Call `mergeSort(arr, m+1, r)`
    - Merge the two halves sorted in step 2 and 3:  
    Call `merge(arr, l, m, r)`

    - In `merge()`, we store left half in L[ ] and right half in R[ ] and compare them and sorted array is stored in original array

2. Concurrent Mergesort:    
    - Functions used:
        ```cpp
            mergeSort()
            merge()
        ```
    - Similar to the last one.
    - When elements less than 5 then selection sort is implemented.
    - The child processes do the `mergeSort()` and the parent `merge()` these two children.

3. Threaded Mergesort:  
    - Function used:
        ```cpp
            threaded_mergesort()
        ```
    - When elements less than 5 then selection sort is implemented.
    - Same as concurrent but instead of creating new processes we create new threads.
    - Large values as large as 10^6 gives segmentation fault.

### Performance Analysis

Normal merge_sort is the quickest amongst all. Concurrent_mergesort(forked) takes more time to sort due to message passing(IPC), forking and duplicating all values takes time. Threaded sort is faster than concurrent(forked) mergesort due to common shared memory and it is slower than normal mergesort due to creation of new thread each time takes time.