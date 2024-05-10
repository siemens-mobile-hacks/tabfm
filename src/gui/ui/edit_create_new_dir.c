#include <swilib.h>
#include "sie/sie.h"
#include "../gui.h"
#include "../tab.h"
#include "../../ipc.h"

#define SOFTKEY_SAVE   {0x001A, 0x0000, (int)"Save"}
#define SOFTKEY_MIDDLE {0x001A, 0x0000, LGP_SAVE_PIC}

static HEADER_DESC HEADER_D = {{0, 0, 0, 0}, NULL, (int)"New directory",LGP_NULL};

static const SOFTKEYSTAB SOFTKEYS_TAB = {
        NULL, 0,
};

static int OnKey(GUI *gui, GUI_MSG *msg) {
    int res = 0;
    GUI *tab_gui = EDIT_GetUserPointer(gui);
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);

    if (msg->keys == 0x1A) {
        EDITCONTROL ec;
        ExtractEditControl(gui, 2, &ec);

        static char file_name[128];
        ws_2str(ec.pWS, file_name, 127);
        if (strlen(file_name)) {
            char path[256];
            unsigned int err;
            sprintf(path, "%s%s", tab_data->path->path, file_name);
            if (isdir(path, &err) == 1) {
                MsgBoxError(1, (int)"The directory is exists!");
                res = -1;
            } else {
                if (sys_mkdir(path, &err)) {
                    CloseChildrenGUI();
                    IPC_Refresh(-1, file_name);
                } else {
                    MsgBoxError(1, (int)"Create directory error!");
                    res = -1;
                }
            }
        } else {
            MsgBoxError(1, (int)"Invalid input!");
            res = -1;
        }
    }
    return res;
}

static void GHook(GUI *gui, int cmd) {
    static SOFTKEY_DESC sk_save = SOFTKEY_SAVE;
    static SOFTKEY_DESC sk_middle = SOFTKEY_MIDDLE;
    if (cmd == TI_CMD_REDRAW) {
        SetSoftKey(gui, &sk_save, SET_LEFT_SOFTKEY);
        SetSoftKey(gui, &sk_middle, SET_MIDDLE_SOFTKEY);
    }
}

static INPUTDIA_DESC INPUTDIA_D = {
        1,
        OnKey,
        GHook,
        NULL,
        0,
        &SOFTKEYS_TAB,
        {0, 0, 0, 0},
        FONT_SMALL,
        100,
        101,
        0,
        { 0 },
        { INPUTDIA_FLAGS_SWAP_SOFTKEYS },
};

int CreateInputTextDialog_NewDir(GUI *tab_gui) {
    void *ma = malloc_adr();
    void *eq = AllocEQueue(ma, mfree_adr());

    EDITCONTROL ec;
    WSHDR *ws = AllocWS(32);
    PrepareEditControl(&ec);
    wsprintf(ws, "%s", "Directory name:");
    ConstructEditControl(&ec, ECT_HEADER, ECF_APPEND_EOL, ws, 32);
    AddEditControlToEditQend(eq, &ec, ma);
    wsprintf(ws, "%s", "Untitled folder");
    ConstructEditControl(&ec, ECT_NORMAL_TEXT, ECF_SET_CURSOR_END, ws, 127);
    AddEditControlToEditQend(eq, &ec, ma);
    FreeWS(ws);

    Sie_GUI_InitHeader(&HEADER_D);
    Sie_GUI_InitInput(&INPUTDIA_D);
    return CreateInputTextDialog(&INPUTDIA_D, &HEADER_D, eq, 1, tab_gui);
}
