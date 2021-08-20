#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

#include "glad/gl.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <common.h>
#include "framelimit.h"
#include "clock.h"
#include "input.h"

using namespace tarragon;

namespace
{
	void glfw_error_callback(int error, const char* description)
	{
		std::cerr << "GLFW Error: " << error << " " << description << std::endl;
	}

	void glfw_framebuffersize_callback(GLFWwindow* window, int width, int height)
	{
		UNUSED_PARAM(window);

		glViewport(0, 0, width, height);
	}

	void GLAPIENTRY gl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		UNUSED_PARAM(length);
		UNUSED_PARAM(userParam);

		auto const src_str = [source]()
		{
			switch (source)
			{
			case GL_DEBUG_SOURCE_API: return "API";
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
			case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
			case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
			case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
			case GL_DEBUG_SOURCE_OTHER: return "OTHER";
			default: return "<invalid>";
			}
		}();

		auto const type_str = [type]()
		{
			switch (type)
			{
			case GL_DEBUG_TYPE_ERROR: return "ERROR";
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
			case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
			case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
			case GL_DEBUG_TYPE_MARKER: return "MARKER";
			case GL_DEBUG_TYPE_OTHER: return "OTHER";
			default: return "<invalid>";
			}
		}();

		auto const severity_str = [severity]()
		{
			switch (severity)
			{
			case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
			case GL_DEBUG_SEVERITY_LOW: return "LOW";
			case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
			case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
			default: return "<invalid>";
			}
		}();

		std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
	}
}

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
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Tarragon", NULL, NULL);
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

	Input input{ window };
	input.on_key().attach([window](Key key, KeyState action, KeyModifier)
	{
		if (key == Key::Escape && action == KeyState::Down)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
	});

	const std::vector<glm::vec3> vertices
	{
		{ -1.0f, -0.5f, 0.0f },
		{  1.0f, -0.5f, 0.0f },
		{  0.0f,  1.0f, 0.0f },
		{  0.0f,  0.0f, 1.0f },
	};

	const std::vector<glm::vec4> colors
	{
		{ 1.0f, 0.0f, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 0.0f, 1.0f },
	};

	const std::vector<uint32_t> indices
	{
		0, 1, 2,
		0, 1, 3,
		1, 2, 3,
		0, 2, 3
	};

	GLuint vertexBuffer, colorBuffer, indexBuffer;
	glCreateBuffers(1, &vertexBuffer);
	glCreateBuffers(1, &colorBuffer);
	glCreateBuffers(1, &indexBuffer);

	glNamedBufferStorage(vertexBuffer, sizeof(glm::vec3)*vertices.size(), vertices.data(), 0);
	glNamedBufferStorage(colorBuffer, sizeof(glm::vec4)*colors.size(), colors.data(), 0);
	glNamedBufferStorage(indexBuffer, sizeof(uint32_t)*indices.size(), indices.data(), 0);

	GLuint vao;
	glCreateVertexArrays(1, &vao);

	glVertexArrayVertexBuffer(vao, 0, vertexBuffer, 0, sizeof(glm::vec3));
	glVertexArrayVertexBuffer(vao, 1, colorBuffer, 0, sizeof(glm::vec4));
	glVertexArrayElementBuffer(vao, indexBuffer);

	glEnableVertexArrayAttrib(vao, 0);
	glEnableVertexArrayAttrib(vao, 1);
	
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE, 0);

	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayAttribBinding(vao, 1, 1);

	GLuint prog = glCreateProgram();
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

const std::string vertexShaderSource =
R"xx(
#version 460

layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 vert_color;

void main()
{
	vert_color = color;
	gl_Position = (projection * view * model) * vec4(pos, 1.0);
}

)xx";

const std::string fragmentShaderSource =
R"xx(
#version 460

in vec4 vert_color;

out vec4 frag_color;

void main()
{
	frag_color = vert_color;
}

)xx";

	auto vertexSourcePtr = (const GLchar*)vertexShaderSource.data();
	glShaderSource(vertexShader, 1, &vertexSourcePtr, nullptr);
	glCompileShader(vertexShader);

	auto fragmentSourcePtr = (const GLchar*)fragmentShaderSource.data();
	glShaderSource(fragmentShader, 1, &fragmentSourcePtr, nullptr);
	glCompileShader(fragmentShader);

	char log[1024];
	memset(log, 0, 1024);

	glGetShaderInfoLog(vertexShader, 1024, nullptr, log);
	std::cout << log << std::endl;
	glGetShaderInfoLog(fragmentShader, 1024, nullptr, log);
	std::cout << log << std::endl;

	glAttachShader(prog, vertexShader);
	glAttachShader(prog, fragmentShader);
	glLinkProgram(prog);
	glDetachShader(prog, vertexShader);
	glDetachShader(prog, fragmentShader);

	GLint modelLocation = glGetUniformLocation(prog, "model");
	GLint viewLocation = glGetUniformLocation(prog, "view");
	GLint projectionLocation = glGetUniformLocation(prog, "projection");

	glm::mat4 model = glm::scale(glm::identity<glm::mat4>(), glm::vec3{ 1.2f });
	glm::mat4 view = glm::lookAtRH(glm::vec3{ 2.0f, 2.0f, 2.0f }, glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f });
	glm::mat4 projection = glm::perspectiveRH(glm::radians(45.0f), 1280.0f/720.0f, 1.0f, 100.0f);

	Clock game_clock;

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	while (!glfwWindowShouldClose(window))
	{
		FrameLimit fps{ 144 };
		game_clock.update();

		model = glm::rotate(model, game_clock.last_delta(), glm::vec3{ 0.0f, 0.0f, 1.0f });

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);
		glUseProgram(prog);
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}
