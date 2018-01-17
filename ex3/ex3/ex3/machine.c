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
/*Functions*/
/*HANDLE CreateThreadSimpleMachine
Parameters:		p_start_routine			- A pointer tha points to a function that notifies the host that a thread has started to execute.
				p_thread_parameters		- A pointer to a struct that contains all the parameters of the thread, including potential errors.
				p_thread_id				- A pointer to the thread's ID.
				fp_debud				- A pointer to the  debug log file.
				file_name				- A pointer to the name of the log file string.
Returns:		A HANDLE variable		- The handle of the thread that was created.
Description:	A function that calls CreateThread function, that creates a thread to execute LaundryMachineThread WINAPI function.*/
HANDLE CreateThreadSimpleMachine(LPTHREAD_START_ROUTINE machine_start_routine, machine_info *machine_parameters, DWORD *machine_thread_id, FILE *fp_debug, char *file_name) {
	/* Parameters */
	HANDLE machine_thread_handle;

	/* Checks if the function received a null pointer */
	if (NULL == machine_start_routine) {
		PrintLog(fp_debug, "Error when creating a thread: Received a null pointer\n", file_name, NULL,machine_parameters->mutex_debug_file);
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
		PrintLog(fp_debug, "Error when creating a thread, thread handler is NULL\n", file_name, NULL, machine_parameters->mutex_debug_file);
		exit(MACHINE_THREAD__CODE_ERROR);
	}
	return machine_thread_handle;
}

/*int WashClothes
Parameters:		robot_thread_params		- A pointer to the threas struct (machine info type).
Returns:		An int variable			- Success = 0 and Error = -1
Description:	A function that responsible to the wash clothes machine operation. 
				it realeses the laundry bin full semaphore -> empty the laundry bin.*/
int WashClothes(machine_info *robot_thread_params) {
	LONG previous_count;
	robot_thread_params->mutex_debug_file.waiting_code = WaitForSingleObject(robot_thread_params->mutex_debug_file.handle, INFINITE);
	if (robot_thread_params->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
		return (ERROR_INDICATION);
	}
	PrintLog(robot_thread_params->fp_debug, "The Robot started the WashClothes function\n", robot_thread_params->log_file, NULL, robot_thread_params->mutex_debug_file);
	if (!(ReleaseMutex(robot_thread_params->mutex_debug_file.handle))) {
		return (ERROR_INDICATION);
	}
	for (int ii = 0; ii < robot_thread_params->bin_full; ii++) {
		robot_thread_params->mutex_debug_file.waiting_code = WaitForSingleObject(robot_thread_params->mutex_debug_file.handle, INFINITE);
		if (robot_thread_params->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
			return (ERROR_INDICATION);
		}
		PrintLog(robot_thread_params->fp_debug, "The Robot want to release the semaphore laundry bin full\n", robot_thread_params->log_file, NULL, robot_thread_params->mutex_debug_file);
		if (!(ReleaseMutex(robot_thread_params->mutex_debug_file.handle))) {
			return (ERROR_INDICATION);
		}
		if (!(ReleaseSemaphore(robot_thread_params->semaphore_laundry_bin_full.handle, 1, &previous_count))) {
			//printf("**************************************************\n");
			PrintLog(robot_thread_params->fp_debug, "realease %s failed\n", robot_thread_params->log_file, _itoa(ii, buffer, INT_BASE), robot_thread_params->mutex_debug_file);
			PrintLog(robot_thread_params->fp_debug, "Error - release semaphore_laundry_bin_full dsgf\n", robot_thread_params->log_file, NULL, robot_thread_params->mutex_debug_file);
			return (ERROR_INDICATION);
		}
		robot_thread_params->mutex_debug_file.waiting_code = WaitForSingleObject(robot_thread_params->mutex_debug_file.handle, INFINITE);
		if (robot_thread_params->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
			return (ERROR_INDICATION);
		}
		PrintLog(robot_thread_params->fp_debug, "realease %s succeded\n", robot_thread_params->log_file, _itoa(ii, buffer, INT_BASE), robot_thread_params->mutex_debug_file);
		if (!(ReleaseMutex(robot_thread_params->mutex_debug_file.handle))) {
			return (ERROR_INDICATION);
		}
	}
	robot_thread_params->mutex_debug_file.waiting_code = WaitForSingleObject(robot_thread_params->mutex_debug_file.handle, INFINITE);
	if (robot_thread_params->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
		return (ERROR_INDICATION);
	}
	PrintLog(robot_thread_params->fp_debug, "The Robot finished the WashClothes function\n", robot_thread_params->log_file, NULL, robot_thread_params->mutex_debug_file);
	if (!(ReleaseMutex(robot_thread_params->mutex_debug_file.handle))) {
		return (ERROR_INDICATION);
	}
	return SUCCESS_INDICATION;
}

/*int ReturnClothesToRoommates
Parameters:		robot_thread_params		- A pointer to the threas struct (machine info type).
Returns:		An int variable			- Success = 0 and Error = -1
Description:	A function that responsible to the clothes return to the roommates closets operation.
				For each roommate: check how much clothes were in the laundry, take the closet mutex ,update the closets semaphores
				,fill the closet(update parmeters) and release the mutex*/ 
int ReturnClothesToRoommates(machine_info *robot_thread_params) {
	LONG previous_count;
	robot_thread_params->mutex_debug_file.waiting_code = WaitForSingleObject(robot_thread_params->mutex_debug_file.handle, INFINITE);
	if (robot_thread_params->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
		return (ERROR_INDICATION);
	}
	PrintLog(robot_thread_params->fp_debug, "The Robot started the ReturnClothesToRoommate function\n", robot_thread_params->log_file, NULL, robot_thread_params->mutex_debug_file);
	if (!(ReleaseMutex(robot_thread_params->mutex_debug_file.handle))) {
		return (ERROR_INDICATION);
	}
	for (int i = 0; i < robot_thread_params->num_of_roommates; i++) {
		robot_thread_params->mutex_debug_file.waiting_code = WaitForSingleObject(robot_thread_params->mutex_debug_file.handle, INFINITE);
		if (robot_thread_params->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
			return (ERROR_INDICATION);
		}
		PrintLog(robot_thread_params->fp_debug, "The Robot started returning clothes for roommate %s\n", robot_thread_params->log_file, _itoa(robot_thread_params->roommates[i].index, buffer, INT_BASE), robot_thread_params->mutex_debug_file);
		if (!(ReleaseMutex(robot_thread_params->mutex_debug_file.handle))) {
			return (ERROR_INDICATION);
		}
		robot_thread_params->mutex_debug_file.waiting_code = WaitForSingleObject(robot_thread_params->mutex_debug_file.handle, INFINITE);
		if (robot_thread_params->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
			return (ERROR_INDICATION);
		}
		PrintLog(robot_thread_params->fp_debug, "The Robot wait for the closet mutex of roommate %s\n", robot_thread_params->log_file, _itoa(robot_thread_params->roommates[i].index, buffer, INT_BASE), robot_thread_params->mutex_debug_file);
		if (!(ReleaseMutex(robot_thread_params->mutex_debug_file.handle))) {
			return (ERROR_INDICATION);
		}
		robot_thread_params->roommates[i].mutex_closet.waiting_code = WaitForSingleObject(robot_thread_params->roommates[i].mutex_closet.handle, INFINITE);
		if (robot_thread_params->roommates[i].mutex_closet.waiting_code != WAIT_OBJECT_0) {
			WaitingStatus(robot_thread_params->roommates[i].mutex_closet.waiting_code, robot_thread_params->fp_debug, robot_thread_params->log_file);
			return (ERROR_INDICATION);
		}
		robot_thread_params->mutex_debug_file.waiting_code = WaitForSingleObject(robot_thread_params->mutex_debug_file.handle, INFINITE);
		if (robot_thread_params->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
			return (ERROR_INDICATION);
		}
		PrintLog(robot_thread_params->fp_debug, "The Robot took the closet mutex of roommate %s\n", robot_thread_params->log_file, _itoa(robot_thread_params->roommates[i].index, buffer, INT_BASE), robot_thread_params->mutex_debug_file);
		if (!(ReleaseMutex(robot_thread_params->mutex_debug_file.handle))) {
			return (ERROR_INDICATION);
		}
		for (int j = 0; j < robot_thread_params->roommates[i].number_of_cloth_in_laundry; j++) {
			robot_thread_params->roommates[i].semahore_closet_empty.waiting_code = WaitForSingleObject(robot_thread_params->roommates[i].semahore_closet_empty.handle, INFINITE);
			if (robot_thread_params->roommates[i].semahore_closet_empty.waiting_code != WAIT_OBJECT_0) {
				WaitingStatus(robot_thread_params->roommates[i].semahore_closet_empty.waiting_code, robot_thread_params->fp_debug, robot_thread_params->log_file);
				return (ERROR_INDICATION);
			}
			if (!(ReleaseSemaphore(robot_thread_params->roommates[i].semahore_closet_full.handle, 1, &previous_count))) {
				PrintLog(robot_thread_params->fp_debug, "Error - release semaphore_laundry_bin_full\n", robot_thread_params->log_file, NULL, robot_thread_params->mutex_debug_file);
				return (ERROR_INDICATION);
			}
		}

		robot_thread_params->roommates[i].number_of_cloth_in_closet += robot_thread_params->roommates[i].number_of_cloth_in_laundry;
		robot_thread_params->roommates[i].number_of_cloth_in_laundry = 0;
		robot_thread_params->mutex_debug_file.waiting_code = WaitForSingleObject(robot_thread_params->mutex_debug_file.handle, INFINITE);
		if (robot_thread_params->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
			return (ERROR_INDICATION);
		}
		PrintLog(robot_thread_params->fp_debug, "The Robot wants to release the closet mutex of roommate %s\n", robot_thread_params->log_file, _itoa(robot_thread_params->roommates[i].index, buffer, INT_BASE), robot_thread_params->mutex_debug_file);
		if (!(ReleaseMutex(robot_thread_params->mutex_debug_file.handle))) {
			return (ERROR_INDICATION);
		}
		if (!(ReleaseMutex(robot_thread_params->roommates[i].mutex_closet.handle))) {
			PrintLog(robot_thread_params->fp_debug, "Error - release mutex closet\n", robot_thread_params->log_file, NULL, robot_thread_params->mutex_debug_file);
			return (ERROR_INDICATION);
		}
		robot_thread_params->mutex_debug_file.waiting_code = WaitForSingleObject(robot_thread_params->mutex_debug_file.handle, INFINITE);
		if (robot_thread_params->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
			return (ERROR_INDICATION);
		}
		PrintLog(robot_thread_params->fp_debug, "Robot released mutex closet of %s roommate\n", robot_thread_params->log_file, _itoa(robot_thread_params->roommates[i].index, buffer, INT_BASE), robot_thread_params->mutex_debug_file);
		if (!(ReleaseMutex(robot_thread_params->mutex_debug_file.handle))) {
			return (ERROR_INDICATION);
		}

		PrintLog(robot_thread_params->fp_debug, "Robot return clothes to roommate %s\n", robot_thread_params->log_file, _itoa(robot_thread_params->roommates[i].index, buffer, INT_BASE), robot_thread_params->mutex_debug_file);
		if (!(ReleaseMutex(robot_thread_params->mutex_debug_file.handle))) {
			return (ERROR_INDICATION);
		}
	}
	PrintLog(robot_thread_params->fp_debug, "The Robot finished the ReturnClothesToRoomate function\n", robot_thread_params->log_file, NULL, robot_thread_params->mutex_debug_file);
	return SUCCESS_INDICATION;
}

/*DWORD WINAPI LaundryMachineThread
Parameters:		lpParam				- LPVOID type, pointer to the thread's parameters, can be any pointer we want.
Returns:		A DWORD variable	- MACHINE_THREAD__CODE_SUCCESS = 0 , MACHINE_THREAD__CODE_ERROR = -1, MACHINE_THREAD__CODE_FAILED=1.
Description:	The main function of the robot thread.*/
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
	while (robot_thread_params->time_thread->time_flag) {
		if (machine_is_on) {
			robot_thread_params->mutex_debug_file.waiting_code = WaitForSingleObject(robot_thread_params->mutex_debug_file.handle, INFINITE);
			if (robot_thread_params->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
				return (ERROR_INDICATION);
			}
			PrintLog(robot_thread_params->fp_debug, "The Robot wait for the laundry bin mutex\n", robot_thread_params->log_file, NULL, robot_thread_params->mutex_debug_file);
			if (!(ReleaseMutex(robot_thread_params->mutex_debug_file.handle))) {
				return (ERROR_INDICATION);
			}
			robot_thread_params->mutex_laundry_bin.waiting_code = WaitForSingleObject(robot_thread_params->mutex_laundry_bin.handle, INFINITE);
			if (robot_thread_params->mutex_laundry_bin.waiting_code != WAIT_OBJECT_0) {
				WaitingStatus(robot_thread_params->mutex_laundry_bin.waiting_code, robot_thread_params->fp_debug, robot_thread_params->log_file);
				return (MACHINE_THREAD__CODE_ERROR);
			}
			robot_thread_params->mutex_debug_file.waiting_code = WaitForSingleObject(robot_thread_params->mutex_debug_file.handle, INFINITE);
			if (robot_thread_params->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
				return (ERROR_INDICATION);
			}
			PrintLog(robot_thread_params->fp_debug, "The Robot took the laundry bin mutex\n", robot_thread_params->log_file, NULL, robot_thread_params->mutex_debug_file);
			if (!(ReleaseMutex(robot_thread_params->mutex_debug_file.handle))) {
				return (ERROR_INDICATION);
			}
			PrintLog(robot_thread_params->fp_report, "Robot Active\n", robot_thread_params->report_file,NULL, robot_thread_params->mutex_report_file);
			if (WashClothes(robot_thread_params) != SUCCESS_INDICATION) {
				return MACHINE_THREAD__CODE_ERROR;
			}
			if (!(ReleaseMutex(robot_thread_params->mutex_report_file.handle))) {
				return (ERROR_INDICATION);
			}
			robot_thread_params->mutex_debug_file.waiting_code = WaitForSingleObject(robot_thread_params->mutex_debug_file.handle, INFINITE);
			if (robot_thread_params->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
				return (ERROR_INDICATION);
			}
			PrintLog(robot_thread_params->fp_debug, "The Robot Finished the laundry operation\n", robot_thread_params->log_file, NULL, robot_thread_params->mutex_debug_file);
			machine_is_on = false;
			if (!(ReleaseMutex(robot_thread_params->mutex_debug_file.handle))) {
				return (ERROR_INDICATION);
			}
			robot_thread_params->mutex_debug_file.waiting_code = WaitForSingleObject(robot_thread_params->mutex_debug_file.handle, INFINITE);
			if (robot_thread_params->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
				return (ERROR_INDICATION);
			}
			PrintLog(robot_thread_params->fp_debug, "The Robot wants to release the laundry bin mutex\n", robot_thread_params->log_file, NULL, robot_thread_params->mutex_debug_file);
			if (!(ReleaseMutex(robot_thread_params->mutex_debug_file.handle))) {
				return (ERROR_INDICATION);
			}
			if (!(ReleaseMutex(robot_thread_params->mutex_laundry_bin.handle))) {
				PrintLog(robot_thread_params->fp_debug, "Error - release mutex laundry bin\n", robot_thread_params->log_file, NULL, robot_thread_params->mutex_debug_file);
				return (MACHINE_THREAD__CODE_ERROR);
			}
			robot_thread_params->mutex_debug_file.waiting_code = WaitForSingleObject(robot_thread_params->mutex_debug_file.handle, INFINITE);
			if (robot_thread_params->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
				return (ERROR_INDICATION);
			}
			PrintLog(robot_thread_params->fp_debug, "The Robot released the laundry bin mutex\n", robot_thread_params->log_file, NULL, robot_thread_params->mutex_debug_file);
			if (!(ReleaseMutex(robot_thread_params->mutex_debug_file.handle))) {
				return (ERROR_INDICATION);
			}
		}
	}
	robot_thread_params->mutex_debug_file.waiting_code = WaitForSingleObject(robot_thread_params->mutex_debug_file.handle, INFINITE);
	if (robot_thread_params->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
		return (ERROR_INDICATION);
	}
	PrintLog(robot_thread_params->fp_debug, "The Robot Finished his WINAPI function!!!\n", robot_thread_params->log_file, NULL, robot_thread_params->mutex_debug_file);
	if (!(ReleaseMutex(robot_thread_params->mutex_debug_file.handle))) {
		return (ERROR_INDICATION);
	}
	return MACHINE_THREAD__CODE_SUCCESS;
}
