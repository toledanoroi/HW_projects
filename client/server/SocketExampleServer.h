/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
/* 
 This file was written for instruction purposes for the 
 course "Introduction to Systems Programming" at Tel-Aviv
 University, School of Electrical Engineering, Winter 2011, 
 by Amnon Drory.
*/
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
#include "SocketExampleShared.h"



#define NUM_OF_WORKER_THREADS 2
#define MAX_LOOPS 3
#define SEND_STR_SIZE 35

//#ifndef SOCKET_EXAMPLE_SERVER_H
//#define SOCKET_EXAMPLE_SERVER_H

//typedef struct {
//	SOCKET user_socket;
//	char *user_name;
//	int index;
//	char user_sign;
//	char *client_ip;
//	int client_port;
//	bool my_turn;
//
//}client;


typedef struct {
	//bool error_in_alloc;
	//bool error_id_null;
	//bool error_closing_handle;
	//bool error_waiting;
	//bool error_thread_creation;
	FILE *fp_log;
	char *server_log;
	int server_port;
	char *server_ip;
	char *board;
	int board_game[3][3];
	SOCKET main_socket;
	SOCKET first_socket;
	client userlist[2];
	bool is_first;
	//bool is_second;
	int turn_ind;
	int who_won;
	bool is_start;
	int num_of_players;
	bool game_ended;
	/*mutexes
	mutex mutex_1;
	mutex mutex_2;
	mutex mutex_time;*/

	/// messages pointer

} server_info;

typedef struct {
	server_info *server_params;
	SOCKET client_socket;


}connection_thread_info;
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

void MainServer(server_info *server_params);
HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE server_start_routine, connection_thread_info *user_socket, DWORD *server_thread_id);
static int FindFirstUnusedThreadSlot();
static void CleanupWorkerThreads();
static DWORD ServiceThread(connection_thread_info *server_params);
int InitAndAlloc(server_info **server_params, char *server_log, char *server_port, HANDLE **h_socket_thread, LPDWORD **socket_thread_id);
int ParsingServer(char *AcceptedStr, clientmessage **message);
int ServerMessageBuilder(int opcode, char *param1, char* param2, char* param3, char* param4, servermessage **msg);
int ServerMessageGenarator(servermessage *message, char **message_string);
int ServerActionAfterClientMessage(clientmessage *message, servermessage **message_back, server_info **server_params,SOCKET user_socket);
void UpdateBoardGameAndCheckGameStatus(server_info **server_params, int x, int y, char user_sign);
void Board2String(char* board, int board_game[3][3]);
int CheckTicTacToe(int board[3][3]);
int InitConnectionThread(connection_thread_info *con_prm, server_info *server_params, SOCKET my_socket);
int SendStringErrorCheck(char* SendStr, SOCKET t_socket);
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//#endif // SOCKET_EXAMPLE_SERVER_H