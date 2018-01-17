/*
File Name	:	main.c
Authors		:	Roi Toledano	203700505
Yarden Avraham	302249925
Project		:	Exercise 4 - x/o - server
Uses		:
Description :
*/

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Includes and Defines*/
#include "ServerTx.h"


int InitAndAlloc(server_info **server_params, char *server_log, char *server_port, HANDLE **h_socket_thread, LPDWORD **socket_thread_id);

int main(int argc, char* argv[]) {

	HANDLE h_socket_thread = NULL;
	LPDWORD socket_thread_id = NULL;
	server_info *server_params = NULL;
	DWORD WaitRes;
	int is_init, is_server;

	/*program input check*/
	if (argc != 3) {
		printf("Error! number of argument to main is wrong\n");
		return PROGRAM_ERROR_VAL;
	}

	/*allocating memory for the server and Initiate his parameters */
	is_init  = InitAndAlloc(&server_params, argv[1], argv[2], &h_socket_thread, &socket_thread_id);
	if (is_init == ERROR_INDICATION) {
		printf("Error - init server parameters\n");
		return ERROR_INDICATION;
	}

	/*create the manager thread*/ 
	is_server = ServerMainFunc(server_params);
	//CreateThreadSimple(ServerMainFunc, server_params, &socket_thread_id, &h_socket_thread);

	//WaitRes = WaitForSingleObject(h_socket_thread, INFINITE);
	//if (WaitRes != WAIT_OBJECT_0) {
	//	printf("Main function | error while waiting to p_thread_handles\n");
	//	exit(PROGRAM_ERROR_VAL);
	//}

	//if (CloseHandle(h_socket_thread) == 0) {
	//	printf("Main function | error while trying to close p_h_thread_manager handle\n");
	//	exit(PROGRAM_ERROR_VAL);
	//}
	printf("goodbye\n");

	free(server_params);
	return PROGRAM_SUCCESS_VAL;
}

int InitAndAlloc(server_info **server_params, char *server_log, char *server_port, HANDLE **h_socket_thread, LPDWORD **socket_thread_id) {
	
	int is_files;
	
	(*h_socket_thread) = (HANDLE*)malloc(sizeof(HANDLE));
	(*socket_thread_id) = (LPDWORD*)malloc(sizeof(LPDWORD));
	(*server_params) = (server_info*)malloc(sizeof(server_info));
	
	if ((*server_params == NULL) || (h_socket_thread == NULL) || (socket_thread_id == NULL)){
		printf("Erorr - Allocation memory\n");
		return ERROR_INDICATION;
	}
	(*server_params)->server_port = atoi(server_port);
	(*server_params)->server_log = server_log;
	(*server_params)->server_ip = SERVER_ADDRESS_STR;

	is_files = FilesInit((*server_params)->report_file, (*server_params)->server_log);
	if (is_files == ERROR_INDICATION) {
		printf("Error - init files\n");
		return ERROR_INDICATION;
	}

}

