#include <swilib.h>
#include <stdlib.h>
#include "ipc.h"

static void Send(int submess, void *data) {
    IPC_REQ *ipc = malloc(sizeof(IPC_REQ));
    ipc->name_to = IPC_NAME;
    ipc->name_from = IPC_NAME;
    ipc->data = data;
    GBS_SendMessage(MMI_CEPID, MSG_IPC, submess, ipc);
}

void IPC_Refresh(int tab_n, int item_n, const char *file_name) {
    IPC_DATA *ipc_data = malloc(sizeof(IPC_DATA));
    ipc_data->param0 = (void*)tab_n;
    ipc_data->param1 = (void*)item_n;
    ipc_data->param2 = (void*)file_name;
    Send(IPC_REFRESH, ipc_data);
}

void IPC_SetPBarData(int gui_id, int value, const WSHDR *ws) {
    IPC_DATA *ipc_data = malloc(sizeof(IPC_DATA));
    ipc_data->param0 = (void*)gui_id;
    ipc_data->param1 = (void*)value;
    ipc_data->param2 = (void*)ws;
    Send(IPC_SETPBARTEXT, ipc_data);
}
