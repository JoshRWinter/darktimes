#pragma once

#include <win/Event.hpp>

struct KeyEvent
{
    KeyEvent()
        : button(win::Button::undefined)
        , press(false)
    {
    }

    KeyEvent(win::Button button, bool press)
        : button(button)
        , press(press)
    {
    }

    win::Button button;
    bool press;
};
