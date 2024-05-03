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
    Send(IPC_REFRESH, (void*)file_name);
}
