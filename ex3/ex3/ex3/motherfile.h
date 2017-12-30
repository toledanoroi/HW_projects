/*
Authors		:	Roi Toledano	203700505
Yarden Avraham	302249925
Project		:	Exercise 3 - Robotic Laundry Machine
Uses		:
Description :
*/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>


#define ERROR_INDICATION -1
#define ERROR_NUM_ROOMMATES -1
#define SUCCESS_INDICATION 0
#define FAILURE_INDICATION 1
#define MAX_CHARS_IN_LINE 51 
#define MAXCHAR 4096 
#define INT_BASE 10

extern char buffer[MAXCHAR];
extern int total_clothes_in_laundry_bin;
extern bool machine_is_on;

typedef struct {
	bool error_closing_handle;
	DWORD waiting_code;
	bool error_waiting;
	HANDLE handle;
} semaphore;

typedef struct {
	bool error_closing_handle;
	DWORD waiting_code;
	bool error_waiting;
	HANDLE handle;
} mutex;


/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Mutexes and Semaphores*/
HANDLE mutex_debug_file;

/*------------------------------------------------------------------------------------------------------------------------------------------*/



int PrintLog(FILE *fp, char *str_out, char* filename, char* substring);
int GetNumLines(FILE *fp, int *Line, FILE *fp_debug, char *logfile,char *file_path);
//int GetProgramParameters(char *f_path_param, DWORD *td, DWORD *max_clothes_in_bin, roommate_info *roommates_array, FILE *fp_debug, char* logfile, int *num_of_roommates);
//int AllocateMemForThreadsAndInit(HANDLE *ptr_roommate_thread_handle, DWORD *ptr_roommate_thread_id,
//	DWORD *ptr_roommate_exitcode, DWORD *ptr_roommate_wait_code, roommate_info *roommates_array, int *num_of_roommate,
//	FILE *fp_debug, char *logfile, machine_info *machine, DWORD max_clothes_in_bin);
void WaitingStatus(DWORD waiting_code, FILE *fp_debug, char *logfile);
char *replace_str(char *str, char *orig, char *rep);

