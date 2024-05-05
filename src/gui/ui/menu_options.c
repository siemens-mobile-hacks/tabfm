#include <swilib.h>
#include "sie/sie.h"
#include "../tab.h"
#include "../icons.h"
#include "../../procs/procs.h"
#include "menu_create.h"

#define MAX_ITEMS 4

extern SIE_GUI_STACK *GUI_STACK;

static HEADER_DESC HEADER_D = {{0, 0, 0, 0}, NULL, (int)"Options", LGP_NULL};

static int ICONS[] = {ICON_BLANK};

static const MENUITEM_DESC MENU_ITEMS[MAX_ITEMS] = {
        {ICONS, (int)"Create", LGP_NULL, 0, NULL,
         MENU_FLAG3, MENU_FLAG2},
        {ICONS, (int)"Mark", LGP_NULL, 0, NULL,
         MENU_FLAG3, MENU_FLAG2},
        {ICONS, (int)"Unmark", LGP_NULL, 0, NULL,
         MENU_FLAG3, MENU_FLAG2},
        {ICONS, (int)"Unmark all", LGP_NULL, 0, NULL,
         MENU_FLAG3, MENU_FLAG2},
};

void Create_Proc(GUI *gui) {
    GUI *tab_gui = MenuGetUserPointer(gui);
    GUI_STACK = Sie_GUI_Stack_Add(GUI_STACK, CreateMenu_Create(tab_gui));
}

void Mark_Proc(GUI *gui) {
    GUI *tab_gui = MenuGetUserPointer(gui);
    GeneralFuncF1(1);
    Mark(tab_gui);
}

void UnMark_Proc(GUI *gui) {
    GUI *tab_gui = MenuGetUserPointer(gui);
    GeneralFuncF1(1);
    UnMark(tab_gui);
}

void UnMarkAll_Proc(GUI *gui) {
    GUI *tab_gui = MenuGetUserPointer(gui);
    GeneralFuncF1(1);
    UnMarkAll(tab_gui);
}

static const MENUPROCS_DESC MENU_PROCS[MAX_ITEMS] = {
        Create_Proc,
        Mark_Proc,
        UnMark_Proc,
        UnMarkAll_Proc,
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

/**********************************************************************************************************************/

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

int CreateMenu_Options(GUI *tab_gui) {
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);

    int to_remove[MAX_ITEMS + 1];
    int items_count = 0;

    if (tab_data->files) {
        if (tab_data->selected_files) {
            to_remove[++items_count] = 0;
            if (IsCurrentFileMarked(tab_gui)) { // cut "Mark" item
                to_remove[++items_count] = 1;
            } else { // cut "Unmark", "Unmark all" items
                to_remove[++items_count] = 2;
                to_remove[++items_count] = 3;
            }
        } else { // cut "Unmark", "Unmark all" items
            to_remove[++items_count] = 2;
            to_remove[++items_count] = 3;
        }
    } else {
        to_remove[++items_count] = 1;
        to_remove[++items_count] = 2;
        to_remove[++items_count] = 3;
    }
    to_remove[0] = items_count;

    Sie_GUI_InitHeaderSmall(&HEADER_D);
    return CreateMenu(1, 0, &MENU_D, &HEADER_D, 0, items_count,
                      (void*)tab_gui, to_remove);
}