#include <swilib.h>
#include <sie/sie.h>
#include "../tab.h"
#include "../icons.h"
#include "../../procs/procs.h"
#include "edit_rename.h"

#define MAX_ITEMS 5

enum MenuItems {
    MENU_ITEM_PASTE,
    MENU_ITEM_RENAME,
    MENU_ITEM_COPY,
    MENU_ITEM_MOVE,
    MENU_ITEM_DELETE,
};

extern SIE_FILE *COPY_FILES, *MOVE_FILES;

static HEADER_DESC HEADER_D = {{0, 0, 0, 0}, NULL, (int)"Operations", LGP_NULL};

static int ICONS[] = {ICON_BLANK};

static const MENUITEM_DESC MENU_ITEMS[MAX_ITEMS] = {
        {ICONS, (int)"Paste", LGP_NULL, 0, NULL,
                MENU_FLAG3, MENU_FLAG2},
        {ICONS, (int)"Rename", LGP_NULL, 0, NULL,
         MENU_FLAG3, MENU_FLAG2},
        {ICONS, (int)"Copy", LGP_NULL, 0, NULL,
                MENU_FLAG3, MENU_FLAG2},
        {ICONS, (int)"Move", LGP_NULL, 0, NULL,
                MENU_FLAG3, MENU_FLAG2},
        {ICONS, (int)"Delete", LGP_NULL, 0, NULL,
                MENU_FLAG3, MENU_FLAG2},
};

void Paste_Proc(GUI *gui) {

}

void Rename_Proc(GUI *gui) {
    GUI *tab_gui = MenuGetUserPointer(gui);
    CreateInputTextDialog_Rename(tab_gui);
}

void Copy_Proc(GUI *gui) {

}

void Move_Proc(GUI *gui) {

}

void Delete_Proc(GUI *gui) {
    GUI *tab_gui = MenuGetUserPointer(gui);
    GeneralFuncF1(1);
    Delete(tab_gui);
}

static const MENUPROCS_DESC MENU_PROCS[MAX_ITEMS] = {
        Paste_Proc,
        Rename_Proc,
        Copy_Proc,
        Move_Proc,
        Delete_Proc,
};

static const int SOFTKEYS[] = {0, 1, 2};

static const SOFTKEY_DESC SOFTKEY_D[] = {
        {0x0018, 0x0000, (int)"Select"},
        {0x0001, 0x0000, (int)"Back"},
        {0x003D, 0x0000, (int)LGP_DOIT_PIC},
};

static const SOFTKEYSTAB SOFTKEYS_TAB = {
        SOFTKEY_D, 0,
};

static const MENU_DESC MENU_D = {
        8,
        NULL,
        NULL,
        NULL,
        SOFTKEYS,
        &SOFTKEYS_TAB,
        MENU_FLAGS_ENABLE_TEXT_SCROLLING | MENU_FLAGS_ENABLE_ICONS,
        NULL,
        MENU_ITEMS,
        MENU_PROCS,
        MAX_ITEMS,
};

int CreateMenu_Operations(GUI *tab_gui) {
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);

    int items_count = 0;
    int to_remove[MAX_ITEMS + 1];
    if (tab_data->selected_files) {
        to_remove[++items_count] = MENU_ITEM_PASTE;
        to_remove[++items_count] = MENU_ITEM_RENAME;
    } else {
        if (!COPY_FILES && !MOVE_FILES) {
            to_remove[++items_count] = MENU_ITEM_PASTE;
        }
        if (!tab_data->current_file) {
            to_remove[++items_count] = MENU_ITEM_RENAME;
            to_remove[++items_count] = MENU_ITEM_COPY;
            to_remove[++items_count] = MENU_ITEM_MOVE;
            to_remove[++items_count] = MENU_ITEM_DELETE;
        }
    }
    to_remove[0] = items_count;
    Sie_GUI_InitHeaderSmall(&HEADER_D);
    return CreateMenu(1, 0, &MENU_D, &HEADER_D, 0, items_count,
                      (void*)tab_gui, to_remove);
}
