#include <swilib.h>
#include "sie/sie.h"
#include "../icons.h"
#include "../../procs/procs.h"

#define MAX_ITEMS 2
#define SOFTKEY_SELECT {0x0018, 0x0000, (int)"Select"}
#define SOFTKEY_MIDDLE {0x003D, 0x0000, (int)LGP_DOIT_PIC}

enum MenuItems {
    MENU_ITEM_SORT,
    MENU_ITEM_SHOW_HIDDEN_FILES,
};

typedef struct {
    GUI *tab_gui;
    int show_hidden_files;
} MENU_DATA;

extern SIE_GUI_STACK *GUI_STACK;
extern int SHOW_HIDDEN_FILES;

static HEADER_DESC HEADER_D = {{0, 0, 0, 0}, NULL, (int)"View", LGP_NULL};

static int ICONS[] = {ICON_BLANK, ICON_UNCHECK, ICON_CHECK};

static const MENUITEM_DESC MENU_ITEMS[MAX_ITEMS] = {
        {ICONS, (int)"Sort", LGP_NULL, 0, NULL,
         MENU_FLAG3, MENU_FLAG2},
        {ICONS, (int)"Show hidden files", LGP_NULL, 0, NULL,
         MENU_FLAG3, MENU_FLAG2},
};

void Sort_Proc(GUI *gui) {
}

void ShowHiddenFiles_Proc(GUI *gui) {
    MENU_DATA *menu_data = MenuGetUserPointer(gui);
    menu_data->show_hidden_files = !menu_data->show_hidden_files;
    RefreshGUI();
}

static const MENUPROCS_DESC MENU_PROCS[MAX_ITEMS] = {
        Sort_Proc,
        ShowHiddenFiles_Proc,
};

static const int SOFTKEYS[] = {0, 1, 2};

static const SOFTKEY_DESC SOFTKEY_D[] = {
        SOFTKEY_SELECT,
        {0x0001, 0x0000, (int)"Back"},
        SOFTKEY_MIDDLE,
};

static const SOFTKEYSTAB SOFTKEYS_TAB = {
        SOFTKEY_D, 0,
};

static int OnKey(GUI *gui, GUI_MSG *msg) {
    MENU_DATA *menu_data = MenuGetUserPointer(gui);
    if (msg->keys == 0x1A) { // Save
        ChangeView(menu_data->tab_gui, menu_data->show_hidden_files);
        return 0;
    }
    return 0;
}

static void GHook(GUI *gui, int cmd) {
    MENU_DATA *menu_data = MenuGetUserPointer(gui);
    if (cmd == TI_CMD_REDRAW) {
        int item_n = GetCurMenuItem(gui);
        static SOFTKEY_DESC sk_select = SOFTKEY_SELECT;
        static SOFTKEY_DESC sk_save = {0x001A, 0x0000, (int)"Save"};
        static SOFTKEY_DESC sk_middle_1 = SOFTKEY_MIDDLE;
        static SOFTKEY_DESC sk_middle_2 = {0x003D, 0x0000, (int)LGP_CHANGE_PIC};
        if (item_n < 1) {
            SetMenuSoftKey(gui, &sk_select, SET_LEFT_SOFTKEY);
            SetMenuSoftKey(gui, &sk_middle_1, SET_MIDDLE_SOFTKEY);
        } else {
            SetMenuSoftKey(gui, &sk_save, SET_LEFT_SOFTKEY);
            SetMenuSoftKey(gui, &sk_middle_2, SET_MIDDLE_SOFTKEY);
        }
        SetMenuItemIcon(gui, MENU_ITEM_SHOW_HIDDEN_FILES, (menu_data->show_hidden_files) ? 2 : 1);
    } else if (cmd == TI_CMD_DESTROY) {
        mfree(menu_data);
    }
}

static const MENU_DESC MENU_D = {
        8,
        OnKey,
        GHook,
        NULL,
        SOFTKEYS,
        &SOFTKEYS_TAB,
        MENU_FLAGS_ENABLE_TEXT_SCROLLING | MENU_FLAGS_ENABLE_ICONS,
        NULL,
        MENU_ITEMS,
        MENU_PROCS,
        MAX_ITEMS,
};

int CreateMenu_View(GUI *tab_gui) {
    MENU_DATA *menu_data = malloc(sizeof(MENU_DATA));
    menu_data->tab_gui = tab_gui;
    menu_data->show_hidden_files = SHOW_HIDDEN_FILES;

    Sie_GUI_InitHeaderSmall(&HEADER_D);
    return CreateMenu(1, 0, &MENU_D, &HEADER_D, 0, MAX_ITEMS,
                      menu_data, NULL);
}
