#include "ServerTx.h"



DWORD WINAPI ServerTx(LPVOID *LpParams) {

	SOCKET *my_socket = (SOCKET*)LpParams;
	char *string_to_send = "roi toledano\n";

	if (send(*my_socket, string_to_send, 13, 0) == SOCKET_ERROR) {
		printf("sending failed");
	}
	return 0;
}