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
/*HANDLE CreateTimingThreadSimple
Parameters:		p_start_routine			- A pointer tha points to a function that notifies the host that a thread has started to execute.
				p_thread_parameters		- A pointer to a struct that contains all the parameters of the thread, including potential errors.
				p_thread_id				- A pointer to the thread's ID.
				fp_debud				- A pointer to the  debug log file.
				file_name				- A pointer to the name of the log file string.
Returns:		A HANDLE variable		- The handle of the thread that was created.
Description:	A function that calls CreateThread function, that creates a thread to execute TimeCounterThread WINAPI function.*/
HANDLE CreateTimingThreadSimple(LPTHREAD_START_ROUTINE timing_start_routine, timing_info *timing_parameters, DWORD *timing_thread_id, FILE *fp_debug, char *file_name) {
	/* Parameters */
	HANDLE thread_handle;

	/* Checks if the function received a null pointer */
	if (NULL == timing_start_routine) {
		PrintLog(fp_debug, "Error when creating a thread\nReceived null pointer\n", file_name, NULL, timing_parameters->mutex_debug_file);
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
		PrintLog(fp_debug, "Error when creating a thread\nThread handler is NULL\n", file_name, NULL, timing_parameters->mutex_debug_file);
		exit(TIMING_THREAD__CODE_ERROR);
	}
	return thread_handle;
}


/*DWORD WINAPI TimeCounterThread
Parameters:		lpParam				- LPVOID type, pointer to the thread's parameters, can be any pointer we want.
Returns:		A DWORD variable	- TIMING_THREAD__CODE_SUCCESS = 0 , TIMING_THREAD__CODE_ERROR = -1, TIMING_THREAD__CODE_FAILED=1.
Description:	The main function of the robot thread.*/
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
	int start_time = GetTickCount();
	PrintLog(time_thread_params->fp_report, "Simulation Started\n", time_thread_params->report_file, NULL, time_thread_params->mutex_report_file);
	while(time_thread_params->time_flag) {
		time_thread_params->mutex_time.waiting_code = WaitForSingleObject(time_thread_params->mutex_time.handle, INFINITE);
		if (time_thread_params->mutex_time.waiting_code != WAIT_OBJECT_0) {
			WaitingStatus(time_thread_params->mutex_time.waiting_code, time_thread_params->fp_debug, time_thread_params->log_file);
			return (TIMING_THREAD__CODE_ERROR);
		}
		time_thread_params->time = (GetTickCount() - start_time);
		//printf("the time is : %ld\n", time_thread_params->time);


		if (time_thread_params->time >= time_thread_params->program_time) {
			time_thread_params->time_flag = false;
			PrintLog(time_thread_params->fp_debug, "The program time passed: %s;\n", time_thread_params->log_file, _itoa(time_thread_params->time, buffer, INT_BASE),time_thread_params->mutex_debug_file);
			PrintLog(time_thread_params->fp_debug, "When The max time is %s \n", time_thread_params->log_file, _itoa(time_thread_params->program_time, buffer, INT_BASE), time_thread_params->mutex_debug_file);
		}
		if (!(ReleaseMutex(time_thread_params->mutex_time.handle))) {
			PrintLog(time_thread_params->fp_debug, "Error - release mutex closet\n", time_thread_params->log_file, NULL, time_thread_params->mutex_debug_file);
			return (TIMING_THREAD__CODE_ERROR);
		}
		Sleep(DELTA_TIME);
	}
	PrintLog(time_thread_params->fp_debug, "Timing thread finished his job\n", time_thread_params->log_file, NULL, time_thread_params->mutex_debug_file);
	return TIMING_THREAD__CODE_SUCCESS;
}
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/***********************************************************END OF FILE**********************************************************************/
/*------------------------------------------------------------------------------------------------------------------------------------------*/