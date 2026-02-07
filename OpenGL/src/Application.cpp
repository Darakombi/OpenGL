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

#include "scenes/SceneClearColor.h"
#include "scenes/SceneTexture2d.h"

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

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int windowWidth = 1280;
	int windowHeight = 720;
	glm::vec2 windowRatio = getRatio(windowWidth, windowHeight);

	window = glfwCreateWindow(windowWidth, windowHeight, "Hello Boss", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		std::cout << "Error";

	std::cout << glGetString(GL_VERSION) << std::endl;

	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	Renderer renderer;

	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

	scene::Scene* currentScene = nullptr;
	scene::SceneMenu* sceneMenu = new scene::SceneMenu(currentScene);
	currentScene = sceneMenu;

	sceneMenu->RegisterScene<scene::SceneClearColor>("Clear color");
	sceneMenu->RegisterScene<scene::SceneTexture2d>("Texture");

	while (!glfwWindowShouldClose(window))
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
		renderer.Clear();

		ImGui_ImplGlfwGL3_NewFrame();

		if (currentScene) {
			currentScene->OnUpdate(0.0f);
			currentScene->OnRender();

			ImGui::Begin("Scene");

			if (currentScene != sceneMenu && ImGui::Button("<-")) {
				delete currentScene;
				currentScene = sceneMenu;
			}
			currentScene->OnImGuiRender();

			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	if (currentScene != sceneMenu)
		delete sceneMenu;
	else delete currentScene;

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}