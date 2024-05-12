#pragma once

#include "../path.h"

typedef struct {
    HEADER_DESC header_desc;
    PATH *path;
    SIE_FILE *files;
    SIE_FILE *current_file;
    SIE_FILE *selected_files;
} TAB_DATA;

void *CreateTabGUI(int tab_n);
void Navigate(GUI *tab_gui, const char *path);
void RefreshTab(GUI *tab_gui, int item_n, const char *file_name);
