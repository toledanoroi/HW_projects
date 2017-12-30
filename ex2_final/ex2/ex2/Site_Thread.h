/*File Name : Site_Thread.h
	Authors:			Roi Toledano	203700505
						Yarden Avraham	302249925
	Project Name:		Exercise 2 - Ping - Processes and Threads
	Using:				C built-in libraries
	Description:		Site_Thread.c header;     */
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Includes and Defines*/

#include <windows.h>
#include <stdbool.h>
#include <stdio.h>
#include "CreatePingProcess.h"

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Structs defenitions*/

typedef enum{
	SITE_THREAD__CODE_SUCCESS,
	SITE_THREAD__CODE_FAILED,
	SITE_THREAD__CODE_ERROR = -1

} SITE_THREAD__return_code;

/*------------------------------------------------------------------------------------------------------------------------------------------*/

typedef struct{

	int site_count;
	char *address;
	DWORD res;
	bool ping_successfully_passed;
	bool error_id_null;
	bool error_process_creation;
	bool error_closing_handle;
	bool error_waiting;
	bool error_thread_creation;
	bool error_alloc;

} SITE_THREAD_params;

/*------------------------------------------------------------------------------------------------------------------------------------------*/

// Functions declarations
DWORD WINAPI SITEThread(LPVOID lpParam);
HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine, SITE_THREAD_params *p_thread_parameters, DWORD *p_thread_id);

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/***********************************************************END OF FILE**********************************************************************/
/*------------------------------------------------------------------------------------------------------------------------------------------*/