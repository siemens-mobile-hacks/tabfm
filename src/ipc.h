#pragma once

#define IPC_NAME "TabFM"

typedef struct {
    void *param0;
    void *param1;
    void *param2;
    int main_gui_id;
} IPC_DATA;

enum {
    IPC_REFRESH,
    IPC_SETPBARTEXT,
};

void IPC_Refresh(int tab_n, int item_n, const char *file_name);
void IPC_SetPBarData(int gui_id, int value, const WSHDR *ws);

