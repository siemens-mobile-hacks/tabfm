#include <swilib.h>
#include <sie/sie.h>
#include "ipc.h"
#include "gui/gui.h"
#include "gui/tab.h"
#include "config_loader.h"

typedef struct {
    CSM_RAM csm;
    GUI *gui;
    MUTEX mtx;
    int gui_id;
} MAIN_CSM;

extern char CFG_PATH[];

static const int minus11 = -11;
static unsigned short maincsm_name_body[140];

int MAIN_GUI_ID;
int OPERATION_FLAG;
SIE_GUI_STACK *GUI_STACK;

static void maincsm_oncreate(CSM_RAM *data) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    csm->csm.state = 0;
    csm->csm.unk1 = 0;
    MAIN_GUI_ID = csm->gui_id = CreateMainGUI(&(csm->gui));
    GUI_STACK = Sie_GUI_Stack_Add(GUI_STACK, MAIN_GUI_ID);
    MutexCreate(&csm->mtx);
}

static void maincsm_onclose(CSM_RAM *data) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    Sie_GUI_Stack_Destroy(GUI_STACK);
    MutexDestroy(&csm->mtx);
    SUBPROC((void *)kill_elf);
}

static int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    if ((msg->msg == MSG_GUI_DESTROYED) && ((int)msg->data0 == csm->gui_id)) {
        csm->csm.state = -3;
    } else if (msg->msg == MSG_IPC) {
        IPC_REQ *ipc = (IPC_REQ*)msg->data0;
        if (strcmpi(ipc->name_to, IPC_NAME) == 0) {
            IPC_DATA *ipc_data = ipc->data;
            MutexLock(&csm->mtx);
            if (msg->submess == IPC_REFRESH) {
                int tab_n = (int)ipc_data->param0;
                int item_n = (int)ipc_data->param1;
                const char *file_name = ipc_data->param2;
                if (tab_n == -1) {
                    tab_n = GetCursorTab(csm->gui);
                }
                GUI *tab_gui = GetGuiByTab(csm->gui, tab_n);
                TAB_DATA *tab_data = MenuGetUserPointer(tab_gui);
                Navigate(tab_gui, tab_data->path->path);
                RefreshTab(tab_gui, item_n, file_name);
            } else if(msg->submess == IPC_SETPBARTEXT) {
                int gui_id = (int)ipc_data->param0;
                if (IsGuiOnTop(gui_id)) {
                    if (ipc_data->param1) {
                        SetPBarValue(gui_id, (int)ipc_data->param1);
                    }
                    if (ipc_data->param2) {
                        SetPBarText(gui_id, ipc_data->param2);
                    }
                }
            }
            mfree(ipc->data);
            mfree(ipc);
            MutexUnlock(&csm->mtx);
        }
    } else if (msg->msg == MSG_RECONFIGURE_REQ) {
        if (strcmpi(CFG_PATH, (char*)msg->data0) == 0) {
            ShowMSG(1, (int)"TabFM config updated!");
            InitConfig();
        }
    }
    return 1;
}

static const struct {
    CSM_DESC maincsm;
    WSHDR maincsm_name;
} MAINCSM = {
        {
                maincsm_onmessage,
                maincsm_oncreate,
#ifdef NEWSGOLD
                0,
                0,
                0,
                0,
#endif
                maincsm_onclose,
                sizeof(MAIN_CSM),
                1,
                &minus11
        },
        {
                maincsm_name_body,
                NAMECSM_MAGIC1,
                NAMECSM_MAGIC2,
                0x0,
                139,
                0
        }
};

void UpdateCSMname(void) {
    wsprintf((WSHDR *)&MAINCSM.maincsm_name, "%t", "TabFM");
}

int main() {
    MAIN_CSM main_csm;
    InitConfig();
    LockSched();
    UpdateCSMname();
    CreateCSM(&MAINCSM.maincsm, &main_csm, 0);
    UnlockSched();
    return 0;
}
