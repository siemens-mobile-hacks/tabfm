#include <swilib.h>
#include <sie/sie.h>
#include "../gui/tab.h"

void Send(GUI *tab_gui) {
    TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);
    if (tab_data->current_file) {
        WSHDR *dir_ws = AllocWS(128);
        WSHDR *file_name_ws = AllocWS(128);
        str_2ws(dir_ws, tab_data->current_file->dir_name, strlen(tab_data->current_file->dir_name) - 1);
        str_2ws(file_name_ws, tab_data->current_file->file_name, 128);
        MediaSendCSM_Open(file_name_ws, dir_ws);
        FreeWS(dir_ws);
        FreeWS(file_name_ws);
    }
}
