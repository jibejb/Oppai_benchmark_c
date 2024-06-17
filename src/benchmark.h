#ifndef BENCHMARK_H_INCLUDED
#define BENCHMARK_H_INCLUDED

#include "thpool.h"
#include <pthread.h>


typedef struct task_t *Task;
struct task_t {
	unsigned int id;
    double temp_s;
    double temp_e;
    double time;
    double delta;
    double ** data;
    int thread_n;
    long N;
};

typedef struct {
    volatile double t;
    volatile double S;
    volatile double Score;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    volatile int benchmark_running;
} Data;

double Oppai_func(double y, double t);

double integral_f_p(double alpha, double beta, void (*task)(void *), threadpool *thpl, int thread_n, int t);
double get_score(double interval);
void *benchmark(void* data);
#endif // BENCHMARK_H_INCLUDED
