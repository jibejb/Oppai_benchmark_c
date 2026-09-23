#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#define _XOPEN_SOURCE 700
#include <time.h>
#include <unistd.h>
#endif

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <pthread.h>
#include "benchmark.h"
#include "thpool.h"

#define INTEGRAL_LIMIT 1000
#define INTEGRAL_SIZE 1000000
#define SCORE_BASE 1000.0
#define SCORE_WINDOW 30


static double get_monotonic_sec(void) {
	#ifdef _WIN32
	static LARGE_INTEGER freq = {0};
	if (freq.QuadPart == 0) {
		QueryPerformanceFrequency(&freq);
	}
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return (double)counter.QuadPart / (double)freq.QuadPart;
	#else
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec + ts.tv_nsec / 1e9;
	#endif
}

double Oppai_func(double y, double t) {
	y = 0.02 * (y - 100);

	double y1 = y + 0.16 * sin(t);
	double y2 = y + 0.2 * sin(t);
	double y3 = y + 1.0;
	double y4 = 10.0 * y + 1.2 * (2.0 + sin(t)) * sin(t);

	double a1 = (1.5 * exp((0.12*sin(t) - 0.5) * y1*y1)) / (1.0 + exp(-20.0*(5.0*y + sin(t))));
	double a2 = ((1.5 + 0.8 * y2*y2*y2) * 1.0/(1.0 + exp(20.0*(5.0*y + sin(t))))) / (1.0 + exp(-(100.0*y3 + 16.0*sin(t))));
	double a3 = (0.2 * (exp(-y3*y3) + 1.0)) / (1.0 + exp(100.0*y3 + 16.0*sin(t)));
	double a4 = 0.1 / exp(2.0 * (y4*y4)*(y4*y4));

	return 65.0 * (a1 + a2 + a3 + a4);
}

void oppai_task(void *data) {
	Task tsk = (Task)data;
	int start_idx = tsk->start_idx;
	int end_idx = tsk->end_idx;
	double delta = tsk->delta;
	double t = tsk->time;
	double *dt = *(double **)tsk->data;
	double start_x = tsk->temp_s;

	for (int i = start_idx; i < end_idx; ++i) {
		double x = start_x + (i - start_idx) * delta;
		double fA = Oppai_func(x, t);
		double fB = Oppai_func(x + delta, t);
		double fM = Oppai_func(x + delta * 0.5, t);
		dt[i] = delta / 6.0 * (fA + 4.0 * fM + fB);
	}
}

double integral_f_p(double alpha, double beta, void (*task)(void *), threadpool *thpl, int thread_n, double t) {
	const int N = INTEGRAL_SIZE;
	double delta = (beta - alpha) / N;

	double *point_data = malloc(sizeof(double) * N);
	Task *tsks = malloc(sizeof(*tsks) * thread_n);

	int base_step = N / thread_n;
	int remainder = N % thread_n;
	int current_idx = 0;

	for (int i = 0; i < thread_n; ++i) {
		Task tsk = malloc(sizeof(*tsk));
		tsks[i] = tsk;
		tsk->id = i;

		int step = base_step + (i < remainder ? 1 : 0);
		tsk->start_idx = current_idx;
		tsk->end_idx = current_idx + step;
		tsk->temp_s = alpha + current_idx * delta;
		tsk->time = t;
		tsk->delta = delta;
		tsk->data = &point_data;
		tsk->thread_n = thread_n;
		tsk->N = N;

		current_idx += step;
		thpool_add_work(*thpl, task, tsk);
	}

	thpool_wait(*thpl);

	double A = 0.0;
	for (int i = 0; i < N; ++i) {
		A += point_data[i];
	}

	free(point_data);
	for (int i = 0; i < thread_n; ++i) {
		free(tsks[i]);
	}
	free(tsks);

	return A;
}

double get_score(double interval) {
	if (interval <= 1e-9) {
		return 0.0;
	}
	return SCORE_BASE / interval;
}

void *benchmark(void *data) {
	Data *sharedData = (Data *)data;
	long int thread_n = sharedData->threads;
	printf("Threads: %ld\n", thread_n);

	const double N = 32.0;
	const unsigned N_sec = 30;
	const double delta_time = 0.5;
	threadpool thpl = thpool_init(thread_n);

	int total_count = 0;
	double window[SCORE_WINDOW] = {0.0};
	int window_idx = 0;

	double start_global = get_monotonic_sec();

	for (double t = 0.0; ; t += delta_time) {
		double now = get_monotonic_sec();
		double elapsed_global = now - start_global;
		if (elapsed_global > N_sec && t >= N) {
			break;
		}

		double t_start = get_monotonic_sec();
		double S = integral_f_p(-INTEGRAL_LIMIT, INTEGRAL_LIMIT, oppai_task, &thpl, thread_n, t);
		double t_end = get_monotonic_sec();
		double interval = t_end - t_start;

		total_count++;

		window[window_idx] = interval;
		window_idx = (window_idx + 1) % SCORE_WINDOW;
		int valid = total_count < SCORE_WINDOW ? total_count : SCORE_WINDOW;
		double window_sum = 0.0;
		for (int i = 0; i < valid; ++i) {
			window_sum += window[i];
		}
		double window_mean = window_sum / valid;

		double mean_score = get_score(window_mean);

		pthread_mutex_lock(&sharedData->mutex);
		sharedData->t = t;
		sharedData->S = S;
		sharedData->Score = mean_score;
		pthread_cond_signal(&sharedData->cond);
		pthread_mutex_unlock(&sharedData->mutex);
	}

	thpool_destroy(thpl);

	pthread_mutex_lock(&sharedData->mutex);
	sharedData->benchmark_running = 0;
	pthread_cond_signal(&sharedData->cond);
	pthread_mutex_unlock(&sharedData->mutex);

	return NULL;
}
