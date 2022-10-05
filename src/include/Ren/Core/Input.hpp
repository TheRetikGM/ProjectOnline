#pragma once
#include <SDL2/SDL.h>
#include <cstring>  // std::memset

#include "Ren/Core/Core.h"
#include "Ren/RenSDL/Keys.h"

namespace Ren
{
    // Key interface used for checking if key was pressed or if it is being held down.
    // Use this, if you cannot (or don't want to) use SDL_Event.
    class KeyInterface
    {
        const static size_t NUM_KEYS = (size_t)Key::NUM_KEYS;
        bool m_keys[NUM_KEYS];
        bool m_keysProcessed[NUM_KEYS];
    public:

        KeyInterface()
            : m_keys()
            , m_keysProcessed() {}

        void Clear()
        {
            std::memset(m_keys, 0, sizeof(m_keys));
            std::memset(m_keysProcessed, 0, sizeof(m_keysProcessed));
        }
        void OnEvent(const SDL_Event& e)
        {
            switch(e.type)
            {
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

        // Check if key was pressed (will evaluate to true only once when holding the key).
        inline bool KeyPressed(Key key)
        {
            if (m_keys[(int)key] && !m_keysProcessed[(int)key])
            {
                m_keysProcessed[(int)key] = true;
                return true;
            }
            return false;
        }
        
        // Check if key is being held down.
        inline bool KeyHeld(Key key)
        {
            return m_keys[(int)key];
        }
    };

    class MouseInterface
    {
    public:

        void OnEvent(const SDL_Event& e)
        {
            m_lastAPos = m_aPos;
            m_wheelOffset = 0;

            switch (e.type)
            {
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

        // Return mouse pos (with offset).
        inline glm::ivec2 GetMousePos() { return m_aPos - m_offset; }
        // Get mouse position from top-left corner of the window.
        inline glm::ivec2 GetAbsoluteMousePos() { return m_aPos; }
        // Get delta of the two last position.
        inline glm::ivec2 GetMouseDelta() { return m_aPos - m_lastAPos; }
        // Get wheel Y offset, which is either of -1, 0, 1
        inline int GetWheelY() { return m_wheelOffset; }

        // Set offset of the mouse position. Using this, you can achieve relative mouse position.
        inline void SetOffset(glm::ivec2 offset) { m_offset = offset; }

        bool ButtonPressed(MouseButton b)
        {
            if (m_buttons[(int)b] && !m_buttonsProcessed[(int)b])
            {
                m_buttonsProcessed[(int)b] = true;
                return true;
            }
            return false;
        }
        inline bool ButtonHeld(MouseButton b) { return m_buttons[(int)b]; }
    private:
        // Abosule position of the mouse (from left top corner of window).
        glm::ivec2 m_aPos{};
        // Gives the choice of offsetting the current mouse position.
        // When user calls for position, it will be automaticcaly offsetted.
        glm::ivec2 m_offset{};
        glm::ivec2 m_lastAPos{};
        // Will be reset every frame. Can have values -1, 0, 1
        int m_wheelOffset{ 0 };

        bool m_buttons[(int)MouseButton::COUNT]{};
        bool m_buttonsProcessed[(int)MouseButton::COUNT]{};
    };
}