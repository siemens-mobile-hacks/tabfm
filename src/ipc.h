#pragma once

#define IPC_NAME "TabFM"

enum {
    IPC_REFRESH,
};

void IPC_Refresh(const char *file_name);

