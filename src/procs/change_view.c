#include <swilib.h>
#include "../ipc.h"
#include "../gui/gui.h"

extern int MAIN_GUI_ID;
extern int SHOW_HIDDEN_FILES;

void ChangeView(GUI *tab_gui, int show_hidden_files) {
    SHOW_HIDDEN_FILES = show_hidden_files;
    CloseChildrenGUI();
    FocusGUI(MAIN_GUI_ID);
    IPC_Refresh(-1, NULL);
}
