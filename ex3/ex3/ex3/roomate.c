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

HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE roommate_start_routine, roommate_info *roommate_parameters, DWORD *roommate_thread_id , FILE *fp_debug, char *file_name) {
	/* Parameters */
	HANDLE thread_handle;

	/* Checks if the function received a null pointer */
	if (NULL == roommate_start_routine) {
		PrintLog(fp_debug, "Error when creating a thread\nReceived null pointer\n", file_name, NULL);
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
		PrintLog(fp_debug, "Error when creating a thread\nThread handler is NULL\n", file_name, NULL);
		exit(ROOMMATE_THREAD__CODE_ERROR);
	}
	return thread_handle;
}


int TakeCleanClothesFromCloset(roommate_info *roommate_thread_params) {
	
	LONG previous_count = NULL;
	LONG previous_count0 = 6;
	LONG previous_count1 = 1;
	LONG previous_count2 = 7;
	//roommate wants the closet mutex
	printf("clothes in closet: %d\n", roommate_thread_params->number_of_cloth_in_closet);
	PrintLog(roommate_thread_params->fp_report, "Roommate %s active\n", roommate_thread_params->report_file, _itoa(roommate_thread_params->index, buffer, INT_BASE));
	roommate_thread_params->semahore_closet_full.waiting_code = WaitForSingleObject(roommate_thread_params->semahore_closet_full.handle, INFINITE);
	if (roommate_thread_params->semahore_closet_full.waiting_code != WAIT_OBJECT_0) {
		WaitingStatus(roommate_thread_params->semahore_closet_full.waiting_code, roommate_thread_params->fp_debug, roommate_thread_params->log_file);
		return (ERROR_INDICATION);
	}
	
	roommate_thread_params->mutex_closet.waiting_code = WaitForSingleObject(roommate_thread_params->mutex_closet.handle, INFINITE);
	if (roommate_thread_params->mutex_closet.waiting_code != WAIT_OBJECT_0) {
		WaitingStatus(roommate_thread_params->mutex_closet.waiting_code, roommate_thread_params->fp_debug, roommate_thread_params->log_file);
		return (ERROR_INDICATION);
	}

	printf("roommate %d wants a new cloth\n", roommate_thread_params->index);
	if (roommate_thread_params->index == 0) {
		ReleaseSemaphore(roommate_thread_params->semahore_closet_full.handle, 1, &previous_count0);
		printf("roommate number %d - closet full semaphore value : %ld\n", roommate_thread_params->index, previous_count0);
		WaitForSingleObject(roommate_thread_params->semahore_closet_full.handle, INFINITE);
	}
	if (roommate_thread_params->index == 1) {
		ReleaseSemaphore(roommate_thread_params->semahore_closet_full.handle, 1, &previous_count1);
		printf("roommate number %d - closet full semaphore value : %ld\n", roommate_thread_params->index, previous_count1);
		WaitForSingleObject(roommate_thread_params->semahore_closet_full.handle, INFINITE);
	}
	if (roommate_thread_params->index == 2) {
		ReleaseSemaphore(roommate_thread_params->semahore_closet_full.handle, 1, &previous_count2);
		printf("roommate number %d - closet full semaphore value : %ld\n", roommate_thread_params->index, previous_count2);
		WaitForSingleObject(roommate_thread_params->semahore_closet_full.handle, INFINITE);
	}

	//for debug
	printf("roommate %d took a new cloth\n", roommate_thread_params->index);
	if (!(ReleaseSemaphore(roommate_thread_params->semahore_closet_empty.handle, 1, &previous_count))) {
		PrintLog(roommate_thread_params->fp_debug, "Error - release semaphore_closet_empty\n", roommate_thread_params->log_file, NULL);
		return (ERROR_INDICATION);
	}
	printf("roommate %d released his closet\n", roommate_thread_params->index);
	(roommate_thread_params->number_of_cloth_in_closet)--;

	if (!(ReleaseMutex(roommate_thread_params->mutex_closet.handle))) {
		PrintLog(roommate_thread_params->fp_debug, "Error - release mutex closet\n", roommate_thread_params->log_file, NULL);
		return (ERROR_INDICATION);
	}
	printf("roommate %d release closet_mutex - done\n", roommate_thread_params->index);
	//if (!(ReleaseMutex(roommate_thread_params->mutex_zero_clothes.handle))) {
	//	PrintLog(roommate_thread_params->fp_debug, "Error - release mutex zero\n", roommate_thread_params->log_file, NULL);
	//	return (ERROR_INDICATION);
	//}

	//printf("roommate %d release zero_mutex - done\n", roommate_thread_params->index);

	return SUCCESS_INDICATION;
}

int SendDirtyClothesToLaundry(roommate_info *roommate_thread_params) {

	roommate_thread_params->mutex_laundry_bin.waiting_code = WaitForSingleObject(roommate_thread_params->mutex_laundry_bin.handle, INFINITE);
	if (roommate_thread_params->mutex_laundry_bin.waiting_code != WAIT_OBJECT_0) {
		WaitingStatus(roommate_thread_params->mutex_laundry_bin.waiting_code, roommate_thread_params->fp_debug, roommate_thread_params->log_file);
		return (ERROR_INDICATION);
	}
	printf("roommate %d took the laundry_bin mutex\n", roommate_thread_params->index);
	// a signal for the machine and block all the others from taking the laundry bin mutex 
	if (machine_is_on == false) {
		printf("still working\n");
		roommate_thread_params->semaphore_laundry_bin_full.waiting_code = WaitForSingleObject(roommate_thread_params->semaphore_laundry_bin_full.handle, INFINITE);
		printf("still working1\n");
		if (roommate_thread_params->semaphore_laundry_bin_full.waiting_code != WAIT_OBJECT_0) {
			WaitingStatus(roommate_thread_params->semaphore_laundry_bin_full.waiting_code, roommate_thread_params->fp_debug, roommate_thread_params->log_file);
			return (ERROR_INDICATION);
		}
		printf("roommate %d throw his cloth to the laundry : wfso semaphore_laundry_bin_done\n", roommate_thread_params->index);
		(roommate_thread_params->number_of_cloth_in_laundry)++;
		total_clothes_in_laundry_bin++;
		printf("still working2\n");
		if (total_clothes_in_laundry_bin == (roommate_thread_params->max_in_bin)) {
			if (!(ReleaseMutex(roommate_thread_params->mutex_laundry_bin.handle))) {
				PrintLog(roommate_thread_params->fp_debug, "Error - release mutex laundry bin", roommate_thread_params->log_file, NULL);
				return (ERROR_INDICATION);
			}
			machine_is_on = true;
			total_clothes_in_laundry_bin = 0;
			return SUCCESS_INDICATION;
		}
		printf("still working3\n");
	}
	printf("want to release the laundry bin mutex\n");
	if (!(ReleaseMutex(roommate_thread_params->mutex_laundry_bin.handle))) {
		PrintLog(roommate_thread_params->fp_debug, "Error - release mutex laundry bin", roommate_thread_params->log_file, NULL);
		return (ERROR_INDICATION);
	}
	printf("released the laundry bin mutex\n");
	printf("roommate number %d finished\n",roommate_thread_params->index);

	return SUCCESS_INDICATION;
}

DWORD WINAPI RoommateThread(LPVOID lpParam) {
	/* Parameters */
	roommate_info *roommate_thread_params = NULL;

	/* Casting lpParam to be type SITE_THREAD_params, a struct we defined and its value is in thread_params */
	roommate_thread_params = (roommate_info *)lpParam;

	/* Check if lpParam is NULL */
	if (NULL == roommate_thread_params)
	{
		return ROOMMATE_THREAD__CODE_FAILED;
	}
	printf("Hi, I'm roommate %d\n", roommate_thread_params->index);

	do {
		printf("roommate %d going to sleep\n", roommate_thread_params->index);
		Sleep(roommate_thread_params->time_for_clothes_change);
		printf("roommate %d is awake\n", roommate_thread_params->index);
		
		// if closet full
		if (TakeCleanClothesFromCloset(roommate_thread_params)) {
			return ROOMMATE_THREAD__CODE_ERROR;
		}
		
		// if laundry bin is not full
		if (SendDirtyClothesToLaundry(roommate_thread_params)) {
			return ROOMMATE_THREAD__CODE_ERROR;
		}
	} while (roommate_thread_params->time_thread->time_flag);
	printf("the thread of roommate %d - finished!!!!!!!!!!\n",roommate_thread_params->index);
	return ROOMMATE_THREAD__CODE_SUCCESS;

}
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/***********************************************************END OF FILE**********************************************************************/
/*------------------------------------------------------------------------------------------------------------------------------------------*/