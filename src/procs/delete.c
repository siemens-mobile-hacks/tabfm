#include <swilib.h>
#include <swilib/nucleus.h>
#include <sie/sie.h>
#include "../ipc.h"
#include "../gui/gui.h"
#include "../gui/tab.h"
#include "../gui/ui/pbar.h"
#include "../gui/ui/error.h"

typedef struct {
    SIE_FILE *files;
    unsigned int total_files;
    int pbar_gui_id;
} DATA;

extern int MAIN_GUI_ID;
extern int OPERATION_FLAG;

static DATA data;

void SUBPROC_Delete() {
    int i = 0;
    unsigned int err = 0;
    unsigned int success;
    SIE_FILE *file = data.files;
    while (file) {
        if (OPERATION_FLAG == -1) {
            break;
        }
        char *path = Sie_FS_GetPathByFile(file);
        SetPBarData(data.pbar_gui_id, file, i, data.total_files);
        if (file->file_attr & SIE_FS_FA_DIRECTORY) {
            success = Sie_FS_DeleteDirRecursive(path, &err);
        } else {
            success = Sie_FS_DeleteFile(path, &err);
        }
        if (!success) {
            ShowError(err, file->file_name);
        }
        mfree(path);
        SetPBarData(data.pbar_gui_id, file, i + 1, data.total_files);

        file = file->next;
        i++;
    }
    Sie_GUI_CloseGUI(data.pbar_gui_id);
    Sie_GUI_FocusGUI_Sync(MAIN_GUI_ID);
    IPC_Refresh(-1, -1, NULL);
    OPERATION_FLAG = 0;
}

static void YesNo(int flag) {
    if (flag == 0) {
        data.total_files = Sie_FS_GetFilesCount(data.files);
        data.pbar_gui_id = ShowPBar((int)"Deleting files...");
        Sie_SubProc_Run(SUBPROC_Delete, NULL);
    } else {
        OPERATION_FLAG = 0;
    }
}

static void Focus(SIE_FILE *files) {
    data.files = files;
    MsgBoxYesNo(1, (int)"Delete?", YesNo);
}

void Delete(GUI *tab_gui) {
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);
    if (!OPERATION_FLAG) {
        OPERATION_FLAG = 1;

        SIE_FILE *files = NULL;
        if (tab_data->selected_files) {
            files = tab_data->selected_files;
        } else if (tab_data->current_file) {
            files = tab_data->current_file;
        }
        if (files) {
            static GBSTMR tmr;
            static SIE_GUI_FOCUS_DATA focus_data = {};
            focus_data.gui_id = MAIN_GUI_ID;
            focus_data.proc = (void (*))(void *)Focus;
            focus_data.data = files;
            CloseChildrenGUI();
            Sie_GUI_FocusGUI_Async(&tmr, &focus_data);
        } else {
            OPERATION_FLAG = 0;
        }
    }
}
