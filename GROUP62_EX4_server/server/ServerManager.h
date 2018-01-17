/*
File Name	:	ServerManager.h
Authors		:	Roi Toledano	203700505
Yarden Avraham	302249925
Project		:	Exercise 4 - x/o
Uses		:
Description :
*/

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Includes and Defines*/

#include "Demodulator.h"

#define SERVER_ADDRESS_STR "127.0.0.1"
#define	NUM_OF_THREADS_IN_SERVER 4
#define MAX_PLAYERS	2

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Structs defenitions*/

typedef struct {
	SOCKET user_socket;
	char *user_name;
	char user_sign;
	char *client_ip;
	int client_port;

}client;

typedef struct {
	SOCKET main_socket;
	client userlist[2];
}connection_mparams;

typedef struct {
	bool error_in_alloc;
	bool error_id_null;
	bool error_closing_handle;
	bool error_waiting;
	bool error_thread_creation;
	FILE *fp_errors;
	FILE *fp_log;
	char *report_file;
	char *server_log;
	/*mutexes
	mutex mutex_1;
	mutex mutex_2;
	mutex mutex_time;*/

	int server_port;
	int server_ip;
} server_info;

typedef enum {
	SERVER_CODE_SUCCESS,
	SERVER_CODE_FAILED,
	SERVER_CODE_ERROR = -1

} server_return_code;




/*------------------------------------------------------------------------------------------------------------------------------------------*/
// Functions declarations

void CreateASocket(int server_port, SOCKET *main_socket);
DWORD WINAPI Connection_Manager(LPVOID lp_param);
int ServerMainFunc(server_info *server_params);
//DWORD WINAPI ServerMainFunc(LPVOID lp_params);
HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE server_start_routine, server_info *server_params, DWORD *server_thread_id);
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/***********************************************************END OF FILE**********************************************************************/
/*------------------------------------------------------------------------------------------------------------------------------------------*/