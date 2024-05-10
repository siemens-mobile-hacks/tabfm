#include <swilib.h>
#include <sie/sie.h>
#include "../../ipc.h"

extern int OPERATION_FLAG;

static const int SOFTKEYS[] = {0, 1, 2};

static const SOFTKEY_DESC SOFTKEY_D[] = {
        {0x0000, 0x0000, (int)""},
        {0x0001, 0x0000, (int)"Cancel"},
        {0x0000, 0x0000, (int)" "},
};

static const SOFTKEYSTAB SOFTKEYS_TAB = {
        SOFTKEY_D, 0,
};

static void GHook(GUI *gui, int cmd) {
    if (cmd == TI_CMD_DESTROY) {
        OPERATION_FLAG = -1;
    }
}

static PBAR_DESC PBAR_D = {
        8,
        NULL,
        GHook,
        NULL,
        SOFTKEYS,
        &SOFTKEYS_TAB
};

void SetPBarData(int pbar_gui_id, SIE_FILE *file, unsigned int current, unsigned int total) {
    WSHDR *ws = AllocWS(192);
    WSHDR *file_name_ws = AllocWS(128);
    float percent = (float)(current) / (float)total;
    str_2ws(file_name_ws, file->file_name, 127);
    wsprintf(ws, "%d/%d\n%w", current, total, file_name_ws);
    IPC_SetPBarData(pbar_gui_id, (int)(100 * percent), ws);
    FreeWS(file_name_ws);
}

int ShowPBar(int message) {
    PBAR_D.proc3 = GetPBarProc3();
    return ShowPBarBox(1, message, &PBAR_D, 0, NULL);
}

