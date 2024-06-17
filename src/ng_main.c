#include "benchmark.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ng_main() {



    Data *sharedData = malloc(sizeof(*sharedData));
    memset(sharedData, 0, sizeof(*sharedData));
    sharedData->t = 0.0;
    sharedData->S = 0.0;
    sharedData->Score = 0.0;
    sharedData->benchmark_running = 1;
    pthread_mutex_init(&sharedData->mutex, NULL);
    pthread_cond_init(&sharedData->cond, NULL);
    pthread_t benchmark_thread;
    if (pthread_create(&benchmark_thread, NULL, benchmark, (void*)sharedData) != 0) {
        perror("Failed to create main thread");
        exit(EXIT_FAILURE);
    }



    while (sharedData->benchmark_running) {
        pthread_mutex_lock(&sharedData->mutex);
        pthread_cond_wait(&sharedData->cond, &sharedData->mutex);

        fprintf(stdout, "S: %lf, Score: %lf\n", sharedData->S, sharedData->Score);

        pthread_mutex_unlock(&sharedData->mutex);

    }


    if (pthread_join(benchmark_thread, NULL) != 0) {
        perror("Failed to join main thread");
        exit(EXIT_FAILURE);
    }

}
