#include <swilib.h>
#include <sie/sie.h>
#include "create/create.h"
#include "../icons.h"

extern SIE_GUI_STACK *GUI_STACK;

static HEADER_DESC HEADER_D = {{0, 0, 0, 0}, NULL, (int)"Options", LGP_NULL};

static int ICONS[] = {ICON_BLANK};

void Create_Proc(GUI *gui) {
    GUI *tab_gui = MenuGetUserPointer(gui);
    GUI_STACK = Sie_GUI_Stack_Add(GUI_STACK, CreateMenu_Create(tab_gui));
}

static const MENUITEM_DESC MENU_ITEMS[] = {
        {ICONS, (int)"Create", LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
};

static const MENUPROCS_DESC MENU_PROCS[] = {
        Create_Proc,
};

static const int SOFTKEYS[] = {0, 1, 2};

static SOFTKEY_DESC SOFTKEY_D[] = {
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
        1,
};

int CreateMenu_Options(const GUI *tab_gui) {
    Sie_GUI_InitHeaderSmall(&HEADER_D);
    return CreateMenu(1, 0, &MENU_D, &HEADER_D, 0, 1,
                      (void*)tab_gui, NULL);
}
