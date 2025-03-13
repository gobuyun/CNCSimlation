// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#include "ShaderProgram.hpp"

#include <cstring>
#include <exception>
#include <iostream>
#include <vector>

namespace mwCNCSimVisualizationSamples
{
ShaderProgram::ShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    initializeOpenGLFunctions();
	try
	{
		CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
	}
	catch (...)
	{
		DeleteShaders();  // Don't leak the shaders.
		throw;
	}
}

ShaderProgram::~ShaderProgram()
{
	DeleteShaders();
}

void ShaderProgram::Activate() const
{
    const_cast<ShaderProgram*>(this)->glUseProgram(m_shaderProgram);
}

void ShaderProgram::Deactivate() const
{
    const_cast<ShaderProgram*>(this)->glUseProgram(0);
}

void ShaderProgram::CreateShaderProgram(
	const char* vertexShaderSource, const char* fragmentShaderSource)
{
	// Create shader
	std::cout << "Creating vertex shader." << std::endl;
	CreateVertexShader(vertexShaderSource);

	std::cout << "Creating fragment shader." << std::endl;
	CreateFragmentShader(fragmentShaderSource);

	std::cout << "Creating shader program." << std::endl;
	m_shaderProgram = glCreateProgram();

	std::cout << "Linking shader." << std::endl;
	glAttachShader(m_shaderProgram, m_vertexShader);
	glAttachShader(m_shaderProgram, m_fragmentShader);
	glLinkProgram(m_shaderProgram);
	CheckShaderLinking(m_shaderProgram);
}

void ShaderProgram::CreateVertexShader(const char* vertexShaderSource)
{
	m_vertexShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(m_vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(m_vertexShader);
	CheckShaderCompilation(m_vertexShader);
}

void ShaderProgram::CreateFragmentShader(const char* fragmentShaderSource)
{
	m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(m_fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(m_fragmentShader);

	CheckShaderCompilation(m_fragmentShader);
}

void ShaderProgram::CheckShaderCompilation(GLuint /*shaderId*/)
{
	GLint isCompiled = 0;
	glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(m_vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(m_vertexShader, maxLength, &maxLength, &errorLog[0]);

		std::cerr << errorLog.data();
		std::string exceptionMessage("Shader could not be compiled\n");
		std::string logMessage(errorLog.data());
		throw std::runtime_error(exceptionMessage + logMessage);
	}
}

void ShaderProgram::CheckShaderLinking(GLuint programId)
{
	GLint isLinked = 0;
	glGetProgramiv(programId, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(programId, maxLength, &maxLength, &infoLog[0]);

		DeleteShaders();  // Don't leak the shaders.

		std::cerr << infoLog.data();
		std::string exceptionMessage("Shader could not be linked\n");
		std::string logMessage(infoLog.data());
		throw std::runtime_error(exceptionMessage + logMessage);
	}
}

void ShaderProgram::DeleteShaders()
{
	if (m_shaderProgram)
	{
		glDeleteShader(m_shaderProgram);
		m_shaderProgram = 0;
	}
	if (m_vertexShader)
	{
		glDeleteShader(m_vertexShader);
		m_vertexShader = 0;
	}

	if (m_fragmentShader)
	{
		glDeleteShader(m_fragmentShader);
		m_fragmentShader = 0;
	}
}

}  // namespace mwCNCSimVisualizationSamples
