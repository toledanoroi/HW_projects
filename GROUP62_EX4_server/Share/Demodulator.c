#include "Demodulator.h"


int ClientMessageGenarator(clientmessage *message, char **message_string) {
	//int jj , total_len; 
	//char messagetype[MAX_MESSAGETYPE_CHAR];

	//strcpy_s(messagetype, MAX_MESSAGETYPE_CHAR, client_messages[message->opcode]);
	////understand how much memory to malloc
	//total_len = strlen(messagetype) + 1; //1 for the :
	//if (strcmp(message->param1, EMPTY_PARAM)) {
	//	total_len += strlen(message->param1) + 1;
	//}
	//if (strcmp(message->param2, EMPTY_PARAM)) {
	//	total_len += strlen(message->param2) + 1;
	//}
	//if (strcmp(message->param2, EMPTY_PARAM)) {
	//	total_len += strlen(message->param2) + 1;
	//}

	//(*message_string) = (char*)malloc(sizeof(char)*total_len);


	return 0;
}

int ClientMessageDemodulator(clientmessage **message, char *message_string) {
	return 0;
}

int ServerMessageGenarator(servermessage *message, char **message_string) {
	return 0;
}

int ServerMessageDemodulator(servermessage **message, char *message_string) {
	return 0;
}