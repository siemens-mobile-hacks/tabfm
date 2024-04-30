#pragma once

typedef struct {
    void *prev;
    void *next;
    char *path;
    int item_n;
} PATH;

PATH *Path_Push(PATH *path, const char *p, int item_n);
PATH *Path_Pop(PATH *path);
void Path_Destroy(PATH *path);
