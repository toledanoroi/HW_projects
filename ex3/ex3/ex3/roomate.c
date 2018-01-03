/*
File Name :		roomate.h
Authors:			Roi Toledano	203700505
					Yarden Avraham	302249925
Project Name:		Exercise 3 - Robotic Laundry Machine
Using:				roomate.h, machine.h , machine.c.
Description:		This module role is handle the roommates threads; open, define and timing  */
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Includes and Defines*/	
#include "machine.h"

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Functions */
/*HANDLE CreateThreadSimple
Parameters:		p_start_routine			- A pointer tha points to a function that notifies the host that a thread has started to execute.
				p_thread_parameters		- A pointer to a struct that contains all the parameters of the thread, including potential errors.
				p_thread_id				- A pointer to the thread's ID.
				fp_debud				- A pointer to the  debug log file.
				file_name				- A pointer to the name of the log file string.
Returns:		A HANDLE variable		- The handle of the thread that was created.
Description:	A function that calls CreateThread function, that creates a thread to execute RoommateThread WINAPI function.*/
HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE roommate_start_routine, roommate_info *roommate_parameters, DWORD *roommate_thread_id , FILE *fp_debug, char *file_name) {
	/* Parameters */
	HANDLE thread_handle;

	/* Checks if the function received a null pointer */
	if (NULL == roommate_start_routine) {
		PrintLog(fp_debug, "Error when creating a thread\nReceived null pointer\n", file_name, NULL, roommate_parameters->mutex_debug_file);
		exit(ROOMMATE_THREAD__CODE_ERROR);
	}

	thread_handle = CreateThread(
		NULL,                /*  default security attributes */
		0,                   /*  use default stack size */
		roommate_start_routine,     /*  thread function */
		roommate_parameters, /*  argument to thread function */
		0,                   /*  use default creation flags */
		roommate_thread_id);        /*  returns the thread identifier */

							 /* Checks if the thread was created properly */
	if (thread_handle == NULL) {
		roommate_parameters->error_thread_creation = true;
		PrintLog(fp_debug, "Error when creating a thread\nThread handler is NULL\n", file_name, NULL, roommate_parameters->mutex_debug_file);
		exit(ROOMMATE_THREAD__CODE_ERROR);
	}
	return thread_handle;
}

/*int TakeCleanClothesFromCloset
Parameters:		roommate_thread_params	- A pointer to the threas struct (roommate_info type).
Returns:		An int variable			- Success = 0 and Error = -1
Description:	A function that responsible to the taking a new cloth from closet operation.
				take closet mutex, update semaphores update struct parmeters and release the closet mutex*/
int TakeCleanClothesFromCloset(roommate_info *roommate_thread_params) {
	
	LONG previous_count = NULL;
	DWORD wait_time;
	wait_time = roommate_thread_params->time_thread->program_time - roommate_thread_params->time_thread->time + DELTA_TIME * 3;
	//LONG previous_count0 = 6;
	//LONG previous_count1 = 1;
	//LONG previous_count2 = 7;

	//roommate wants the closet mutex
	PrintLog(roommate_thread_params->fp_report, "Roommate %s active\n", roommate_thread_params->report_file, _itoa(roommate_thread_params->index, buffer, INT_BASE),roommate_thread_params->mutex_report_file);
	
	//PrintLog(roommate_thread_params->fp_debug, "Roommate %s wait for semaphore : closet full \n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
	roommate_thread_params->semahore_closet_full.waiting_code = WaitForSingleObject(roommate_thread_params->semahore_closet_full.handle, wait_time);
	if (roommate_thread_params->semahore_closet_full.waiting_code != WAIT_OBJECT_0) {
		WaitingStatus(roommate_thread_params->semahore_closet_full.waiting_code, roommate_thread_params->fp_debug, roommate_thread_params->log_file);
		if (roommate_thread_params->semahore_closet_full.waiting_code == WAIT_TIMEOUT) {
			// the program tome ended so we need to release the thread.
			PrintLog(roommate_thread_params->fp_debug, "Because the proccess time was finished\n", roommate_thread_params->log_file, NULL, roommate_thread_params->mutex_debug_file);
			return (SUCCESS_INDICATION);
		}
		return (ERROR_INDICATION);
	}
	//PrintLog(roommate_thread_params->fp_debug, "Roommate %s took the semaphore : closet full \n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
	//PrintLog(roommate_thread_params->fp_debug, "Roommate %s wait for mutex : closet\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
	roommate_thread_params->mutex_closet.waiting_code = WaitForSingleObject(roommate_thread_params->mutex_closet.handle, wait_time);
	if (roommate_thread_params->mutex_closet.waiting_code != WAIT_OBJECT_0) {
		WaitingStatus(roommate_thread_params->mutex_closet.waiting_code, roommate_thread_params->fp_debug, roommate_thread_params->log_file);
		if (roommate_thread_params->semahore_closet_full.waiting_code == WAIT_TIMEOUT) {
			// the program tome ended so we need to release the thread.
			PrintLog(roommate_thread_params->fp_debug, "Because the proccess time was finished\n", roommate_thread_params->log_file, NULL, roommate_thread_params->mutex_debug_file);
			return (SUCCESS_INDICATION);
		}
		return (ERROR_INDICATION);
	}
	//PrintLog(roommate_thread_params->fp_debug, "Roommate %s took the mutex : closet\nThat means:\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
	//PrintLog(roommate_thread_params->fp_debug, "Roommate %s took a new cloth\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
	//PrintLog(roommate_thread_params->fp_debug, "Roommate %s want to release the semaphore : closet empty \n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
	if (!(ReleaseSemaphore(roommate_thread_params->semahore_closet_empty.handle, 1, &previous_count))) {
		PrintLog(roommate_thread_params->fp_debug, "Error - release semaphore_closet_empty\n", roommate_thread_params->log_file, NULL, roommate_thread_params->mutex_debug_file);
		return (ERROR_INDICATION);
	}
	//PrintLog(roommate_thread_params->fp_debug, "Roommate %s released the semaphore : closet empty; released his closet\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
	(roommate_thread_params->number_of_cloth_in_closet)--;
	//PrintLog(roommate_thread_params->fp_debug, "Roommate %s want to release the mutex : closet\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
	if (!(ReleaseMutex(roommate_thread_params->mutex_closet.handle))) {
		PrintLog(roommate_thread_params->fp_debug, "Error - release mutex closet\n", roommate_thread_params->log_file, NULL, roommate_thread_params->mutex_debug_file);
		return (ERROR_INDICATION);
	}
	//PrintLog(roommate_thread_params->fp_debug, "Roommate %s released the closet_mutex \n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
	//PrintLog(roommate_thread_params->fp_debug, "Roommate %s finished TakeCleanClothesfromCloset function\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);

	return SUCCESS_INDICATION;
}

/*int SendDirtyClothesToLaundry
Parameters:		roommate_thread_params	- A pointer to the threas struct (roommate_info type).
Returns:		An int variable			- Success = 0 and Error = -1
Description:	A function that responsible to send the dirty clothes of the current roommate to the luandry bin.
				take the mutex laundry bin, update laundry bin semaphores, update the roommate parameters
				if needed send a signal to the laundry machine to start
				when finish operation realease the laundry bin mutex*/
int SendDirtyClothesToLaundry(roommate_info *roommate_thread_params) {
	DWORD wait_time;
	wait_time = roommate_thread_params->time_thread->program_time - roommate_thread_params->time_thread->time + DELTA_TIME * 3;
	//PrintLog(roommate_thread_params->fp_debug, "Roommate %s started SendDirtyClothesToLaundry Function\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
	//PrintLog(roommate_thread_params->fp_debug, "Roommate %s wait for laundry_bin mutex\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);

	roommate_thread_params->mutex_laundry_bin.waiting_code = WaitForSingleObject(roommate_thread_params->mutex_laundry_bin.handle, wait_time);
	if (roommate_thread_params->mutex_laundry_bin.waiting_code != WAIT_OBJECT_0) {
		WaitingStatus(roommate_thread_params->mutex_laundry_bin.waiting_code, roommate_thread_params->fp_debug, roommate_thread_params->log_file);
		if (roommate_thread_params->semahore_closet_full.waiting_code == WAIT_TIMEOUT) {
			PrintLog(roommate_thread_params->fp_debug, "Because the proccess time was finished\n", roommate_thread_params->log_file, NULL, roommate_thread_params->mutex_debug_file);
			// the program tome ended so we need to release the thread.
			return (SUCCESS_INDICATION);
		}
		return (ERROR_INDICATION);
	}
	//PrintLog(roommate_thread_params->fp_debug, "Roommate %s took the laundry_bin mutex\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
	if (machine_is_on == false) {
		//PrintLog(roommate_thread_params->fp_debug, "Roommate %s wait for laundry bin full semaphore\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
		roommate_thread_params->semaphore_laundry_bin_full.waiting_code = WaitForSingleObject(roommate_thread_params->semaphore_laundry_bin_full.handle, wait_time);
		if (roommate_thread_params->semaphore_laundry_bin_full.waiting_code != WAIT_OBJECT_0) {
			WaitingStatus(roommate_thread_params->semaphore_laundry_bin_full.waiting_code, roommate_thread_params->fp_debug, roommate_thread_params->log_file);
			///
			if (roommate_thread_params->semahore_closet_full.waiting_code == WAIT_TIMEOUT) {
				// the program tome ended so we need to release the thread.
				PrintLog(roommate_thread_params->fp_debug, "Because the proccess time was finished\n", roommate_thread_params->log_file, NULL, roommate_thread_params->mutex_debug_file);
				return (SUCCESS_INDICATION);
			}

			return (ERROR_INDICATION);
		}
		//PrintLog(roommate_thread_params->fp_debug, "Roommate %s took the laundry bin full semaphore\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
		//PrintLog(roommate_thread_params->fp_debug, "Roommate %s throw his cloth to the laundry\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
		(roommate_thread_params->number_of_cloth_in_laundry)++;
		total_clothes_in_laundry_bin++;
		if (total_clothes_in_laundry_bin == (roommate_thread_params->max_in_bin)) {
			//PrintLog(roommate_thread_params->fp_debug, "The laundry bin is full; Roommate %s want to release the laundry bin mutex\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
			if (!(ReleaseMutex(roommate_thread_params->mutex_laundry_bin.handle))) {
				PrintLog(roommate_thread_params->fp_debug, "Error - release mutex laundry bin", roommate_thread_params->log_file, NULL, roommate_thread_params->mutex_debug_file);
				return (ERROR_INDICATION);
			}
			//PrintLog(roommate_thread_params->fp_debug, "Roommate %s released the laundry bin mutex for the machine\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
			machine_is_on = true;
			total_clothes_in_laundry_bin = 0;
			return SUCCESS_INDICATION;
		}
		
	}
	//PrintLog(roommate_thread_params->fp_debug, "Roommate %s want to release the laundry bin mutex\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
	if (!(ReleaseMutex(roommate_thread_params->mutex_laundry_bin.handle))) {
		PrintLog(roommate_thread_params->fp_debug, "Error - release mutex laundry bin", roommate_thread_params->log_file, NULL, roommate_thread_params->mutex_debug_file);
		return (ERROR_INDICATION);
	}
	//PrintLog(roommate_thread_params->fp_debug, "Roommate %s released the laundry bin mutex\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
	//PrintLog(roommate_thread_params->fp_debug, "Roommate %s finished the SendDirtyClothesToLaundry Function\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
	//PrintLog(roommate_thread_params->fp_debug, "Roommate %s finished\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
	return SUCCESS_INDICATION;
}


/*DWORD WINAPI RoommateThread
Parameters:		lpParam				- LPVOID type, pointer to the thread's parameters, can be any pointer we want.
Returns:		A DWORD variable	- ROOMMATE_THREAD__CODE_SUCCESS = 0 , ROOMMATE_THREAD__CODE_ERROR = -1, ROOMMATE_THREAD__CODE_FAILED = 1.
Description:	The main function of each roommate thread.*/
DWORD WINAPI RoommateThread(LPVOID lpParam) {
	/* Parameters */
	roommate_info *roommate_thread_params = NULL;
	long prev = NULL;


	/* Casting lpParam to be type SITE_THREAD_params, a struct we defined and its value is in thread_params */
	roommate_thread_params = (roommate_info *)lpParam;

	/* Check if lpParam is NULL */
	if (NULL == roommate_thread_params)
	{
		return ROOMMATE_THREAD__CODE_FAILED;
	}

	while (roommate_thread_params->time_thread->time_flag) {
		//PrintLog(roommate_thread_params->fp_debug, "Roommate %s going to sleep\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
		Sleep(roommate_thread_params->time_for_clothes_change);
		//PrintLog(roommate_thread_params->fp_debug, "Roommate %s started\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
		//PrintLog(roommate_thread_params->fp_debug, "Roommate %s is awake\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
		// if closet full
		if (TakeCleanClothesFromCloset(roommate_thread_params)) {
			return ROOMMATE_THREAD__CODE_ERROR;
		}
		
		// if laundry bin is not full
		if (SendDirtyClothesToLaundry(roommate_thread_params)) {
			return ROOMMATE_THREAD__CODE_ERROR;
		}
	} 
	PrintLog(roommate_thread_params->fp_debug, "Roommate %s Finished The WINAPI function\nNow he need to return a value and finish the thread\n", roommate_thread_params->log_file, _itoa(roommate_thread_params->index, buffer, INT_BASE), roommate_thread_params->mutex_debug_file);
	return(ROOMMATE_THREAD__CODE_SUCCESS);
	//ExitThread(1);

}
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/***********************************************************END OF FILE**********************************************************************/
/*------------------------------------------------------------------------------------------------------------------------------------------*/