// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#include <QOpenGLFunctions>

#pragma once
//#include "OpenGLIncludes.hpp"

//#include <GL/gl.h>

namespace mwCNCSimVisualizationSamples
{
class ShaderProgram : protected QOpenGLFunctions
{
public:
	/// Creates a new shader program when provided the needed shader code.
	///
	/// @param vertexShaderSource Source code for the vertex shader.
	/// @param fragmentShaderSource Source code for the fragment shader.
	ShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);

	/// Deletes the shader program.
	///
	/// Needs to be called with an active rendering context.
	virtual ~ShaderProgram();

public:
	/// Makes the shader program active.
	///
	/// Needs to be called with an active rendering context.
	virtual void Activate() const;

	/// Makes the shader program inactive.
	///
	/// Needs to be called with an active rendering context.
	/// Attributes will be disabled as well.
	virtual void Deactivate() const;

private:
	/// Creates the shader program including vertex and fragment shader
	///
	/// @param vertexShaderSource Source code for the vertex shader.
	/// @param fragmentShaderSource Source code for the fragment shader.
	void CreateShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);

	/// Creates the vertex shader.
	///
	/// @param vertexShaderSource Source code for the vertex shader.
	void CreateVertexShader(const char* vertexShaderSource);

	/// Creates the fragment shader.
	///
	/// @param fragmentShaderSource Source code for the fragment shader.
	void CreateFragmentShader(const char* fragmentShaderSource);

	/// Checks whether a shader compiled without errors.
	///
	/// If an error occurred all shaders will be deleted.
	/// Afterwards the error message will be printed to stderr
	/// and a corresponding exception will be thrown.
	/// @param shaderId The OpenGL id of the shader.
	void CheckShaderCompilation(GLuint shaderId);

	/// Checks whether a shader program linked without errors.
	///
	/// If an error occurred all shaders will be deleted.
	/// Afterwards the error message will be printed to stderr
	/// and a corresponding exception will be thrown.
	/// @param programId The OpenGL id of the program.
	void CheckShaderLinking(GLuint programId);

	/// Deletes all shaders that have been created.
	void DeleteShaders();

protected:
	GLuint m_vertexShader = 0;
	GLuint m_fragmentShader = 0;
    GLuint m_shaderProgram = 0;
};
}  // namespace mwCNCSimVisualizationSamples
