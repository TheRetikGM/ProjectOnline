/**
 * @brief Implementation of input
 * #file Core/Input.cpp
 */
#include "Ren/Core/Input.h"
extern "C" {
    #include <SDL.h>
}

using namespace Ren;


void KeyInterface::Clear() {
    std::memset(m_keys, 0, sizeof(m_keys));
    std::memset(m_keysProcessed, 0, sizeof(m_keysProcessed));
}


void KeyInterface::OnEvent(const SDL_Event& e) {
    switch(e.type) {
    case SDL_KEYDOWN:
        if (e.key.keysym.scancode < NUM_KEYS && e.key.repeat == 0)
            m_keys[e.key.keysym.scancode] = true;
        break;
    case SDL_KEYUP:
        if (e.key.keysym.scancode < NUM_KEYS && e.key.repeat == 0)
        {
            m_keys[e.key.keysym.scancode] = false;
            m_keysProcessed[e.key.keysym.scancode] = false;
        }
        break;
    default:
        break;
    }
}

bool KeyInterface::KeyPressed(Key key) {
    if (m_keys[(int)key] && !m_keysProcessed[(int)key]) {
        m_keysProcessed[(int)key] = true;
        return true;
    }
    return false;
}


void MouseInterface::OnEvent(const SDL_Event& e) {
    m_lastAPos = m_aPos;
    m_wheelOffset = 0;

    switch (e.type) {
    case SDL_MOUSEMOTION:
        m_aPos.x = e.motion.x;
        m_aPos.y = e.motion.y;
        break;
    case SDL_MOUSEBUTTONDOWN:
        m_buttons[e.button.button] = true;
        break;
    case SDL_MOUSEBUTTONUP:
        m_buttons[e.button.button] = false;
        m_buttonsProcessed[e.button.button] = false;
        break;
    case SDL_MOUSEWHEEL:
        m_wheelOffset = e.wheel.y;
        break;
    default:
        break;
    }
}

bool MouseInterface::ButtonPressed(MouseButton b) {
    if (m_buttons[(int)b] && !m_buttonsProcessed[(int)b]) {
        m_buttonsProcessed[(int)b] = true;
        return true;
    }
    return false;
}

