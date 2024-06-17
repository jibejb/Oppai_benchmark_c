#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include "benchmark.h"
#include "cpu_core_count.h"
#include "thpool.h"
#include "linkedlist.h"



#define INTEGRAL_LIMIT 1000
#define INTEGRAL_SIZE 1000000

double Oppai_func(double y, double t) {
	y = 0.02 * (y - 100);

	double a1 = (1.5 * exp((0.12*sin(t)-0.5)*pow((y+0.16*sin(t)), 2))) / (1 + exp(-20*(5*y+sin(t))));
	double a2 = ((1.5 + 0.8*pow((y+0.2*sin(t)), 3)) * pow(1+exp(20*(5*y+sin(t))), -1)) / (1 + exp(-(100*(y+1) + 16*sin(t))));
	double a3 = (0.2 * (exp(-pow(y+1, 2)) + 1)) / (1 + exp(100*(y+1)+16*sin(t)));
	double a4 = 0.1 / exp(2*pow((10*y+1.2*(2+sin(t))*sin(t)), 4));

	return 65 * (a1 + a2 + a3 + a4);

}

void oppai_task(void * data)
{
	Task tsk = (Task) data;
	int j = 0;
	int id = tsk->id;
	double temp_s = tsk->temp_s;
	double temp_e = tsk->temp_e;
	double delta = tsk->delta;
	double t = tsk->time;
	double *dt = *tsk->data;
	int thread_n = tsk->thread_n;
	int N = tsk->N;
	for (double jd = temp_s; jd < temp_e; jd += delta) {
			double fA = Oppai_func(jd, t);
			double fB = Oppai_func(jd + delta, t);
			double fM = Oppai_func((jd + jd + delta) / 2, t);
			dt[j + id * (N / thread_n)] = delta / 6 * (fA + 4 * fM + fB);
			++j;

	}
}

double integral_f_p(double alpha, double beta, void (*task)(void *), threadpool *thpl, int thread_n, int t) {

	int N = INTEGRAL_SIZE;
	Task tsks[thread_n];
	double delta = (beta - alpha) / N;
	double seg = (beta - alpha) / thread_n;
	double *point_data = malloc(sizeof(double) * (N + 1));

	for (int i=0; i < thread_n; ++i) {
		Task tsk = malloc(sizeof(*tsk));
		tsks[i] = tsk;
		tsk->id = i;
		tsk->temp_s = seg * i;
		tsk->temp_e = seg * (i + 1);
		tsk->time = t;
		tsk->delta = delta;
		tsk->data = &point_data;
		tsk->thread_n = thread_n;
		tsk->N = N;
		thpool_add_work(*thpl,task,tsk);
	}
	thpool_wait(*thpl);
	double A = 0.0;
	for (int i = 0; i < N+1; ++i)
	{
		A += point_data[i];
	}
	free(point_data);
		for (int i = 0; i < thread_n; ++i)
	{
		free(tsks[i]);
	}
	return A;
}

double get_score(double interval) {
	return 1.0 / ((double)interval * 1000.0) * 1000000;
//	return (double)interval;
}

void *benchmark(void *data)
{
	Data *sharedData = (Data*)data;
	long int thread_n = cpu_core();
	double const N = 32.0;
	const unsigned N_sec = 30;
	const double delta_time = 0.5;
	threadpool thpl = thpool_init(thread_n);
	time_t start_time1 = time(NULL);
	for (double t = 0.0; (time(NULL) - start_time1) <= N_sec || t < N; t+=delta_time) {
		time_t start_time2 = time(NULL);
		double S = integral_f_p(-INTEGRAL_LIMIT, INTEGRAL_LIMIT,oppai_task, &thpl, thread_n, t);
		time_t end_time2 = time(NULL);
		link score = make_node((double)(end_time2 - start_time2));
		push_node(score);
		double mean_score = get_score(mean_node());

		pthread_mutex_lock(&sharedData->mutex);
		sharedData->t = t;
		sharedData->S = S;
		sharedData->Score = mean_score;
		pthread_cond_signal(&sharedData->cond);
		pthread_mutex_unlock(&sharedData->mutex);
	}
	thpool_destroy(thpl);
	destroy_node();
	pthread_mutex_lock(&sharedData->mutex);
	sharedData->benchmark_running = 0;
	pthread_cond_signal(&sharedData->cond);
	pthread_mutex_unlock(&sharedData->mutex);
	return NULL;
}

