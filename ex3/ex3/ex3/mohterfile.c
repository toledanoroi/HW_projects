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

