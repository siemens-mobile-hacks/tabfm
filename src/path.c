#include <swilib.h>
#include <stdlib.h>
#include <string.h>
#include "path.h"

PATH *Path_Push(PATH *path, const char *p, int prev_item_n) {
    PATH *new = malloc(sizeof(PATH));
    zeromem(new, sizeof(PATH));
    new->prev = path;
    new->path = malloc(strlen(p) + 1);
    strcpy(new->path, p);
    if (path) {
        path->next = new;
        path->item_n = prev_item_n;
    }
    return new;
}

PATH *Path_Pop(PATH *path) {
    PATH *prev = path->prev;
    mfree(path->path);
    mfree(path);
    if (prev) {
        prev->next = NULL;
        return prev;
    } else {
        return NULL;
    }
}

void Path_Destroy(PATH *path) {
    while (path) {
        mfree(path->path);
        path = path->prev;
    }
}
