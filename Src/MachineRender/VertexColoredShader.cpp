// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#include "VertexColoredShader.hpp"

namespace mwCNCSimVisualizationSamples
{
const char* VertexColoredShader::s_vertexShaderText =
	"#version 100\n"
	"precision mediump float;\n"
	"uniform highp float uAmbientIntensity;\n"
	"uniform highp mat4 uModelViewMatrix;\n"
	"uniform highp mat4 uNormalModelViewMatrix;\n"
	"uniform highp mat4 uProjectionMatrix;\n"
    "uniform highp vec3 uOverrideColor;\n"
    "uniform int uEnableOverrideColor;\n"
	"\n"
	"attribute vec3 aPos;\n"
	"attribute vec3 aNormal;\n"
	"attribute vec4 aColor;\n"
	"\n"
	"varying vec3 vColor;\n"
	"\n"
	"void main()\n"
	"{\n"
    "   vec4 useColor = aColor;"
    "   if (uEnableOverrideColor == 1) useColor = vec4(uOverrideColor.xyz, 1.0);\n"
	"   vec4 eyeSpacePosition = uModelViewMatrix * vec4(aPos,1);\n"
	"   vec3 eyeSpaceNormal = normalize((uNormalModelViewMatrix * vec4(aNormal,0)).xyz);\n"
	"   gl_Position = uProjectionMatrix * eyeSpacePosition;\n"
	"   \n"
	"   // Assuming that ambient light is white\n"
    "   vec4 ambient = uAmbientIntensity * useColor;\n"
	"   \n"
	"   // Assuming that light is white and at camera position\n"
	"   // Also use full ambient reflectivity\n"
    "   vec4 diffuse = useColor * abs(dot(eyeSpaceNormal, vec3(0,0,1)));\n"
	"   \n"
	"   vColor = (ambient + diffuse).xyz;\n"
	"}";
const char* VertexColoredShader::s_fragmentShaderText =
	"#version 100\n"
	"precision mediump float;\n"
	"varying vec3 vColor;\n"
	"void main()\n"
	"{\n"
	"    gl_FragColor = vec4(vColor, 1.0);\n"
	"}\n";

VertexColoredShader::VertexColoredShader() : ShaderProgram(s_vertexShaderText, s_fragmentShaderText)
{
	// Store attribute positions
	m_shaderLocations.AmbientIntensityLocation =
		glGetUniformLocation(m_shaderProgram, "uAmbientIntensity");
	m_shaderLocations.MVMatrixLocation = glGetUniformLocation(m_shaderProgram, "uModelViewMatrix");
	m_shaderLocations.NormalMVMatrixLocation =
		glGetUniformLocation(m_shaderProgram, "uNormalModelViewMatrix");
	m_shaderLocations.ProjectionMatrixLocation =
		glGetUniformLocation(m_shaderProgram, "uProjectionMatrix");
    m_shaderLocations.OverrideColor = glGetUniformLocation(m_shaderProgram, "uOverrideColor");
    m_shaderLocations.EnableOverrideColor = glGetUniformLocation(m_shaderProgram, "uEnableOverrideColor");
	m_shaderLocations.VertexPosLocation = glGetAttribLocation(m_shaderProgram, "aPos");
	m_shaderLocations.VertexNormalLocation = glGetAttribLocation(m_shaderProgram, "aNormal");
	m_shaderLocations.VertexColorLocation = glGetAttribLocation(m_shaderProgram, "aColor");
}

const VertexColoredShader::VertexColoredShaderLocations& VertexColoredShader::GetShaderLocations()
	const
{
	return m_shaderLocations;
}

void VertexColoredShader::Activate() const
{
	ShaderProgram::Activate();
    const_cast<VertexColoredShader*>(this)->glEnableVertexAttribArray(m_shaderLocations.VertexPosLocation);
    const_cast<VertexColoredShader*>(this)->glEnableVertexAttribArray(m_shaderLocations.VertexNormalLocation);
    const_cast<VertexColoredShader*>(this)->glEnableVertexAttribArray(m_shaderLocations.VertexColorLocation);
}

void VertexColoredShader::Deactivate() const
{
    const_cast<VertexColoredShader*>(this)->glDisableVertexAttribArray(m_shaderLocations.VertexPosLocation);
    const_cast<VertexColoredShader*>(this)->glDisableVertexAttribArray(m_shaderLocations.VertexNormalLocation);
    const_cast<VertexColoredShader*>(this)->glDisableVertexAttribArray(m_shaderLocations.VertexColorLocation);
	ShaderProgram::Deactivate();
}
}  // namespace mwCNCSimVisualizationSamples
