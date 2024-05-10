#pragma once

#define IPC_NAME "TabFM"

typedef struct {
    void *param0;
    void *param1;
    void *param2;
} IPC_DATA;

enum {
    IPC_REFRESH,
    IPC_SETPBARTEXT,
};

void IPC_Refresh(int tab_n, const char *file_name);
void IPC_SetPBarData(int gui_id, int value, const WSHDR *ws);

