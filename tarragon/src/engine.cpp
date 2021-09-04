#include "engine.h"

#include <cassert>
#include <iostream>

#include <GLFW/glfw3.h>

#include "camera.h"
#include "clock.h"
#include "input.h"
#include "chunkrenderer.h"
#include "chunkupdater.h"

namespace tarragon
{
    namespace
    {
        Engine* get_engine(GLFWwindow *pwindow)
        {
            assert(pwindow != nullptr);

            auto pengine = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(pwindow));
            assert(pengine != nullptr);

            return pengine;
        }

        void GLAPIENTRY gl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
        {
            UNUSED_PARAM(length);
            UNUSED_PARAM(userParam);

            auto const src_str = [source]()
            {
                switch (source)
                {
                case GL_DEBUG_SOURCE_API:
                    return "API";
                case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                    return "WINDOW SYSTEM";
                case GL_DEBUG_SOURCE_SHADER_COMPILER:
                    return "SHADER COMPILER";
                case GL_DEBUG_SOURCE_THIRD_PARTY:
                    return "THIRD PARTY";
                case GL_DEBUG_SOURCE_APPLICATION:
                    return "APPLICATION";
                case GL_DEBUG_SOURCE_OTHER:
                    return "OTHER";
                default:
                    return "<invalid>";
                }
            }();

            auto const type_str = [type]()
            {
                switch (type)
                {
                case GL_DEBUG_TYPE_ERROR:
                    return "ERROR";
                case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                    return "DEPRECATED_BEHAVIOR";
                case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                    return "UNDEFINED_BEHAVIOR";
                case GL_DEBUG_TYPE_PORTABILITY:
                    return "PORTABILITY";
                case GL_DEBUG_TYPE_PERFORMANCE:
                    return "PERFORMANCE";
                case GL_DEBUG_TYPE_MARKER:
                    return "MARKER";
                case GL_DEBUG_TYPE_OTHER:
                    return "OTHER";
                default:
                    return "<invalid>";
                }
            }();

            auto const severity_str = [severity]()
            {
                switch (severity)
                {
                case GL_DEBUG_SEVERITY_NOTIFICATION:
                    return "NOTIFICATION";
                case GL_DEBUG_SEVERITY_LOW:
                    return "LOW";
                case GL_DEBUG_SEVERITY_MEDIUM:
                    return "MEDIUM";
                case GL_DEBUG_SEVERITY_HIGH:
                    return "HIGH";
                default:
                    return "<invalid>";
                }
            }();

            std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
        }
    }

    void Engine::glfw_error_callback(int error, const char *description)
    {
        std::cerr << "GLFW Error  " << error << ": " << description << std::endl;
    }

    void Engine::glfw_framebuffersize_callback(GLFWwindow *window, int width, int height)
    {
        Engine *pengine = get_engine(window);

        glViewport(0, 0, width, height);
        pengine->m_pcamera->set_resolution(width, height);
    }

    void Engine::glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        Engine *pengine = get_engine(window);
        pengine->m_pinput->handle_key_callback(key, scancode, action, mods);
    }

    void Engine::glfw_mousebutton_callback(GLFWwindow* window, int button, int action, int mods)
    {
        Engine *pengine = get_engine(window);
        pengine->m_pinput->handle_mousebutton_callback(button, action, mods);
    }

    void Engine::glfw_cursorpos_callback(GLFWwindow* window, double xpos, double ypos)
    {
        Engine *pengine = get_engine(window);
        pengine->m_pinput->handle_cursorpos_callback(xpos, ypos);
    }

    bool Engine::initialize_components()
    {
        m_pclock = std::make_unique<Clock>();

        m_pcamera = std::make_unique<Camera>();
        m_pcamera->set_fov(70.0f);
        m_pcamera->set_planes(0.25f, 1000.0f);
        m_pcamera->set_resolution(1280, 720);

        m_pinput = std::make_unique<Input>(window());
        m_pinput->initialize();

        m_pfreecam = std::make_unique<FreelookCamera>(camera(), input());
        m_pfreecam->initialize();

        m_pchunk_cache = std::make_unique<ChunkCache>();

        m_pchunk_renderer = std::make_unique<ChunkRenderer>(camera(), m_pchunk_cache.get());
        m_pchunk_renderer->initialize();

        m_pchunk_updater = std::make_unique<ChunkUpdater>();
        m_pchunk_updater->initialize();

        return true;
    }

    bool Engine::initialize()
    {
        if (m_is_initialized)
            return true;

        if (!glfwInit())
        {
            glfwTerminate();
            return false;
        }

        glfwSetErrorCallback(&Engine::glfw_error_callback);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        m_pwindow = glfwCreateWindow(1280, 720, "Tarragon", nullptr, nullptr);
        if (!m_pwindow)
        {
            std::cout << "Failed to create window";
            glfwTerminate();
            return false;
        }
        glfwSetWindowUserPointer(m_pwindow, reinterpret_cast<void*>(this));
        glfwMakeContextCurrent(m_pwindow);
        glfwSetFramebufferSizeCallback(m_pwindow, &Engine::glfw_framebuffersize_callback);

        int version = gladLoadGL(glfwGetProcAddress);
        if (!version)
        {
            std::cout << "Failed to initialize OpenGL";
            glfwTerminate();
            return false;
        }

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_message_callback, nullptr);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        if (!initialize_components())
            return false;

        glfwSetKeyCallback(m_pwindow, &Engine::glfw_key_callback);
        glfwSetMouseButtonCallback(m_pwindow, &Engine::glfw_mousebutton_callback);
        glfwSetCursorPosCallback(m_pwindow, &Engine::glfw_cursorpos_callback);

        m_is_initialized = true;
        return true;
    }

    void Engine::update()
    {
        m_pclock->update();

        m_pinput->update(*m_pclock);
        m_pfreecam->update(*m_pclock);
        m_pchunk_renderer->update(*m_pclock);
        m_pchunk_updater->update(*m_pclock);

        glfwPollEvents();
    }

    void Engine::draw()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_pchunk_renderer->draw();

        glfwSwapBuffers(m_pwindow);
    }
}
