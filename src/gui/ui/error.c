#include <swilib.h>
#include <stdio.h>

int ShowError(unsigned int error, const char *file_name) {
    char msg[192];
    sprintf(msg, "Error: %d\n%s", error, file_name);
    return MsgBoxError(1, (int)msg);
}
