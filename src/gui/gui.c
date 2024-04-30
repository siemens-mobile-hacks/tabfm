#include <swilib.h>
#include <string.h>
#include "tab.h"

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

static void Locret() {}

const TABGUI_DESC TABGUI_D = {
        8,
        OnKey,
        GHook,
        Locret,
        NULL,
        NULL,
};

int CreateMainGUI() {
    GUI *tab_gui = GetTabGUI(malloc_adr(), mfree_adr());
    SetGUIDefinition(tab_gui, &TABGUI_D);
    for (int i = 0; i < 4; i++) {
        void *gui = CreateTabGUI(i);
        SetGUIToTabGUI(tab_gui, i, NULL, gui);
    }
    UpdateTab1stItem(tab_gui, 0);
    return CreateGUI(tab_gui);
}
