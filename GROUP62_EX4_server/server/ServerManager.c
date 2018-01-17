/*
File Name	:	ServerManager.c
Authors		:	Roi Toledano	203700505
Yarden Avraham	302249925
Project		:	Exercise 4 - x/o
Uses		:
Description :
*/

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Includes and Defines*/
 #include "ServerTx.h"

int ServerMainFunc(server_info *server_params) {

	//server_info *server_params = (server_info*)lp_params;
	
	
	SOCKET MainSocket = INVALID_SOCKET;
	HANDLE h_connection_thread;
	LPDWORD connection_thread_id;
	DWORD WaitRes;
	connection_mparams *server_manager_params;
	unsigned long Address;
	SOCKADDR_IN service;
	int bindRes; //, ListenRes;


	h_connection_thread = (HANDLE)malloc(sizeof(HANDLE));
	connection_thread_id = (LPDWORD)malloc(sizeof(LPDWORD));


	// Initialize Winsock.
	WSADATA wsaData;
	int StartupRes = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (StartupRes != NO_ERROR) {
		printf("error %ld at WSAStartup( ), ending program.\n", WSAGetLastError());
		// Tell the user that we could not find a usable WinSock DLL.                                  
		return;
	}
	/* The WinSock DLL is acceptable. Proceed. */

	// Create a socket.    
	MainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (MainSocket == INVALID_SOCKET) {
		printf("Error at socket( ): %ld\n", WSAGetLastError());
		//goto server_cleanup_1;
		//return error
	}

	// Bind the socket.
	// Declare variables
	Address = inet_addr(SERVER_ADDRESS_STR);
	if (Address == INADDR_NONE) {
		printf("The string \"%s\" cannot be converted into an ip address. ending program.\n",
			SERVER_ADDRESS_STR);
		//goto server_cleanup_2;
	}

	service.sin_family = AF_INET;			//AF_INET is the Internet address family. 
	service.sin_addr.s_addr = Address;		//"127.0.0.1" is the local IP address to which the socket will be bound.
	service.sin_port = htons(server_params->server_port);	//The htons function converts a u_short from host to TCP/IP network byte order 
											//( which is big-endian )

											// Call the bind function, passing the created socket and the sockaddr_in structure as parameters. 
	bindRes = bind(MainSocket, (SOCKADDR*)&service, sizeof(service));
	if (bindRes == SOCKET_ERROR) {
		printf("bind( ) failed with error %ld. Ending program\n", WSAGetLastError());
		//goto server_cleanup_2;
	}


	//CreateASocket(server_params->server_port, &MainSocket);
	


	server_manager_params = (connection_mparams*)malloc(sizeof(connection_mparams));
	server_manager_params->main_socket = MainSocket;
	
	
	server_manager_params->userlist[1].client_ip = NULL;
	server_manager_params->userlist[1].client_port = NULL;
	server_manager_params->userlist[1].user_name = NULL;
	server_manager_params->userlist[1].user_sign = NULL;
	server_manager_params->userlist[1].user_socket = NULL;
	server_manager_params->userlist[2].client_ip = NULL;
	server_manager_params->userlist[2].client_port = NULL;
	server_manager_params->userlist[2].user_name = NULL;
	server_manager_params->userlist[2].user_sign = NULL;
	server_manager_params->userlist[2].user_socket = NULL;



	// create connection thread
	h_connection_thread = CreateThreadSimple(Connection_Manager, server_manager_params, &connection_thread_id);

	if (h_connection_thread == NULL) {
		printf("Error! failed to create connection_thread\n");
		return SERVER_CODE_ERROR;
	}
	
	WaitRes = WaitForSingleObject(h_connection_thread, INFINITE);

	if (WaitRes != WAIT_OBJECT_0) {
		DWORD x = GetLastError();
		printf("Error! failed to wait connection thread to terminate\n");
		return SERVER_CODE_ERROR;
	}

	if (CloseHandle(h_connection_thread) == 0) {
		printf("Main function | error while trying to close p_h_thread_manager handle\n");
		exit(SERVER_CODE_ERROR);
	}

	free(server_manager_params);
	return 0;
}

/*void CreateASocket(int server_port, SOCKET *main_socket) {
	unsigned long Address;
	SOCKADDR_IN service;
	int bindRes; //, ListenRes;


	// Initialize Winsock.
	WSADATA wsaData;
	int StartupRes = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (StartupRes != NO_ERROR){
		printf("error %ld at WSAStartup( ), ending program.\n", WSAGetLastError());
		// Tell the user that we could not find a usable WinSock DLL.                                  
		return;
	}
	// The WinSock DLL is acceptable. Proceed. 

	// Create a socket.    
	*main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (*main_socket == INVALID_SOCKET)	{
		printf("Error at socket( ): %ld\n", WSAGetLastError());
		//goto server_cleanup_1;
		//return error
	}

	// Bind the socket.
	// Declare variables
	Address = inet_addr(SERVER_ADDRESS_STR);
	if (Address == INADDR_NONE){
		printf("The string \"%s\" cannot be converted into an ip address. ending program.\n",
			SERVER_ADDRESS_STR);
		//goto server_cleanup_2;
	}

	service.sin_family = AF_INET;			//AF_INET is the Internet address family. 
	service.sin_addr.s_addr = Address;		//"127.0.0.1" is the local IP address to which the socket will be bound.
	service.sin_port = htons(server_port);	//The htons function converts a u_short from host to TCP/IP network byte order 
											//( which is big-endian )

	// Call the bind function, passing the created socket and the sockaddr_in structure as parameters. 
	bindRes = bind(*main_socket, (SOCKADDR*)&service, sizeof(service));
	if (bindRes == SOCKET_ERROR){
		printf("bind( ) failed with error %ld. Ending program\n", WSAGetLastError());
		//goto server_cleanup_2;
	}
}*/

DWORD WINAPI Connection_Manager(LPVOID lp_param) {

	connection_mparams *m_params = (connection_mparams*)lp_param;
	SOCKET MainSocket = m_params->main_socket;
	int ListenRes, jj,kk, index, num_of_players = 0;
	DWORD exit_code;
	struct timeval waiting_time;
	fd_set read_fd_set;
	HANDLE client_thread_handles_arr[NUM_OF_THREADS_IN_SERVER];
	HANDLE rx_tx_thread_handles[2];
	LPDWORD rx_tx_thread_id[NUM_OF_THREADS_IN_SERVER];
	LPDWORD clients_thread_ids[NUM_OF_THREADS_IN_SERVER];
	SOCKET clients_threads_inputs[NUM_OF_THREADS_IN_SERVER];
	DWORD WaitRes;

	// Listen on the Socket.
	ListenRes = listen(MainSocket, SOMAXCONN);
	if (ListenRes == SOCKET_ERROR){
		printf("Failed listening on socket, error %ld.\n", WSAGetLastError());
	}

	FD_ZERO(&read_fd_set);
	FD_SET(MainSocket, &read_fd_set);
	
	waiting_time.tv_sec = (long)60;
	waiting_time.tv_usec = (long)(60 * (1e6));

	// Initialize all thread handles to NULL, to mark that they have not been initialized
	for (index = 0; index < NUM_OF_THREADS_IN_SERVER; index++) {
		client_thread_handles_arr[index] = NULL;
	}


	printf("Waiting for a client to connect...\n");
	for (jj = 0; jj < MAX_PLAYERS; jj++)
	{
		exit_code = select(1, &read_fd_set, NULL, NULL, &waiting_time);

		if (exit_code == 0) {
			printf("No players connected.Exiting.\n");
			// TODO exit with no errors
			
		}

		m_params->userlist[jj].user_socket = accept(MainSocket, NULL, NULL);

		if (m_params->userlist[jj].user_socket == INVALID_SOCKET)
		{
			printf("Accepting connection with client failed, error %ld\n", WSAGetLastError());
			//goto server_cleanup_3;
		}

		// update time to 4 minutes
		waiting_time.tv_sec = (long)4 * 60;
		waiting_time.tv_usec = (long)(4 * 60 * 1e6);

		printf("The %d Client Connected.\n",jj + 1);

		clients_threads_inputs[jj] = m_params->userlist[jj].user_socket;

		// create a communication client thread
		client_thread_handles_arr[jj] = CreateThreadSimple(ServerRxTx, &clients_threads_inputs[jj], &(clients_thread_ids[jj]));
		
		
		WaitRes = WaitForSingleObject(client_thread_handles_arr[jj], INFINITE);
		/*CreateThreadSimple(ServerRx,
			&clients_threads_inputs[jj],
			&(clients_thread_ids[jj]), &(client_thread_handles_arr[jj]));*/
	} 
	// WaitRes = WaitForMultipleObjects(...)
	//for ( kk = 0; kk < MAX_PLAYERS; kk++ )
	return 0;
}


/*HANDLE CreateThreadSimple
Parameters:		p_start_routine			- A pointer tha points to a function that notifies the host that a thread has started to execute.
p_thread_parameters		- A pointer to a struct that contains all the parameters of the thread, including potential errors.
p_thread_id				- A pointer to the thread's ID.
fp_debud				- A pointer to the  debug log file.
file_name				- A pointer to the name of the log file string.
Returns:		A HANDLE variable		- The handle of the thread that was created.
Description:	A function that calls CreateThread function, that creates a thread to execute RoommateThread WINAPI function.*/
HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE server_start_routine, server_info *server_params, DWORD *server_thread_id) {
	/* Parameters */
	HANDLE thread_handle;

	/* Checks if the function received a null pointer */
	if (NULL == server_start_routine) {
		printf("Error when creating a thread\nReceived null pointer\n");
		exit(SERVER_CODE_ERROR);
	}

	thread_handle = CreateThread(
		NULL,                /*  default security attributes */
		0,                   /*  use default stack size */
		server_start_routine,     /*  thread function */
		server_params, /*  argument to thread function */
		0,                   /*  use default creation flags */
		server_thread_id);        /*  returns the thread identifier */

									/* Checks if the thread was created properly */
	if (thread_handle == NULL) {
		server_params->error_thread_creation = true;
		printf("Error when creating a thread\nThread handler is NULL\n");
		exit(SERVER_CODE_ERROR);
	}
	return thread_handle;
}