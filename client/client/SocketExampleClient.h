/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
/* 
 This file was written for instruction purposes for the 
 course "Introduction to Systems Programming" at Tel-Aviv
 University, School of Electrical Engineering, Winter 2011, 
 by Amnon Drory.
*/
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
#include "SocketExampleShared.h"


//#ifndef SOCKET_EXAMPLE_CLIENT_H
//#define SOCKET_EXAMPLE_CLIENT_H

extern SOCKET m_socket;


/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

int ClientMessageGenarator(clientmessage *message, char **message_string);
int ParsingClient(char *AcceptedStr, servermessage **message);
void MainClient(char* log_file,char *server_ip,int server_port, char *user_name,FILE *fp_log_client);
int ClientMessageBuilder(int opcode, char *param1, char *param2, char *param3, char *param4, clientmessage **msg);
int InitClient(char* log_file, char *server_ip, int server_port, char *user_name, FILE *fp_log_client, client **my_client, SOCKET m_socket);
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//#endif // SOCKET_EXAMPLE_CLIENT_H