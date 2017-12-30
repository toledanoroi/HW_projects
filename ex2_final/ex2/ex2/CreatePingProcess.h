/*File Name : CreatePingProcess.h
	Authors:			Roi Toledano	203700505
						Yarden Avraham	302249925
	Project Name:		Exercise 2 - Ping - Processes and Threads
	Using:				C built-in libraries
	Description:		CreatePingProcess.c header;     */
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Includes and Defines*/
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <tchar.h> /* for TCHAR, _T() */

#define TIMEOUT_IN_MILLISECONDS 20000
#define BRUTAL_TERMINATION_CODE 0x55
#define MAXSIZE 200

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Structs defenitions*/
typedef enum{
	PING_PROCESS__CODE_SUCCESS,
	PING_PROCESS__CODE_FAILED,
	PING_PROCESS__CODE_ERROR = -1

} PING_PROCESS_return_code;

/*------------------------------------------------------------------------------------------------------------------------------------------*/

typedef struct address_info{

	char *address;
	DWORD exitcode;
	bool closing_handle_status;
	bool waiting_status;
	bool allocation_status;
	bool process_creation_status;

}AddressInfo;

/*------------------------------------------------------------------------------------------------------------------------------------------*/

/*Functions declarations*/
BOOL CreateProcessSimple(LPTSTR CommandLine, PROCESS_INFORMATION *ProcessInfoPtr);
DWORD CreateProcessSimpleMain(AddressInfo *ping_params);

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/***********************************************************END OF FILE**********************************************************************/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
