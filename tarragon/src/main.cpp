#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

#include "glad/gl.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <common.h>
#include "framelimit.h"
#include "camera.h"
#include "shader.h"
#include "shadersource.h"

#include "chunk.h"

using namespace tarragon;

static Clock game_clock;
static Camera camera;

namespace
{
	void glfw_error_callback(int error, const char *description)
	{
		std::cerr << "GLFW Error: " << error << " " << description << std::endl;
	}

	void glfw_framebuffersize_callback(GLFWwindow *window, int width, int height)
	{
		UNUSED_PARAM(window);

		glViewport(0, 0, width, height);
		camera.set_resolution(width, height);
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

struct Vertex
{
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec2 TexCoord;
};

int main(int argc, const char **argv)
{
	UNUSED_PARAM(argc);
	UNUSED_PARAM(argv);

	if (!glfwInit())
	{
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwSetErrorCallback(glfw_error_callback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	GLFWwindow *window = glfwCreateWindow(1280, 720, "Tarragon", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);

	int version = gladLoadGL(glfwGetProcAddress);
	if (!version)
	{
		std::cout << "Failed to initialize OpenGL";
		return EXIT_FAILURE;
	}

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(gl_message_callback, nullptr);

	glfwSetFramebufferSizeCallback(window, glfw_framebuffersize_callback);

	std::vector<Vertex> vertices
	{
		{ 
			{-1.0f, -0.5f, 0.0f},
			{1.0f, 0.0f, 0.0f, 1.0f},
			{}
		},
		{ 
			{1.0f, -0.5f, 0.0f},
			{0.0f, 1.0f, 0.0f, 1.0f},
			{}
		},
		{ 
			{0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 1.0f, 1.0f},
			{}
		},
		{ 
			{0.0f, 0.0f, 1.0f},
			{1.0f, 1.0f, 0.0f, 1.0f},
			{}
		},
	};

	const std::vector<uint32_t> indices{
		0, 1, 2,
		0, 1, 3,
		1, 2, 3,
		0, 2, 3
	};

	GLuint vertexBuffer, indexBuffer;
	glCreateBuffers(1, &vertexBuffer);
	glCreateBuffers(1, &indexBuffer);

	glNamedBufferStorage(vertexBuffer, sizeof(Vertex) * vertices.size(), vertices.data(), 0);
	glNamedBufferStorage(indexBuffer, sizeof(uint32_t) * indices.size(), indices.data(), 0);

	GLuint vao;
	glCreateVertexArrays(1, &vao);

	glVertexArrayElementBuffer(vao, indexBuffer);

	glEnableVertexArrayAttrib(vao, 0);
	glVertexArrayVertexBuffer(vao, 0, vertexBuffer, offsetof(Vertex, Position), sizeof(Vertex));
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

	glEnableVertexArrayAttrib(vao, 1);
	glVertexArrayVertexBuffer(vao, 1, vertexBuffer, offsetof(Vertex, Color), sizeof(Vertex));
	glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE, 0);

	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayAttribBinding(vao, 1, 1);

	Shader shader;
	shader.add_shader(ShaderType::Vertex, VERTEX_SOURCE);
	shader.add_shader(ShaderType::Fragment, FRAGMENT_SOURCE);
	shader.link();

	Input input{window};
	input.set_cursorstate(CursorState::Normal);

	camera.set_fov(45.0f);
	camera.set_planes(1.0f, 100.0f);
	camera.set_resolution(1280, 720);

	FreelookCamera freeCam{&camera, &input};

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	while (!glfwWindowShouldClose(window))
	{
		FrameLimit fps{144};

		game_clock.update();

		input.update(game_clock);
		freeCam.update(game_clock);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);
		shader.use();
		shader["view"].write(camera.view());
		shader["projection"].write(camera.projection());

		for (int z = -10; z < 10; z += 3)
		{
			for (int y = -10; y < 10; y += 3)
			{
				for (int x = -10; x < 10; x += 3)
				{
					auto model = glm::translate(glm::identity<glm::mat4>(), glm::vec3{ x, y, z });
					shader["model"].write(model);
					glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
				}
			}
		}
		
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}
