#include <swilib.h>
#include "../ipc.h"
#include "../gui/gui.h"

extern int MAIN_GUI_ID;
extern int ORDERING, SHOW_HIDDEN_FILES;

void ChangeView(GUI *tab_gui, int ordering, int show_hidden_files) {
    ORDERING = ordering;
    SHOW_HIDDEN_FILES = show_hidden_files;
    CloseChildrenGUI();
    FocusGUI(MAIN_GUI_ID);
    for (int i = 0; i < 4; i++) IPC_Refresh(i, 0, NULL);
}
