#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 500


int array[MAX_SIZE];
int sortedArray[MAX_SIZE];
int aSize = 0;


typedef struct {
    int starting_index;
    int ending_index;
} parameters;

// Function declarations
void *sorter(void *params);  // Thread function for sorting
void *merger(void *params);  // Thread function for merging

int main(int argc, const char *argv[]) {
    // Read integers from the input file
    FILE *input_file = fopen("IntegerList.txt", "r");
    if (!input_file) {
        fprintf(stderr, "Error: Could not open IntegerList.txt\n");
        return 1;
    }

    char buffer[1000];
    if (fgets(buffer, sizeof(buffer), input_file) != NULL) {
        char *token = strtok(buffer, ",");
        while (token != NULL && aSize < MAX_SIZE) {
            array[aSize++] = atoi(token);
            token = strtok(NULL, ",");
        }
    }
    fclose(input_file);

    // Create sorting threads
    pthread_t tid1, tid2;
    parameters *data1 = (parameters *) malloc(sizeof(parameters));
    parameters *data2 = (parameters *) malloc(sizeof(parameters));

    data1->starting_index = 0;
    data1->ending_index = aSize / 2 - 1;
    data2->starting_index = aSize / 2;
    data2->ending_index = aSize - 1;

    pthread_create(&tid1, NULL, sorter, data1);
    pthread_create(&tid2, NULL, sorter, data2);

    // Wait for sorting threads to finish
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    // Create merging thread
    pthread_t tid_merge;
    pthread_create(&tid_merge, NULL, merger, NULL);

    // Wait for merging thread to finish
    pthread_join(tid_merge, NULL);

    // Write sorted array to output file
    FILE *output_file = fopen("SortedIntegerList.txt", "w");
    if (!output_file) {
        fprintf(stderr, "Error: Could not create SortedIntegerList.txt\n");
        return 1;
    }
    for (int i = 0; i < aSize; i++) {
        fprintf(output_file, "%d", sortedArray[i]);
        if (i < aSize - 1) {
            fprintf(output_file, ",");
        }
    }
    fclose(output_file);

    free(data1);
    free(data2);
    return 0;
}

void *sorter(void *params) {
    //  Implementing Bubble Sort
    parameters *p = (parameters *)params;
    int start = p->starting_index;
    int end = p->ending_index;

    for (int i = start; i <= end; i++) {
        for (int j = start; j < end - (i - start); j++) {
            if (array[j] > array[j + 1]) {
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
    pthread_exit(0);
}

void *merger(void *params) {
    // Merge two sorted sublists
    int i = 0;
    int j = aSize / 2;
    int k = 0;

    while (i < aSize / 2 && j < aSize) {
        if (array[i] < array[j]) {
            sortedArray[k++] = array[i++];
        } else {
            sortedArray[k++] = array[j++];
        }
    }
    while (i < aSize / 2) {
        sortedArray[k++] = array[i++];
    }
    while (j < aSize) {
        sortedArray[k++] = array[j++];
    }

    pthread_exit(0);
}
