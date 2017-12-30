/*File Name : Site_Thread.c
	Authors:			Roi Toledano	203700505
						Yarden Avraham	302249925
	Project Name:		Exercise 2 - Ping - Processes and Threads
	Using:				C built-in libraries
	Description:		Site Thread Module creates a new thread for each address we wish to check its ping     */

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Includes and Defines*/
#include <windows.h>
#include "Site_Thread.h"

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*HANDLE CreateThreadSimple
	Parameters:		p_start_routine			- A pointer tha points to a function that notifies the host that a thread has started to execute.
					p_thread_parameters		- A pointer to a struct that contains all the parameters of the thread, including potential errors.
					p_thread_id				- A pointer to the thread's ID.
	Returns:		A HANDLE variable		- The handle of the thread that was created.
	Description:	A function that calls CreateThread function, that creates a thread to execute within the virtual address space of the 
					calling process, and checks if the thread was created properly.*/
HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine, SITE_THREAD_params *p_thread_parameters, DWORD *p_thread_id){
	
	/* Parameters */
	HANDLE thread_handle;

	/* Checks if the function received a null pointer */
	if (NULL == p_start_routine){
		printf("Error when creating a thread\n");
		printf("Received null pointer\n");
		exit(SITE_THREAD__CODE_ERROR);
	}

	thread_handle = CreateThread(
		NULL,                /*  default security attributes */
		0,                   /*  use default stack size */
		p_start_routine,     /*  thread function */
		p_thread_parameters, /*  argument to thread function */
		0,                   /*  use default creation flags */
		p_thread_id);        /*  returns the thread identifier */

	/* Checks if the thread was created properly */
	if (thread_handle == NULL) {
		p_thread_parameters->error_thread_creation = true;
		printf("Error when creating a thread\n");
		printf("Thread handler is NULL\n");
		exit(SITE_THREAD__CODE_ERROR);
	}
	return thread_handle;
}
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*DHANDLE SITEThread
	Parameters:		lpParam				- LPVOID type, pointer to the thread's parameters, can be any pointer we want.
	Returns:		A DWORD variable	- SITE_THREAD__CODE_SUCCESS = 0 , SITE_THREAD__CODE_FAILED = 1.
	Description:	A function that creates a new thread for a process.*/
DWORD WINAPI SITEThread(LPVOID lpParam)
{
	/* Parameters */
	SITE_THREAD_params *thread_params = NULL;
	AddressInfo ping_params;
	int ping_flag = 0;

	/* Check if lpParam is NULL */
	if (NULL == lpParam)
	{
		return SITE_THREAD__CODE_FAILED;
	}
	
	/* Casting lpParam to be type SITE_THREAD_params, a struct we defined and its value is in thread_params */
	thread_params = (SITE_THREAD_params *)lpParam;

	/*Initialazing paramater of AddressInfo*/
	ping_params.address = thread_params->address;
	ping_params.exitcode = 1;
	ping_params.allocation_status = false;
	ping_params.closing_handle_status = false;
	ping_params.process_creation_status = false;
	ping_params.waiting_status = false;

	/* First try of ping */
	thread_params->res = CreateProcessSimpleMain(&ping_params);

	/* Checks the ping again if failed the first time */
	if ((thread_params->res) != 0) {
		if (!ping_flag) {
			ping_flag = 1;
			thread_params->res = CreateProcessSimpleMain(&ping_params);
		}
	}
	
	/*Updates the Thread's parameters*/
	if (ping_params.exitcode == 0) {
		thread_params->ping_successfully_passed = true;
	}
	else {
		thread_params->ping_successfully_passed = false;
	}
	thread_params->error_closing_handle = ping_params.closing_handle_status;
	thread_params->error_waiting = ping_params.waiting_status;
	thread_params->error_process_creation = ping_params.process_creation_status;
	return SITE_THREAD__CODE_SUCCESS;
}

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/***********************************************************END OF FILE**********************************************************************/
/*------------------------------------------------------------------------------------------------------------------------------------------*/