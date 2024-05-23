#include <swilib.h>
#include <sie/sie.h>
#include "../gui/tab.h"

void SetAs(GUI *tab_gui) {
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);
    if (tab_data->current_file) {
        if (Sie_Ext_MimeTypeCompare(tab_data->current_file->file_name, "image") ||
            Sie_Ext_MimeTypeCompare(tab_data->current_file->file_name, "audio")) {
            WSHDR *dir_ws = AllocWS(128);
            WSHDR *file_name_ws = AllocWS(128);
            str_2ws(dir_ws, tab_data->current_file->dir_name, strlen(tab_data->current_file->dir_name) - 1);
            str_2ws(file_name_ws, tab_data->current_file->file_name, 128);
            PersMenuCSM_Open(file_name_ws, dir_ws, 0, 0);
            FreeWS(dir_ws);
            FreeWS(file_name_ws);
        }
    }
}
