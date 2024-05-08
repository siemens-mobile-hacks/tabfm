#include <swilib.h>
#include "ipc.h"

static void Send(int submess, void *data) {
    static IPC_REQ ipc = {};
    ipc.name_to = IPC_NAME;
    ipc.name_from = IPC_NAME;
    ipc.data = data;
    GBS_SendMessage(MMI_CEPID, MSG_IPC, submess, &ipc);
}

void IPC_Refresh(const char *file_name) {
    static IPC_DATA ipc_data = {};
    ipc_data.param0 = (void*)file_name;
    Send(IPC_REFRESH, &ipc_data);
}

void IPC_SetPBarData(int gui_id, int value, const WSHDR *ws) {
    static IPC_DATA ipc_data = {};
    ipc_data.param0 = (void*)gui_id;
    ipc_data.param1 = (void*)value;
    ipc_data.param2 = (void*)ws;
    Send(IPC_SETPBARTEXT, &ipc_data);
}
