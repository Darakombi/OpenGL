#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <array>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"

struct Color {
	float r = 0.0f, g = 0.0f, b = 0.0f;
};

static void LerpFloat(float& from, float& to, const float t) {
	if (std::fabs(from - to) < t / 20) {
		to = (float)(rand() % 1000) / 1000;
	}
	from += (to - from) * t;
}
static void LerpColor(Color& from, Color& to, const float t) {
	LerpFloat(from.r, to.r, t);
	LerpFloat(from.g, to.g, t);
	LerpFloat(from.b, to.b, t);
}

static void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

const static glm::vec2 getRatio(const int v1, const int v2)
{
	int max = (v1 >= v2) ? v2 : v1;
	int min = (max == v2) ? v1 : v2;

	while (min != 0)
	{
		int temp = max % min;
		max = min;
		min = temp;
	}

	return { v1 / max, v2 / max };
}

const static int getNumInRange(int min, int max) {
	return (rand() % (max - min + 1)) + min;
}

int main()
{
	srand((unsigned int)time(NULL));

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	int windowWidth = 1280;
	int windowHeight = 720;
	glm::vec2 windowRatio = getRatio(windowWidth, windowHeight);

	window = glfwCreateWindow(windowWidth, windowHeight, "Hello Boss", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		std::cout << "Error";

	std::cout << glGetString(GL_VERSION) << std::endl;

	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	/*float positions[] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f
	};*/
	float positions[] = {
		0.0f  , 0.0f  , 0.0f, 0.0f,
		400.0f, 0.0f  , 1.0f, 0.0f,
		400.0f, 400.0f, 1.0f, 1.0f,
		0.0f  , 400.0f, 0.0f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	VertexArray va;
	VertexBuffer vb(positions, sizeof(positions));
	VertexBufferLayout layout;
	layout.Push<float>(2);
	layout.Push<float>(2);
	va.AddBuffer(vb, layout);

	IndexBuffer ib(indices, sizeof(indices) / sizeof(unsigned int));

	float w = static_cast<float>(windowWidth);
	float h = static_cast<float>(windowHeight);

	glm::mat4 proj = glm::ortho(0.0f, w, 0.0f, h);
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

	Shader shader("res/shaders/Basic.shader");
	shader.Bind();

	Color color, targetColor;

	Texture texture("res/textures/lion.png");
	shader.SetUniform1i("u_Texture", 0);

	Renderer renderer;

	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

	glm::vec3 translation(0, 0, 0);

	int tick = 0;
	int change = 10000;
	int x = 0, y = 0;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		renderer.Clear();

		tick++;

		processInput(window);
		LerpColor(color, targetColor, 0.0075f);

		ImGui_ImplGlfwGL3_NewFrame();

		shader.Bind();
		texture.Bind();
		shader.SetUniform4f("u_Tint", color.r, color.g, color.b, 1.0f);

		if (tick % change == 0) {
			x = rand() % ((int)w - 400);
			y = rand() % ((int)h - 400);
		}
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0));

		glm::mat4 mvp = proj * view * model;
		shader.SetUniformMat4f("u_MVP", mvp);
		renderer.Draw(va, ib, shader);

		ImGui::SliderInt("Ticks", &change, 1, 10000);
		//ImGui::SliderFloat3("Translation", &translation.x, 0.0f, w);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}