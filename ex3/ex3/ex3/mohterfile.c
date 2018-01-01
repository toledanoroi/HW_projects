/*
Authors		:	Roi Toledano	203700505
Yarden Avraham	302249925
Project		:	Exercise 3 - Robotic Laundry Machine
Uses		:
Description : main of the process; read and write files, manage threads opening.
*/

#include "machine.h"

int PrintLog(FILE *fp, char *str_out, char* filename, char* substring) {
	
	errno_t err_debug = fopen_s(&fp, filename, "a");
	char* str_final;
	if (err_debug != SUCCESS_INDICATION) {
		return ERROR_INDICATION;
	}
	else {
		if (substring != NULL) {
			str_final = replace_str(str_out, "%s", substring);
			fprintf_s(fp, "%s", str_final);
		}
		else { 
			fprintf_s(fp, "%s", str_out);
		}
		
	}

	fclose(fp);
	return SUCCESS_INDICATION;
}

int GetNumLines(FILE *fp, int *Line, FILE *fp_debug, char *logfile, char *file_path) {

	errno_t err = fopen_s(&fp, file_path, "r");
	if (!err) {
		PrintLog(fp_debug, "the file was opened\n", logfile, NULL);
		//printf("the file was opened\n");
	}
	else {
		PrintLog(fp_debug, "Error occured in file openeing\n", logfile, NULL);
		//printf("Error occured in file openeing\n");
		return ERROR_INDICATION;
	}
	if (fp == NULL) {
		PrintLog(fp_debug, "File is not exist\n", logfile, NULL);
		return (ERROR_INDICATION);
	}
	while (!feof(fp))
	{
		char ch = fgetc(fp);
		if (ch == '\n' || ch == EOF) {
			(*Line)++;
		}
	}
	fclose(fp);
	return (SUCCESS_INDICATION);
}

int FreeAndCloseAllMemory(HANDLE *ptr_roommate_thread_handle, DWORD *ptr_roommate_thread_id,
	DWORD *ptr_roommate_exitcode, DWORD *ptr_roommate_wait_code, roommate_info *roommates_array,
	machine_info *machine, HANDLE *ptr_machine_thread_handle,FILE *fp_debug,char *log_file,
	DWORD *ptr_machine_thread_id, DWORD *ptr_machine_exitcode, DWORD *ptr_machine_wait_code, timing_info *time_thread,
	HANDLE *ptr_time_thread_handle, DWORD *ptr_time_thread_id, DWORD *ptr_time_exitcode, DWORD *ptr_time_wait_code, DWORD num_of_threads) {

	for (int i = 0; i < num_of_threads; i++) {
		if (!CloseHandle(ptr_roommate_thread_handle[i])) {
			PrintLog(fp_debug, "Error when closing handle\n", log_file, NULL);
			return(ERROR_INDICATION);
		}
	}

	//free all memory
	free(machine);
	//free(ptr_machine_thread_handle);
	free(ptr_machine_thread_id);
	free(ptr_machine_exitcode);
	free(ptr_machine_wait_code);
	free(time_thread);
	//free(ptr_time_thread_handle);
	free(ptr_time_thread_id);
	free(ptr_time_exitcode);
	free(ptr_time_wait_code);
	free(ptr_roommate_wait_code);
	free(ptr_roommate_exitcode);
	free(ptr_roommate_thread_id);
	free(ptr_roommate_thread_handle);
	free(roommates_array);

	return(SUCCESS_INDICATION);
}


void WaitingStatus(DWORD waiting_code, FILE *fp_debug, char *logfile) {
	// there is an option to add the id 
	switch (waiting_code)
	{
	case WAIT_TIMEOUT :
		PrintLog(fp_debug, "Timeout has elapsed, the object is not signaled.\n", logfile,NULL);
		break;
	case WAIT_FAILED:
		PrintLog(fp_debug, "WaitForSignalObject has failed.\n", logfile, NULL);
		break;
	case WAIT_ABANDONED:
		PrintLog(fp_debug, "The handle to the object was not released by the thread before the thread was terminated.\n", logfile, NULL);
		break;
	default:
		break;
	}
}

char *replace_str(char *str, char *orig, char *rep)
{
	static char buffer[4096];
	char *p;

	if (!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
		return str;

	strncpy(buffer, str, p - str); // Copy characters from 'str' start to 'orig' st$
	buffer[p - str] = '\0';

	sprintf(buffer + (p - str), "%s%s", rep, p + strlen(orig));

	return buffer;
}

//int GetProgramParameters(char *f_path_param, DWORD *td, DWORD *max_clothes_in_bin, roommate_info *roommates_array, FILE *fp_debug, char* logfile, int *num_of_roommates) {
//	FILE *fp_in;
//	char line[MAX_CHARS_IN_LINE];
//	char time_str[MAX_CHARS_IN_LINE];
//	char clothes_str[MAX_CHARS_IN_LINE];
//	int is_getnumline, num_of_lines = 0, is_parse, jj = 0, comma = 0;
//	errno_t err = fopen_s(&fp_in, f_path_param, "r");
//	if (!err) {
//		PrintLog(fp_debug, "the file was opened\n", logfile, NULL);
//		//printf("the file was opened\n");
//	}
//	else {
//		PrintLog(fp_debug, "Error occured in file openeing\n", logfile, NULL);
//		//printf("Error occured in file openeing\n");
//		return ERROR_INDICATION;
//	}
//	if (fp_in == NULL) {
//		PrintLog(fp_debug, "File is not exist\n", logfile, NULL);
//		return (ERROR_INDICATION);
//	}
//	is_getnumline = GetNumLines(fp_in, &num_of_lines, fp_debug, logfile, NULL);
//	if (is_getnumline != SUCCESS_INDICATION) {
//		PrintLog(fp_debug, "Error - reading number of lines in file\n", logfile, NULL);
//		return (ERROR_INDICATION);
//	}
//	(*num_of_roommates) = num_of_lines - 2;
//	roommates_array = (roommate_info*)malloc(((*num_of_roommates)) * sizeof(roommate_info));
//	if (roommates_array == NULL) {
//		PrintLog(fp_debug, "Allocating memmory failed\n", logfile, NULL);
//		return ERROR_INDICATION;
//	}
//	fseek(fp_in, 0, SEEK_SET);
//	for (int i = 0; i < num_of_lines; i++) {
//		jj = 0;
//		if (fgets(line, MAX_CHARS_IN_LINE, fp_in) != NULL) {
//			if (i == 0) {
//				(*td) = atoi(line);
//			}
//			else if (i == 1) {
//				(*max_clothes_in_bin) = atoi(line);
//			}
//			else {
//				while ((*(line + jj)) != '\n' && (*(line + jj)) != EOF && (*(line + jj)) != '\0') {
//					if ((*(line + jj)) == ',') {
//						comma = jj;
//						strncpy(time_str, line, comma);
//						time_str[comma] = '\0';
//						(*(roommates_array + (i - 2))).time_for_clothes_change = atoi(time_str);
//					}
//					jj++;
//				}
//				strncpy(clothes_str, (line + comma + 1), (jj - (comma + 1)));
//				clothes_str[jj - (comma + 1)] = '\0';
//				(*(roommates_array + (i - 2))).number_of_clothes = atoi(clothes_str);
//			}
//		}
//		else {
//			PrintLog(fp_debug, "Error - Reading Line\n", logfile, NULL);
//			return (ERROR_INDICATION);
//		}
//	}
//	fclose(fp_in);
//	return SUCCESS_INDICATION;
//}
//
//int AllocateMemForThreadsAndInit(HANDLE *ptr_roommate_thread_handle, DWORD *ptr_roommate_thread_id,
//	DWORD *ptr_roommate_exitcode, DWORD *ptr_roommate_wait_code, roommate_info *roommates_array, int *num_of_roommate,
//	FILE *fp_debug, char *logfile, machine_info *machine, DWORD max_clothes_in_bin) {
//
//	char buffer[MAXCHAR];
//	// allocations for the roommates array
//	ptr_roommate_thread_handle = (HANDLE*)malloc(((*num_of_roommate)) * sizeof(HANDLE));
//	ptr_roommate_thread_id = (DWORD*)malloc(((*num_of_roommate)) * sizeof(DWORD));
//	ptr_roommate_exitcode = (DWORD*)malloc(((*num_of_roommate)) * sizeof(DWORD));
//	ptr_roommate_wait_code = (DWORD*)malloc(((*num_of_roommate)) * sizeof(DWORD));
//	// allocations for the Laundry Machine 
//
//	// checks if one of the allocations failed.
//	if (ptr_roommate_thread_handle == NULL || ptr_roommate_thread_id == NULL || ptr_roommate_exitcode == NULL || ptr_roommate_wait_code == NULL) {
//		PrintLog(fp_debug, "Allocating memmory failed\n", logfile, NULL);
//		for (int jj = 0; jj < (*num_of_roommate); jj++) {
//			(*(roommates_array + jj)).error_alloc = true;
//		}
//		return ERROR_INDICATION;
//	}
//
//	//init machine params 
//	machine->bin_full = max_clothes_in_bin;
//	machine->roommates = roommates_array;
//	machine->error_id_null = false;
//	machine->error_in_alloc = false;
//	machine->error_closing_handle = false;
//	machine->error_thread_creation = false;
//	machine->error_waiting = false;
//	machine->fp_debug = fp_debug;
//	machine->log_file = logfile;
//	machine->semaphore_laundry_bin_full.handle = CreateSemaphore(NULL, 0, machine->bin_full, "laundry_bin_semaphore_full");
//	if (machine->semaphore_laundry_bin_full.handle == NULL) {
//		PrintLog(fp_debug, "Error - the Robot /t full laundry bin semaphore definition\n", logfile, NULL);
//		if (!CloseHandle(machine->semaphore_laundry_bin_full.handle)) {
//			machine->semaphore_laundry_bin_full.error_closing_handle = true;
//		}
//	}
//	machine->semaphore_laundry_bin_empty.handle = CreateSemaphore(NULL, machine->bin_full, machine->bin_full, "laundry_bin_semaphore_empty");
//	if (machine->semaphore_laundry_bin_empty.handle == NULL) {
//		PrintLog(fp_debug, "Error - the Robot /t empty laundry bin semaphore definition\n", logfile, NULL);
//		if (!CloseHandle(machine->semaphore_laundry_bin_empty.handle)) {
//			machine->semaphore_laundry_bin_empty.error_closing_handle = true;
//		}
//	}
//	machine->mutex_laundry_bin.handle = CreateMutex(NULL, FALSE, "laundry_bin_mutex");
//	if (machine->mutex_laundry_bin.handle == NULL) {
//		PrintLog(fp_debug, "Error - the Robot /t laundry bin Mutex definition\n", logfile, NULL);
//		if (!CloseHandle(machine->mutex_laundry_bin.handle)) {
//			machine->mutex_laundry_bin.error_closing_handle = true;
//		}
//	}
//
//
//	//initiate the roommates threads Parameters
//	for (int jj = 0; jj < (*num_of_roommate); jj++) {
//		roommates_array[jj].error_closing_handle = false;
//		roommates_array[jj].error_waiting = false;
//		roommates_array[jj].error_alloc = false;
//		roommates_array[jj].error_thread_creation = false;
//		roommates_array[jj].error_id_null = false;
//		roommates_array[jj].number_of_cloth_in_laundry = 0;
//		roommates_array[jj].number_of_cloth_in_closet = roommates_array[jj].number_of_clothes;
//		roommates_array[jj].fp_debug = fp_debug;
//		roommates_array[jj].log_file = logfile;
//		roommates_array[jj].semahore_closet_empty.handle = CreateSemaphore(NULL,
//			roommates_array[jj].number_of_clothes,
//			roommates_array[jj].number_of_clothes,
//			"closet_empty_semaphore");
//		if (roommates_array[jj].semahore_closet_empty.handle == NULL) {
//			// report error and maybe close handle
//			PrintLog(fp_debug, "Error - roommate number %s \t closet semaphore definition\n", logfile, _itoa(jj, buffer, INT_BASE));
//			if (!CloseHandle(roommates_array[jj].semahore_closet_empty.handle)) {
//				roommates_array[jj].semahore_closet_empty.error_closing_handle = true;
//			}
//
//		}
//		roommates_array[jj].semahore_closet_full.handle = CreateSemaphore(NULL,
//			0,
//			roommates_array[jj].number_of_clothes,
//			"closet_full_semaphore");
//		if (roommates_array[jj].semahore_closet_full.handle == NULL) {
//			// report error and maybe close handle
//			PrintLog(fp_debug, "Error - roommate number %s \t closet semaphore definition\n", logfile, _itoa(jj, buffer, INT_BASE));
//			if (!CloseHandle(roommates_array[jj].semahore_closet_full.handle)) {
//				roommates_array[jj].semahore_closet_full.error_closing_handle = true;
//			}
//		}
//		roommates_array[jj].mutex_closet.handle = CreateMutex(NULL, FALSE, "closet_mutex");
//		if (roommates_array[jj].mutex_closet.handle == NULL) {
//			PrintLog(fp_debug, "Error - roommate number %s \t closet mutex definition\n", logfile, _itoa(jj, buffer, INT_BASE));
//			if (!CloseHandle(roommates_array[jj].mutex_closet.handle)) {
//				roommates_array[jj].mutex_closet.error_closing_handle = true;
//			}
//		}
//		roommates_array[jj].mutex_laundry_bin.handle = OpenMutex(SYNCHRONIZE, FALSE, "laundry_bin_mutex");
//		if (roommates_array[jj].mutex_laundry_bin.handle == NULL) {
//			PrintLog(fp_debug, "Error - roommate number %s \t laundry bin mutex definition\n", logfile, _itoa(jj, buffer, INT_BASE));
//			if (!CloseHandle(roommates_array[jj].mutex_laundry_bin.handle)) {
//				roommates_array[jj].mutex_laundry_bin.error_closing_handle = true;
//			}
//		}
//		roommates_array[jj].semaphore_laundry_bin_full.handle = OpenSemaphore(SYNCHRONIZE, FALSE, "laundry_bin_semaphore_full");
//		if (roommates_array[jj].semaphore_laundry_bin_full.handle == NULL) {
//			PrintLog(fp_debug, "Error - roommate number %s \t full laundry bin semaphore definition\n", logfile, _itoa(jj, buffer, INT_BASE));
//			if (!CloseHandle(roommates_array[jj].semaphore_laundry_bin_full.handle)) {
//				roommates_array[jj].semaphore_laundry_bin_full.error_closing_handle = true;
//			}
//		}
//		roommates_array[jj].semaphore_laundry_bin_empty.handle = OpenSemaphore(SYNCHRONIZE, FALSE, "laundry_bin_semaphore_empty");
//		if (roommates_array[jj].semaphore_laundry_bin_empty.handle == NULL) {
//			PrintLog(fp_debug, "Error - roommate number %s \t empty laundry bin semaphore definition\n", logfile, _itoa(jj, buffer, INT_BASE));
//			if (!CloseHandle(roommates_array[jj].semaphore_laundry_bin_empty.handle)) {
//				roommates_array[jj].semaphore_laundry_bin_empty.error_closing_handle = true;
//			}
//		}
//	}
//	return SUCCESS_INDICATION;
//}