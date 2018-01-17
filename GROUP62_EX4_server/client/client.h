
#include "Demodulator.h"


#define GOOD_RESPONSE 0
#define BAD_RESPONSE 1

#define ACLIENT_ERROR -1
#define ACLIENT_FAILURE 1
#define ACLIENT_SUCCESS 0
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Structs defenitions*/
typedef struct {
	FILE *fp_errors;
	FILE *fp_log;
	char *report_file;
	char *client_log;
	/*mutexes
	mutex mutex_1;
	mutex mutex_2;
	mutex mutex_time;*/
	int server_port;
	char *server_ip;
	char *client_ip;
	char *user_name;

} client_info;

typedef enum {
	CLIENT_CODE_SUCCESS,
	CLIENT_CODE_FAILED,
	CLIENT_CODE_ERROR = -1

} client_return_code;



