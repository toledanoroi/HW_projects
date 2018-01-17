//#define _CRT_SECURE_NO_WARNINGS
//#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "ClientTx.h"


int main(int argc, char *argv[]) {
	
	if (argc != 5) {
		printf("Error! number of argument to main is wrong\n");
		return ACLIENT_ERROR;
	}
	//HANDLE client_rx_tx_handles[2];
	HANDLE hThread[2];
	LPDWORD client_rx_tx_thread_id[2];


	//client_rx_tx_handles = (HANDLE*)malloc(sizeof(HANDLE) * 2);
	DWORD wait_code;

	SOCKADDR_IN clientService;
	SOCKET m_socket;
	
	//connection & create a socket
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	char *log_file = argv[1];
	char *server_ip = argv[2];
	int server_port = atoi(argv[3]);
	char *user_name = argv[4];

	

	// Initialize Winsock.
	WSADATA wsaData; //Create a WSADATA object called wsaData.
					 //The WSADATA structure contains information about the Windows Sockets implementation.

					 //Call WSAStartup and check for errors.
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");

	//Call the socket function and return its value to the m_socket variable. 
	// For this application, use the Internet address family, streaming sockets, and the TCP/IP protocol.

	// Create a socket.
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Check for errors to ensure that the socket is a valid socket.
	if (m_socket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
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
	Sleep(15000);
	if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
		printf("Failed to connect.\n");
		WSACleanup();
		return;
	}
	else
		printf("connected to server\n");

	// Send and receive data.
	/*
	In this code, two integers are used to keep track of the number of bytes that are sent and received.
	The send and recv functions both return an integer value of the number of bytes sent or received,
	respectively, or an error. Each function also takes the same parameters:
	the active socket, a char buffer, the number of bytes to send or receive, and any flags to use.

	*/
	Sleep(8000);
	//hThread[0] = CreateThreadSimple(ClientTx, &m_socket, &client_rx_tx_thread_id[0]);
	hThread[1] = CreateThreadSimple(ClientRx, &m_socket, &client_rx_tx_thread_id[1]);

	wait_code = WaitForSingleObject(hThread[1], INFINITE);
	//wait_code = WaitForMultipleObjects(2, hThread, FALSE, INFINITE);
	if (wait_code != 0) {
		printf("RX TX failed\n");
	}


	//TerminateThread(hThread[0], 0x555);
	TerminateThread(hThread[1], 0x555);

	//CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);

	closesocket(m_socket);

	WSACleanup();

	return;

}

int InitAndAllocClient(client_info **client_params, char *client_log, char *server_port, char* server_ip, char* user_name) {

	int is_files;

	(*client_params) = (client_info*)malloc(sizeof(client_info));
	if (*client_params == NULL) {
		return ERROR_INDICATION;
	}

	(*client_params)->server_port = atoi(server_port);
	(*client_params)->user_name= user_name;
	(*client_params)->server_ip = server_ip;
	(*client_params)->client_log = client_log;

	is_files = FilesInit((*client_params)->report_file, (*client_params)->client_log);
	if (is_files == ERROR_INDICATION) {
		printf("Error - init files\n");
		return ERROR_INDICATION;
	}

}