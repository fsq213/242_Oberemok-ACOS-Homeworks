#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define MIN_N 16

int *array;
int array_size;
long long global_sum = 0;
pthread_mutex_t sum_mutex;

typedef struct {
    int start_index;
    int end_index;
    int thread_id;
} ThreadData;

void* calculate_sum(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    long long local_sum = 0;
    
    for (int i = data->start_index; i < data->end_index; i++) {
        local_sum += array[i];
    }
    
    pthread_mutex_lock(&sum_mutex);
    global_sum += local_sum;
    pthread_mutex_unlock(&sum_mutex);
    
    printf("Thread %d: Calculated sum for indices [%d, %d) = %lld\n", 
           data->thread_id, data->start_index, data->end_index, local_sum);
    
    return NULL;
}

void print_array(int* arr, int size) {
    printf("Array contents:\n[");
    for (int i = 0; i < size; i++) {
        printf("%d", arr[i]);
        if (i < size - 1) {
            printf(", ");
        }
        if ((i + 1) % 10 == 0 && i < size - 1) {
            printf("\n ");
        }
    }
    printf("]\n");
}

int main() {
    int N;
    pthread_t threads[4];
    ThreadData thread_data[4];
    
    srand(time(NULL));
    
    printf("Enter integer N (minimum %d): ", MIN_N);
    scanf("%d", &N);
    
    if (N < MIN_N) {
        printf("Error: N must be at least %d\n", MIN_N);
        return 1;
    }
    
    array_size = N;
    
    array = (int*)malloc(N * sizeof(int));
    if (array == NULL) {
        printf("Error: Memory allocation failed\n");
        return 1;
    }
    
    for (int i = 0; i < N; i++) {
        array[i] = rand() % 100;
    }
    
    print_array(array, N);
    
    if (pthread_mutex_init(&sum_mutex, NULL) != 0) {
        printf("Error: Mutex initialization failed\n");
        free(array);
        return 1;
    }
    
    int base_size = N / 4;
    int remainder = N % 4;
    int start_index = 0;
    
    printf("\nPartition information:\n");
    for (int i = 0; i < 4; i++) {
        int part_size = base_size + (i < remainder ? 1 : 0);
        
        thread_data[i].start_index = start_index;
        thread_data[i].end_index = start_index + part_size;
        thread_data[i].thread_id = i + 1;
        
        printf("Thread %d: indices [%d, %d) - %d elements\n", 
               i + 1, start_index, start_index + part_size, part_size);
        
        start_index += part_size;
    }
    printf("\n");
    
    for (int i = 0; i < 4; i++) {
        if (pthread_create(&threads[i], NULL, calculate_sum, &thread_data[i]) != 0) {
            printf("Error: Failed to create thread %d\n", i);
            pthread_mutex_destroy(&sum_mutex);
            free(array);
            return 1;
        }
    }
    
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_mutex_destroy(&sum_mutex);
    
    printf("\nFinal result:\n");
    printf("Sum of all elements = %lld\n", global_sum);
    
    long long verification_sum = 0;
    for (int i = 0; i < N; i++) {
        verification_sum += array[i];
    }
    printf("Verification sum = %lld\n", verification_sum);
    
    if (global_sum == verification_sum) {
        printf("Sum verification successful!\n");
    } else {
        printf("Sum verification failed!\n");
    }
    
    free(array);
    
    return 0;
}