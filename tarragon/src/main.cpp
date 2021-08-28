#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "glad/gl.h"
#include <GLFW/glfw3.h>

#include "engine.h"
#include <common.h>
#include "framelimit.h"
#include "camera.h"
#include "shader.h"
#include "shadersource.h"
#include "chunk.h"

using namespace tarragon;

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

	Engine engine{};
	engine.initialize();

	while (!glfwWindowShouldClose(engine.window()))
	{
		FrameLimit fps{144};

		engine.update();
		engine.draw();
	}

	glfwDestroyWindow(engine.window());
	glfwTerminate();
	return EXIT_SUCCESS;
}
