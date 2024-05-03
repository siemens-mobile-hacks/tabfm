#include <swilib.h>
#include <sie/sie.h>
#include "../ipc.h"
#include "../gui/gui.h"
#include "../gui/tab.h"

typedef struct {
    GUI *tab_gui;
    SIE_FILE *src_file;
    char *out_dir;
    int waitbox_gui_id;
} DATA;

char DEST_FILE_NAME[128];

extern int MAIN_GUI_ID;
extern SIE_GUI_STACK *GUI_STACK;

void SUBPROC_CreateNewFile(DATA *data) {
    unsigned int err = 0;

    SIE_FILE *file = Sie_FS_CopyFileElement(data->src_file);
    file->dir_name = realloc(file->dir_name, strlen(data->out_dir) + 1);
    strcpy(file->dir_name, data->out_dir);
    SIE_FILE *dest_file = Sie_FS_GetUniqueFile(file);
    Sie_FS_DestroyFileElement(file);

    char *src = Sie_FS_GetPathByFile(data->src_file);
    char *dest = Sie_FS_GetPathByFile(dest_file);
    int status = Sie_FS_CopyFile(src, dest, &err);
    Sie_GUI_CloseGUI(data->waitbox_gui_id);
    FocusGUI(MAIN_GUI_ID);
    mfree(src);
    mfree(dest);

    if (status >= 0) {
        strcpy(DEST_FILE_NAME, dest_file->file_name);
        IPC_Refresh(DEST_FILE_NAME);
    } else {
    }
    Sie_FS_DestroyFileElement(data->src_file);
    Sie_FS_DestroyFileElement(dest_file);
}

static void Focus(DATA *data) {
    data->waitbox_gui_id = ShowWaitBox(1, (int)"Creating file...");
    Sie_SubProc_Run(SUBPROC_CreateNewFile, (void*)data);
}

void CreateNewFile(SIE_FILE *file, const char *out_dir) {
    static GBSTMR tmr;
    static DATA data;
    static SIE_GUI_FOCUS_DATA focus_data;
    data.src_file = Sie_FS_CopyFileElement(file);
    data.out_dir = (char*)out_dir;
    focus_data.gui_id = MAIN_GUI_ID;
    focus_data.proc = (void(*))(void*)Focus;
    focus_data.data = &data;
    CloseChildrenGUI();
    Sie_GUI_FocusGUI(&tmr, &focus_data);
}