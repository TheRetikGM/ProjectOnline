/**
 * @file Ren/Core/Input.hpp
 * @brief Check for input from mouse and keyboard
 */
#pragma once
#include <cstring>  // std::memset
#include <glm/glm.hpp>
#include "Ren/RenSDL/Keys.h"

namespace Ren {
    // Key interface used for checking if key was pressed or if it is being held down.
    // Use this, if you cannot (or don't want to) use SDL_Event.
    class KeyInterface {
        const static size_t NUM_KEYS = (size_t)Key::NUM_KEYS;
        bool m_keys[NUM_KEYS];
        bool m_keysProcessed[NUM_KEYS];
    public:

        KeyInterface() : m_keys() , m_keysProcessed() {}
        void Clear();
        void OnEvent(const SDL_Event& e);

        // Check if key was pressed (will evaluate to true only once when holding the key).
        bool KeyPressed(Key key);

        // Check if key is being held down.
        inline bool KeyHeld(Key key) { return m_keys[(int)key]; }
    };

    class MouseInterface {
    public:
        void OnEvent(const SDL_Event& e);

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

        bool ButtonPressed(MouseButton b);
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
