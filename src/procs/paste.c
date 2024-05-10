#include <swilib.h>
#include <swilib/nucleus.h>
#include <sie/sie.h>
#include "../ipc.h"
#include "../gui/gui.h"
#include "../gui/tab.h"
#include "../gui/ui/pbar.h"

extern int MAIN_GUI_ID;
extern int OPERATION_FLAG;
extern SIE_FILE *COPY_FILES, *MOVE_FILES;

char *DIR_NAME;
char DEST_FILE_NAME[256];

int WAIT_FLAG;
SIE_FILE *WAIT_FILE;

typedef struct {
    int src_tab_n;
    unsigned int total_files;
    int pbar_gui_id;
} DATA;

inline static SIE_FILE *GetFiles() {
    return (COPY_FILES) ? COPY_FILES : MOVE_FILES;
}

static int GetCancelFlag() { return OPERATION_FLAG == -1;}

static unsigned int CopyMoveFile(SIE_FILE *src_file, const char *dest) {
    unsigned int result = 0;
    unsigned int err = 0;
    char *src = Sie_FS_GetPathByFile(src_file);
    if (COPY_FILES) {
        if (!Sie_FS_IsDir(src, &err)) {
            result = (Sie_FS_CopyFile(src, dest, &err) >= 0);
        } else {
            result = Sie_FS_CopyDir(src, dest, &err, GetCancelFlag);
        }
    } else {
        result = Sie_FS_MoveFile(src, dest, &err, GetCancelFlag);
    }
    mfree(src);
    return result;
}

static SIE_FILE *GetUniqueFile(SIE_FILE *file, const char *dir) {
    SIE_FILE *f = Sie_FS_CopyFileElement(file);
    mfree(f->dir_name);
    f->dir_name = malloc(strlen(dir) + 1);
    strcpy(f->dir_name, dir);

    SIE_FILE *unique = Sie_FS_GetUniqueFile(f);
    Sie_FS_DestroyFileElement(f);
    return unique;
}

static void YesNo(int flag) {
    unsigned int err;
    unsigned int success = 0;
    SIE_FILE *new_file = Sie_FS_CopyFileElement(WAIT_FILE);
    if (flag == 0) { // Replace
        mfree(new_file->dir_name);
        new_file->dir_name = malloc(strlen(DIR_NAME) + 1);
        strcpy(new_file->dir_name, DIR_NAME);

        char *dest = Sie_FS_GetPathByFile(new_file);
        if (new_file->file_attr & SIE_FS_FA_DIRECTORY) {
            Sie_FS_DeleteDirRecursive(dest, &err);
        } else {
            Sie_FS_DeleteFile(dest, &err);
        }
        success = CopyMoveFile(WAIT_FILE, dest);
        mfree(dest);

    } else { // Paste
        new_file = GetUniqueFile(new_file, DIR_NAME);
        char *dest = Sie_FS_GetPathByFile(new_file);
        success = CopyMoveFile(WAIT_FILE, dest);
        mfree(dest);
    }
    if (success) {
        strcpy(DEST_FILE_NAME, new_file->file_name);
    }
    Sie_FS_DestroyFileElement(new_file);
    WAIT_FLAG = 2;
}

void SUBPROC_Paste(DATA *data) {
    SIE_FILE *files = GetFiles();

    int i = 0;
    SIE_FILE *file = files;
    while (file) {
        if (OPERATION_FLAG == -1) {
            break;
        }

        if (WAIT_FLAG == 1) {
            NU_Sleep(50);
            continue;
        } else if (WAIT_FLAG == 2) {
            WAIT_FLAG = 0;
            goto END;
        }
        if (strcmpi(file->dir_name, DIR_NAME) == 0) { // current dir
            unsigned int err = 0;
            unsigned int success = 0;
            SIE_FILE *new_file = Sie_FS_GetUniqueFile(file);
            char *dest = Sie_FS_GetPathByFile(new_file);
            SetPBarData(data->pbar_gui_id, new_file, i, data->total_files);
            if (COPY_FILES) {
                char *src = Sie_FS_GetPathByFile(file);
                if (!Sie_FS_IsDir(src, &err)) { // file
                    success = (Sie_FS_CopyFile(src, dest, &err) >= 0);
                } else {
                    success = Sie_FS_CopyDir(src, dest, &err, GetCancelFlag);
                }
                mfree(src);
            }
            if (success) {
                strcpy(DEST_FILE_NAME, new_file->file_name);
            }
            mfree(dest);
            Sie_FS_DestroyFileElement(new_file);
            SetPBarData(data->pbar_gui_id, new_file, i + 1, data->total_files);
        } else {
            char *dest = malloc(strlen(DIR_NAME) + strlen(file->file_name) + 1);
            sprintf(dest, "%s%s", DIR_NAME, file->file_name);
            SetPBarData(data->pbar_gui_id, file, i, data->total_files);
            if (Sie_FS_FileExists(dest)) {
                char msg[160];
                sprintf(msg, "Replace file: %s?", file->file_name);
                MsgBoxYesNo(1, (int)msg, YesNo);
                WAIT_FLAG = 1;
                WAIT_FILE = file;
                continue;
            } else {
                if (CopyMoveFile(file, dest)) {
                    strcpy(DEST_FILE_NAME, file->file_name);
                }
            }
            mfree(dest);
            SetPBarData(data->pbar_gui_id, file, i + 1, data->total_files);
        }
        END:
        file = file->next;
        i++;
    }
    Sie_FS_DestroyFiles(COPY_FILES);
    Sie_FS_DestroyFiles(MOVE_FILES);
    COPY_FILES = MOVE_FILES = NULL;

    IPC_Refresh(data->src_tab_n, NULL);
    Sie_GUI_CloseGUI(data->pbar_gui_id);
    Sie_GUI_FocusGUI_Sync(MAIN_GUI_ID);
    IPC_Refresh(-1, DEST_FILE_NAME);
    WAIT_FLAG = 0;
    OPERATION_FLAG = 0;
}

static void Focus(DATA *data) {
    char *message = (COPY_FILES) ? "Copying files..." : "Moving files...";
    data->pbar_gui_id = ShowPBar((int)message);
    data->total_files = Sie_FS_GetFilesCount(GetFiles());
    Sie_SubProc_Run(SUBPROC_Paste, data);
}

void Paste(GUI *tab_gui) {
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);
    if (!OPERATION_FLAG && (COPY_FILES || MOVE_FILES)) {
        OPERATION_FLAG = 1;

        SIE_FILE *files = GetFiles();
        int src_disk = files->dir_name[0] - '0';

        static GBSTMR tmr;
        static DATA data;
        static SIE_GUI_FOCUS_DATA focus_data = {};
        DIR_NAME = tab_data->path->path;
        data.src_tab_n = (src_disk < 4) ? src_disk : src_disk - 1;
        focus_data.gui_id = MAIN_GUI_ID;
        focus_data.proc = (void (*))(void *)Focus;
        focus_data.data = &data;
        CloseChildrenGUI();
        Sie_GUI_FocusGUI_Async(&tmr, &focus_data);
    }
}


