#include "ClientTx.h"


DWORD WINAPI ClientTx(LPVOID *LpParams) {

	SOCKET *my_socket = (SOCKET*)LpParams;
	char SendStr[256];
	TransferResult_t SendRes;

	while (1)
	{
		printf("please enter a string:\n");
		gets_s(SendStr, sizeof(SendStr)); //Reading a string from the keyboard

		if (STRINGS_ARE_EQUAL(SendStr, "quit"))
			return 0x555; //"quit" signals an exit from the client side

		SendRes = SendString(SendStr, my_socket);

		if (SendRes == TRNS_FAILED)
		{
			printf("Socket error while trying to write data to socket\n");
			return 0x555;
		}
	}
	return 0;
}