#pragma once

#include <glm/glm.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "component.h"
#include "input.h"

namespace tarragon
{
    class Camera : public System
    {
    private:
        int m_width = 1, m_height = 1;
        float m_near_plane = 1.0f, m_far_plane = 2.0f;
        float m_fov = 1.0f;
        glm::mat4 m_projection{};

        glm::vec3 m_position{};
        glm::quat m_rotation{glm::identity<glm::quat>()};
        glm::mat4 m_view{};

        void update_projection();
        void update_view();

    public:
        Camera()
        {
            update_projection();
            update_view();
        }
        Camera(Camera const&) = delete;
        Camera& operator= (Camera const&) = delete;

        glm::vec3 const& position() { return m_position; }

        glm::mat4 const& view() const { return m_view; }
        glm::mat4 const& projection() const { return m_projection; }

        void set_resolution(int width, int height);
        void set_planes(float near_plane, float far_plane);
        void set_fov(float fov);

        void set_position(glm::vec3 const& position);
        void set_rotation(glm::quat const& rotation);
    };

    class FreelookCamera : public UpdateComponent
    {
    private:
        Camera *m_pcamera;
        Input *m_pinput;

        glm::vec3 m_position{};
        glm::vec2 m_look{};
        glm::quat m_rotation{glm::identity<glm::quat>()};

        bool m_mouse_locked = false;

    public:
        FreelookCamera(Camera *pcamera, Input *pinput)
            : m_pcamera{pcamera}
            , m_pinput{pinput}
        { }
        virtual ~FreelookCamera() = default;

        FreelookCamera(FreelookCamera const&) = delete;
        FreelookCamera& operator= (FreelookCamera const&) = delete;
        
        virtual void initialize() override
        {
            
        }

        void set_position(glm::vec3 const& position) { m_position = position; }

        virtual void update(Clock const& clock) override;
    };
}
