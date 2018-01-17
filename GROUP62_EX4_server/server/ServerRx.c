#include "ServerTx.h"


DWORD WINAPI ServerRxTx(LPVOID *LpParams) {

	SOCKET *my_socket = (SOCKET*)LpParams;
	BOOL Done = FALSE;
	char SendStr[50];
	TransferResult_t SendRes;
	TransferResult_t RecvRes;

	strcpy(SendStr, "Welcome to this server!");

	SendRes = SendString(SendStr, *my_socket);

	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(*my_socket);
		return 1;
	}
	while (!Done)
	{
		char *AcceptedStr = NULL;

		RecvRes = ReceiveString(&AcceptedStr, *my_socket);

		if (RecvRes == TRNS_FAILED)
		{
			printf("Service socket error while reading, closing thread.\n");
			closesocket(*my_socket);
			return 1;
		}
		else if (RecvRes == TRNS_DISCONNECTED)
		{
			printf("Connection closed while reading, closing thread.\n");
			closesocket(*my_socket);
			return 1;
		}
		else
		{
			printf("Got string : %s\n", AcceptedStr);
		}

		//After reading a single line, checking to see what to do with it
		//If got "hello" send back "what's up?"
		//If got "how are you?" send back "great"
		//If got "bye" send back "see ya!" and then end the thread
		//Otherwise, send "I don't understand"

		if (STRINGS_ARE_EQUAL(AcceptedStr, "hello"))
		{
			strcpy(SendStr, "what's up?");
		}
		else if (STRINGS_ARE_EQUAL(AcceptedStr, "how are you?"))
		{
			strcpy(SendStr, "great");
		}
		else if (STRINGS_ARE_EQUAL(AcceptedStr, "bye"))
		{
			strcpy(SendStr, "see ya!");
			Done = TRUE;
		}
		else
		{
			strcpy(SendStr, "I don't understand");
		}

		SendRes = SendString(SendStr, *my_socket);

		if (SendRes == TRNS_FAILED)
		{
			printf("Service socket error while writing, closing thread.\n");
			closesocket(*my_socket);
			return 1;
		}

		free(AcceptedStr);
	}
	return 0;
}