#include <swilib.h>
#include <sie/sie.h>
#include "../tab.h"
#include "../icons.h"
#include "../../procs/procs.h"

typedef struct {
    GUI *tab_gui;
    SIE_FILE *files;
} MENU_DATA;

static HEADER_DESC HEADER_D = {{0, 0, 0, 0}, NULL, (int)"New file", LGP_NULL};

static int ICONS[] = {ICON_BLANK};

static const int SOFTKEYS[] = {0, 1, 2};

static const SOFTKEY_DESC SOFTKEY_D[] = {
        {0x0018, 0x0000, (int)"Select"},
        {0x0001, 0x0000, (int)"Back"},
        {0x003D, 0x0000, (int)LGP_DOIT_PIC},
};

static const SOFTKEYSTAB SOFTKEYS_TAB = {
        SOFTKEY_D, 0,
};

static int OnKey(GUI *gui, GUI_MSG *msg) {
    MENU_DATA *menu_data = MenuGetUserPointer(gui);
    int item_n = GetCurMenuItem(gui);

    if (msg->keys == 0x18 || msg->keys == 0x3D) {
        SIE_FILE *file = Sie_FS_GetFileByID(menu_data->files, item_n);
        CreateNewFile(menu_data->tab_gui, file);
    }
    return 0;
}


static void GHook(GUI *gui, int cmd) {
    MENU_DATA *menu_data = MenuGetUserPointer(gui);
    if (cmd == TI_CMD_DESTROY) {
        Sie_FS_DestroyFiles(menu_data->files);
        mfree(menu_data);
    }
}

void ItemProc(void *gui, int item_n, void *data) {
    MENU_DATA *menu_data = (MENU_DATA*)data;

    void *item = AllocMenuItem(gui);
    WSHDR *ws = AllocMenuWS(gui, 256);
    SIE_FILE *file = Sie_FS_GetFileByID(menu_data->files, item_n);
    str_2ws(ws, file->file_name, 255);
    SetMenuItemIconArray(gui, item, ICONS);
    SetMenuItemText(gui, item, ws, item_n);
}

static const MENU_DESC MENU_D = {
        8,
        OnKey,
        GHook,
        NULL,
        SOFTKEYS,
        &SOFTKEYS_TAB,
        MENU_FLAGS_ENABLE_TEXT_SCROLLING | MENU_FLAGS_ENABLE_ICONS,
        ItemProc,
        NULL,
        NULL
};

int CreateMenu_CreateNewFile(GUI *tab_gui) {
    Sie_GUI_InitHeaderSmall(&HEADER_D);

    MENU_DATA *menu_data = malloc(sizeof(MENU_DATA));
    menu_data->tab_gui = (GUI*)tab_gui;
    menu_data->files = Sie_FS_FindFiles("0:\\Templates\\*");
    int count = (int)Sie_FS_GetFilesCount(menu_data->files);
    return CreateMenu(1, 0, &MENU_D, &HEADER_D,
                      0, count,menu_data, NULL);
}
