#pragma once

enum Ordering {
    ORDERING_BY_NAME_ASC,
    ORDERING_BY_NAME_DESC,
};

int CreateMainGUI(GUI **gui);
void CloseChildrenGUI();

