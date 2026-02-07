#include "SceneTexture2d.h"

#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace scene {

	SceneTexture2d::SceneTexture2d()
		: m_Proj(glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f)),
		m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))),
		m_TranslationA(0.0f, 0.0f, 0.0f), m_TranslationB(500.0f, 500.0f, 0.0f)
	{
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

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		m_VAO = std::make_unique<VertexArray>();
		m_VBO = std::make_unique<VertexBuffer>(positions, sizeof(positions));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);

		m_VAO->AddBuffer(*m_VBO, layout);

		m_IBO = std::make_unique<IndexBuffer>(indices, sizeof(indices) / sizeof(unsigned int));

		m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
		m_Shader->Bind();

		m_Texture = std::make_unique<Texture>("res/textures/lion.png");
		m_Shader->SetUniform1i("u_Texture", 0);
	}

	void SceneTexture2d::OnRender() {
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		Renderer renderer;

		m_Shader->Bind();
		m_Texture->Bind();

		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
			glm::mat4 mvp = m_Proj * m_View * model;
			m_Shader->SetUniformMat4f("u_MVP", mvp);
			renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
		}
		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
			glm::mat4 mvp = m_Proj * m_View * model;
			m_Shader->SetUniformMat4f("u_MVP", mvp);
			renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
		}
	}

	void SceneTexture2d::OnImGuiRender() {
		ImGui::SliderFloat3("TranslationA", &m_TranslationA.x, 0.0f, 800);
		ImGui::SliderFloat3("TranslationB", &m_TranslationB.x, 0.0f, 800);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

}