#include <cstdlib>
#include <iostream>

#include "glad/gl.h"
#include <GLFW/glfw3.h>

#include "framelimit.h"

namespace
{
	void glfw_error_callback(int error, const char* description)
	{
		std::cerr << "GLFW Error: " << error << " " << description << std::endl;
	}

	void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		std::cout << "Key event: " << key << std::endl;
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	void glfw_framebuffersize_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}
}

int main(int argc, const char **argv)
{
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
	{
		glfwTerminate();
		return EXIT_FAILURE;
	}

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

	glfwSetKeyCallback(window, glfw_key_callback);
	glfwSetFramebufferSizeCallback(window, glfw_framebuffersize_callback);

	glClearColor(1.f, 0.f, 0.f, 1.f);
	while (!glfwWindowShouldClose(window))
	{
		tarragon::FrameLimit fps{ 60 };

		glClear(GL_COLOR_BUFFER_BIT);

		// render ...

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}
