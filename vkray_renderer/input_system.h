#pragma once

#include "application.h"

class InputSystem
{
public:
    InputSystem(Application* app);

    void cursorPositionCallback(const double xpos, const double ypos);
    void mouseButtonCallback(const int button, const int action, const int mods);
    void scrollCallback(const double xoffset, const double yoffset);

private:

};

