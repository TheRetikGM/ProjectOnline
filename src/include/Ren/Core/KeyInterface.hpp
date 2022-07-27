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
}