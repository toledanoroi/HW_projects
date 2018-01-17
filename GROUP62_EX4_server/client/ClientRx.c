#include "ClientTx.h"


HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE client_start_routine, client_info *client_params, DWORD *client_thread_id) {
	/* Parameters */
	HANDLE thread_handle;

	/* Checks if the function received a null pointer */
	if (NULL == client_start_routine) {
		printf("Error when creating a thread\nReceived null pointer\n");
		exit(ACLIENT_ERROR);
	}

	thread_handle = CreateThread(
		NULL,                /*  default security attributes */
		0,                   /*  use default stack size */
		client_start_routine,     /*  thread function */
		client_params, /*  argument to thread function */
		0,                   /*  use default creation flags */
		client_thread_id);        /*  returns the thread identifier */

								  /* Checks if the thread was created properly */
	if (thread_handle == NULL) {
		//client_params->error_thread_creation = true;
		printf("Error when creating a thread\nThread handler is NULL\n");
		exit(ACLIENT_ERROR);
	}
	return thread_handle;
}

DWORD WINAPI ClientRx(LPVOID *LpParams) {

	SOCKET *my_socket = (SOCKET*)LpParams;
	TransferResult_t RecvRes;

	while (1)
	{
		char *AcceptedStr = NULL;
		RecvRes = ReceiveString(&AcceptedStr, my_socket);

		if (RecvRes == TRNS_FAILED)
		{
			printf("Socket error while trying to write data to socket\n");
			return 0x555;
		}
		else if (RecvRes == TRNS_DISCONNECTED)
		{
			printf("Server closed connection. Bye!\n");
			return 0x555;
		}
		else
		{
			printf("%s\n", AcceptedStr);
		}

		free(AcceptedStr);
	}
	return 0;
}