/*
Authors		:	Roi Toledano	203700505
				Yarden Avraham	302249925
Project		:	Exercise 3 - Robotic Laundry Machine
Uses		:
Description : main of the process; read and write files, manage threads opening.
*/

#include "machine.h"



int PrintLogBeforeInit(FILE *fp, char *str_out, char* filename, char* substring) {


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


/*int GetNumLines
Parameters:		fp - A pointer to the the file we wish to read.
				Line - A pointer to an int parameter, this will be updated to hold
					   the number of lines in the file (therefore its a pointer).
				fp_debug - A pointer to the debug file, if something goes wrong.
				logfile - A string that represents the file's name of the debug file.
				file_path - A string that represents the file's name of the file we wish to read.
Returns :		An int variable - Success = 0 and Error = -1
Description :	A function that counts the number of lines in the input file and
				updates the Line parameter.*/
int GetNumLines(FILE *fp, int *Line, FILE *fp_debug, char *logfile, char *file_path) {

	errno_t err = fopen_s(&fp, file_path, "r");
	if (!err) {
		PrintLogBeforeInit(fp_debug, "the file was opened\n", logfile, NULL);
		//printf("the file was opened\n");
	}
	else {
		PrintLogBeforeInit(fp_debug, "Error occured in file openeing\n", logfile, NULL);
		//printf("Error occured in file openeing\n");
		return ERROR_INDICATION;
	}
	if (fp == NULL) {
		PrintLogBeforeInit(fp_debug, "File is not exist\n", logfile, NULL);
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



/*int GetNumLines
Parameters:		waiting_code - A DWORD parameter that holds the exit code of a thread.
				fp_debug - A pointer to the debug file, if something goes wrong.
				logfile - A string that represents the file's name of the debug file.
Returns :		An int variable - Success = 0 and Error = -1
Description :	A function that prints to the debug file a message according to the
				waiting_code value.*/
void WaitingStatus(DWORD waiting_code, FILE *fp_debug, char *logfile) {
	// there is an option to add the id 
	switch (waiting_code)
	{
	case WAIT_TIMEOUT :
		PrintLogBeforeInit(fp_debug, "Timeout has elapsed, the object is not signaled.\n", logfile,NULL);
		break;
	case WAIT_FAILED:
		PrintLogBeforeInit(fp_debug, "WaitForSignalObject has failed.\n", logfile, NULL);
		break;
	case WAIT_ABANDONED:
		PrintLogBeforeInit(fp_debug, "The handle to the object was not released by the thread before the thread was terminated.\n", logfile, NULL);
		break;
	default:
		break;
	}
}

/*int replace_str
Parameters:		str - A string that contains the orig string.
				orig - A string we wish to replace from the str string.
				rep - A string that replaces orig string.
Returns :		A fixed string with the %s that was replaced with the the rep string. 
Description :	A function that switchs %s that is in a current string to a different string.*/
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

/*int PrintLog
Parameters:		fp	- A pointer to the the file we wish to write in it.
str_out - The string we wish to write in the file.
filename - The string that represents the files name.
substring - The string that %s is replaced with.
Returns:		An int variable			- Success = 0 and Error = -1
Description:	A function that writes to the specific file we send it.*/
int PrintLog(FILE *fp, char *str_out, char* filename, char* substring, mutex file_mutex) {

	// file_mutex.waiting_code = WaitForSingleObject(file_mutex.handle, INFINITE);
	//if (file_mutex.waiting_code != WAIT_OBJECT_0) {
	//	return (ERROR_INDICATION);
	//}
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
	//if (!(ReleaseMutex(file_mutex.handle))) {
	//	return (ERROR_INDICATION);
	//}

	return SUCCESS_INDICATION;
}
