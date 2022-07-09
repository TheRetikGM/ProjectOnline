#pragma once
#include "Core.h"
#include <bitset>
#include <SDL2/SDL.h>

namespace Ren
{
    // Key interface used for checking if key was pressed or if it is being held down.
    // Use this, if you cannot (or don't want to) use SDL_Event.
    class KeyInterface
    {
        std::bitset<322> m_keys;   // 322 SDLK_DOWN keys.
        std::bitset<322> m_keysProcessed;
    public:

        KeyInterface()
            : m_keys()
            , m_keysProcessed() {}

        void Clear()
        {
            m_keys.reset();
            m_keysProcessed.reset();
        }
        void OnEvent(const SDL_Event& e)
        {
            switch(e.type)
            {
            case SDL_KEYDOWN:
                if (e.key.keysym.sym < m_keys.size() && e.key.repeat == 0)
                    m_keys.set(e.key.keysym.sym);
                break;
            case SDL_KEYUP:
                if (e.key.keysym.sym < m_keys.size() && e.key.repeat == 0)
                {
                    m_keys.reset(e.key.keysym.sym);
                    m_keysProcessed.reset(e.key.keysym.sym);
                }
                break;
            default:
                break;
            }
        }

        // Check if key was pressed (will evaluate to true only once when holding the key).
        inline bool KeyPressed(const SDL_Keycode& key)
        {
            if (m_keys.test(key) && !m_keysProcessed.test(key))
            {
                m_keysProcessed.set(key);
                return true;
            }
            return false;
        }
        
        // Check if key is being held down.
        inline bool KeyHeld(const SDL_KeyCode& key)
        {
            return m_keys.test(key);
        }
    };
}