#pragma once

#include <memory>

#include "glad/gl.h"
#include "component.h"
#include "input.h"
#include "camera.h"
#include "chunkcache.h"
#include "chunkrenderer.h"
#include "chunkupdater.h"
#include "chunktransfer.h"

struct GLFWwindow;

namespace tarragon
{
    class Engine
    {
    private:
        static void glfw_error_callback(int error, const char *description);
        static void glfw_framebuffersize_callback(GLFWwindow *window, int width, int height);
        static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void glfw_mousebutton_callback(GLFWwindow* window, int button, int action, int mods);
        static void glfw_cursorpos_callback(GLFWwindow* window, double xpos, double ypos);

        bool m_is_initialized = false;

        GLFWwindow *m_pwindow = nullptr;
        std::unique_ptr<Clock> m_pclock;
        std::unique_ptr<Input> m_pinput;
        std::unique_ptr<Camera> m_pcamera;
        std::unique_ptr<FreelookCamera> m_pfreecam;
        std::unique_ptr<ChunkCache> m_pchunk_cache;
        std::unique_ptr<ChunkRenderer> m_pchunk_renderer;
        std::unique_ptr<ChunkUpdater> m_pchunk_updater;
        std::unique_ptr<ChunkTransfer> m_pchunk_transfer;

        bool initialize_components();

    public:
        Engine() = default;
        Engine(Engine const&) = delete;
        Engine& operator= (Engine const&) = delete;

        bool initialize();

        void update();
        void draw();

        GLFWwindow* window() { return m_pwindow; }
        Clock* clock() { return m_pclock.get(); }
        Input* input() { return m_pinput.get(); }
        Camera* camera() { return m_pcamera.get(); }
    };
}
