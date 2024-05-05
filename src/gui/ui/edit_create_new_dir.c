#include <swilib.h>
#include "sie/sie.h"
#include "../gui.h"
#include "../tab.h"
#include "../../ipc.h"

static HEADER_DESC HEADER_D = {{0, 0, 0, 0}, NULL, (int)"New directory",LGP_NULL};

static const SOFTKEYSTAB SOFTKEYS_TAB = {
        NULL, 0,
};

int OnKey(GUI *gui, GUI_MSG *msg) {
    int res = 0;
    GUI *tab_gui = EDIT_GetUserPointer(gui);
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);

    if (msg->keys == 0x1A) {
        EDITCONTROL ec;
        ExtractEditControl(gui, 2, &ec);

        unsigned int err;
        WSHDR *ws = AllocWS(256);
        str_2ws(ws, tab_data->path->path, 256);
        wstrcat(ws, ec.pWS);
        if (isdir_ws(ws, &err) == 1) {
            MsgBoxError(1, (int)"The directory is exists");
            res = -1;
        } else {
            static char dir[128];
            sys_mkdir_ws(ws, &err);
            ws_2str(ec.pWS, dir, 127);
            CloseChildrenGUI();
            IPC_Refresh(dir);
        }
        FreeWS(ws);
    }
    return res;
}

static INPUTDIA_DESC INPUTDIA_D = {
        1,
        OnKey,
        NULL,
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
    ConstructEditControl(&ec, ECT_NORMAL_TEXT, ECF_SET_CURSOR_END, ws, 128);
    AddEditControlToEditQend(eq, &ec, ma);
    FreeWS(ws);

    Sie_GUI_InitHeader(&HEADER_D);
    Sie_GUI_InitInput(&INPUTDIA_D);
    return CreateInputTextDialog(&INPUTDIA_D, &HEADER_D, eq, 1, tab_gui);
}
