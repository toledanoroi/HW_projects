/*
File Name	:	MainStruct.h
Authors		:	Roi Toledano	203700505
				Yarden Avraham	302249925
Project		:	Exercise 4 - x / o
Uses		:
Description :
*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Includes and Defines*/
#include <Windows.h>
#include <stdio.h>


#define ERROR_INDICATION -1
#define ERROR_NUM_ROOMMATES -1
#define SUCCESS_INDICATION 0
#define FAILURE_INDICATION 1
#define MAX_CHARS_IN_LINE 51 
#define MAXCHAR 4096 
#define INT_BASE 10
#define DELTA_TIME 1000

extern char buffer[MAXCHAR];
extern int total_clothes_in_laundry_bin;
extern bool machine_is_on;
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Structs defenitions */
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
/*Functions Declaration */