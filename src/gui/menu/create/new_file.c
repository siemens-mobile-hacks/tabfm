#include <swilib.h>
#include <sie/sie.h>
#include "../../icons.h"

static HEADER_DESC HEADER_D = {{0, 0, 0, 0}, NULL, (int)"New file", LGP_NULL};

static int ICONS[] = {ICON_BLANK};

static const int SOFTKEYS[] = {0, 1, 2};

static SOFTKEY_DESC SOFTKEY_D[] = {
        {0x0018, 0x0000, (int)"Select"},
        {0x0001, 0x0000, (int)"Back"},
        {0x003D, 0x0000, (int)LGP_DOIT_PIC},
};

static const SOFTKEYSTAB SOFTKEYS_TAB = {
        SOFTKEY_D, 0,
};

static void GHook(GUI *gui, int cmd) {
    SIE_FILE *files = MenuGetUserPointer(gui);
    if (cmd == TI_CMD_DESTROY) {
        Sie_FS_DestroyFiles(files);
    }
}

void ItemProc(void *gui, int item_n, void *data) {
    SIE_FILE *files = (SIE_FILE*)data;

    void *item = AllocMenuItem(gui);
    WSHDR *ws = AllocMenuWS(gui, 256);
    SIE_FILE *file = Sie_FS_GetFileByID(files, item_n);
    str_2ws(ws, file->file_name, 255);
    SetMenuItemIconArray(gui, item, ICONS);
    SetMenuItemText(gui, item, ws, item_n);
}

static const MENU_DESC MENU_D = {
        8,
        NULL,
        GHook,
        NULL,
        SOFTKEYS,
        &SOFTKEYS_TAB,
        MENU_FLAGS_ENABLE_TEXT_SCROLLING | MENU_FLAGS_ENABLE_ICONS,
        ItemProc,
        NULL,
        NULL
};

int CreateMenu_CreateNewFile() {
    Sie_GUI_InitHeaderSmall(&HEADER_D);
    SIE_FILE *files = Sie_FS_FindFiles("0:\\Templates\\*");
    int count = (int)Sie_FS_GetFilesCount(files);
    return CreateMenu(1, 0, &MENU_D, &HEADER_D,
                      0, count,files, NULL);
}
