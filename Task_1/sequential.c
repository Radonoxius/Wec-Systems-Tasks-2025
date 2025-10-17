#include<stdlib.h>
#include<stdint.h>
#include<stdio.h>
#include<time.h>

const uint64_t ARRAY_SIZE = 40000000;

uint64_t *create_random_array() {
    uint64_t *array = (uint64_t *) malloc(sizeof(uint64_t) * ARRAY_SIZE);

    if (array == NULL) {
        printf("An error occured while allocating memory for the array.\n");
        exit(1);
    }

    FILE *urandom_handle = fopen("/dev/urandom", "r");
    size_t numbers_read = fread(array, sizeof(uint64_t), ARRAY_SIZE, urandom_handle);

    if (numbers_read < ARRAY_SIZE) {
        printf("An error occured while generating an array.\n");
        free(array);
        exit(1);
    }

    fclose(urandom_handle);
    
    return array;
}

int main() {
    uint64_t *a1 = create_random_array();
    uint64_t *a2 = create_random_array();

    uint64_t *result = (uint64_t *) malloc(sizeof(uint64_t) * ARRAY_SIZE);

    struct timespec start_time, end_time;
    
    timespec_get(&start_time, TIME_UTC);
    for (uint64_t i = 0; i < ARRAY_SIZE; i++)
        result[i] = a1[i] + a2[i];
    timespec_get(&end_time, TIME_UTC);
    
    double duration =
        (((double) end_time.tv_sec) + (((double) end_time.tv_nsec) * 0.000000001)) -
        (((double) start_time.tv_sec) + (((double) start_time.tv_nsec) * 0.000000001));

    printf(
        "Time taken to add all elements sequentially: %f ns\n",
        1000000000.0 * duration
    );
    
    free(result);
    free(a2);
    free(a1);
}