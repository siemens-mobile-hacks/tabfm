#include <swilib.h>
#include <string.h>
#include <sie/sie.h>
#include "tab.h"
#include "icons.h"

extern int MAIN_GUI_ID;
extern SIE_GUI_STACK *GUI_STACK;

static const int ICONS[][4] = {
        {0, 0, 0, ICON_DISK},
        {0, 0, 0, ICON_DISK},
        {0, 0, 0, ICON_DISK},
        {0, 0, 0, ICON_MMC}
};

static int OnKey(GUI *gui, GUI_MSG *msg) {
    if (msg->gbsmsg->submess == LEFT_BUTTON || msg->gbsmsg->submess == RIGHT_BUTTON) {
        RefreshGUI();
    }
    return 0;
}

static void GHook(GUI *gui, int cmd) {
    if (cmd == TI_CMD_FOCUS) {
        DisableIDLETMR();
    }
}

const TABGUI_DESC TABGUI_D = {
        8,
        OnKey,
        GHook,
        NULL,
        NULL,
        NULL,
};

int CreateMainGUI() {
    GUI *tab_gui = GetTabGUI(malloc_adr(), mfree_adr());
    SetGUIDefinition(tab_gui, &TABGUI_D);
    for (int i = 0; i < 4; i++) {
        void *gui = CreateTabGUI(i);
        SetGUIToTabGUI(tab_gui, i, ICONS[i], gui);
    }
    UpdateTab1stItem(tab_gui, 0);
    return CreateGUI(tab_gui);
}

void CloseChildrenGUI() {
    GUI_STACK = Sie_GUI_Stack_CloseChildren(GUI_STACK, MAIN_GUI_ID);
}
