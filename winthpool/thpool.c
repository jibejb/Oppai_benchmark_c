#include "thpool.h"
#include <string.h>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

static VOID CALLBACK WorkCallbackWraper(
	PTP_CALLBACK_INSTANCE Instance,
	PVOID                 Parameter,
	PTP_WORK              Work
)
{

	work_args* wargs = (work_args*)Parameter;
	wargs->function_p(wargs->arg_p);
	// Instance, Parameter, and Work not used in this example.
	UNREFERENCED_PARAMETER(Instance);
//	UNREFERENCED_PARAMETER(Parameter);
	UNREFERENCED_PARAMETER(Work);

/*

	if (!SetEvent(wargs->worker_ev))
	{
		printf("SetEvent failed (%d)\n", GetLastError());
		free(wargs);
		return;
	}
*/
	free(wargs);

	return;
}

threadpool thpool_init(int num_threads) {
	thpool_* thpl = malloc(sizeof(*thpl));
	if (NULL == thpl) {
		perror("alloc thpool mem flase");
		return NULL;
	}

	PTP_POOL win_thpool = CreateThreadpool(NULL);
	if (NULL == win_thpool) {
		fprintf(stderr, "CreateThreadpool failed. LastError: %u\n", GetLastError());
		return NULL;
	}

	SetThreadpoolThreadMaximum(win_thpool, num_threads);
	if (FALSE == SetThreadpoolThreadMinimum(win_thpool, num_threads)) {
		fprintf(stderr, "SetThreadpoolThreadMinimum failed. LastError: %u\n", GetLastError());
		return NULL;
	}

	TP_CALLBACK_ENVIRON CallBackEnviron;
	InitializeThreadpoolEnvironment(&CallBackEnviron);
	PTP_CLEANUP_GROUP cleanupgroup = CreateThreadpoolCleanupGroup();

	if (NULL == cleanupgroup) {
		fprintf(stderr, "CreateThreadpoolCleanupGroup failed. LastError: %u\n",
			GetLastError());
		return NULL;
	}

	SetThreadpoolCallbackPool(&CallBackEnviron, win_thpool);
	SetThreadpoolCallbackCleanupGroup(&CallBackEnviron, cleanupgroup, NULL);

	thpl->cleanup_group = cleanupgroup;
	thpl->win_thpool = win_thpool;
	thpl->thread_n = num_threads;
	thpl->var = 0;

/*
	HANDLE *worker_events = malloc(sizeof(HANDLE) * num_threads);
	if (NULL == worker_events) {
		perror("alloc events mem failed");
		return NULL;
	}
	memset(worker_events, 0, sizeof(HANDLE) * num_threads);
	thpl->work_events = worker_events;
*/
	PTP_WORK *workitems = malloc(sizeof(PTP_WORK) * num_threads);
	if (NULL == workitems) {
		perror("alloc workitems mem failed");
		return NULL;
	}
	memset(workitems, 0, sizeof(PTP_WORK) * num_threads);
	thpl->workitems = workitems;


	return thpl;
}

int thpool_add_work(threadpool thpl, void (*function_p)(void*), void* arg_p) {
//	HANDLE worker_ev;

	// Create a manual-reset event object. The write thread sets this
// object to the signaled state when it finishes writing to a 
// shared buffer. 
/*
	worker_ev = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		TEXT("workerEvent")  // object name
	);

	if (worker_ev == NULL)
	{
		printf("CreateEvent failed (%d)\n", GetLastError());
		return -1;
	}
*/
	if (!(thpl->var < thpl->thread_n)) {
		perror("Thread pool full!");
		return -1;
	}
	work_args *wargs = malloc(sizeof(*wargs));
	if (NULL == wargs) {
		perror("thpool add wargs alloc failed\n");
		return -1;
	}
//	wargs->worker_ev = worker_ev;
	wargs->function_p = function_p;
	wargs->arg_p = arg_p;


	PTP_WORK workitem = CreateThreadpoolWork(WorkCallbackWraper, wargs, NULL);
	if (workitem == NULL) {
		fprintf(stderr, "CreateThreadpoolWork Failed!: %u\n", GetLastError());

		return -1;
	}
	SubmitThreadpoolWork(workitem);
	
	(thpl->workitems)[thpl->var] = workitem;
//	(thpl->work_events)[thpl->var] = worker_ev;

		
	
	++(thpl->var);
//	printf("work added thread_n %d, var %d\n", thpl->thread_n, thpl->var);
	return 0;
}

void thpool_wait(threadpool thpl) {

	for (int i = 0; i < thpl->thread_n; ++i)
	{
		
		WaitForThreadpoolWorkCallbacks((thpl->workitems)[i], 0);
/*
		DWORD dwWaitResult;

		dwWaitResult = WaitForSingleObject(
			(thpl->work_events)[i], // event handle
			INFINITE);    // indefinite wait

		switch (dwWaitResult)
		{
			// Event object was signaled
		case WAIT_OBJECT_0:
			//
			// work complete.
			//
			break;

			// An error occurred
		default:
			printf("Wait error (%d)\n", GetLastError());
			exit(23);
		}
*/
		CloseThreadpoolWork((thpl->workitems)[i]);
		
	}
	thpl->var = 0;
}

void thpool_pause(threadpool thpl) {
	fprintf(stderr, "thpool_pause to be impl!");
}

void thpool_resume(threadpool thpl) {
	fprintf(stderr, "thpool_resume to be impl!");
}

void thpool_destroy(threadpool thpl) {
	CloseThreadpoolCleanupGroup(thpl->cleanup_group);
	free(thpl);
}

int thpool_num_threads_working(threadpool thpl) {
	return thpl->var;
}