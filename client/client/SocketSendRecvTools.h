/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
/* 
 This file was written for instruction purposes for the 
 course "Introduction to Systems Programming" at Tel-Aviv
 University, School of Electrical Engineering, Winter 2011, 
 by Amnon Drory.
*/
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//#ifndef SOCKET_SEND_RECV_TOOLS_H
//#define SOCKET_SEND_RECV_TOOLS_H
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <strsafe.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define SUCCESS_INDICATION 0
#define FAILURE_INDICATION 1
#define ERROR_INDICATION -1
#define PROGRAM_ERROR_VAL -1
#define PROGRAM_SUCCESS_VAL 0
#define MAX_CHARS_IN_LINE 51
#define INT_BASE 10				// for _itoa function
#define DELTA_TIME 1000			//1s
#define OCCUPPIED -1


//define TCP protocol 
#define NUM_OF_CLIENT_MESSAGES 5
#define NUM_OF_SERVER_MESSAGES 11
#define MAX_MESSAGETYPE_CHAR 20
#define MAX_MESSAGEPARAMS_CHAR 20
#define EMPTY_PARAM "empty@"
//message type


//= { 1,2,0,2,0,1,1,1,1,1,1 };
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
// global variables
extern char *client_messages[NUM_OF_CLIENT_MESSAGES];
//= { "NEW_USER_REQUEST", "PLAY_REQUEST", "USER_LIST_QUERY", "GAME_STATE_QUERY", "BOARD_VIEW_QUERY" };
extern int num_of_client_message_params[5];


extern char *server_messages[NUM_OF_SERVER_MESSAGES];
//= { "NEW_USER_ACCEPTED", "NEW_USER_DECLINED", "GAME_STARTED", "BOARD_VIEW", "TURN_SWITCH", "PLAY_ACCEPTED", "PLAY_DECLINED", "GAME_ENDED", "USER_LIST_REPLAY", "GAME_STATE_REPLAY", "BOARD_VIEW_REPLAY" };
extern int num_of_server_message_params[11];

extern char buffer[MAXCHAR];
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Structs defenitions*/

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
	bool is_first;
} mutex;

/*------------------------------------------------------------------*/
// Functions declarations
int FilesInit(FILE *fp, char *f_name);
int PrintLog(FILE *fp, char str_out[MAXCHAR], char* filename, char* substring1, char* substring2);
char *replace_str(char *str, char *orig, char *rep);
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

typedef enum { TRNS_FAILED, TRNS_DISCONNECTED, TRNS_SUCCEEDED } TransferResult_t;

/**
 * SendBuffer() uses a socket to send a buffer.
 *
 * Accepts:
 * -------
 * Buffer - the buffer containing the data to be sent.
 * BytesToSend - the number of bytes from the Buffer to send.
 * sd - the socket used for communication.
 *
 * Returns:
 * -------
 * TRNS_SUCCEEDED - if sending succeeded
 * TRNS_FAILED - otherwise
 */
TransferResult_t SendBuffer( const char* Buffer, int BytesToSend, SOCKET sd );

/**
 * SendString() uses a socket to send a string.
 * Str - the string to send. 
 * sd - the socket used for communication.
 */ 
TransferResult_t SendString( const char *Str, SOCKET sd );

/**
 * Accepts:
 * -------
 * ReceiveBuffer() uses a socket to receive a buffer.
 * OutputBuffer - pointer to a buffer into which data will be written
 * OutputBufferSize - size in bytes of Output Buffer
 * BytesReceivedPtr - output parameter. if function returns TRNS_SUCCEEDED, then this 
 *					  will point at an int containing the number of bytes received.
 * sd - the socket used for communication.
 *
 * Returns:
 * -------
 * TRNS_SUCCEEDED - if receiving succeeded
 * TRNS_DISCONNECTED - if the socket was disconnected
 * TRNS_FAILED - otherwise
 */ 
TransferResult_t ReceiveBuffer( char* OutputBuffer, int RemainingBytesToReceive, SOCKET sd );

/**
 * ReceiveString() uses a socket to receive a string, and stores it in dynamic memory.
 * 
 * Accepts:
 * -------
 * OutputStrPtr - a pointer to a char-pointer that is initialized to NULL, as in:
 *
 *		char *Buffer = NULL;
 *		ReceiveString( &Buffer, ___ );
 *
 * a dynamically allocated string will be created, and (*OutputStrPtr) will point to it.
 * 
 * sd - the socket used for communication.
 * 
 * Returns:
 * -------
 * TRNS_SUCCEEDED - if receiving and memory allocation succeeded
 * TRNS_DISCONNECTED - if the socket was disconnected
 * TRNS_FAILED - otherwise
 */ 
TransferResult_t ReceiveString( char** OutputStrPtr, SOCKET sd );


/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/



//#endif // SOCKET_SEND_RECV_TOOLS_H