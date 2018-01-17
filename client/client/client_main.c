
#include "SocketExampleClient.h"
;

int main(int argc, char *argv[]){

	if (argc != 5) {
		printf("Error! number of argument to main is wrong\n");
		return ERROR_INDICATION;
	}
	char *log_file = argv[1];
	char *server_ip = argv[2];
	int server_port = atoi(argv[3]);
	char *user_name = argv[4];
	FILE *fp_log_client = NULL;
	if (FilesInit(fp_log_client, log_file) == ERROR_INDICATION) {
		printf("Erorr - init client file\n");
	}
	MainClient(log_file, server_ip, server_port, user_name, fp_log_client);
}