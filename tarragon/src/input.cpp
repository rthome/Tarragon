#include "input.h"

#include "common.h"

namespace tarragon
{
    void Input::glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        Input* input = reinterpret_cast<Input*>(glfwGetWindowUserPointer(window));
        if (input)
            input->handle_key_callback(static_cast<Key>(key), scancode, static_cast<KeyState>(action), static_cast<KeyModifier>(mods));
    }

	void Input::glfw_mousebutton_callback(GLFWwindow* window, int button, int action, int mods)
    {
        Input* input = reinterpret_cast<Input*>(glfwGetWindowUserPointer(window));
        if (input)
            input->handle_mousebutton_callback(static_cast<MouseButton>(button), static_cast<KeyState>(action), static_cast<KeyModifier>(mods));
    }

	void Input::glfw_cursorpos_callback(GLFWwindow* window, double xpos, double ypos)
    {
        Input* input = reinterpret_cast<Input*>(glfwGetWindowUserPointer(window));
        if (input)
            input->handle_cursorpos_callback(static_cast<float>(xpos), static_cast<float>(ypos));
    }

    void Input::handle_key_callback(Key key, int scancode, KeyState action, KeyModifier mods)
    {
        UNUSED_PARAM(scancode);

        m_key_states[key] = action;

        m_key_sigsource.publish(key, action, mods);
    }

    void Input::handle_mousebutton_callback(MouseButton button, KeyState action, KeyModifier mods)
    {
        m_mousebutton_states[button] = action;

        m_mousebutton_sigsource.publish(button, action, mods);
    }

    void Input::handle_cursorpos_callback(float xpos, float ypos)
    {
        auto mouse_pos_old = m_mouse_pos;
        m_mouse_pos = glm::vec2{ xpos, ypos, };
        m_mouse_delta = m_mouse_pos - mouse_pos_old;

        m_cursor_sigsource.publish(m_mouse_pos, m_mouse_delta);
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