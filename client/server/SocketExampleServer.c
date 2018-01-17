/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
/* 
 This file was written for instruction purposes for the 
 course "Introduction to Systems Programming" at Tel-Aviv
 University, School of Electrical Engineering.
Last updated by Amnon Drory, Winter 2011.
 */
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/




#include "SocketExampleServer.h"
#include <string.h>

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/


/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

HANDLE ThreadHandles[NUM_OF_WORKER_THREADS];
SOCKET ThreadInputs[NUM_OF_WORKER_THREADS];
connection_thread_info thread_info[NUM_OF_WORKER_THREADS];

DWORD ThreadID[NUM_OF_WORKER_THREADS];

char *client_messages[NUM_OF_CLIENT_MESSAGES] = { "NEW_USER_REQUEST", "PLAY_REQUEST", "USER_LIST_QUERY", "GAME_STATE_QUERY", "BOARD_VIEW_QUERY" };
int num_of_client_message_params[5] = { 1,2,0,0,0 };


char *server_messages[NUM_OF_SERVER_MESSAGES] = { "NEW_USER_ACCEPTED", "NEW_USER_DECLINED", "GAME_STARTED", "BOARD_VIEW", "TURN_SWITCH", "PLAY_ACCEPTED", "PLAY_DECLINED", "GAME_ENDED", "USER_LIST_REPLAY", "GAME_STATE_REPLAY", "BOARD_VIEW_REPLAY" };
int num_of_server_message_params[11] = { 2,1,0,1,2,0,1,1,4,0,2 };
char buffer[MAXCHAR];
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

void MainServer(server_info *server_params)
{
	int Ind;
	int Loop, count = 0;
	SOCKET MainSocket = server_params->main_socket;
	unsigned long Address;
	int server_port = server_params->server_port;
	//HANDLE server_hthread = NULL;
	//DWORD server_thread_id;
	FILE *fp_log = server_params->fp_log;
	SOCKADDR_IN service;
	int bindRes;
	int ListenRes;
	struct timeval waiting_time;
	fd_set read_fd_set;

	FD_ZERO(&read_fd_set);
	FD_SET(MainSocket, &read_fd_set);
	waiting_time.tv_sec = (long)60 * 5;
	waiting_time.tv_usec = (long)(60 * 5 *(1e6));

	// Initialize Winsock.
    WSADATA wsaData;
    int StartupRes = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );	           

    if ( StartupRes != NO_ERROR )
	{
        printf( "error %ld at WSAStartup( ), ending program.\n", WSAGetLastError() );
		// Tell the user that we could not find a usable WinSock DLL.                                  
		return;
	}
 
    /* The WinSock DLL is acceptable. Proceed. */

    // Create a socket.    
    MainSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    if ( MainSocket == INVALID_SOCKET ) 
	{
        printf( "Error at socket( ): %ld\n", WSAGetLastError( ) );
		goto server_cleanup_1;
    }

    // Bind the socket.
	/*
		For a server to accept client connections, it must be bound to a network address within the system. 
		The following code demonstrates how to bind a socket that has already been created to an IP address 
		and port.
		Client applications use the IP address and port to connect to the host network.
		The sockaddr structure holds information regarding the address family, IP address, and port number. 
		sockaddr_in is a subset of sockaddr and is used for IP version 4 applications.
   */
	// Create a sockaddr_in object and set its values.
	// Declare variables

	Address = inet_addr( SERVER_ADDRESS_STR );
	if ( Address == INADDR_NONE )
	{
		printf("The string \"%s\" cannot be converted into an ip address. ending program.\n",
				SERVER_ADDRESS_STR );
		goto server_cleanup_2;
	}

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = Address;
    service.sin_port = htons(server_port); //The htons function converts a u_short from host to TCP/IP network byte order 
	                                   //( which is big-endian ).
	/*
		The three lines following the declaration of sockaddr_in service are used to set up 
		the sockaddr structure: 
		AF_INET is the Internet address family. 
		"127.0.0.1" is the local IP address to which the socket will be bound. 
	    2345 is the port number to which the socket will be bound.
	*/

	// Call the bind function, passing the created socket and the sockaddr_in structure as parameters. 
	// Check for general errors.
    bindRes = bind( MainSocket, ( SOCKADDR* ) &service, sizeof( service ) );
	if ( bindRes == SOCKET_ERROR ) 
	{
        printf( "bind( ) failed with error %ld. Ending program\n", WSAGetLastError( ) );
		goto server_cleanup_2;
	}
    
    // Listen on the Socket.
	ListenRes = listen( MainSocket, SOMAXCONN );
    if ( ListenRes == SOCKET_ERROR ) 
	{
        printf( "Failed listening on socket, error %ld.\n", WSAGetLastError() );
		goto server_cleanup_2;
	}

	// Initialize all thread handles to NULL, to mark that they have not been initialized
	for ( Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++ )
		ThreadHandles[Ind] = NULL;

    printf( "Waiting for a client to connect...\n" );
    
	for ( Loop = 0; Loop < MAX_LOOPS; Loop++ )
	{
		int exit_code = select(1, &read_fd_set, NULL, NULL, &waiting_time);

		if ((exit_code == 0)&&(count==0)) {
			printf("No players connected.Exiting.\n");
			// TODO exit with no errors
		}	
		Ind = FindFirstUnusedThreadSlot();
		server_params->userlist[Ind].user_socket = accept(MainSocket, NULL, NULL);
		if (server_params->userlist[Ind].user_socket == INVALID_SOCKET){
			printf("Accepting connection with client failed, error %ld\n", WSAGetLastError());
			goto server_cleanup_3;
		}
		count++;
		printf("Client %d Connected.\n", count);
		if (Ind == NUM_OF_WORKER_THREADS){ //no slot is available
			printf("No slots available for client, dropping the connection.\n");
			closesocket(server_params->userlist[Loop].user_socket); //Closing the socket, dropping the connection.
		}
		else{
			ThreadInputs[Ind] = server_params->userlist[Ind].user_socket;// shallow copy: don't close 
												// AcceptSocket, instead close 
												// ThreadInputs[Ind] when the
												// time comes.
			server_params->userlist[Ind].index = Ind;
			//maybe delete
			if (Ind == 0) {
				server_params->first_socket = server_params->userlist[Ind].user_socket;
			}
			if (InitConnectionThread((thread_info+Ind), server_params, ThreadInputs[Ind])) {
				printf("Error - init connectoin thread\n");
			}

			ThreadHandles[Ind] = CreateThreadSimple((LPTHREAD_START_ROUTINE)ServiceThread, (thread_info + Ind), &ThreadID[Ind]);
				
			
			//ThreadHandles[Ind] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ServiceThread, &(ThreadInputs[Ind]), 0, NULL);
		}

    } // for ( Loop = 0; Loop < MAX_LOOPS; Loop++ )

server_cleanup_3:

	CleanupWorkerThreads();

server_cleanup_2:
	if ( closesocket( MainSocket ) == SOCKET_ERROR )
		printf("Failed to close MainSocket, error %ld. Ending program\n", WSAGetLastError() ); 

server_cleanup_1:
	if ( WSACleanup() == SOCKET_ERROR )		
		printf("Failed to close Winsocket, error %ld. Ending program.\n", WSAGetLastError() );
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

static int FindFirstUnusedThreadSlot()
{ 
	int Ind;

	for ( Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++ )
	{
		if ( ThreadHandles[Ind] == NULL )
			break;
		else
		{
			// poll to check if thread finished running:
			DWORD Res = WaitForSingleObject( ThreadHandles[Ind], 0 ); 
				
			if ( Res == WAIT_OBJECT_0 ) // this thread finished running
			{				
				CloseHandle( ThreadHandles[Ind] );
				ThreadHandles[Ind] = NULL;
				break;
			}
		}
	}

	return Ind;
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

static void CleanupWorkerThreads()
{
	int Ind; 

	for ( Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++ )
	{
		if ( ThreadHandles[Ind] != NULL )
		{
			// poll to check if thread finished running:
			DWORD Res = WaitForSingleObject( ThreadHandles[Ind], INFINITE ); 
				
			if ( Res == WAIT_OBJECT_0 ) 
			{
				closesocket( ThreadInputs[Ind] );
				CloseHandle( ThreadHandles[Ind] );
				ThreadHandles[Ind] = NULL;
				break;
			}
			else
			{
				printf( "Waiting for thread failed. Ending program\n" );
				return;
			}
		}
	}
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//Service thread is the thread that opens for each successful client connection and "talks" to the client.
static DWORD ServiceThread(connection_thread_info *connection_params )
{
	SOCKET t_socket = connection_params->client_socket;
	server_info *server_params = connection_params->server_params;
	char SendStr[SEND_STR_SIZE];
	BOOL Done = FALSE;
	TransferResult_t RecvRes;
	clientmessage *msg_c;
	msg_c = (clientmessage*)malloc(sizeof(clientmessage));
	servermessage *msg_s;
	msg_s = (servermessage*)malloc(sizeof(servermessage));

	strcpy( SendStr, "Welcome to Tic Tac Toe Game server\n!" );
	if ((SendStringErrorCheck(SendStr, t_socket))) {
		printf("Error - Sending string\n");
	}
	
	while (server_params->game_ended == false) {		
		char *AcceptedStr = NULL;		
		RecvRes = ReceiveString1( &AcceptedStr , t_socket );
		if ( RecvRes == TRNS_FAILED ){
			printf( "Service socket error while reading, closing thread.\n" );
			closesocket( t_socket );
			return 1;
		}
		else if ( RecvRes == TRNS_DISCONNECTED ){
			printf( "Connection closed while reading, closing thread.\n" );
			closesocket( t_socket );
			return 1;
		}
		else {
			printf( "Got string : %s\n", AcceptedStr );
		}

		int is_parse = ParsingServer(AcceptedStr , &msg_c);
		if (is_parse == ERROR_INDICATION) {
			printf("error\n");
		}
		int is_action = ServerActionAfterClientMessage(msg_c, &msg_s, &server_params,t_socket);
		if (is_action == ERROR_INDICATION) {
			printf("error\n");
		}
		free( AcceptedStr );		
	}

	printf("Conversation ended.\n");
	closesocket( t_socket );
	return 0;
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

int InitAndAlloc(server_info **server_params, char *server_log, char *server_port, HANDLE **h_socket_thread, LPDWORD **socket_thread_id) {

	int is_files;


	(*h_socket_thread) = (HANDLE*)malloc(sizeof(HANDLE));
	(*socket_thread_id) = (LPDWORD*)malloc(sizeof(LPDWORD));
	(*server_params) = (server_info*)malloc(sizeof(server_info));
	(*server_params)->board = (char*)malloc(sizeof(char)*25);
	if ((*server_params == NULL) || (h_socket_thread == NULL) || (socket_thread_id == NULL) || ((*server_params)->board == NULL) || ((*server_params)->board == NULL)) {
		printf("Erorr - Allocation memory\n");
		return ERROR_INDICATION;
	}
	(*server_params)->server_port = atoi(server_port);
	(*server_params)->server_log = server_log;
	(*server_params)->server_ip = SERVER_ADDRESS_STR;
	strcpy_s((*server_params)->board,25, "| | | |\n| | | |\n| | | |\n");
	(*server_params)->main_socket = INVALID_SOCKET;

	//userlist parameters
	(*server_params)->userlist[0].client_ip = NULL;
	(*server_params)->userlist[0].client_port = -555;
	(*server_params)->userlist[0].user_name = NULL;
	(*server_params)->userlist[0].user_sign = ' ';
	(*server_params)->userlist[0].user_socket = -555;
	(*server_params)->userlist[1].client_ip = NULL;
	(*server_params)->userlist[1].client_port = -555;
	(*server_params)->userlist[1].user_name = NULL;
	(*server_params)->userlist[1].user_sign = ' ';
	(*server_params)->userlist[1].user_socket = -555;
	

	(*server_params)->first_socket = -1;
	(*server_params)->game_ended = false;
	(*server_params)->is_first = false;
	(*server_params)->is_start = false;
	(*server_params)->num_of_players = 0;
	(*server_params)->turn_ind = 0;
	(*server_params)->who_won = NO_ONE;
	
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			(*server_params)->board_game[i][j] = -1;
		}
	}


	is_files = FilesInit((*server_params)->fp_log, (*server_params)->server_log);
	if (is_files == ERROR_INDICATION) {
		printf("Error - init files\n");
		return ERROR_INDICATION;
	}
	return SUCCESS_INDICATION;
}
//------------------------------------------------------------------------------
int ParsingServer(char *AcceptedStr, clientmessage **message) {
	int i = 0;
	char *string;
	char *param_i;
	char c = ';';
	string = strtok(AcceptedStr, ":");
	if (string == NULL) {
		(*message)->no_param = true;
		string = AcceptedStr;
	}
	for (i; i < 5; i++) {
		if (STRINGS_ARE_EQUAL(string, client_messages[i]))
			(*message)->opcode = i;
	}
	//check if relevant
	//if ((*message)->no_param = true)
	//	return SUCCESS_INDICATION;

	i = 0;
	param_i = strchr(AcceptedStr, c);
	while ((param_i != NULL) && (i < 4)) {
		(*message)->params[i] = strtok(AcceptedStr, ";");
		i++;
	}
	return SUCCESS_INDICATION;
}
int ServerMessageBuilder(int opcode, char *param1, char* param2, char* param3, char* param4, servermessage **msg) {
	(*msg)->opcode = opcode;
	//(*msg)->params[0] = param1;
	//(*msg)->params[1] = param2;
	//(*msg)->params[2] = param3;
	//(*msg)->params[3] = param4;

	// check if srtcpy needed
	(*msg)->params[0] = strcpy((*msg)->params[0],param1);
	(*msg)->params[1] = strcpy((*msg)->params[1], param2);
	(*msg)->params[2] = strcpy((*msg)->params[2], param3);
	(*msg)->params[3] = strcpy((*msg)->params[3], param4);



	return SUCCESS_INDICATION;
}
//-------------------------------------------------------------------------------------
int ServerMessageGenarator(servermessage *message, char **message_string) {
	/// if first time dont free memory else free message_string before malloc
	size_t msg_size = strlen(server_messages[message->opcode]);
	for (int t = 0; t < num_of_client_message_params[message->opcode]; t++) {
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

	for (int i = 0; i < num_of_client_message_params[message->opcode]; i++)
	{
		(*message_string) = strcat((*message_string), message->params[i]);
	}


	return SUCCESS_INDICATION;
}
int ServerActionAfterClientMessage(clientmessage *message, servermessage **message_back, server_info **server_params, SOCKET user_socket) {
	int opcode = message->opcode;
	char *msg_out;
	msg_out = (char*)malloc(sizeof(char)*MAXCHAR);
	int his_turn;
	int not_his_turn;
	if (user_socket == (*server_params)->userlist[0].user_socket) {
		his_turn = 0;
		not_his_turn = 1;
	}	
	else {
		his_turn = 1;
		not_his_turn = 0;
	}

	switch (opcode)
	{
	case (0):// NEW_USER_REQUEST
		if ((*server_params)->num_of_players == 0)
		{
			(*server_params)->userlist[0].user_name = message->params[0];
			ServerMessageBuilder(0, "x", "1", NULL, NULL, message_back);
			ServerMessageGenarator((*message_back), &msg_out);
			SendStringErrorCheck(msg_out, (*server_params)->userlist[0].user_socket);
			(*server_params)->num_of_players = 1;
		}
		else {
			if (STRINGS_ARE_EQUAL(message->params[0], (*server_params)->userlist[0].user_name)) {
				//decline, name was taked already
				//(*message_back)->opcode = 1;
				//(*message_back)->params[0] = "Request to join was refused"; //I think this shouldn't be here, its in RecvDataThread case (1)
				ServerMessageBuilder(1, NULL, NULL, NULL, NULL, message_back);
				ServerMessageGenarator((*message_back), &msg_out);
				SendStringErrorCheck(msg_out, user_socket);
			}
			else {//second player
				//(*message_back)->opcode = 0;
				(*server_params)->userlist[1].user_name = message->params[0];
				//(*message_back)->params[0] = "o";
				//(*message_back)->params[1] = "2";
				ServerMessageBuilder(0, "o", "2", NULL, NULL, message_back);
				ServerMessageGenarator((*message_back), &msg_out);
				SendStringErrorCheck(msg_out, user_socket);
				(*server_params)->num_of_players = 2;
				(*server_params)->is_start = true;
			}
		}
	case (1): // play request

		if ((*server_params)->turn_ind == his_turn)//if its his turn
		{//x , y holds the players move
			int x = atoi(message->params[0]);
			int y = atoi(message->params[1]);
			if ((x < 1 || x > 3 || y < 1 || y > 3)) {
				ServerMessageBuilder(6, "Illegal move", NULL, NULL, NULL, message_back);
				ServerMessageGenarator((*message_back), &msg_out);
				SendStringErrorCheck(msg_out, user_socket);
			}
			if ((*server_params)->board_game[3-x][y - 1] == OCCUPPIED)//needs to change, still haven't written how we will hold the board game. 
			{
				ServerMessageBuilder(6, "Illegal move", NULL, NULL, NULL, message_back);
				ServerMessageGenarator((*message_back), &msg_out);
				SendStringErrorCheck(msg_out, user_socket);
			}
			else { // good inputs -> lets play 
				UpdateBoardGameAndCheckGameStatus(server_params, 3-x, y-1, (*server_params)->userlist[his_turn].user_sign);
				Board2String(((*server_params)->board), (*server_params)->board_game);	
				//send play accepted
				ServerMessageBuilder(5, NULL, NULL, NULL, NULL, message_back);
				ServerMessageGenarator((*message_back), &msg_out);
				SendStringErrorCheck(msg_out, user_socket);

				//send board_view to all players
				ServerMessageBuilder(3, ((*server_params)->board), NULL, NULL, NULL, message_back);
				ServerMessageGenarator((*message_back), &msg_out);
				SendStringErrorCheck(msg_out, user_socket);
				SendStringErrorCheck(msg_out, (*server_params)->userlist[not_his_turn].user_socket);
					
				// if there isn't a winner yet
				if ((*server_params)->who_won != NO_ONE) {
					// send turn switch
					(*server_params)->turn_ind = not_his_turn;
					//check where is better to update these values
					//(*server_params)->userlist[0].my_turn = false;
					//(*server_params)->userlist[1].my_turn = true;
					ServerMessageBuilder(4,(*server_params)->userlist[not_his_turn].user_name , (char*)((*server_params)->userlist[not_his_turn].user_sign), NULL, NULL, message_back);
					ServerMessageGenarator((*message_back), &msg_out);
					SendStringErrorCheck(msg_out, user_socket);
					SendStringErrorCheck(msg_out, (*server_params)->userlist[not_his_turn].user_socket);
				}
				else { // there is a winner - winner or tie 
					if ((*server_params)->who_won == BOTH) { // there is a tie 							
						ServerMessageBuilder(7, (*server_params)->userlist[his_turn].user_name, (*server_params)->userlist[not_his_turn].user_name, NULL, NULL, message_back);
						ServerMessageGenarator((*message_back), &msg_out);
						SendStringErrorCheck(msg_out, user_socket);
						SendStringErrorCheck(msg_out, (*server_params)->userlist[not_his_turn].user_socket);
						(*server_params)->game_ended = true;
					}
					else { //there is winner 
						ServerMessageBuilder(7, (*server_params)->userlist[0].user_name, NULL , NULL, NULL, message_back);
						ServerMessageGenarator((*message_back), &msg_out);
						SendStringErrorCheck(msg_out, user_socket);
						SendStringErrorCheck(msg_out, (*server_params)->userlist[not_his_turn].user_socket);
						(*server_params)->game_ended = true;
					}			
				}
			}
		}
		else { // not his turn or the game has not started
			if (!((*server_params)->is_start)) {
				ServerMessageBuilder(6, "Game has not started", NULL, NULL, NULL, message_back);
				ServerMessageGenarator((*message_back), &msg_out);
				SendStringErrorCheck(msg_out, user_socket);
				break;
			}
			ServerMessageBuilder(6, "Not your turn", NULL, NULL, NULL, message_back);
			ServerMessageGenarator((*message_back), &msg_out);
			SendStringErrorCheck(msg_out, user_socket);
		}
	case (2): //USER_LIST_QUERY
		if (((*server_params)->num_of_players) == 2) {
			ServerMessageBuilder(8, (char*)((*server_params)->userlist[0].user_sign), (*server_params)->userlist[0].user_name, (char*)((*server_params)->userlist[1].user_sign), (*server_params)->userlist[1].user_name, message_back);
			ServerMessageGenarator((*message_back), &msg_out);
			SendStringErrorCheck(msg_out, user_socket);
		}
		else if ((((*server_params)->num_of_players) == 1)) {
			ServerMessageBuilder(8, (char*)((*server_params)->userlist[0].user_sign), (*server_params)->userlist[0].user_name, NULL, NULL, message_back);
			ServerMessageGenarator((*message_back), &msg_out);
			SendStringErrorCheck(msg_out, user_socket);
		}
		else {//couldn't understand if this is an option, if no players are int.
		
		}
	case(3): //GAME_STATE_QUERY
		if (!((*server_params)->is_start)) {//game hasn't started
			ServerMessageBuilder(9, NULL, NULL, NULL, NULL, message_back);
			ServerMessageGenarator((*message_back), &msg_out);
			SendStringErrorCheck(msg_out, user_socket);	
		}
		else if ((*server_params)->game_ended) {
			ServerMessageBuilder(9, "end", NULL, NULL, NULL, message_back);
			ServerMessageGenarator((*message_back), &msg_out);
			SendStringErrorCheck(msg_out, user_socket);
		}
		else { // the game has started
			ServerMessageBuilder(9, (*server_params)->userlist[(*server_params)->turn_ind].user_name, (char*)((*server_params)->userlist[(*server_params)->turn_ind].user_sign), NULL, NULL, message_back);
			ServerMessageGenarator((*message_back), &msg_out);
			SendStringErrorCheck(msg_out, user_socket);
		}
	case(4): // Board view Query  
		if ((!(*server_params)->is_start)) {  //Game hasn't started
			ServerMessageBuilder(10, NULL, NULL, NULL, NULL, message_back);
			ServerMessageGenarator((*message_back), &msg_out);
			SendStringErrorCheck(msg_out, user_socket);
		}
		else {
			ServerMessageBuilder(10, (*server_params)->board, NULL, NULL, NULL, message_back);
			ServerMessageGenarator((*message_back), &msg_out);
			SendStringErrorCheck(msg_out, user_socket);
		}
	}
	return SUCCESS_INDICATION;
}
HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE server_start_routine, connection_thread_info *user_socket, DWORD *server_thread_id) {
	/* Parameters */
	HANDLE thread_handle;

	/* Checks if the function received a null pointer */
	if (NULL == server_start_routine) {
		printf("Error when creating a thread\nReceived null pointer\n");
		exit(ERROR_INDICATION);
	}

	thread_handle = CreateThread(
		NULL,                /*  default security attributes */
		0,                   /*  use default stack size */
		server_start_routine,     /*  thread function */
		user_socket, /*  argument to thread function */
		0,                   /*  use default creation flags */
		server_thread_id);        /*  returns the thread identifier */

								  /* Checks if the thread was created properly */
	if (thread_handle == NULL) {
		printf("Error when creating a thread\nThread handler is NULL\n");
		exit(ERROR_INDICATION);
	}
	return thread_handle;
}
int SendStringErrorCheck(char* SendStr , SOCKET t_socket ) {
	TransferResult_t SendRes;

	SendRes = SendString1(SendStr, t_socket);

	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(t_socket);
		return 1;
	}
	return SUCCESS_INDICATION;
}
void Board2String(char* board, int board_game[3][3]) {
	char tmp_board_game[3][3];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			switch (board_game[i][j]) {
			case(0):
				tmp_board_game[i][j] = 'o';
			case(1):
				tmp_board_game[i][j] = 'x';
			case(-1):
				tmp_board_game[i][j] = ' ';
			}

		}
	}
	board[1] = tmp_board_game[0][0];
	board[3] = tmp_board_game[0][0];
	board[5] = tmp_board_game[0][0];
	board[9] = tmp_board_game[0][0];
	board[11] = tmp_board_game[0][0];
	board[13] = tmp_board_game[0][0];
	board[17] = tmp_board_game[0][0];
	board[19] = tmp_board_game[0][0];
	board[21] = tmp_board_game[0][0];
}
void UpdateBoardGameAndCheckGameStatus(server_info **server_params, int x, int y, char user_sign) {
	int count = 0;
	if (user_sign == 'x') {
		(*server_params)->board_game[x][y] = 1;
	}
	else {
		(*server_params)->board_game[x][y] = 0;
	}
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if ((*server_params)->board_game[i][j] == -1) {
				count++;
			}
		}
	}
	if (count == 0) {
		(*server_params)->who_won = BOTH;
		return;
	}
	if (CheckTicTacToe((*server_params)->board_game) == 1) {//'x' won the game
		(*server_params)->who_won = 1;
	}
	else if (CheckTicTacToe((*server_params)->board_game) == -1) {//'o' won the game
		(*server_params)->who_won = 0;
	}
}
int CheckTicTacToe(int board[3][3])
{
	int count = 0;
	int row, col;

	// Check each of 3 rows:
	for (row = 0; row < 3; ++row)
	{
		count = 0;
		for (col = 0; col < 3; ++col)
		{
			count += (board[row][col] == 1) ? 1 : (board[row][col] == 0) ? -1 : 0;
		}
		if (count == 3 || count == -3)
		{
			return count / abs(count); // Return either 1 or -1
		}
	}

	// Check each of 3 columns.
	for (col = 0; col < 3; ++col)
	{
		count = 0;
		for (row = 0; row < 3; ++row)
		{
			count += (board[row][col] == 1) ? 1 : (board[row][col] == 0) ? -1 : 0;
		}
		if (count == 3 || count == -3)
		{
			return count / abs(count); // Return either 1 or -1
		}
	}

	// Check Left-to-Right downward Diagonal:
	count = 0;
	for (col = 0; col < 3; ++col)
	{
		count += (board[col][col] == 1) ? 1 : (board[col][col] == 0) ? -1 : 0;
	}
	if (count == 3 || count == -3)
	{
		return count / abs(count); // Return either 1 or -1
	}

	// Check Left-to-Right upward Diagonal
	count = 0;
	for (col = 0; col < 3; ++col)
	{
		count += (board[col][2 - col] == 1) ? 1 : (board[col][2 - col] == 0) ? -1 : 0;
	}
	if (count == 3 || count == -3)
	{
		return count / abs(count); // Return either 1 == x  or -1 == o
	}

	return 0;//no one won
}
int InitConnectionThread(connection_thread_info *con_prm, server_info *server_params, SOCKET my_socket) {
	con_prm->server_params = server_params;
	con_prm->client_socket = my_socket;

	return SUCCESS_INDICATION;
}