#pragma once

#include "../path.h"

typedef struct {
    HEADER_DESC header_desc;
    PATH *path;
    SIE_FILE *files;
} TAB_DATA;

void *CreateTabGUI(int tab_n);
void Navigate(GUI *tab_gui, const char *path);
void RefreshTab(GUI *tab_gui, int item_n);
void RefreshTabByFileName(GUI *tab_gui, const char *file_name);
