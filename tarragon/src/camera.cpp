#include "camera.h"

#include <glm/gtx/common.hpp>
#include <glm/ext/quaternion_transform.hpp>

namespace tarragon
{
    void Camera::update_projection()
    {
        m_projection = glm::perspectiveRH(glm::radians(m_fov), static_cast<float>(m_width) / static_cast<float>(m_height), m_near_plane, m_far_plane);
    }

    void Camera::update_view()
    {
        glm::vec3 up = m_rotation * glm::vec3{0.0f, 0.0f, 1.0f};
        glm::vec3 forward = m_rotation * glm::vec3(0.0f, -1.0f, 0.0f);
        m_view = glm::lookAtRH(m_position, m_position + forward, up);
    }

    void Camera::set_resolution(int width, int height)
    {
        m_width = width;
        m_height = height;
        update_projection();
    }

    void Camera::set_planes(float near_plane, float far_plane)
    {
        m_near_plane = near_plane;
        m_far_plane = far_plane;
        update_projection();
    }

    void Camera::set_fov(float fov)
    {
        m_fov = fov;
        update_projection();
    }

    void Camera::set_position(glm::vec3 const& position)
    {
        m_position = position;
        update_view();
    }

    void Camera::set_rotation(glm::quat const& rotation)
    {
        m_rotation = rotation;
        update_view();
    }


    void FreelookCamera::update(Clock const& clock)
    {
        constexpr auto turn_speed = 0.08f;
        constexpr auto move_speed = 8.0f;

        if (!m_mouse_locked && m_pinput->mousebutton_is_down(MouseButton::Left))
        {
            m_pinput->set_cursorstate(CursorState::Locked);
            m_mouse_locked = true;
        }

        if (m_mouse_locked && m_pinput->key_is_down(Key::Escape))
        {
            m_pinput->set_cursorstate(CursorState::Normal);
            m_mouse_locked = false;
        }

        if (m_mouse_locked)
        {
            m_look += m_pinput->mouse_delta() * turn_speed;
            m_look.x = fmod(m_look.x, 360.0f);
            m_look.y = glm::clamp(m_look.y, -85.0f, 85.0f);

            m_rotation = glm::rotate(glm::identity<glm::quat>(), glm::radians(-m_look.x), glm::vec3{0.0f, 0.0f, 1.0f}) * glm::rotate(glm::identity<glm::quat>(), glm::radians(m_look.y), glm::vec3{1.0f, 0.0f, 0.0f});

            glm::vec3 movement{};
            if (m_pinput->key_is_down(Key::W))
                movement += glm::vec3{0.0f, -1.0f, 0.0f};
            if (m_pinput->key_is_down(Key::S))
                movement += glm::vec3{0.0f, 1.0f, 0.0f};
            if (m_pinput->key_is_down(Key::A))
                movement += glm::vec3{1.0f, 0.0f, 0.0f};
            if (m_pinput->key_is_down(Key::D))
                movement += glm::vec3{-1.0f, 0.0f, 0.0f};

            m_position += m_rotation * (move_speed * clock.last_delta() * movement);

            m_pcamera->set_position(m_position);
            m_pcamera->set_rotation(m_rotation);
        }
    }
}
