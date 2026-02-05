#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

struct ShaderProgramSource {
	std::string VertexShader;
	std::string FragmentShader;
};

class Shader
{
private:
	unsigned int m_RendererID;
	std::string m_Filepath;
	std::unordered_map < std::string, int > m_UniformLocationCache;
public:
	Shader(std::string filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	void SetUniform1i(const std::string& name, int v1);
	void SetUniform4f(const std::string& name, float v1, float v2, float v3, float v4);
	void SetUniformMat4f(const std::string& name, glm::mat4& matrix);
private:
	const ShaderProgramSource ParseShader(std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source) const;
	unsigned int CreateShader(const std::string& vertextShader, const std::string& fragmentShader) const;
	int GetUniformLocation(const std::string& name);
};