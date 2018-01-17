/*
File Name	:	main.c
Authors		:	Roi Toledano	203700505
				Yarden Avraham	302249925
Project		:	Exercise 3 - Robotic Laundry Machine
Uses		:
Description : main of the process; read and write files, manage threads opening. 
*/

#include "machine.h"
int total_clothes_in_laundry_bin = 0;
bool machine_is_on = false;
char buffer[MAXCHAR];

//Declartions of function
int InitThreads(roommate_info *roommates_array, int num_of_roommate, FILE *fp_debug, char *logfile, FILE *fp_report,
	char *reportfile, machine_info *machine, DWORD max_clothes_in_bin, DWORD td, timing_info *time_thread);

int Allocating(HANDLE **ptr_roommate_thread_handle, DWORD **ptr_roommate_thread_id,
	DWORD **ptr_roommate_exitcode, DWORD **ptr_roommate_wait_code, roommate_info **roommates_array, int num_of_roommate,
	FILE *fp_debug, char *logfile, FILE *fp_report, char *reportfile, machine_info **machine, timing_info **time_thread);

int FilesInit(FILE *fp_report, FILE *fp_debug, char *logfile, char *report_file);

int GetProgramParameters(char *f_path_param, DWORD *td, DWORD *max_clothes_in_bin, roommate_info *roommates_array, FILE *fp_debug, char* logfile, int num_of_lines, int *max_sleep_time);

int FreeAndCloseAllMemory(HANDLE *ptr_roommate_thread_handle, DWORD *ptr_roommate_thread_id,
	DWORD *ptr_roommate_exitcode, DWORD *ptr_roommate_wait_code, roommate_info *roommates_array,
	machine_info *machine, FILE *fp_debug, char *log_file, timing_info *time_thread, DWORD num_of_threads);

/*------------------------------------------------------------------------------------------------------------------------------------------------*/
/*Functions*/

int main(int argc, char *argv[]) {
	//parameters
	int is_fopen, is_free, is_init_thread, is_alloc1, is_file_init, is_getnumline;
	int jj, num_of_lines = 0;
	//file pointers
	FILE *fp_in = NULL;
	FILE *fp_debug = NULL;
	FILE *fp_report = NULL;
	FILE *fp_parameters = NULL;
	
	//timing parameters
	timing_info *time_thread = NULL;
	DWORD td;
	int max_sleep_time = 0;

	//machine parameters
	DWORD max_clothes_in_bin;
	machine_info *machine = NULL;
	//HANDLE *ptr_machine_thread_handle = NULL;
	//DWORD *ptr_machine_thread_id = NULL;
	//DWORD *ptr_machine_exitcode = NULL;
	//DWORD *ptr_machine_wait_code = NULL;
	
	//roommates parameters
	roommate_info *roommates_array = NULL;
	int num_of_roommate = ERROR_NUM_ROOMMATES;
	HANDLE *ptr_roommate_thread_handle = NULL;
	DWORD *ptr_roommate_thread_id = NULL;
	DWORD *ptr_roommate_exitcode = NULL;
	DWORD *ptr_roommate_wait_code = NULL;


	//HANDLE *ptr_time_thread_handle = NULL;
	//DWORD *ptr_time_thread_id = NULL;
	//DWORD *ptr_time_exitcode = NULL;
	//DWORD *ptr_time_wait_code = NULL;
	
	//initiallizing files
	is_file_init = FilesInit(fp_report, fp_debug, argv[3], argv[2]);
	if (is_file_init != SUCCESS_INDICATION) {
		PrintLogBeforeInit(fp_debug, "Error - initiate files\n", argv[3], NULL);
		return (ERROR_INDICATION);
	}
	// check if the input from command line is valid
	if (argc != 4) {
		PrintLogBeforeInit(fp_debug, "Error!!! Not Enough Files Entered - Exit Program %s\n", argv[3], _itoa(15,buffer,INT_BASE));
		return ERROR_INDICATION;
	}
	
	// get the number of rommates from the parameters file
	is_getnumline = GetNumLines(fp_in, &num_of_lines, fp_debug, argv[3],argv[1]);
	if (is_getnumline != SUCCESS_INDICATION) {
		PrintLogBeforeInit(fp_debug, "Error - reading number of lines in file\n", argv[3], NULL);
		return (ERROR_INDICATION);
	}

	num_of_roommate = num_of_lines - 2;
	if (num_of_roommate < 0) {
		PrintLogBeforeInit(fp_debug, "Error - no roommates given\n", argv[3], NULL);
		exit(FAILURE_INDICATION);
	}
	
	// allocating memory for all the threads
	is_alloc1 = Allocating(&ptr_roommate_thread_handle, &ptr_roommate_thread_id,
		&ptr_roommate_exitcode, &ptr_roommate_wait_code, &roommates_array, num_of_roommate, fp_debug, argv[3], fp_report, argv[2], &machine,
		&time_thread);
	if (is_alloc1 != SUCCESS_INDICATION) {
		PrintLogBeforeInit(fp_debug, "Error - allocating memory", argv[3], NULL);
		return ERROR_INDICATION;
	}

	// get the threads parameters from the parameters file. 
	is_fopen = GetProgramParameters(argv[1], &td, &max_clothes_in_bin, roommates_array, fp_debug, argv[3] ,num_of_lines,&max_sleep_time);
	if (is_fopen != SUCCESS_INDICATION) {
		PrintLogBeforeInit(fp_debug, "Error - retreiving program parameters", argv[3], NULL);
		return ERROR_INDICATION;
	}
	
	//Init threads parameters 
	is_init_thread = InitThreads(roommates_array, num_of_roommate, fp_debug, argv[3], fp_report, argv[2], machine, max_clothes_in_bin,td,time_thread);
	if (is_init_thread != SUCCESS_INDICATION) {
		PrintLogBeforeInit(fp_debug, "AllocateMemForThreadsAndInit function failed\n", argv[3], NULL);
		exit(FAILURE_INDICATION);
	}
	
	
	DWORD num_of_threads = num_of_roommate + 2;

	for (jj = 0; jj < num_of_threads; jj++) {
		if (jj == 0) {
			// Create a thread for the time counter
			ptr_roommate_thread_handle[jj] = CreateTimingThreadSimple(TimeCounterThread, time_thread, &(ptr_roommate_thread_id[jj]), fp_debug, argv[3]);
			time_thread->mutex_debug_file.waiting_code = WaitForSingleObject(time_thread->mutex_debug_file.handle, INFINITE);
			if (time_thread->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
				return (ERROR_INDICATION);
			}
			PrintLog(fp_debug, "This is thread : %s \t Time Thread\n", argv[3],_itoa((ptr_roommate_thread_id[jj]),buffer,INT_BASE), time_thread->mutex_debug_file);
			if (!(ReleaseMutex(time_thread->mutex_debug_file.handle))) {
				return (ERROR_INDICATION);
			}
		}
		else if (jj == 1) {
			// Create a thread for the machine
			ptr_roommate_thread_handle[jj] = CreateThreadSimpleMachine(LaundryMachineThread, machine, &(ptr_roommate_thread_id[jj]), fp_debug, argv[3]);
			time_thread->mutex_debug_file.waiting_code = WaitForSingleObject(time_thread->mutex_debug_file.handle, INFINITE);
			if (time_thread->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
				return (ERROR_INDICATION);
			}
			PrintLog(fp_debug, "This is thread : %s \t Machine Thread\n", argv[3], _itoa((ptr_roommate_thread_id[jj]), buffer, INT_BASE), time_thread->mutex_debug_file);
			if (!(ReleaseMutex(time_thread->mutex_debug_file.handle))) {
				return (ERROR_INDICATION);
			}
		}
		else {
			// Create a thread for each roommate
			ptr_roommate_thread_handle[jj] = CreateThreadSimple(RoommateThread, &(roommates_array[jj - 2]), &(ptr_roommate_thread_id[jj]), fp_debug, argv[3]);
			time_thread->mutex_debug_file.waiting_code = WaitForSingleObject(time_thread->mutex_debug_file.handle, INFINITE);
			if (time_thread->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
				return (ERROR_INDICATION);
			}
			PrintLog(fp_debug, "This is thread : %s \t Roommate", argv[3], _itoa((ptr_roommate_thread_id[jj]), buffer, INT_BASE), time_thread->mutex_debug_file);
			PrintLog(fp_debug, " %s Thread\n", argv[3], _itoa((roommates_array[jj - 2].index), buffer, INT_BASE), time_thread->mutex_debug_file);
			if (!(ReleaseMutex(time_thread->mutex_debug_file.handle))) {
				return (ERROR_INDICATION);
			}
		}
		
	}

	// wait for all the thread to be done.
	DWORD waitcode_main = WaitForMultipleObjects(num_of_threads, ptr_roommate_thread_handle, TRUE, INFINITE);
	//DWORD waitcode_main = WaitForSingleObject(ptr_roommate_thread_handle[1], INFINITE);
	time_thread->mutex_debug_file.waiting_code = WaitForSingleObject(time_thread->mutex_debug_file.handle, INFINITE);
	if (time_thread->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
		return (ERROR_INDICATION);
	}
	PrintLog(fp_debug, "Main thread is going to sleep\n", argv[3], NULL, time_thread->mutex_debug_file);
	PrintLog(fp_debug, "The thread is awake and start closing the program\n", argv[3], NULL, time_thread->mutex_debug_file);
	if (!(ReleaseMutex(time_thread->mutex_debug_file.handle))) {
		return (ERROR_INDICATION);
	}
	//get exit codes 
	for (int kk = 0; kk < num_of_threads; kk++) {
		ptr_roommate_exitcode[kk] = GetExitCodeThread(ptr_roommate_thread_handle[kk], (&(ptr_roommate_exitcode[kk])));
		if (ptr_roommate_exitcode[kk] == 0) {
			time_thread->mutex_debug_file.waiting_code = WaitForSingleObject(time_thread->mutex_debug_file.handle, INFINITE);
			if (time_thread->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
				return (ERROR_INDICATION);
			}
			PrintLog(fp_debug, "Error with exitcodes of the thread\n",argv[3],NULL, time_thread->mutex_debug_file);
			if (!(ReleaseMutex(time_thread->mutex_debug_file.handle))) {
				return (ERROR_INDICATION);
			}
			return ERROR_INDICATION;
		}
		else {
			time_thread->mutex_debug_file.waiting_code = WaitForSingleObject(time_thread->mutex_debug_file.handle, INFINITE);
			if (time_thread->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
				return (ERROR_INDICATION);
			}
			PrintLog(fp_debug, "The exit code of thread number %s", argv[3], _itoa(kk, buffer, INT_BASE), time_thread->mutex_debug_file);
			PrintLog(fp_debug, "is : %s\n", argv[3], _itoa(ptr_roommate_exitcode[kk], buffer, INT_BASE), time_thread->mutex_debug_file);
			if (!(ReleaseMutex(time_thread->mutex_debug_file.handle))) {
				return (ERROR_INDICATION);
			}
		}
	}
	
	//realease all the active threads and free allocated memory
	is_free =  FreeAndCloseAllMemory(ptr_roommate_thread_handle, ptr_roommate_thread_id,
		ptr_roommate_exitcode, ptr_roommate_wait_code, roommates_array,
		machine, fp_debug, argv[3], time_thread, num_of_threads);
	if (is_free != SUCCESS_INDICATION) {
		time_thread->mutex_debug_file.waiting_code = WaitForSingleObject(time_thread->mutex_debug_file.handle, INFINITE);
		if (time_thread->mutex_debug_file.waiting_code != WAIT_OBJECT_0) {
			return (ERROR_INDICATION);
		}
		PrintLog(fp_debug, "Error when try to free memory\n", argv[3], NULL, time_thread->mutex_debug_file);
		if (!(ReleaseMutex(time_thread->mutex_debug_file.handle))) {
			return (ERROR_INDICATION);
		}
		return (ERROR_INDICATION);
	}
	return (SUCCESS_INDICATION);
}



int FreeAndCloseAllMemory(HANDLE *ptr_roommate_thread_handle, DWORD *ptr_roommate_thread_id,
	DWORD *ptr_roommate_exitcode, DWORD *ptr_roommate_wait_code, roommate_info *roommates_array,
	machine_info *machine, FILE *fp_debug, char *log_file, timing_info *time_thread, DWORD num_of_threads) {

	//Close Semaphores ans Mutex Handles
	if (!CloseHandle(machine->mutex_laundry_bin.handle)) {
		PrintLog(fp_debug, "Error when closing handle\n", log_file, NULL, time_thread->mutex_debug_file);
		return(ERROR_INDICATION);
	}
	if (!CloseHandle(machine->semaphore_laundry_bin_empty.handle)) {
		PrintLog(fp_debug, "Error when closing handle\n", log_file, NULL, time_thread->mutex_debug_file);
		return(ERROR_INDICATION);
	}
	if (!CloseHandle(machine->semaphore_laundry_bin_full.handle)) {
		PrintLog(fp_debug, "Error when closing handle\n", log_file, NULL, time_thread->mutex_debug_file);
		return(ERROR_INDICATION);
	}

	for (int j = 0; j < num_of_threads - 2; j++) {
		if (!CloseHandle(roommates_array[j].mutex_closet.handle)) {
			PrintLog(fp_debug, "Error when closing handle\n", log_file, NULL, time_thread->mutex_debug_file);
			return(ERROR_INDICATION);
		}
		if (!CloseHandle(roommates_array[j].semahore_closet_empty.handle)) {
			PrintLog(fp_debug, "Error when closing handle\n", log_file, NULL, time_thread->mutex_debug_file);
			return(ERROR_INDICATION);
		}
		if (!CloseHandle(roommates_array[j].semahore_closet_full.handle)) {
			PrintLog(fp_debug, "Error when closing handle\n", log_file, NULL, time_thread->mutex_debug_file);
			return(ERROR_INDICATION);
		}
		if (!CloseHandle(roommates_array[j].mutex_laundry_bin.handle)) {
			PrintLog(fp_debug, "Error when closing handle\n", log_file, NULL, time_thread->mutex_debug_file);
			return(ERROR_INDICATION);
		}
		if (!CloseHandle(roommates_array[j].semaphore_laundry_bin_empty.handle)) {
			PrintLog(fp_debug, "Error when closing handle\n", log_file, NULL, time_thread->mutex_debug_file);
			return(ERROR_INDICATION);
		}
		if (!CloseHandle(roommates_array[j].semaphore_laundry_bin_full.handle)) {
			PrintLog(fp_debug, "Error when closing handle\n", log_file, NULL, time_thread->mutex_debug_file);
			return(ERROR_INDICATION);
		}
	}

	//Close threads Handles
	for (int i = 0; i < num_of_threads; i++) {
		if (!CloseHandle(ptr_roommate_thread_handle[i])) {
			PrintLog(fp_debug, "Error when closing handle\n", log_file, NULL, time_thread->mutex_debug_file);
			return(ERROR_INDICATION);
		}
	}

	//free all memory
	free(machine);
	//free(ptr_machine_thread_handle);
	//free(ptr_machine_thread_id);
	//free(ptr_machine_exitcode);
	//free(ptr_machine_wait_code);
	//free(ptr_time_thread_handle);
	//free(ptr_time_thread_id);
	//free(ptr_time_exitcode);
	//free(ptr_time_wait_code);
	free(time_thread);
	free(ptr_roommate_wait_code);
	free(ptr_roommate_exitcode);
	free(ptr_roommate_thread_id);
	free(ptr_roommate_thread_handle);
	free(roommates_array);

	return (SUCCESS_INDICATION);
}

int GetProgramParameters(char *f_path_param, DWORD *td, DWORD *max_clothes_in_bin, roommate_info *roommates_array, FILE *fp_debug, char* logfile, int num_of_lines, int *max_sleep_time) {
	FILE *fp_in;
	char line[MAX_CHARS_IN_LINE];
	char time_str[MAX_CHARS_IN_LINE];
	char clothes_str[MAX_CHARS_IN_LINE];
	int  jj = 0, comma = 0;
	errno_t err = fopen_s(&fp_in, f_path_param, "r");
	if (!err) {
		PrintLogBeforeInit(fp_debug, "the file was opened\n", logfile, NULL);
		//printf("the file was opened\n");
	}
	else {
		PrintLogBeforeInit(fp_debug, "Error occured in file openeing\n", logfile, NULL);
		//printf("Error occured in file openeing\n");
		return ERROR_INDICATION;
	}
	if (fp_in == NULL) {
		PrintLogBeforeInit(fp_debug, "File is not exist\n", logfile, NULL);
		return (ERROR_INDICATION);
	}

	fseek(fp_in, 0, SEEK_SET);
	for (int i = 0; i < num_of_lines; i++) {
		jj = 0;
		if (fgets(line, MAX_CHARS_IN_LINE, fp_in) != NULL) {
			if (i == 0) {
				(*td) = atoi(line);
			}
			else if (i == 1) {
				(*max_clothes_in_bin) = atoi(line);
			}
			else {
				while ((*(line + jj)) != '\n' && (*(line + jj)) != EOF && (*(line + jj)) != '\0') {
					if ((*(line + jj)) == ',') {
						comma = jj;
						strncpy(time_str, line, comma);
						time_str[comma] = '\0';
						roommates_array[i - 2].time_for_clothes_change = atoi(time_str);
						if (roommates_array[i - 2].time_for_clothes_change > max_sleep_time) {
							max_sleep_time = roommates_array[i - 2].time_for_clothes_change;
						}
					}
					jj++;
				}
				strncpy(clothes_str, (line + comma + 1), (jj - (comma + 1)));
				clothes_str[jj - (comma + 1)] = '\0';
				(*(roommates_array + (i - 2))).number_of_clothes = atoi(clothes_str);
				(*(roommates_array + (i - 2))).max_in_bin = (*max_clothes_in_bin);

				(*(roommates_array + (i - 2))).index = i - 2;
			}
		}
		else {
			PrintLogBeforeInit(fp_debug, "Error - Reading Line\n", logfile, NULL);
			return (ERROR_INDICATION);
		}
	}
	fclose(fp_in);
	return SUCCESS_INDICATION;
}

int FilesInit(FILE *fp_report, FILE *fp_debug, char *logfile, char *report_file) {

	errno_t err_debug = fopen_s(&fp_debug, logfile, "w");
	if (err_debug != SUCCESS_INDICATION) {
		printf("Error when opening debug file\n");
		return ERROR_INDICATION;
	}
	fclose(fp_debug);

	errno_t err_report = fopen_s(&fp_report, report_file, "w");
	if (err_report != SUCCESS_INDICATION) {
		PrintLogBeforeInit(fp_debug, "Error when opening report file\n", logfile, NULL);
		return ERROR_INDICATION;
	}
	fclose(fp_report);
	return SUCCESS_INDICATION;
}

int Allocating(HANDLE **ptr_roommate_thread_handle, DWORD **ptr_roommate_thread_id,
	DWORD **ptr_roommate_exitcode, DWORD **ptr_roommate_wait_code, roommate_info **roommates_array, int num_of_roommate,
	FILE *fp_debug, char *logfile, FILE *fp_report, char *reportfile, machine_info **machine, timing_info **time_thread) {
	//allocations for the roommates array
	(*roommates_array) = (roommate_info*)malloc(sizeof(roommate_info)*num_of_roommate);
	(*ptr_roommate_thread_handle) = (HANDLE*)malloc(((num_of_roommate)+2) * sizeof(HANDLE));
	(*ptr_roommate_thread_id) = (DWORD*)malloc(((num_of_roommate)+2) * sizeof(DWORD));
	(*ptr_roommate_exitcode) = (DWORD*)malloc(((num_of_roommate)+2) * sizeof(DWORD));
	(*ptr_roommate_wait_code) = (DWORD*)malloc(((num_of_roommate)+2) * sizeof(DWORD));
	// allocations for the Laundry Machine 
	(*machine) = (machine_info*)malloc(sizeof(machine_info));
	//(*ptr_machine_thread_handle) = (HANDLE*)malloc(sizeof(HANDLE));
	//(*ptr_machine_thread_id) = (DWORD*)malloc(sizeof(DWORD));
	//(*ptr_machine_exitcode) = (DWORD*)malloc(sizeof(DWORD));
	//(*ptr_machine_wait_code) = (DWORD*)malloc(sizeof(DWORD));
	//allocation for time thread
	(*time_thread) = (timing_info*)malloc(sizeof(timing_info));
	//(*ptr_time_thread_handle) = (HANDLE*)malloc(sizeof(HANDLE));
	//(*ptr_time_thread_id) = (DWORD*)malloc(sizeof(DWORD));
	//(*ptr_time_exitcode) = (DWORD*)malloc(sizeof(DWORD));
	//(*ptr_time_wait_code) = (DWORD*)malloc(sizeof(DWORD));

	// checks if one of the allocations failed.
	if (*roommates_array == NULL || *machine == NULL || *ptr_roommate_thread_handle == NULL || *ptr_roommate_thread_id == NULL
		|| *ptr_roommate_exitcode == NULL || *ptr_roommate_wait_code == NULL || *time_thread == NULL) {
		PrintLogBeforeInit(fp_debug, "Allocating memmory failed\n", logfile, NULL);
		return ERROR_INDICATION;
	}
	return SUCCESS_INDICATION;
}


int InitThreads(roommate_info *roommates_array, int num_of_roommate, FILE *fp_debug, char *logfile, FILE *fp_report,
	char *reportfile, machine_info *machine, DWORD max_clothes_in_bin, DWORD td, timing_info *time_thread) {

	//init timing param
	time_thread->fp_debug = fp_debug;
	time_thread->fp_report = fp_report;
	time_thread->log_file = logfile;
	time_thread->report_file = reportfile;
	time_thread->time_flag = true;
	time_thread->time = START_TIME;
	time_thread->program_time = td;
	time_thread->mutex_debug_file.handle = CreateMutex(NULL, FALSE, "mutex_debug_file");
	if (time_thread->mutex_debug_file.handle == NULL) {
		if (!CloseHandle(time_thread->mutex_debug_file.handle)) {
			time_thread->mutex_debug_file.error_closing_handle = true;
			return (ERROR_INDICATION);
		}
	}
	time_thread->mutex_report_file.handle = CreateMutex(NULL, FALSE, "mutex_report_file");
	if (time_thread->mutex_report_file.handle == NULL) {
		if (!CloseHandle(time_thread->mutex_report_file.handle)) {
			PrintLog(fp_debug, "Error in the report file mutex definition\n", logfile, NULL, time_thread->mutex_debug_file);
			time_thread->mutex_report_file.error_closing_handle = true;
		}
	}
	time_thread->mutex_time.handle = CreateMutex(NULL, FALSE, "time_mutex");
	if (time_thread->mutex_time.handle == NULL) {
		PrintLog(fp_debug, "Error in the timing thread mutex definition\n", logfile, NULL, time_thread->mutex_debug_file);
		if (!CloseHandle(time_thread->mutex_time.handle)) {
			time_thread->mutex_time.error_closing_handle = true;
		}
	}
	time_thread->error_id_null = false;
	time_thread->error_alloc = false;
	time_thread->error_closing_handle = false;
	time_thread->error_thread_creation = false;
	time_thread->error_waiting = false;

	//init machine params 
	machine->bin_full = max_clothes_in_bin;
	machine->roommates = roommates_array;
	machine->time_thread = time_thread;
	machine->error_id_null = false;
	machine->error_in_alloc = false;
	machine->error_closing_handle = false;
	machine->error_thread_creation = false;
	machine->error_waiting = false;
	machine->fp_debug = fp_debug;
	machine->fp_report = fp_report;
	machine->report_file = reportfile;
	machine->log_file = logfile;

	machine->mutex_debug_file.handle = OpenMutex(SYNCHRONIZE, FALSE, "mutex_debug_file");
	if (machine->mutex_debug_file.handle == NULL) {
		PrintLog(fp_debug, "Error - in debug file mutex opening for the robot\n", logfile, NULL,time_thread->mutex_debug_file);
		if (!CloseHandle(machine->mutex_debug_file.handle)) {
			machine->mutex_debug_file.error_closing_handle = true;
		}
	}
	machine->mutex_report_file.handle = OpenMutex(SYNCHRONIZE, FALSE, "mutex_report_file");
	if (machine->mutex_report_file.handle == NULL) {
		PrintLog(fp_debug, "Error - in debug file mutex opening for the robot\n", logfile, NULL, time_thread->mutex_debug_file);
		if (!CloseHandle(machine->mutex_report_file.handle)) {
			machine->mutex_report_file.error_closing_handle = true;
		}
	}

	machine->semaphore_laundry_bin_full.handle = CreateSemaphore(NULL, machine->bin_full, machine->bin_full, "laundry_bin_semaphore_full");
	machine->num_of_roommates = num_of_roommate;
	if (machine->semaphore_laundry_bin_full.handle == NULL) {
		PrintLog(fp_debug, "Error - the Robot /t full laundry bin semaphore definition\n", logfile, NULL, time_thread->mutex_debug_file);
		if (!CloseHandle(machine->semaphore_laundry_bin_full.handle)) {
			machine->semaphore_laundry_bin_full.error_closing_handle = true;
		}
	}
	machine->semaphore_laundry_bin_empty.handle = CreateSemaphore(NULL, 0, machine->bin_full, "laundry_bin_semaphore_empty");
	if (machine->semaphore_laundry_bin_empty.handle == NULL) {
		PrintLog(fp_debug, "Error - the Robot /t empty laundry bin semaphore definition\n", logfile, NULL, time_thread->mutex_debug_file);
		if (!CloseHandle(machine->semaphore_laundry_bin_empty.handle)) {
			machine->semaphore_laundry_bin_empty.error_closing_handle = true;
		}
	}
	machine->mutex_laundry_bin.handle = CreateMutex(NULL, FALSE, "laundry_bin_mutex");
	if (machine->mutex_laundry_bin.handle == NULL) {
		PrintLog(fp_debug, "Error - the Robot /t laundry bin Mutex definition\n", logfile, NULL, time_thread->mutex_debug_file);
		if (!CloseHandle(machine->mutex_laundry_bin.handle)) {
			machine->mutex_laundry_bin.error_closing_handle = true;
		}
	}





	//initiate the roommates threads Parameters
	for (int jj = 0; jj < (num_of_roommate); jj++) {
		char mutex_name[MAXCHAR] = "closet_mutex_";
		char *mutex_ind = _itoa(jj, buffer, INT_BASE);
		strcat(mutex_name, mutex_ind);

		roommates_array[jj].error_closing_handle = false;
		roommates_array[jj].error_waiting = false;
		roommates_array[jj].error_alloc = false;
		roommates_array[jj].error_thread_creation = false;
		roommates_array[jj].error_id_null = false;
		roommates_array[jj].number_of_cloth_in_laundry = 0;
		roommates_array[jj].number_of_cloth_in_closet = roommates_array[jj].number_of_clothes - 1;
		roommates_array[jj].fp_debug = fp_debug;
		roommates_array[jj].fp_report = fp_report;
		roommates_array[jj].report_file = reportfile;
		roommates_array[jj].log_file = logfile;
		roommates_array[jj].time_thread = time_thread;

		roommates_array[jj].mutex_debug_file.handle = OpenMutex(SYNCHRONIZE, FALSE, "mutex_debug_file");
		if (roommates_array[jj].mutex_debug_file.handle == NULL) {
			PrintLog(fp_debug, "Error - in debug file mutex opening for the robot\n", logfile, NULL, time_thread->mutex_debug_file);
			if (!CloseHandle(roommates_array[jj].mutex_debug_file.handle)) {
				roommates_array[jj].mutex_debug_file.error_closing_handle = true;
			}
		}
		roommates_array[jj].mutex_report_file.handle = OpenMutex(SYNCHRONIZE, FALSE, "mutex_report_file");
		if (roommates_array[jj].mutex_report_file.handle == NULL) {
			PrintLog(fp_debug, "Error - in debug file mutex opening for the robot\n", logfile, NULL, time_thread->mutex_debug_file);
			if (!CloseHandle(roommates_array[jj].mutex_report_file.handle)) {
				roommates_array[jj].mutex_report_file.error_closing_handle = true;
			}
		}


		roommates_array[jj].semahore_closet_empty.handle = CreateSemaphore(NULL,
			0,
			roommates_array[jj].number_of_clothes - 1,
			NULL);
		if (roommates_array[jj].semahore_closet_empty.handle == NULL) {
			// report error and maybe close handle
			PrintLog(fp_debug, "Error - roommate number %s \t closet semaphore definition\n", logfile, _itoa(jj, buffer, INT_BASE), time_thread->mutex_debug_file);
			if (!CloseHandle(roommates_array[jj].semahore_closet_empty.handle)) {
				roommates_array[jj].semahore_closet_empty.error_closing_handle = true;
			}
		}

		roommates_array[jj].semahore_closet_full.handle = CreateSemaphore(NULL,
			roommates_array[jj].number_of_clothes - 1,
			roommates_array[jj].number_of_clothes - 1,
			NULL);
		if (roommates_array[jj].semahore_closet_full.handle == NULL) {
			// report error and maybe close handle
			PrintLog(fp_debug, "Error - roommate number %s \t closet semaphore definition\n", logfile, _itoa(jj, buffer, INT_BASE), time_thread->mutex_debug_file);
			if (!CloseHandle(roommates_array[jj].semahore_closet_full.handle)) {
				roommates_array[jj].semahore_closet_full.error_closing_handle = true;
			}
		}

		roommates_array[jj].mutex_closet.handle = CreateMutex(NULL, FALSE, mutex_name);
		if (roommates_array[jj].mutex_closet.handle == NULL) {
			PrintLog(fp_debug, "Error - roommate number %s \t closet mutex definition\n", logfile, _itoa(jj, buffer, INT_BASE), time_thread->mutex_debug_file);
			if (!CloseHandle(roommates_array[jj].mutex_closet.handle)) {
				roommates_array[jj].mutex_closet.error_closing_handle = true;
			}
		}

		//roommates_array[jj].mutex_time.handle = OpenMutex(SYNCHRONIZE, FALSE, "time_mutex");
		//if (roommates_array[jj].mutex_time.handle == NULL) {
		//	PrintLog(fp_debug, "Error - in time mutex opening for roommate number %s\n", logfile, _itoa(jj, buffer, INT_BASE));
		//	if (!CloseHandle(roommates_array[jj].mutex_time.handle)) {
		//		roommates_array[jj].mutex_time.error_closing_handle = true;
		//	}
		//}

		roommates_array[jj].mutex_laundry_bin.handle = OpenMutex(SYNCHRONIZE, FALSE, "laundry_bin_mutex");
		if (roommates_array[jj].mutex_laundry_bin.handle == NULL) {
			PrintLog(fp_debug, "Error - in laundry bin mutex opening for roommate number %s\n", logfile, _itoa(jj, buffer, INT_BASE), time_thread->mutex_debug_file);
			if (!CloseHandle(roommates_array[jj].mutex_laundry_bin.handle)) {
				roommates_array[jj].mutex_laundry_bin.error_closing_handle = true;
			}
		}

		roommates_array[jj].semaphore_laundry_bin_full.handle = OpenSemaphore(SYNCHRONIZE, FALSE, "laundry_bin_semaphore_full");
		if (roommates_array[jj].semaphore_laundry_bin_full.handle == NULL) {
			PrintLog(fp_debug, "Error - roommate number %s \t full laundry bin semaphore definition\n", logfile, _itoa(jj, buffer, INT_BASE), time_thread->mutex_debug_file);
			if (!CloseHandle(roommates_array[jj].semaphore_laundry_bin_full.handle)) {
				roommates_array[jj].semaphore_laundry_bin_full.error_closing_handle = true;
			}
		}

		roommates_array[jj].semaphore_laundry_bin_empty.handle = OpenSemaphore(SYNCHRONIZE, FALSE, "laundry_bin_semaphore_empty");
		if (roommates_array[jj].semaphore_laundry_bin_empty.handle == NULL) {
			PrintLog(fp_debug, "Error - roommate number %s \t empty laundry bin semaphore definition\n", logfile, _itoa(jj, buffer, INT_BASE), time_thread->mutex_debug_file);
			if (!CloseHandle(roommates_array[jj].semaphore_laundry_bin_empty.handle)) {
				roommates_array[jj].semaphore_laundry_bin_empty.error_closing_handle = true;
			}
		}

	}

	return SUCCESS_INDICATION;
}



/*------------------------------------------------------------------------------------------------------------------------------------------*/
/***********************************************************END OF FILE**********************************************************************/
/*------------------------------------------------------------------------------------------------------------------------------------------*/