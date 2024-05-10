#include <swilib.h>

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

int ShowPBar(int message) {
    PBAR_D.proc3 = GetPBarProc3();
    return ShowPBarBox(1, message, &PBAR_D, 0, NULL);
}
