/*File Name : CreatePingProcess.c
	Authors:			Roi Toledano	203700505
						Yarden Avraham	302249925
	Project Name:		Exercise 2 - Ping - Processes and Threads
	Using:				CreatePingProcess.h
	Description:		Ping Process Module creates a new process with a ping command (one of the built in commands in windows)      */

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Includes and Defines*/
#include "CreatePingProcess.h"

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*DWORD CreateProcessSimpleMain
	Parameters:		ping_params			- A pointer to a struct that contains all the parameters that the process needs for make a ping.
	Returns:		A DWORD variable	- PING_PROCESS__CODE_SUCCESS = 0 , PING_PROCESS__CODE_FAILED = 1, PING_PROCESS__CODE_ERROR = -1.
	Description:	The main of this mudule: Gets parameters, creates process, checks exit codes, close handles and update values.*/
DWORD CreateProcessSimpleMain(AddressInfo *ping_params)
{
	//Parameters
	int closed_thread = -1;
	int closed_process = -1;
	PROCESS_INFORMATION procinfo;
	DWORD				waitcode;
	DWORD				exitcode;
	BOOL				retVal;
	TCHAR				command[MAXSIZE] = _T("ping -n 1 ");	/* <ISP> TCHAR is a win32  */
																/* generic char which may be either a simple (ANSI) char or a unicode char, */
																/* depending on behind-the-scenes operating system definitions. Type LPTSTR */
																/* is a string of TCHARs. Type LPCTSTR is a const string of TCHARs. */
																/*  Start the child process. */

	// attach the address to the ping command
	strcat_s(command, 50, ping_params->address);
	
	// Create a Ping Process 
	retVal = CreateProcessSimple(command, &procinfo);

	//Checks if failed
	if (retVal == 0)
	{
		ping_params->process_creation_status = true;
		printf("Process Creation Failed!\n");
		return PING_PROCESS__CODE_ERROR;
	}

	// Wait the process will finish 
	waitcode = WaitForSingleObject(procinfo.hProcess, TIMEOUT_IN_MILLISECONDS); /* Waiting 20 secs for the process to end */

	// Checks the waitcode
	switch (waitcode)
	{
	case WAIT_TIMEOUT:								// Error
		ping_params->waiting_status = true;
		return PING_PROCESS__CODE_ERROR;
	case WAIT_OBJECT_0:								// The process finishes correctly 
		break;
	default:
		break;
	}

	/* If the Process is still alive - kill the project */
	if (waitcode == WAIT_TIMEOUT) 
	{
		printf("Process was not terminated before timeout!\nTerminating brutally!\n"); //NEED TO DELETE
		TerminateProcess(procinfo.hProcess, BRUTAL_TERMINATION_CODE); /* Terminating process with an exit code of 55h */
		Sleep(10);  /* Waiting a few milliseconds for the process to terminate */
	}

	GetExitCodeProcess(procinfo.hProcess, &exitcode);	
	// close handlers 
	closed_process = CloseHandle(procinfo.hProcess);/* Closing the handle to the process */
	closed_thread = CloseHandle(procinfo.hThread); /* Closing the handle to the main thread of the process */
	
	//checks if the handler closing failed 
	if (closed_process == 0 || closed_thread == 0) {
		ping_params->closing_handle_status = true;
		return PING_PROCESS__CODE_ERROR;
	}

	// checks for problems in exitcode
	if (exitcode == 0) {
		ping_params->exitcode = 0;
		return PING_PROCESS__CODE_SUCCESS;
	}
	else {
		return PING_PROCESS__CODE_FAILED;
	}
}

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*BOOL CreateProcessSimple
	Parameters:		CommandLine			- the command that the process will do.
					ProcessInfoPtr		- A pointer to a struct that contains all the process parameters ; for update values.
	Returns:		A BOOL variable		- Success = 0 , Fail = 1.
	Description:	A simple function that calls CreateProcess function.*/
BOOL CreateProcessSimple(LPTSTR CommandLine, PROCESS_INFORMATION *ProcessInfoPtr)
{

	STARTUPINFO	startinfo = { sizeof(STARTUPINFO), NULL, 0 }; /* <ISP> here we */
															  /* initialize a "Neutral" STARTUPINFO variable. Supplying this to */
															  /* CreateProcess() means we have no special interest in this parameter. */
															  /* This is equivalent to what we are doing by supplying NULL to most other */
															  /* parameters of CreateProcess(). */

	return CreateProcess(
		NULL,					/*  No module name (use command line). */
		CommandLine,			/*  Command line. */
		NULL,					/*  Process handle not inheritable. */
		NULL,					/*  Thread handle not inheritable. */
		FALSE,					/*  Set handle inheritance to FALSE. */
		CREATE_NO_WINDOW,		/*  creation/priority flags. */
		NULL,					/*  Use parent's environment block. */
		NULL,					/*  Use parent's starting directory. */
		&startinfo,				/*  Pointer to STARTUPINFO structure. */
		ProcessInfoPtr			/*  Pointer to PROCESS_INFORMATION structure. */
	);
}

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/***********************************************************END OF FILE**********************************************************************/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
