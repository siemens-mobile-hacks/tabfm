#include <swilib.h>
#include <sie/sie.h>
#include <stdlib.h>
#include <string.h>
#include "../path.h"

#ifdef ELKA
    #define ICON_DIR   963
    #define ICON_UNK   1002
    #define ICON_BLANK 1336
#endif

typedef struct {
    int icon;
    char lgp[256];
    HEADER_DESC desc;
} TAB_HEADER;

typedef struct {
    TAB_HEADER header;
    PATH *path;
    SIE_FILE *files;
    int items_count;
} TAB_DATA;

const char LGP_BACK[] = "Back";
const char LGP_QUIT[] = "Quit";

static HEADER_DESC HEADER_D={{0, 0, 0, 0}, NULL, LGP_NULL, LGP_NULL};

static int ICONS_DEFAULT[] = { ICON_DIR, ICON_UNK, ICON_BLANK};

static const int SOFTKEYS[] = {0, 1, 2};

static SOFTKEY_DESC SOFTKEY_D[] = {
        {0x0018, 0x0000, (int)"Options"},
        {0x0001, 0x0000, (int)LGP_QUIT},
        {0x003D, 0x0000, (int)LGP_DOIT_PIC},
};

static const SOFTKEYSTAB SOFTKEYS_TAB = {
        SOFTKEY_D, 0,
};

void Navigate(GUI *tab_gui, const char *path, int item_n) {
    unsigned int items_count = 0;
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);
    char *mask = malloc(strlen(path) + 2 + 1);
    sprintf(mask, "%s\\*", path);
    SIE_FILE *files = Sie_FS_FindFiles(mask);
    files = Sie_FS_SortFilesByNameAsc(files, 1);
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

void UpdateHeader(GUI *tab_gui) {
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);
    sprintf(tab_data->header.lgp, "%s\\", tab_data->path->path);
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
    TAB_DATA *tab_data = MenuGetUserPointer(gui);
    if (cmd == TI_CMD_REDRAW) {
        UpdateHeader(gui);
        SOFTKEY_D[1].lgp_id = (tab_data->path->prev) ? (int)LGP_BACK : (int)LGP_QUIT;
    } else if (cmd == TI_CMD_DESTROY) {
        Sie_FS_DestroyFiles(tab_data->files);
        Path_Destroy(tab_data->path);
        mfree(tab_data);
    }
}

static void Locret() {}

static void ItemProc(void *gui, int item_n, void *data) {
    void *item = AllocMenuItem(gui);
    TAB_DATA *tab_data = (TAB_DATA*)data;

    WSHDR *ws = AllocMenuWS(gui, 256);
    SIE_FILE *file = Sie_FS_GetFileByID(tab_data->files, item_n);
    str_2ws(ws, file->file_name, 255);

    if (!(file->file_attr & SIE_FS_FA_DIRECTORY)) {
        int uid = GetExtUidByFileName_ws(ws);
        TREGEXPLEXT *reg_expl_ext = get_regextpnt_by_uid(uid);
        if (reg_expl_ext && strlen(reg_expl_ext->ext)) {
            int *icon = (int *)(reg_expl_ext->icon1);
            if (*icon > 0 && *icon < 3000) {
                SetMenuItemIconArray(gui, item, icon);
            }
            else if (strlen((const char*)*icon)) {
                char *ext = Sie_Ext_GetExtByFileName((const char*)*icon);
                if (strcmp(ext, "png") == 0) {
                    SetMenuItemIconArray(gui, item, icon);
                }
            } else {
                SetMenuItemIconArray(gui, item, &ICONS_DEFAULT[2]); // blank
            }
        } else {
            SetMenuItemIconArray(gui, item, &ICONS_DEFAULT[1]); // unknown
        }
    } else {
        SetMenuItemIconArray(gui, item, ICONS_DEFAULT); // dir
    }
    SetMenuItemText(gui, item, ws, item_n);
}

static const MENU_DESC DESC = {
        8,
        OnKey,
        GHook,
        Locret,
        SOFTKEYS,
        &SOFTKEYS_TAB,
        0x11,
        ItemProc,
        NULL,
        NULL,
};

void *CreateTabGUI(int tab_n) {
    TAB_DATA *tab_data = malloc(sizeof(TAB_DATA));
    HEADER_DESC *header_desc = &(tab_data->header.desc);

    zeromem(tab_data, sizeof(TAB_DATA));
    memcpy(header_desc, &HEADER_D, sizeof(HEADER_DESC));
    header_desc->icon = &(tab_data->header.icon);
    header_desc->lgp_id = (int)tab_data->header.lgp;

    GUI *tab_gui = GetMenuGUI(malloc_adr(), mfree_adr());
    Sie_GUI_PatchHeader(header_desc);
#ifdef ELKA
    tab_data->header.icon = 951;
#endif

    SetHeaderToMenu(tab_gui, header_desc, malloc_adr());
    SetMenuToGUI(tab_gui, &DESC);

    char str[32];
    sprintf(str, "%d:", (tab_n < 3) ? tab_n : 4);
    tab_data->path = Path_Push(NULL, str, 0);
    strcpy(tab_data->header.lgp, tab_data->path->path);
    sprintf(str, "%s\\*", tab_data->path->path);
    tab_data->files = Sie_FS_FindFiles(str);
    tab_data->files = Sie_FS_SortFilesByNameAsc(tab_data->files, 1);
    unsigned int count = Sie_FS_GetFilesCount(tab_data->files);
    SetMenuItemCount(tab_gui, (int)count);
    tab_data->items_count = count;

    MenuSetUserPointer(tab_gui, tab_data);
    UpdateHeader(tab_gui);

    return tab_gui;
}
