#include "input.h"

#include <cassert>

#include "common.h"

namespace tarragon
{
    void Input::handle_key_callback(int key, int scancode, int action, int mods)
    {
        UNUSED_PARAM(scancode);

        auto kkey = static_cast<Key>(key);
        auto kaction = static_cast<KeyState>(action);
        auto kmods = static_cast<KeyModifier>(mods);

        m_key_states[kkey] = kaction;

        m_key_sigsource.publish(kkey, kaction, kmods);
    }

    void Input::handle_mousebutton_callback(int button, int action, int mods)
    {
        auto mbutton = static_cast<MouseButton>(button);
        auto maction = static_cast<KeyState>(action);
        auto mmods = static_cast<KeyModifier>(mods);

        m_mousebutton_states[mbutton] = maction;

        m_mousebutton_sigsource.publish(mbutton, maction, mmods);
    }

    void Input::handle_cursorpos_callback(double xpos, double ypos)
    {
        glm::vec2 position{static_cast<float>(xpos), static_cast<float>(ypos)};
        auto delta = position - m_mouse_pos;

        m_cursor_sigsource.publish(position, delta);
    }

    void Input::update(Clock const& clock)
    {
        UNUSED_PARAM(clock);

        double xpos, ypos;
        glfwGetCursorPos(m_pwindow, &xpos, &ypos);

        auto mouse_pos_old = m_mouse_pos;
        m_mouse_pos = glm::vec2{ static_cast<float>(xpos), static_cast<float>(ypos), };
        m_mouse_delta = m_mouse_pos - mouse_pos_old;
    }

    KeyState Input::mousebutton_state(MouseButton button) const
    {
        auto keystate = m_mousebutton_states.find(button);
        if (keystate == std::cend(m_mousebutton_states))
            return static_cast<KeyState>(glfwGetMouseButton(m_pwindow, static_cast<int>(button)));
        else
            return keystate->second;
    }

    bool Input::mousebutton_is_up(MouseButton button) const
    {
        return mousebutton_state(button) == KeyState::Up;
    }

    bool Input::mousebutton_is_down(MouseButton button) const
    {
        return mousebutton_state(button) == KeyState::Down;
    }

    void Input::set_cursorstate(CursorState state)
    {
        if (m_cursorstate == state)
            return;

        if (state == CursorState::Normal)
        {
            glfwSetInputMode(m_pwindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else if (state == CursorState::Locked)
        {
            glfwSetInputMode(m_pwindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            if (glfwRawMouseMotionSupported())
                glfwSetInputMode(m_pwindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }

        m_cursorstate = state;

        double x, y;
        glfwGetCursorPos(m_pwindow, &x, &y);
        m_mouse_pos = glm::vec2{ static_cast<float>(x), static_cast<float>(y) };
        m_mouse_delta = {};
    }

    KeyState Input::key_state(Key key) const
    {
        auto keystate = m_key_states.find(key);
        if (keystate == std::cend(m_key_states))
            return static_cast<KeyState>(glfwGetKey(m_pwindow, static_cast<int>(key)));
        else
            return keystate->second;
    }

    bool Input::key_is_up(Key key) const
    {
        return key_state(key) == KeyState::Up;
    }

    bool Input::key_is_down(Key key) const
    {
        auto state = key_state(key);
        return state == KeyState::Down || state == KeyState::Repeat;
    }

    bool Input::key_is_repeating(Key key) const
    {
        return key_state(key) == KeyState::Repeat;
    }
}
