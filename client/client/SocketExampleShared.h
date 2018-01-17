/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
/* 
 This file was written for instruction purposes for the 
 course "Introduction to Systems Programming" at Tel-Aviv
 University, School of Electrical Engineering, Winter 2011, 
 by Amnon Drory.
*/
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//#ifndef SOCKET_EXAMPLE_SHARED_H
//#define SOCKET_EXAMPLE_SHARED_H
#include "SocketSendRecvTools.h"
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#define SERVER_ADDRESS_STR "127.0.0.1"
#define SERVER_PORT 2345
#define NOT_ACCEPTED 1
#define DECLINED -888
#define BUFFER_SIZE 1

// winning_status
#define WINNER -10
#define DIDNT_FINISH 0
#define LOSSER 10 
#define NO_ONE -1


#define STRINGS_ARE_EQUAL( Str1, Str2 ) ( strcmp( (Str1), (Str2) ) == 0 )
#define SUBSTRINGS_ARE_EQUAL( Str1, Str2, n) ( strncmp( (Str1), (Str2), (n)) == 0 )
#define PLAY_LEN 4


typedef struct {
	int opcode;
	char *params[4];
	bool no_param;

} clientmessage;

typedef struct {
	int opcode;
	char *params[4];
	bool no_param;

} servermessage;

typedef struct {
	SOCKET user_socket;
	char *user_name;
	int index;
	char user_sign;
	char *client_ip;
	int client_port;
	int server_port;
	bool my_turn;
	int is_accept;
	char *server_ip;
	FILE *fp_log;
	char *client_log;
	bool exit_flag;
	bool game_started;
	bool is_alive;
	int num_of_players;
	int winning_status;
	
	//semaphore msg_in_full;
	//semaphore msg_in_empty;
	//semaphore msg_out_full;
	//semaphore msg_out_empty;
	mutex mutex_buffer_full;
	mutex mutex_buffer_empty;
	
	//buffers
	clientmessage *msg_out;
	servermessage *serv_msg_in;
	char *msg_in;

}client;


//#endif // SOCKET_EXAMPLE_SHARED_H