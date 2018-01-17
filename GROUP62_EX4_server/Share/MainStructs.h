/*
File Name	:	MainStructs.h
Authors		:	Roi Toledano	203700505
				Yarden Avraham	302249925
Project		:	Exercise 4 - x/o
Uses		:
Description :
*/


/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Includes and Defines*/
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>
#include <strsafe.h>
//#include <winsock2.h>
#//include <ws2tcpip.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")


#define SUCCESS_INDICATION 0
#define FAILURE_INDICATION 1
#define ERROR_INDICATION -1
#define PROGRAM_ERROR_VAL -1
#define PROGRAM_SUCCESS_VAL 0
#define MAX_CHARS_IN_LINE 51 
#define MAXCHAR 4096 
#define INT_BASE 10				// for _itoa function
#define STRINGS_ARE_EQUAL( Str1, Str2 ) ( strcmp( (Str1), (Str2) ) == 0 )
#define DELTA_TIME 1000			//1s
/*------------------------------------------------------------------------------------------------------------------------------------------*/
// global variables
extern char buffer[MAXCHAR];

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
} mutex;

/*------------------------------------------------------------------------------------------------------------------------------------------*/
// Functions declarations
int FilesInit(FILE *fp, char *f_name);
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
TransferResult_t SendBuffer(const char* Buffer, int BytesToSend, SOCKET sd);

/**
* SendString() uses a socket to send a string.
* Str - the string to send.
* sd - the socket used for communication.
*/
TransferResult_t SendString(const char *Str, SOCKET sd);

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
TransferResult_t ReceiveBuffer(char* OutputBuffer, int RemainingBytesToReceive, SOCKET sd);

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
TransferResult_t ReceiveString(char** OutputStrPtr, SOCKET sd);


/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/***********************************************************END OF FILE**********************************************************************/
/*------------------------------------------------------------------------------------------------------------------------------------------*/




