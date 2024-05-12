#include <swilib.h>
#include <sie/sie.h>
#include "../tab.h"
#include "../gui.h"
#include "../../ipc.h"

static HEADER_DESC HEADER_D = {{0, 0, 0, 0}, NULL, (int)"Rename",LGP_NULL};

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
        if (strcmpi(tab_data->current_file->file_name, file_name) != 0) {
            unsigned int err;
            char old_name[256], new_name[256];
            sprintf(old_name, "%s%s", tab_data->current_file->dir_name, tab_data->current_file->file_name);
            sprintf(new_name, "%s%s", tab_data->current_file->dir_name, file_name);
            if (sys_rename(old_name, new_name, &err)) {
                CloseChildrenGUI();
                IPC_Refresh(-1, -1, file_name);
            } else {
                MsgBoxError(1, (int)"Rename error!");
                res = -1;
            }
        } else {
            CloseChildrenGUI();
        }
    }
    return res;
}

static void GHook(GUI *gui, int cmd) {
    if (cmd == TI_CMD_REDRAW) {
        static SOFTKEY_DESC sk_save ={0x001A, 0x0000, (int)"Save"};
        static SOFTKEY_DESC sk_middle = {0x001A, 0x0000, LGP_SAVE_PIC};
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

int CreateInputTextDialog_Rename(GUI *tab_gui) {
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);

    void *ma = malloc_adr();
    void *eq = AllocEQueue(ma, mfree_adr());

    EDITCONTROL ec;
    WSHDR *ws = AllocWS(127);
    PrepareEditControl(&ec);
    wsprintf(ws, "%s", "Name:");
    ConstructEditControl(&ec, ECT_HEADER, ECF_APPEND_EOL, ws, 32);
    AddEditControlToEditQend(eq, &ec, ma);

    str_2ws(ws, tab_data->current_file->file_name, 127);
    ConstructEditControl(&ec, ECT_NORMAL_TEXT, ECF_SET_CURSOR_END, ws, 127);
    AddEditControlToEditQend(eq, &ec, ma);
    FreeWS(ws);

    Sie_GUI_InitHeader(&HEADER_D);
    Sie_GUI_InitInput(&INPUTDIA_D);
    return CreateInputTextDialog(&INPUTDIA_D, &HEADER_D, eq, 1, tab_gui);
}
