#include <swilib.h>
#include <swilib/nucleus.h>
#include <sie/sie.h>
#include "../ipc.h"
#include "../gui/gui.h"
#include "../gui/tab.h"
#include "../gui/ui/pbar.h"

typedef struct {
    SIE_FILE *files;
    unsigned int total_files;
    int pbar_box_id;
} DATA;

extern int MAIN_GUI_ID;
extern int OPERATION_FLAG;

static DATA data;

static void SetPBarData(SIE_FILE *file, int i) {
    WSHDR *ws = AllocWS(192);
    WSHDR *file_name_ws = AllocWS(128);
    float percent = (float)(i + 1) / (float)data.total_files;
    str_2ws(file_name_ws, file->file_name, 127);
    wsprintf(ws, "%d/%d\n%w", i + 1, data.total_files, file_name_ws);
    IPC_SetPBarData(data.pbar_box_id, (int)(100 * percent), ws);
    FreeWS(file_name_ws);
}

void SUBPROC_Delete() {
    int i = 0;
    unsigned int err;
    SIE_FILE *file = data.files;
    while (file) {
        if (OPERATION_FLAG == -1) {
            break;
        }

        SetPBarData(file, i);
        char *path = Sie_FS_GetPathByFile(file);
        if (file->file_attr & SIE_FS_FA_DIRECTORY) {
            Sie_FS_DeleteDirRecursive(path, &err);
        } else {
            Sie_FS_DeleteFile(path, &err);
        }
        mfree(path);

        file = file->next;
        i++;
    }
    Sie_GUI_CloseGUI(data.pbar_box_id);
    Sie_GUI_FocusGUI_Sync(MAIN_GUI_ID);
    IPC_Refresh(NULL);
    OPERATION_FLAG = 0;
}

static void YesNo(int flag) {
    if (flag == 0) {
        data.total_files = Sie_FS_GetFilesCount(data.files);
        data.pbar_box_id = ShowPBar((int)"Deleting files...");
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
