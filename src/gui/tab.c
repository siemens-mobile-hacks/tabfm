#include <swilib.h>
#include <sie/sie.h>
#include <stdlib.h>
#include <string.h>
#include "../path.h"

typedef struct {
    PATH *path;
    SIE_FILE *files;
} TAB_DATA;

const char LGP_BACK[] = "Back";
const char LGP_QUIT[] = "Quit";

static const int SK[] = {0,1,2};

static SOFTKEY_DESC SK_DESC[] = {
        {0x0018, 0x0000, (int)"Options"},
        {0x0001, 0x0000, (int)LGP_QUIT},
        {0x003D, 0x0000, (int)LGP_DOIT_PIC},
};

static const SOFTKEYSTAB SK_TAB = {
        SK_DESC, 0,
};

void Navigate(GUI *tab_gui, const char *path, int item_n) {
    unsigned int items_count = 0;
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);
    char *mask = malloc(strlen(path) + 2 + 1);
    sprintf(mask, "%s\\*", path);
    SIE_FILE *files = Sie_FS_FindFiles(mask);
    if (files) {
        Sie_FS_DestroyFiles(tab_data->files);
        tab_data->files = files;
        items_count = Sie_FS_GetFilesCount(tab_data->files);
    } else {
        item_n = 0;
    }
    Menu_SetItemCountDyn(tab_gui, (int)items_count);
    UpdateMenuCursorItem(tab_gui, item_n);
    mfree(mask);
}

static int OnKey(GUI *gui, GUI_MSG *msg) {
    TAB_DATA *tab_data = (TAB_DATA*)MenuGetUserPointer(gui);
    int item_n = GetCurMenuItem(gui);
    if (msg->keys == 0x01) {
        if (tab_data->path->prev) {
            PATH *path = Path_Pop(tab_data->path);
            tab_data->path = path;
            Navigate(gui, tab_data->path->path, tab_data->path->item_n);
            return -1;
        } else {
            return 1;
        }
    }
    else if (msg->keys == 0x3D) {
        SIE_FILE *file = Sie_FS_GetFileByID(tab_data->files, item_n);
        if (file) {
            if (file->file_attr & SIE_FS_FA_DIRECTORY) {
                char *path = Sie_FS_GetPathByFile(file);
                tab_data->path = Path_Push(tab_data->path, path, item_n);
                mfree(path);
                Navigate(gui, tab_data->path->path, 0);
            }
        }
    }
    return 0;
}

static void GHook(GUI *gui, int cmd) {
    TAB_DATA *gui_data = MenuGetUserPointer(gui);
    if (cmd == TI_CMD_REDRAW) {
        SK_DESC[1].lgp_id = (gui_data->path->prev) ? (int)LGP_BACK : (int)LGP_QUIT;
    } else if (cmd == TI_CMD_DESTROY) {
        Sie_FS_DestroyFiles(gui_data->files);
        Path_Destroy(gui_data->path);
        mfree(gui_data);
    }
}

static void Locret() {}

static void ItemProc(void *gui, int item_n, void *data) {
    void *item = AllocMenuItem(gui);
    TAB_DATA *gui_data = (TAB_DATA*)data;

    WSHDR *ws = AllocMenuWS(gui, 256);
    SIE_FILE *file = Sie_FS_GetFileByID(gui_data->files, item_n);
    str_2ws(ws, file->file_name, 255);
    SetMenuItemText(gui, item, ws, item_n);
}

static const MENU_DESC DESC = {
        8,
        OnKey,
        GHook,
        Locret,
        SK,
        &SK_TAB,
        0x10,
        ItemProc,
        NULL,
        0
};

void *CreateTabGUI(int tab_n) {
    void *gui = GetMenuGUI(malloc_adr(), mfree_adr());
    SetMenuToGUI(gui, &DESC);

    char str[32];
    TAB_DATA *gui_data = malloc(sizeof(TAB_DATA));
    zeromem(gui_data, sizeof(TAB_DATA));
    sprintf(str, "%d:", (tab_n < 3) ? tab_n : 4);
    gui_data->path = Path_Push(NULL, str, 0);
    sprintf(str, "%s\\*", gui_data->path->path);
    gui_data->files = Sie_FS_FindFiles(str);
    SetMenuItemCount(gui, (int)Sie_FS_GetFilesCount(gui_data->files));

    MenuSetUserPointer(gui, gui_data);
    return gui;
}
