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

HANDLE CreateThreadSimpleMachine(LPTHREAD_START_ROUTINE machine_start_routine, machine_info *machine_parameters, DWORD *machine_thread_id, FILE *fp_debug, char *file_name) {
	/* Parameters */
	HANDLE machine_thread_handle;

	/* Checks if the function received a null pointer */
	if (NULL == machine_start_routine) {
		PrintLog(fp_debug, "Error when creating a thread: Received a null pointer\n", file_name, NULL);
		exit(MACHINE_THREAD__CODE_ERROR);
	}

	machine_thread_handle = CreateThread(
		NULL,						/*  default security attributes */
		0,							/*  use default stack size */
		machine_start_routine,	    /*  thread function */
		machine_parameters,			/*  argument to thread function */
		0,							/*  use default creation flags */
		machine_thread_id);			/*  returns the thread identifier */

	/* Checks if the thread was created properly */
	if (machine_thread_handle == NULL) {
		machine_parameters->error_thread_creation = true;
		PrintLog(fp_debug, "Error when creating a thread, thread handler is NULL\n", file_name, NULL);
		exit(MACHINE_THREAD__CODE_ERROR);
	}
	return machine_thread_handle;
}


int WashClothes(machine_info *robot_thread_params) {
	LONG previous_count;
	for (int ii = 0; ii < robot_thread_params->bin_full; ii++) {
		if (!(ReleaseSemaphore(robot_thread_params->semaphore_laundry_bin_full.handle, 1, &previous_count))) {
			printf("**************************************************\n");
			printf("realease %d failed\n",ii);
			PrintLog(robot_thread_params->fp_debug, "Error - release semaphore_laundry_bin_full dsgf\n", robot_thread_params->log_file, NULL);
			return (ERROR_INDICATION);
		}
		printf("prev_count = %d \n", previous_count);
		printf("**************************************************\n");
		printf("realease %d succeded\n", ii);	
	}
	return SUCCESS_INDICATION;
}
int ReturnClothesToRoommates(machine_info *robot_thread_params) {
	LONG previous_count;
	//int temp=-1;
	for (int i = 0; i < robot_thread_params->num_of_roommates; i++) {
		printf("started returning clothes for roommate %d\n", robot_thread_params->roommates[i].index);
		robot_thread_params->roommates[i].mutex_closet.waiting_code = WaitForSingleObject(robot_thread_params->roommates[i].mutex_closet.handle, INFINITE);
		if (robot_thread_params->roommates[i].mutex_closet.waiting_code != WAIT_OBJECT_0) {
			WaitingStatus(robot_thread_params->roommates[i].mutex_closet.waiting_code, robot_thread_params->fp_debug, robot_thread_params->log_file);
			return (ERROR_INDICATION);
		}
		//if (robot_thread_params->roommates[i].number_of_cloth_in_closet == 0) {
		//	temp = 1;
		//}
		for (int j = 0; j < robot_thread_params->roommates[i].number_of_cloth_in_laundry; j++) {
			robot_thread_params->roommates[i].semahore_closet_empty.waiting_code = WaitForSingleObject(robot_thread_params->roommates[i].semahore_closet_empty.handle, INFINITE);
			if (robot_thread_params->roommates[i].semahore_closet_empty.waiting_code != WAIT_OBJECT_0) {
				WaitingStatus(robot_thread_params->roommates[i].semahore_closet_empty.waiting_code, robot_thread_params->fp_debug, robot_thread_params->log_file);
				return (ERROR_INDICATION);
			}
			if (!(ReleaseSemaphore(robot_thread_params->roommates[i].semahore_closet_full.handle, 1, &previous_count))) {
				printf("the robot fail to realeased the %dth cloth of roommate %d\n", j + 1, i);
				PrintLog(robot_thread_params->fp_debug, "Error - release semaphore_laundry_bin_full\n", robot_thread_params->log_file, NULL);
				return (ERROR_INDICATION);
			}
			printf("the robot realeased the %dth cloth of roommate %d\n", j + 1, i);
		}

		robot_thread_params->roommates[i].number_of_cloth_in_closet += robot_thread_params->roommates[i].number_of_cloth_in_laundry;
		robot_thread_params->roommates[i].number_of_cloth_in_laundry = 0;


		if (!(ReleaseMutex(robot_thread_params->roommates[i].mutex_closet.handle))) {
			PrintLog(robot_thread_params->fp_debug, "Error - release mutex closet\n", robot_thread_params->log_file, NULL);
			return (ERROR_INDICATION);
		}
		printf("Robot released mutex closet of %d roommate\n", robot_thread_params->roommates[i].index);
		//if (temp == 1) {
		//	if (!(ReleaseMutex(robot_thread_params->roommates[i].mutex_zero_clothes.handle))) {
		//		PrintLog(robot_thread_params->fp_debug, "Error - release mutex closet\n", robot_thread_params->log_file, NULL);
		//		return (ERROR_INDICATION);
		//	}
		//}

	}
	return SUCCESS_INDICATION;
}

DWORD WINAPI LaundryMachineThread(LPVOID lpParam) {
	/* Parameters */
	machine_info *robot_thread_params = NULL;

	/* Casting lpParam to be type SITE_THREAD_params, a struct we defined and its value is in thread_params */
	robot_thread_params = (machine_info *)lpParam;

	/* Check if lpParam is NULL */
	if (NULL == robot_thread_params)
	{
		return MACHINE_THREAD__CODE_FAILED;
	}
	printf("Hi I'm the Robot\n");
	

	while (robot_thread_params->time_thread->time_flag) {
		if (machine_is_on) {

			robot_thread_params->mutex_laundry_bin.waiting_code = WaitForSingleObject(robot_thread_params->mutex_laundry_bin.handle, INFINITE);
			if (robot_thread_params->mutex_laundry_bin.waiting_code != WAIT_OBJECT_0) {
				WaitingStatus(robot_thread_params->mutex_laundry_bin.waiting_code, robot_thread_params->fp_debug, robot_thread_params->log_file);
				return (ERROR_INDICATION);
			}

			PrintLog(robot_thread_params->fp_report, "Robot Active\n", robot_thread_params->report_file,NULL);
			if (WashClothes(robot_thread_params) != SUCCESS_INDICATION) {
				return MACHINE_THREAD__CODE_ERROR;
			}

			if (ReturnClothesToRoommates(robot_thread_params) != SUCCESS_INDICATION) {
				return MACHINE_THREAD__CODE_ERROR;
			}
			printf("Robot Finished the laundry operation\n");
			machine_is_on = false;
			if (!(ReleaseMutex(robot_thread_params->mutex_laundry_bin.handle))) {
				PrintLog(robot_thread_params->fp_debug, "Error - release mutex laundry bin\n", robot_thread_params->log_file, NULL);
				return (ERROR_INDICATION);
			}
			printf("Robot released the laundry bin mutex\n");
		}
	}

	return MACHINE_THREAD__CODE_SUCCESS;

}
