#include <swilib.h>
#include <sie/sie.h>
#include <stdlib.h>
#include <string.h>
#include "../procs/procs.h"
#include "gui.h"
#include "tab.h"
#include "icons.h"
#include "ui/menu_options.h"

#define LGP_BACK "Back"
#define LGP_EXIT "Exit"

#define SOFTKEY_BACK    {0x0001, 0x0000, (int)LGP_NULL}
#define SOFTKEY_MIDDLE  {0x003D, 0x0000, (int)LGP_DOIT_PIC}

extern int OPERATION_FLAG;
extern SIE_GUI_STACK *GUI_STACK;

extern int ORDERING, SHOW_HIDDEN_FILES;

static int ICON_HEADER = 951;
static HEADER_DESC HEADER_D={{0, 0, 0, 0}, &ICON_HEADER, LGP_NULL, LGP_NULL};

static const int ICONS[] = {ICON_DIR, ICON_UNK, ICON_BLANK, ICON_CHECK};

static const int SOFTKEYS[] = {0, 1, 2};

static SOFTKEY_DESC SOFTKEY_D[] = {
        {0x0018, 0x0000, (int)"Options"},
        SOFTKEY_BACK, SOFTKEY_MIDDLE
};

static const SOFTKEYSTAB SOFTKEYS_TAB = {
        SOFTKEY_D, 0,
};

void SetFiles(GUI *tab_gui, const char *path) {
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);
    char *mask = malloc(strlen(path) + 2 + 1);
    sprintf(mask, "%s*", path);
    tab_data->files = Sie_FS_FindFiles(mask);
    mfree(mask);
    if (!SHOW_HIDDEN_FILES) {
        tab_data->files = Sie_FS_ExcludeFilesByFileAttr(tab_data->files, SIE_FS_FA_HIDDEN);
    }
    switch (ORDERING) {
        case ORDERING_BY_NAME_ASC:
            tab_data->files = Sie_FS_SortFilesByNameAsc(tab_data->files, 1);
            break;
        case ORDERING_BY_NAME_DESC:
            tab_data->files = Sie_FS_SortFilesByNameDesc(tab_data->files, 1);
            break;
        default:
            tab_data->files = Sie_FS_SortFilesByNameAsc(tab_data->files, 1);
    }
}

void RefreshHeader(GUI *tab_gui) {
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);

    WSHDR *ws = AllocWS(256);
    str_2ws(ws, tab_data->path->path, 256);
    SetHeaderText(GetHeaderPointer(tab_gui), ws, malloc_adr(), mfree_adr());
}

void Navigate(GUI *tab_gui, const char *path) {
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);

    Sie_FS_DestroyFiles(tab_data->files);
    Sie_FS_DestroyFiles(tab_data->selected_files);
    tab_data->selected_files = NULL;
    SetFiles(tab_gui, path);
    RefreshHeader(tab_gui);
}

void RefreshTab(GUI *tab_gui, int item_n, const char *file_name) {
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);
    unsigned int items_count = 0;
    if (tab_data->files) {
        if (file_name) {
            item_n = Sie_FS_GetIDByFileName(tab_data->files, file_name);
        } else if (item_n < 0) {
            item_n = GetCurMenuItem(tab_gui);
        }
        items_count = Sie_FS_GetFilesCount(tab_data->files);
    }
    if (item_n < 0) {
        item_n = 0;
    }
    if (item_n > items_count - 1) {
        item_n = (int)items_count - 1;
    }
    Menu_SetItemCountDyn(tab_gui, (int)items_count);
    tab_data->path->item_n = item_n;
    UpdateMenuCursorItem(tab_gui, item_n);
}

static int OnKey(GUI *gui, GUI_MSG *msg) {
    TAB_DATA *tab_data = (TAB_DATA*)MenuGetUserPointer(gui);
    int item_n = GetCurMenuItem(gui);

    if (msg->keys == 0x18) { // options
        if (!OPERATION_FLAG) {
            GUI_STACK = Sie_GUI_Stack_Add(GUI_STACK, CreateMenu_Options(gui));
        }
    } else if (msg->keys == 0x3D) { // enter
        SIE_FILE *file = Sie_FS_GetFileByID(tab_data->files, item_n);
        if (file) {
            if (tab_data->selected_files) {
                ToggleMark(gui);
                RefreshGUI();
            } else {
                char *path = Sie_FS_GetPathByFile(file);
                if (file->file_attr & SIE_FS_FA_DIRECTORY) {
                    tab_data->path = Path_Push(tab_data->path, path, item_n);
                    Navigate(gui, tab_data->path->path);
                    RefreshTab(gui, 0, NULL);
                } else {
                    Sie_Exec_File(path);
                }
                mfree(path);
            }
        }
    } else if (msg->keys == 0x01) { // back
        if (tab_data->path->prev) {
            PATH *path = Path_Pop(tab_data->path);
            tab_data->path = path;
            Navigate(gui, tab_data->path->path);
            RefreshTab(gui, tab_data->path->item_n, NULL);
            return -1;
        } else {
            return 1;
        }
    } else {
        if (msg->gbsmsg->msg == KEY_DOWN || msg->gbsmsg->msg == LONG_PRESS) {
            switch (msg->gbsmsg->submess) {
                case GREEN_BUTTON:
                    Paste(gui);
                    break;
                case '*':
                    ToggleMark(gui);
                    RefreshGUI();
                    break;
                case '#':
                    Delete(gui);
                    break;
            }
        }
    }
    return 0;
}

static void GHook(GUI *gui, int cmd) {
    TAB_DATA *tab_data = MenuGetUserPointer(gui);
    int item_n = GetCurMenuItem(gui);
    if (cmd == TI_CMD_REDRAW) {
        static SOFTKEY_DESC sk_back = SOFTKEY_BACK;
        static SOFTKEY_DESC sk_middle = SOFTKEY_MIDDLE;
        Sie_FS_DestroyFiles(tab_data->current_file);
        tab_data->current_file = NULL;
        if (tab_data->files) {
            SIE_FILE *file = Sie_FS_GetFileByID(tab_data->files, item_n);
            if (file) {
                tab_data->current_file = Sie_FS_CopyFileElement(file);
            }
        }
        sk_back.lgp_id = (tab_data->path->prev) ? (int)LGP_BACK : (int)LGP_EXIT;
        sk_middle.lgp_id = (tab_data->selected_files) ? LGP_CHANGE_PIC : LGP_DOIT_PIC;
        SetMenuSoftKey(gui, &sk_back, SET_RIGHT_SOFTKEY);
        SetMenuSoftKey(gui, &sk_middle, SET_MIDDLE_SOFTKEY);
    } else if (cmd == TI_CMD_DESTROY) {
        Sie_FS_DestroyFiles(tab_data->files);
        Sie_FS_DestroyFiles(tab_data->current_file);
        Sie_FS_DestroyFiles(tab_data->selected_files);
        Path_Destroy(tab_data->path);
        mfree(tab_data);
    }
}

static void Locret() {}

static void ItemProc(void *gui, int item_n, void *data) {
    TAB_DATA *tab_data = (TAB_DATA*)data;

    void *item = AllocMenuItem(gui);
    WSHDR *ws = AllocMenuWS(gui, 256);
    SIE_FILE *file = Sie_FS_GetFileByID(tab_data->files, item_n);

    str_2ws(ws, file->file_name, 255);
    if (Sie_FS_ContainsFile(tab_data->selected_files, file)) { // marked
        SetMenuItemIconArray(gui, item, &ICONS[3]);
    } else {
        if (!(file->file_attr & SIE_FS_FA_DIRECTORY)) {
            int uid = GetExtUidByFileName_ws(ws);
            TREGEXPLEXT *reg_expl_ext = get_regextpnt_by_uid(uid);
            if (reg_expl_ext && strlen(reg_expl_ext->ext)) {
                int *icon = (int *)(reg_expl_ext->icon1);
                if (*icon > 0 && *icon < 3000) {
                    SetMenuItemIconArray(gui, item, icon);
                } else if (strlen((const char *)*icon)) {
                    char *ext = Sie_Ext_GetExtByFileName((const char *)*icon);
                    if (strcmp(ext, "png") == 0) {
                        SetMenuItemIconArray(gui, item, icon);
                    }
                } else {
                    SetMenuItemIconArray(gui, item, &ICONS[2]); // blank
                }
            } else {
                SetMenuItemIconArray(gui, item, &ICONS[1]); // unknown
            }
        } else {
            SetMenuItemIconArray(gui, item, ICONS); // dir
        }
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
        MENU_FLAGS_ENABLE_TEXT_SCROLLING | MENU_FLAGS_ENABLE_ICONS,
        ItemProc,
        NULL,
        NULL,
};

void *CreateTabGUI(int tab_n) {
    TAB_DATA *tab_data = malloc(sizeof(TAB_DATA));
    HEADER_DESC *header_desc = &(tab_data->header_desc);
    zeromem(tab_data, sizeof(TAB_DATA));
    memcpy(header_desc, &HEADER_D, sizeof(HEADER_DESC));

    GUI *tab_gui = GetMenuGUI(malloc_adr(), mfree_adr());
    Sie_GUI_InitHeader(header_desc);

    SetHeaderToMenu(tab_gui, header_desc, malloc_adr());
    SetMenuToGUI(tab_gui, &DESC);
    MenuSetUserPointer(tab_gui, tab_data);

    char str[32];
    sprintf(str, "%d:", (tab_n < 3) ? tab_n : 4);
    tab_data->path = Path_Push(NULL, str, 0);
    SetFiles(tab_gui, tab_data->path->path);

    unsigned int count = Sie_FS_GetFilesCount(tab_data->files);
    SetMenuItemCount(tab_gui, (int)count);
    RefreshHeader(tab_gui);

    return tab_gui;
}
