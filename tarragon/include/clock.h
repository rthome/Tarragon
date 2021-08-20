#pragma once

#include <GLFW/glfw3.h>

namespace tarragon
{
    class Clock final
    {
    private:
        double m_start;
        uint64_t m_frame_count;

        double m_last_update;
        double m_last_delta;

    public:
        Clock()
            : m_start{ glfwGetTime() }
        { }

        void update()
        {
            auto now = glfwGetTime();
            m_frame_count++;
            m_last_delta = now - m_last_update;
            m_last_update = now;
        }

        float last_delta() const { return static_cast<float>(m_last_delta); }
        float seconds_since_start() const { return static_cast<float>(glfwGetTime() - m_start); }
        uint64_t frame_count() const { return m_frame_count; }
    };
}
