
#include "SocketExampleServer.h"


int main(int argc, char* argv[]){

	HANDLE *h_socket_thread = NULL;
	LPDWORD *socket_thread_id = NULL;
	server_info *server_params = NULL;
	int is_init;

	/*program input check*/
	if (argc != 3) {
		printf("Error! number of argument to main is wrong\n");
		return PROGRAM_ERROR_VAL;
	}
	/*allocating memory for the server and Initiate his parameters */
	is_init = InitAndAlloc(&server_params, argv[1], argv[2], &h_socket_thread, &socket_thread_id);
	if (is_init == ERROR_INDICATION) {
		printf("Error - init server parameters\n");
		return ERROR_INDICATION;
	}

	MainServer(server_params);
	

	printf("goodbye\n");

	free(server_params);
}