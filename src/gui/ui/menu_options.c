#include <swilib.h>
#include "sie/sie.h"
#include "../tab.h"
#include "../icons.h"
#include "../../procs/procs.h"
#include "menu_view.h"
#include "menu_create.h"
#include "menu_operations.h"

#define MAX_ITEMS 9

enum MenuItems {
    MENU_ITEM_CREATE,
    MENU_ITEM_MARK,
    MENU_ITEM_MARK_ALL,
    MENU_ITEM_UNMARK,
    MENU_ITEM_UNMARK_ALL,
    MENU_ITEM_OPERATIONS,
    MENU_ITEM_VIEW,
    MENU_ITEM_SET_AS,
    MENU_ITEM_SEND,
};

extern SIE_GUI_STACK *GUI_STACK;

static HEADER_DESC HEADER_D = {{0, 0, 0, 0}, NULL, (int)"Options", LGP_NULL};

static int ICONS[] = {ICON_BLANK};

static const MENUITEM_DESC MENU_ITEMS[MAX_ITEMS] = {
        {ICONS, (int)"Create", LGP_NULL, 0, NULL,
         MENU_FLAG3, MENU_FLAG2},
        {ICONS, (int)"Mark", LGP_NULL, 0, NULL,
         MENU_FLAG3, MENU_FLAG2},
        {ICONS, (int)"Mark all", LGP_NULL, 0, NULL,
         MENU_FLAG3, MENU_FLAG2},
        {ICONS, (int)"Unmark", LGP_NULL, 0, NULL,
         MENU_FLAG3, MENU_FLAG2},
        {ICONS, (int)"Unmark all", LGP_NULL, 0, NULL,
         MENU_FLAG3, MENU_FLAG2},
        {ICONS, (int)"Operations", LGP_NULL, 0, NULL,
         MENU_FLAG3, MENU_FLAG2},
        {ICONS, (int)"View", LGP_NULL, 0, NULL,
                MENU_FLAG3, MENU_FLAG2},
        {ICONS, (int)"Set as...", LGP_NULL, 0, NULL,
                MENU_FLAG3, MENU_FLAG2},
        {ICONS, (int)"Send...", LGP_NULL, 0, NULL,
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

void MarkAll_Proc(GUI *gui) {
    GUI *tab_gui = MenuGetUserPointer(gui);
    GeneralFuncF1(1);
    MarkAll(tab_gui);
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

void Operations_Proc(GUI *gui) {
    GUI *tab_gui = MenuGetUserPointer(gui);
    GUI_STACK = Sie_GUI_Stack_Add(GUI_STACK, CreateMenu_Operations(tab_gui));
}

void View_Proc(GUI *gui) {
    GUI *tab_gui = MenuGetUserPointer(gui);
    GUI_STACK = Sie_GUI_Stack_Add(GUI_STACK, CreateMenu_View(tab_gui));
}

void SetAs_Proc(GUI *gui) {
    GUI *tab_gui = MenuGetUserPointer(gui);
    GeneralFuncF1(1);
    SetAs(tab_gui);
}

void Send_Proc(GUI *gui) {
    GUI *tab_gui = MenuGetUserPointer(gui);
    GeneralFuncF1(1);
    Send(tab_gui);
}

static const MENUPROCS_DESC MENU_PROCS[MAX_ITEMS] = {
        Create_Proc,
        Mark_Proc,
        MarkAll_Proc,
        UnMark_Proc,
        UnMarkAll_Proc,
        Operations_Proc,
        View_Proc,
        SetAs_Proc,
        Send_Proc,
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

int CreateMenu_Options(GUI *tab_gui) {
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);

    int items_count = 0;
    int to_remove[MAX_ITEMS + 1];
    if (tab_data->files) {
        if (tab_data->selected_files) {
            to_remove[++items_count] = MENU_ITEM_CREATE;
            if (IsCurrentFileMarked(tab_gui)) {
                to_remove[++items_count] = MENU_ITEM_MARK;
            } else {
                to_remove[++items_count] = MENU_ITEM_UNMARK;
                to_remove[++items_count] = MENU_ITEM_UNMARK_ALL;
            }
            if (Sie_FS_GetFilesCount(tab_data->files) == Sie_FS_GetFilesCount(tab_data->selected_files)) {
                to_remove[++items_count] = MENU_ITEM_MARK_ALL;
            }
        } else {
            to_remove[++items_count] = MENU_ITEM_UNMARK;
            to_remove[++items_count] = MENU_ITEM_UNMARK_ALL;
            if (!Sie_Ext_MimeTypeCompare(tab_data->current_file->file_name, "image")) {
                if (!Sie_Ext_MimeTypeCompare(tab_data->current_file->file_name, "audio")) {
                    to_remove[++items_count] = MENU_ITEM_SET_AS;
                }
            }
        }
    } else {
        to_remove[++items_count] = MENU_ITEM_MARK;
        to_remove[++items_count] = MENU_ITEM_MARK_ALL;
        to_remove[++items_count] = MENU_ITEM_UNMARK;
        to_remove[++items_count] = MENU_ITEM_UNMARK_ALL;
        to_remove[++items_count] = MENU_ITEM_OPERATIONS;
        to_remove[++items_count] = MENU_ITEM_VIEW;
        to_remove[++items_count] = MENU_ITEM_SET_AS;
        to_remove[++items_count] = MENU_ITEM_SEND;
    }
    to_remove[0] = items_count;

    Sie_GUI_InitHeaderSmall(&HEADER_D);
    return CreateMenu(1, 0, &MENU_D, &HEADER_D, 0, items_count,
                      (void*)tab_gui, to_remove);
}
