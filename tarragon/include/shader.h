#pragma once

#include <string_view>
#include <vector>

#include "glad/gl.h"

namespace tarragon
{
    enum class ShaderType : GLenum
    {
        Vertex = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER,
        Compute = GL_COMPUTE_SHADER,
        TesselationControl = GL_TESS_CONTROL_SHADER,
        TesselationEvaluation = GL_TESS_EVALUATION_SHADER,
    };

    class UniformWriter final
    {
    private:
        GLuint m_location;
    
    public:
        UniformWriter(GLuint location)
            : m_location{ location }
        { }

        void write(glm::vec2 const& value) { glUniform2fv(m_location, 1, glm::value_ptr(value)); }
        void write(glm::vec3 const& value) { glUniform3fv(m_location, 1, glm::value_ptr(value)); }
        void write(glm::vec4 const& value) { glUniform4fv(m_location, 1, glm::value_ptr(value)); }
        void write(glm::mat3 const& value) { glUniformMatrix3fv(m_location, 1, GL_FALSE, glm::value_ptr(value)); }
        void write(glm::mat4 const& value) { glUniformMatrix4fv(m_location, 1, GL_FALSE, glm::value_ptr(value)); }
    };

    class Shader final
    {
    private:
        GLuint m_program;
        std::vector<GLuint> m_shaders;

    public:
        Shader()
        {
            m_program = glCreateProgram();
        }

        ~Shader()
        {
            for (auto &&shader : m_shaders)
                glDeleteShader(shader);
            glDeleteProgram(m_program);
        }

        GLuint program() const { return m_program; }
        void use() const { glUseProgram(m_program); }

        void add_shader(ShaderType type, std::string_view source)
        {
            GLuint shader = glCreateShader(static_cast<GLenum>(type));
            auto psource = static_cast<const GLchar *>(source.data());
            glShaderSource(shader, 1, &psource, nullptr);
            glCompileShader(shader);

            m_shaders.push_back(shader);
        }

        void link()
        {
            for (auto &&shader : m_shaders)
                glAttachShader(m_program, shader);
            glLinkProgram(m_program);
            for (auto &&shader : m_shaders)
                glDetachShader(m_program, shader);
        }

        UniformWriter operator[](std::string_view uniform)
        {
            GLuint location = glGetUniformLocation(m_program, uniform.data());
            return UniformWriter{ location };
        }
    };
}
