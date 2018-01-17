#include "client.h"


DWORD WINAPI ClientRx(LPVOID *LpParams);

HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE client_start_routine, client_info *client_params, DWORD *client_thread_id);
