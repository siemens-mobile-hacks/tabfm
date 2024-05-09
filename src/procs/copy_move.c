#include <swilib.h>
#include <sie/sie.h>
#include "../ipc.h"
#include "../gui/gui.h"
#include "../gui/tab.h"

#define COPY 0x00
#define MOVE 0x01

extern int MAIN_GUI_ID;
extern int IN_PROGRESS;
extern SIE_FILE *COPY_FILES, *MOVE_FILES;

void CopyMove(GUI *tab_gui, int t) {
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);
    if (!IN_PROGRESS) {
        IN_PROGRESS = 1;

        Sie_FS_DestroyFiles(COPY_FILES);
        Sie_FS_DestroyFiles(MOVE_FILES);
        COPY_FILES = MOVE_FILES = NULL;

        SIE_FILE *files = NULL;
        if (tab_data->selected_files) {
            files = Sie_FS_CloneFiles(tab_data->selected_files);
        } else if (tab_data->current_file) {
            files = Sie_FS_CopyFileElement(tab_data->current_file);
        }
        CloseChildrenGUI();
        FocusGUI(MAIN_GUI_ID);
        if (files) {
            if (t == COPY) {
                COPY_FILES = files;
            } else {
                MOVE_FILES = files;
            }
        }
        IPC_Refresh(NULL);
        IN_PROGRESS = 0;
    }
}

void Copy(GUI *tab_gui) {
    CopyMove(tab_gui, COPY);
}

void Move(GUI *tab_gui) {
    CopyMove(tab_gui, MOVE);
}
