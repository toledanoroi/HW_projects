/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
/* 
 This file was written for instruction purposes for the 
 course "Introduction to Systems Programming" at Tel-Aviv
 University, School of Electrical Engineering.
Last updated by Amnon Drory, Winter 2011.
 */
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#define _WINSOCK_DEPRECATED_NO_WARNINGS

//#include <stdio.h>
//#include <string.h>
//#include <winsock2.h>
#include "SocketExampleClient.h"

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

SOCKET m_socket;
char *client_messages[NUM_OF_CLIENT_MESSAGES] = { "NEW_USER_REQUEST", "PLAY_REQUEST", "USER_LIST_QUERY", "GAME_STATE_QUERY", "BOARD_VIEW_QUERY" };
int num_of_client_message_params[5] = { 1,2,0,0,0 };
char buffer[MAXCHAR];

char *server_messages[NUM_OF_SERVER_MESSAGES] = { "NEW_USER_ACCEPTED", "NEW_USER_DECLINED", "GAME_STARTED", "BOARD_VIEW", "TURN_SWITCH", "PLAY_ACCEPTED", "PLAY_DECLINED", "GAME_ENDED", "USER_LIST_REPLAY", "GAME_STATE_REPLAY", "BOARD_VIEW_REPLAY" };
int num_of_server_message_params[11] = {2,1,0,1,2,0,1,1,4,0,2};
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//Reading data coming from the server
static DWORD RecvDataThread(client *my_client)
{
	//client *my_client = (client*)LpParams;
	
	TransferResult_t RecvRes;
	
	while (my_client->is_alive == true) {
		char *tmp_msg = NULL;
		//char *AcceptedStr = NULL;
		//RecvRes = ReceiveString( &AcceptedStr , my_client->user_socket );
		RecvRes = ReceiveString(&tmp_msg , my_client->user_socket);
		(my_client->msg_in) = strcpy((my_client->msg_in), tmp_msg);
		if ( RecvRes == TRNS_FAILED )
		{
			printf("Socket error while trying to write data to socket\n");
			return 0x555;
		}
		else if ( RecvRes == TRNS_DISCONNECTED )
		{
			printf("Server closed connection. Bye!\n");
			return 0x555;
		}
		else
		{
			//check what is payload definition
			if (PrintLog(my_client->fp_log, "Recieved from server: %s1", my_client->client_log, my_client->msg_in, NULL)) {
				printf("error - print Log\n");
			}
			if (ParsingClient(my_client->msg_in, &(my_client->serv_msg_in))) {
				printf("error parsing\n");
			}

			// switch case for server messages 
			//Printing_server_message();
			//plotting to cmd

			switch (my_client->serv_msg_in->opcode)
			{
			case (0):
				//NEW_USER_ACCEPTED
				printf("the user: %s Accepted\n", my_client->user_name);
				PrintLog(my_client->fp_log, "the user: %s1 Accepted\n", my_client->client_log, my_client->user_name, NULL);
				my_client->user_sign = (my_client->serv_msg_in->params[0])[0];
				my_client->num_of_players = atoi(my_client->serv_msg_in->params[1]);
				
			case (1):
				//NEW_USER_DECLINED
				printf("Request to join was refused\n");
				PrintLog(my_client->fp_log, "Request to join was refused\n", my_client->client_log, NULL, NULL);
				my_client->is_alive = false;
			case (2):
				//GAME_STARTED
				printf("Game is on!\n");
				PrintLog(my_client->fp_log, "Game is on!\n", my_client->client_log, NULL, NULL);
				my_client->game_started = true;
			case (3):
				//BOARD_VIEW
				printf("%s", my_client->serv_msg_in->params[0]);
			case(4):
				//TURN SWITCH
				printf("%s's turn (%s)\n", my_client->serv_msg_in->params[0], my_client->serv_msg_in->params[1]);
				PrintLog(my_client->fp_log, "%s1's turn (%s2)\n", my_client->client_log, my_client->serv_msg_in->params[0], my_client->serv_msg_in->params[1]);
				if (STRINGS_ARE_EQUAL(my_client->serv_msg_in->params[0], my_client->user_name)){
					my_client->my_turn = true;
				}
				else
					my_client->my_turn = false;
			case (5):
				//PLAY_ACCEPTED
				printf("Well played\n");
				PrintLog(my_client->fp_log, "Well played\n", my_client->client_log, NULL, NULL);
			case (6):
				//PLAY DECLINED
				printf("Error: %s\n", my_client->serv_msg_in->params[0]);
				PrintLog(my_client->fp_log, "Error: %s1\n", my_client->client_log, my_client->serv_msg_in->params[0], NULL);
			case (7):
				//GAME_ENDED
				if (my_client->serv_msg_in->params[1] == NULL) {
					printf("Game ended. The winner is %s!\n", my_client->serv_msg_in->params[0]);
					PrintLog(my_client->fp_log, "Game ended. The winner is %s!\n", my_client->client_log, my_client->serv_msg_in->params[0], NULL);
					if (STRINGS_ARE_EQUAL(my_client->serv_msg_in->params[0], my_client->user_name)) {
						my_client->winning_status = WINNER;
					}
					else
						my_client->winning_status = LOSSER;
				}
				else {
					printf("Game ended. Everybody wins!\n");
					PrintLog(my_client->fp_log, "Game ended. Everybody wins!\n", my_client->client_log, NULL, NULL);
					my_client->winning_status = WINNER;
				}
				my_client->is_alive = false;

			case (8):
				//USER_LIST_REPLY
				if (my_client->serv_msg_in->params[2] == NULL) {
					printf("Players:%s:%s\n", my_client->serv_msg_in->params[1], my_client->serv_msg_in->params[0]);
					PrintLog(my_client->fp_log, "Players:%s1:%s2\n", my_client->client_log, my_client->serv_msg_in->params[1], my_client->serv_msg_in->params[0]);
				}
				else {
					printf("Players:%s:%s,", my_client->serv_msg_in->params[1], my_client->serv_msg_in->params[0]);
					PrintLog(my_client->fp_log, "Players:%s1:%s2,", my_client->client_log, my_client->serv_msg_in->params[1], my_client->serv_msg_in->params[0]);
					printf("Players:%s:%s\n", my_client->serv_msg_in->params[3], my_client->serv_msg_in->params[2]);
					PrintLog(my_client->fp_log, "Players:%s1:%s2\n", my_client->client_log, my_client->serv_msg_in->params[3], my_client->serv_msg_in->params[2]);

				}
			case (9):
				//GAME_STATE_REPLY
				if (my_client->serv_msg_in->params[0] == NULL) {
					printf("Game has not started\n");
					PrintLog(my_client->fp_log, "Game has not started\n", my_client->client_log,NULL, NULL);
				}
				else if (STRINGS_ARE_EQUAL(my_client->serv_msg_in->params[0], "end")) {// error client should be off 
					printf("Error: Game has already ended\n");
					PrintLog(my_client->fp_log, "Error: Game has already ended\n", my_client->client_log, NULL, NULL);
				}
				else{
					printf("%s's turn (%s)\n", my_client->serv_msg_in->params[0], my_client->serv_msg_in->params[1]);
					PrintLog(my_client->fp_log, "%s1's turn (%s2)\n", my_client->client_log, my_client->serv_msg_in->params[0], my_client->serv_msg_in->params[1]);
				}
			case (10):
				//BOARD_VIEW_REPLY
				if (my_client->serv_msg_in->params[0] == NULL) {
					printf("| | | |\n| | | |\n| | | |\n");
				}
				else
					printf("%s\n", my_client->serv_msg_in->params[0]);	
			default:
				break;
			}


			// if user declined client->is_alive = false, client->exit_flag = true; client->is_approve = declined;


		}
		//free(AcceptedStr);
	}

	return 0;
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//Sending data to the server
static DWORD SendDataThread(client *my_client)
{
	//client *my_client = (client*)LpParams;
	TransferResult_t SendRes;
	clientmessage *msg = NULL;
	msg = (clientmessage*)malloc(sizeof(clientmessage));
	for (int i = 0; i < 4; i++) {
		msg->params[i] = (char*)malloc(sizeof(char)*MAXCHAR);
	}
	char *msg_out = NULL;
	msg_out = (char*)malloc(sizeof(char)*MAXCHAR);

	//build a message struct 
	if (ClientMessageBuilder(0, my_client->user_name, my_client->user_name, NULL, NULL,&msg)) {
		printf("error - ClientMessageBuilder\n");
	}
	
	// message struct to a string
	if (ClientMessageGenarator(msg, &msg_out)) {
		printf("error - ClientMessageGenerator\n");
	}
	
	SendRes = SendString(msg_out, my_client->user_socket);
	//error checking 

	//check what is payload definition
	PrintLog(my_client->fp_log, "Sent to server: %s1", my_client->client_log, msg_out, NULL);

	while (my_client->is_accept != SUCCESS_INDICATION){
		if (my_client->is_accept == DECLINED) {
			my_client->is_alive = false;
		}
		Sleep(100);
	}
	
	while (my_client->mutex_buffer_full.is_first) {
		Sleep(100);
	}
	while (my_client->is_alive) 
	{
		printf("waiting for full mutex\n");
		my_client->mutex_buffer_full.waiting_code = WaitForSingleObject(my_client->mutex_buffer_full.handle, INFINITE);
		//error checking
		if (my_client->mutex_buffer_full.waiting_code != WAIT_OBJECT_0) {
			printf("Error mutex full\n");
			return (ERROR_INDICATION);
		}
		printf("I got the full mutex\n");
		if (ClientMessageGenarator(my_client->msg_out, &msg_out)) {
			printf("error - ClientMessageGenerator\n");
		}
		SendRes = SendString(msg_out, m_socket);
	
		if ( SendRes == TRNS_FAILED ) 
		{
			printf("Socket error while trying to write data to socket\n");
			return 0x555;
		}

		if (!(ReleaseMutex(my_client->mutex_buffer_empty.handle))) {
			printf("Error - release mutex empty\n");
			return ERROR_INDICATION;
		}
			
	}
	return SUCCESS_INDICATION;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
static DWORD I_O_Main(client *my_client)
{

	char str[MAXCHAR];
	char row, col;

	while (my_client->is_accept != SUCCESS_INDICATION) {
		if (my_client->is_accept == DECLINED) {
			my_client->is_alive = false;
		}
		Sleep(100);
	}

	while (my_client->is_alive)
	{
		bool flag = true;
		my_client->mutex_buffer_empty.waiting_code = WaitForSingleObject(my_client->mutex_buffer_empty.handle, INFINITE);
		if (my_client->mutex_buffer_empty.waiting_code != WAIT_OBJECT_0) {
			printf("Error mutex empty\n");
			return (ERROR_INDICATION);
		}
		if (my_client->mutex_buffer_full.is_first == true) {
			my_client->mutex_buffer_full.waiting_code = WaitForSingleObject(my_client->mutex_buffer_full.handle, INFINITE);
			if (my_client->mutex_buffer_full.waiting_code != WAIT_OBJECT_0) {
				printf("Error mutex full\n");
				return (ERROR_INDICATION);
			}
			my_client->mutex_buffer_full.is_first = false;
		}
		printf("please Enter a legal command:\n");
		gets_s(str, sizeof(str)); //Reading a string from the keyboard

		
		if (STRINGS_ARE_EQUAL(str, "exit")) {
			my_client->is_alive = false;
			printf("client wants exit.\nGoodbye!\n");
			return SUCCESS_INDICATION;
		}	
		else if (STRINGS_ARE_EQUAL(str, "players")) {
			//signal to the sending thread to send USER_LIST_QUERY
			ClientMessageBuilder(2, my_client->user_name, NULL, NULL, NULL, &(my_client->msg_out));
		}
		else if (STRINGS_ARE_EQUAL(str, "state")) {
			ClientMessageBuilder(3, my_client->user_name,NULL,NULL,NULL, &(my_client->msg_out));
		}
		else if (STRINGS_ARE_EQUAL(str, "board")) {
			//signal to the sending thread to send BOARD_VIEW_QUERY
			ClientMessageBuilder(4, my_client->user_name, NULL, NULL, NULL, &(my_client->msg_out));
		}
		else if (SUBSTRINGS_ARE_EQUAL(str,"play",PLAY_LEN)) {
			row = str[5];
			col = str[7];
			ClientMessageBuilder(1, my_client->user_name, (char*)row, (char*)col, NULL, &(my_client->msg_out));
		}
		else {
			printf("Error: Illegal command\n");
			PrintLog(my_client->fp_log, "Error: Illegal command\n", my_client->client_log, NULL, NULL);
			flag = false;
		}
		
		if (flag) {
			if (!(ReleaseMutex(my_client->mutex_buffer_full.handle))) {
				printf("Error - release mutex full\n");
				return ERROR_INDICATION;
			}
		}
		else {
			if (!(ReleaseMutex(my_client->mutex_buffer_empty.handle))) {
				printf("Error - release mutex full\n");
				return ERROR_INDICATION;
			}
		}

	}

	return SUCCESS_INDICATION;
}

int InitClient(char* log_file, char *server_ip, int server_port, char *user_name, FILE *fp_log_client, client **my_client, SOCKET m_socket) {

	(*my_client) = (client*)malloc(sizeof(client));
	(*my_client)->msg_in = (char*)malloc(sizeof(char)*MAXCHAR);
	(*my_client)->msg_out = (clientmessage*)malloc(sizeof(clientmessage));
	(*my_client)->serv_msg_in = (servermessage*)malloc(sizeof(servermessage));
	if (((*my_client) == NULL) || ((*my_client)->msg_in == NULL) || ((*my_client)->msg_out == NULL) || ((*my_client)->serv_msg_in == NULL)) {
		printf("Error - allocating client struct");
	}

	(*my_client)->client_log = log_file;
	(*my_client)->user_name = user_name;
	(*my_client)->server_ip = server_ip;
	(*my_client)->server_port = server_port;
	(*my_client)->fp_log = fp_log_client;
	(*my_client)->user_socket = m_socket;
	(*my_client)->is_accept = NOT_ACCEPTED;
	(*my_client)->exit_flag = false;
	(*my_client)->game_started = false;
	(*my_client)->winning_status = DIDNT_FINISH;
	(*my_client)->is_alive = true;

	(*my_client)->mutex_buffer_full.handle = CreateMutex(NULL, FALSE, "mutex_buffer_full");
	if ((*my_client)->mutex_buffer_full.handle == NULL) {
		if (!CloseHandle((*my_client)->mutex_buffer_full.handle)) {
			PrintLog((*my_client)->fp_log, "Error in the mutex_buffer_full definition\n", (*my_client)->client_log, NULL, NULL);
			(*my_client)->mutex_buffer_full.error_closing_handle = true;
		}
	}
	(*my_client)->mutex_buffer_full.is_first = true;

	(*my_client)->mutex_buffer_empty.handle = CreateMutex(NULL, FALSE, "mutex_buffer_empty");
	if ((*my_client)->mutex_buffer_empty.handle == NULL) {
		if (!CloseHandle((*my_client)->mutex_buffer_empty.handle)) {
			PrintLog((*my_client)->fp_log, "Error in the mutex_buffer_empty definition\n", (*my_client)->client_log, NULL, NULL);
			(*my_client)->mutex_buffer_empty.error_closing_handle = true;
		}
	}
	return SUCCESS_INDICATION;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
HANDLE CreateClientThreadSimple(LPTHREAD_START_ROUTINE client_start_routine, client *client_params, DWORD *client_thread_id) {
	/* Parameters */
	HANDLE thread_handle;

	/* Checks if the function received a null pointer */
	if (NULL == client_start_routine) {
		printf("Error when creating a thread\nReceived null pointer\n");
		exit(ERROR_INDICATION);
	}

	thread_handle = CreateThread(
		NULL,                /*  default security attributes */
		0,                   /*  use default stack size */
		client_start_routine,     /*  thread function */
		client_params,			 /*  argument to thread function */
		0,						/*  use default creation flags */
		client_thread_id);        /*  returns the thread identifier */

								  /* Checks if the thread was created properly */
	if (thread_handle == NULL) {
		printf("Error when creating a thread\nThread handler is NULL\n");
		exit(ERROR_INDICATION);
	}
	return thread_handle;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int ParsingClient(char *AcceptedStr, servermessage **message) {
	int i = 0;
	char *string;
	char *param_i;
	const char c = ';';
	string = strtok(AcceptedStr, ":");
	if (string == NULL) {
		(*message)->no_param = true;
		string = AcceptedStr;
	}
	for (i; i < 5; i++) {
		if (STRINGS_ARE_EQUAL(string, client_messages[i]))
			(*message)->opcode = i;
	}
	if ((*message)->no_param = true)
		return SUCCESS_INDICATION;

	i = 0;
	param_i = strchr(AcceptedStr, c);
	while ((param_i != NULL) && (i < 3)) {
		(*message)->params[i] = strtok(AcceptedStr, ";");
	}

	return SUCCESS_INDICATION;
}

//build clientmessagestruct
int ClientMessageBuilder(int opcode, char *param1, char *param2, char *param3, char *param4, clientmessage **msg) {
	(*msg)->opcode = opcode;
	(*msg)->params[0] = strcpy((*msg)->params[0], param1);
	(*msg)->params[1] = strcpy((*msg)->params[1], param2);
	if (param3 == NULL) {
		(*msg)->params[2] = NULL;
	}
	else {
	(*msg)->params[2] = strcpy((*msg)->params[2], param3);
	}
	if (param4 == NULL) {
		(*msg)->params[3] = NULL;
	}
	else {
		(*msg)->params[3] = strcpy((*msg)->params[3], param4);
	}
	return SUCCESS_INDICATION;
}

//client_message2string
int ClientMessageGenarator(clientmessage *message, char **message_string) {

	size_t msg_size = strlen(client_messages[message->opcode]);
	for (int t = 0; t < num_of_client_message_params[message->opcode] + 1; t++) {
		if (message->params[t] == NULL)
			continue;
		else
			msg_size += strlen(message->params[t]);
	}
	(*message_string) = (char*)malloc(sizeof(char)*msg_size);
	if ((*message_string) == NULL) {
		printf("Error - allocating client message string\n");
		return ERROR_INDICATION;
	}
	(*message_string) = strcpy((*message_string), client_messages[message->opcode]);

	(*message_string) = strcat((*message_string), ":");
	for (int i = 0; i < num_of_client_message_params[message->opcode]; i++)
	{
		(*message_string) = strcat((*message_string), message->params[i]);
		if (i != (num_of_client_message_params[message->opcode] - 1)) {
			(*message_string) = strcat((*message_string),",");
		}
	}


	return SUCCESS_INDICATION;
}


void MainClient(char* log_file, char *server_ip, int server_port, char *user_name, FILE *fp_log_client)
{
	SOCKADDR_IN clientService;
	HANDLE hThread[3]; // Rx, Tx and IO 
	DWORD thread_id[3];
	client *my_client;

	// Init client struct
	
    // Initialize Winsock.
    WSADATA wsaData; //Create a WSADATA object called wsaData.
	//The WSADATA structure contains information about the Windows Sockets implementation.
	
	//Call WSAStartup and check for errors.
    int iResult = WSAStartup( MAKEWORD(2, 2), &wsaData );
    if ( iResult != NO_ERROR )
        printf("Error at WSAStartup()\n");

	//Call the socket function and return its value to the m_socket variable. 
	// For this application, use the Internet address family, streaming sockets, and the TCP/IP protocol.
	// Create a socket.
    m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	// Check for errors to ensure that the socket is a valid socket.
    if ( m_socket == INVALID_SOCKET ) {
        printf( "Error at socket(): %ld\n", WSAGetLastError() );
        WSACleanup();
        return;
    }
	/*
	 The parameters passed to the socket function can be changed for different implementations. 
	 Error detection is a key part of successful networking code. 
	 If the socket call fails, it returns INVALID_SOCKET. 
	 The if statement in the previous code is used to catch any errors that may have occurred while creating 
	 the socket. WSAGetLastError returns an error number associated with the last error that occurred.
	 */
	//For a client to communicate on a network, it must connect to a server.
    // Connect to a server.
    //Create a sockaddr_in object clientService and set  values.
    clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(server_ip); //Setting the IP address to connect to
    clientService.sin_port = htons(server_port); //Setting the port to connect to.
	
	/*
		AF_INET is the Internet address family. 
	*/
    // Call the connect function, passing the created socket and the sockaddr_in structure as parameters. 
	// Check for general errors.
	if ( connect( m_socket, (SOCKADDR*) &clientService, sizeof(clientService) ) == SOCKET_ERROR) {
        printf( "Failed connecting to server on %s:%d\n",server_ip, server_port );
		PrintLog(fp_log_client,"Failed connecting to server on %s1:%s1\n",log_file, server_ip, _itoa(server_port,buffer,INT_BASE));
        WSACleanup();
        return;
    }
	else {
		printf("Connected to server on %s:%d\n", server_ip, server_port);
		PrintLog(fp_log_client, "Connected to server on %s1:%s2\n", log_file, server_ip, _itoa(server_port, buffer, INT_BASE));
	}

    // Send and receive data.
	/*
		In this code, two integers are used to keep track of the number of bytes that are sent and received. 
		The send and recv functions both return an integer value of the number of bytes sent or received, 
		respectively, or an error. Each function also takes the same parameters: 
		the active socket, a char buffer, the number of bytes to send or receive, and any flags to use.

	*/	
	int is_init = InitClient(log_file, server_ip, server_port, user_name, fp_log_client, &my_client,m_socket);

	//(LPTHREAD_START_ROUTINE)
	hThread[0] = CreateClientThreadSimple((LPTHREAD_START_ROUTINE)SendDataThread, my_client, &(thread_id[0]));
		
	hThread[1]= CreateClientThreadSimple((LPTHREAD_START_ROUTINE)RecvDataThread, my_client, &(thread_id[1]));

	hThread[2] = CreateClientThreadSimple((LPTHREAD_START_ROUTINE)I_O_Main, my_client, &(thread_id[2]));
	WaitForMultipleObjects(3,hThread,FALSE,INFINITE);

	TerminateThread(hThread[0],0x555);
	TerminateThread(hThread[1],0x555);
	TerminateThread(hThread[2], 0x555);

	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	CloseHandle(hThread[2]);
	
	closesocket(m_socket);
	
	WSACleanup();
    
	return;
}

