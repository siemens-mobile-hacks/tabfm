#include <swilib.h>
#include <sie/sie.h>
#include "../gui/tab.h"

unsigned int IsCurrentFileMarked(GUI *tab_gui) {
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);
    return Sie_FS_ContainsFile(tab_data->selected_files, tab_data->current_file);
}

void Mark(GUI *tab_gui) {
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);
    if (tab_data->current_file) {
        SIE_FILE *file = Sie_FS_CopyFileElement(tab_data->current_file);
        if (!tab_data->selected_files) {
            tab_data->selected_files = file;
        } else {
            SIE_FILE *last = Sie_FS_GetLastFile(tab_data->selected_files);
            last->next = file;
            file->prev = last;
        }
    }
}

void UnMark(GUI *tab_gui) {
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);
    if (tab_data->current_file) {
        SIE_FILE *file = Sie_FS_DeleteFileElement(tab_data->selected_files, tab_data->current_file);
        if (file) {
            SIE_FILE *next = file->next;
            Sie_FS_DestroyFileElement(file);

            if (file == tab_data->selected_files) {
                if (next) {
                    next->prev = NULL;
                    tab_data->selected_files = next;
                } else {
                    tab_data->selected_files = NULL;
                }
            }
        }
    }
}

void ToggleMark(GUI *tab_gui) {
    if (IsCurrentFileMarked(tab_gui)) {
        UnMark(tab_gui);
    } else {
        Mark(tab_gui);
    }
}

void UnMarkAll(GUI *tab_gui) {
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);
    Sie_FS_DestroyFiles(tab_data->selected_files);
    tab_data->selected_files = NULL;
}
