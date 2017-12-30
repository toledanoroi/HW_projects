/*
File Name :			timingthread.c
Authors:			Roi Toledano	203700505
					Yarden Avraham	302249925
Project Name:		Exercise 3 - Robotic Laundry Machine
Using:				roomate.h, machine.h , machine.c,roomate.c, timingthread.h, motherfile.c, motherfile.h .
Description:		This module role is handle the timing thread that count the program time and close all the threads when it finished; open, define and timing  */
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Includes and Defines*/
#include "machine.h"

/*------------------------------------------------------------------------------------------------------------------------------------------*/

HANDLE CreateTimingThreadSimple(LPTHREAD_START_ROUTINE timing_start_routine, timing_info *timing_parameters, DWORD *timing_thread_id, FILE *fp_debug, char *file_name) {
	/* Parameters */
	HANDLE thread_handle;

	/* Checks if the function received a null pointer */
	if (NULL == timing_start_routine) {
		PrintLog(fp_debug, "Error when creating a thread\nReceived null pointer\n", file_name, NULL);
		exit(TIMING_THREAD__CODE_ERROR);
	}

	thread_handle = CreateThread(
		NULL,                /*  default security attributes */
		0,                   /*  use default stack size */
		timing_start_routine,     /*  thread function */
		timing_parameters, /*  argument to thread function */
		0,                   /*  use default creation flags */
		timing_thread_id);        /*  returns the thread identifier */

									/* Checks if the thread was created properly */
	if (thread_handle == NULL) {
		timing_parameters->error_thread_creation = true;
		PrintLog(fp_debug, "Error when creating a thread\nThread handler is NULL\n", file_name, NULL);
		exit(TIMING_THREAD__CODE_ERROR);
	}
	return thread_handle;
}



DWORD WINAPI TimeCounterThread(LPVOID lpParam) {
	/* Parameters */
	timing_info *time_thread_params = NULL;

	/* Casting lpParam to be type SITE_THREAD_params, a struct we defined and its value is in thread_params */
	time_thread_params = (timing_info *)lpParam;

	/* Check if lpParam is NULL */
	if (NULL == time_thread_params)
	{
		return TIMING_THREAD__CODE_FAILED;
	}
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("Hi, I'm the time counter\n");
	int start_time = GetTickCount();

	while(time_thread_params->time_flag) {
		time_thread_params->mutex_time.waiting_code = WaitForSingleObject(time_thread_params->mutex_time.handle, INFINITE);
		if (time_thread_params->mutex_time.waiting_code != WAIT_OBJECT_0) {
			WaitingStatus(time_thread_params->mutex_time.waiting_code, time_thread_params->fp_debug, time_thread_params->log_file);
			return (ERROR_INDICATION);
		}
		time_thread_params->time = (GetTickCount() - start_time);
		printf("the time is : %ld\n", time_thread_params->time);


		if (time_thread_params->time >= time_thread_params->program_time) {
			time_thread_params->time_flag = false;
			printf("the program time passed: %ld;\n When The max time is %ld\n", time_thread_params->time, time_thread_params->program_time);
		}
		if (!(ReleaseMutex(time_thread_params->mutex_time.handle))) {
			PrintLog(time_thread_params->fp_debug, "Error - release mutex closet\n", time_thread_params->log_file, NULL);
			return (ERROR_INDICATION);
		}
		Sleep(1000);
	}
	return TIMING_THREAD__CODE_SUCCESS;
}
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/***********************************************************END OF FILE**********************************************************************/
/*------------------------------------------------------------------------------------------------------------------------------------------*/