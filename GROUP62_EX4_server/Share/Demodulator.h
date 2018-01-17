
#include "MainStructs.h"

//define TCP protocol 
#define NUM_OF_CLIENT_MESSAGES 5
#define NUM_OF_SERVER_MESSAGES 11
#define MAX_MESSAGETYPE_CHAR 20
#define MAX_MESSAGEPARAMS_CHAR 20
#define EMPTY_PARAM "empty@"
//message type

char client_messages[NUM_OF_CLIENT_MESSAGES][MAX_MESSAGETYPE_CHAR] = { "NEW_USER_REQUEST", "PLAY_REQUEST", "USER_LIST_QUERY", "GAME_STATE_QUERY", "BOARD_VIEW_QUERY" };
int num_of_client_message_params[5] = { 1,2,0,2,0 };

char server_messages[NUM_OF_SERVER_MESSAGES][MAX_MESSAGETYPE_CHAR] = { "NEW_USER_ACCEPTED", "NEW_USER_DECLINED", "GAME_STARTED", "BOARD_VIEW", "TURN_SWITCH",
"PLAY_ACCEPTED", "PLAY_DECLINED", "GAME_ENDED", "USER_LIST_REPLAY", "GAME_STATE_REPLAY", "BOARD_VIEW_REPLAY" };
int num_of_server_message_params[5] = { 1,2,0,2,0 };

typedef struct {
	int opcode;
	char param1[MAX_MESSAGEPARAMS_CHAR];
	char param2[MAX_MESSAGEPARAMS_CHAR];
	char param3[MAX_MESSAGEPARAMS_CHAR];

} clientmessage;

typedef struct {
	int opcode;
	char param1[MAX_MESSAGEPARAMS_CHAR];
	char param2[MAX_MESSAGEPARAMS_CHAR];
	char param3[MAX_MESSAGEPARAMS_CHAR];
	char param4[MAX_MESSAGEPARAMS_CHAR];
	char param5[MAX_MESSAGEPARAMS_CHAR];
	char param6[MAX_MESSAGEPARAMS_CHAR];

} servermessage;


int ClientMessageGenarator(clientmessage *message, char **message_string);

int ClientMessageDemodulator(clientmessage **message, char *message_string);

int ServerMessageGenarator(servermessage *message, char **message_string);

int ServerMessageDemodulator(servermessage **message, char *message_string);