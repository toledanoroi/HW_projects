/*File Name : main.c
	Authors:			Roi Toledano	203700505
						Yarden Avraham	302249925 
	Project Name:		Exercise 2 - Ping - Processes and Threads
	Using:				Site_Thread.c,Site_Thread.h
	Description:		This is the main module of the project. Manage the all process.
						read addresses and open for each address a Thread for all the ping checks.  */

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Includes and Defines*/
#include "Site_Thread.h"
#define ERROR_CODE ((int)(-1))
#define SUCCESS_CODE ((int)(0))

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*void ThreadsValuesChecks
	Parameters:		ptr_thread_params	- A pointer to a struct that contains the thread parameters.
					size				- Number of threads.
	Returns:		No returns			- a void function.
	Description:	The function checks all the Error flags ans the ping status for each thread.
					If there is an Error - the function prints a relevant note to the cmd
					Moreover, the function prints to cmd if the address is reachable or not.*/
void ThreadsValuesChecks(SITE_THREAD_params *ptr_thread_params, int size) {

	for (int jj = 0; jj < size; jj++) {
		printf("%s", ptr_thread_params[jj].address);
		if (ptr_thread_params[jj].error_waiting)			printf("Waiting for thread - ERROR\n");
		if (ptr_thread_params[jj].error_closing_handle)		printf("Closing handle - ERROR\n");
		if (ptr_thread_params[jj].error_process_creation)	printf("Process creation - ERROR\n");
		if (ptr_thread_params[jj].error_id_null)			printf("Thread ID is NULL - ERROR\n");
		if (ptr_thread_params[jj].error_thread_creation)	printf("Thread creation - ERROR\n");
		if (ptr_thread_params[jj].error_alloc)				printf("Allocation - ERROR\n");
		
		if (ptr_thread_params[jj].ping_successfully_passed)	printf(" reachable\n");
		else												printf(" unreachable\n");
		
	}
	
}

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*int main
	Parameters:		argc	- number of strings ("commands") in cmd.
					argv[]	- A pointer to the strings array from cmd.
	Returns:		Success/Fail - An integer : 0 = success (-1) = Fail
	Description:	The Main of the project. Gets an array of adresses an opens for each address a thread that handles Pings.
					waits for threads to finish and prints the results.*/
int main(int argc, char *argv[]) {
	//parameters
	int jj;
	HANDLE *ptr_thread_handle = NULL;
	DWORD *ptr_thread_id = NULL;
	DWORD *ptr_exitcode = NULL;
	SITE_THREAD_params *ptr_thread_params = NULL;
	DWORD *ptr_wait_code = NULL;
	DWORD wait_code_multiple;

	// allocations for the array
	ptr_thread_handle = (HANDLE*)malloc((argc - 1) * sizeof(HANDLE));
	ptr_thread_id = (DWORD*)malloc((argc - 1) * sizeof(DWORD));
	ptr_exitcode = (DWORD*)malloc((argc - 1) * sizeof(DWORD));
	ptr_wait_code = (DWORD*)malloc((argc - 1) * sizeof(DWORD));
	ptr_thread_params = (SITE_THREAD_params*)malloc((argc - 1) * sizeof(SITE_THREAD_params));

	// checks if one of the allocations failed.
	if (ptr_thread_handle == NULL || ptr_thread_id == NULL || ptr_exitcode == NULL || ptr_thread_params == NULL) {
		printf("Allocating memmory failed\n");
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		for (int jj = 0; jj < argc - 1; jj++) {
			ptr_thread_params[jj].error_alloc = true;
		}
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		return ERROR_CODE;
	}
	//initiate threads parameters
	for (int jj = 0; jj < argc - 1; jj++) {
		ptr_thread_params[jj].address = argv[jj + 1];
		ptr_thread_params[jj].error_closing_handle = false;
		ptr_thread_params[jj].ping_successfully_passed = false;
		ptr_thread_params[jj].error_waiting = false;
		ptr_thread_params[jj].error_process_creation = false;
		ptr_thread_params[jj].error_alloc = false;
		ptr_thread_params[jj].error_thread_creation = false;
		ptr_thread_params[jj].error_id_null = false;
	}

	// check if the input from command line is valid
	if (argc < 2) {
		printf("Not enough addresses entered - Exit program\n");
		return ERROR_CODE;
	}
	// Create a thread for each address
	for (jj = 0; jj < argc - 1; jj++) {
		ptr_thread_handle[jj] = CreateThreadSimple(SITEThread, &(ptr_thread_params[jj]), &(ptr_thread_id[jj]));
	}
	
	//For parallel functionality - wait that all the threads finish 
	wait_code_multiple = WaitForMultipleObjects(argc - 1, ptr_thread_handle, TRUE, INFINITE);
	
	//Checks if one of the threads failed.
	for (jj = 0; jj < argc - 1; jj++) {
		if (GetExitCodeThread(ptr_thread_handle[jj], (&(ptr_exitcode[jj]))) == 0) {
			printf("Error when getting thread number %d with address %s exit code\n", jj, ptr_thread_params[jj].address);
			return ERROR_CODE;
		}
	}
	
	//Checking and Printing results
	ThreadsValuesChecks(ptr_thread_params, argc - 1);

	//Closing all Thread handles
	for (jj = 0; jj < argc - 1; jj++) {
		if (CloseHandle(ptr_thread_handle[jj]) == 0) {
			printf("ERROR while closing Thread number %d with address %s\n", jj, ptr_thread_params[jj].address);
			return ERROR_CODE;
		}
	}

	//Freeing all allocated memory
	free(ptr_thread_handle);
	free(ptr_thread_id);
	free(ptr_wait_code);
	free(ptr_thread_params);
	free(ptr_exitcode);
	
	return SUCCESS_CODE;
}

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/***********************************************************END OF FILE**********************************************************************/
/*------------------------------------------------------------------------------------------------------------------------------------------*/