// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#pragma once
#include "ShaderProgram.hpp"

namespace mwCNCSimVisualizationSamples
{
/// A simple shader for rendering vertex colored entities.
class VertexColoredShader : public ShaderProgram
{
public:
	/// Contains locations of uniforms and attributes in the shader.
	class VertexColoredShaderLocations
	{
	public:
		GLint AmbientIntensityLocation;
		GLint MVMatrixLocation;
		GLint NormalMVMatrixLocation;
		GLint ProjectionMatrixLocation;
        GLint OverrideColor;
        GLint EnableOverrideColor;
		GLint VertexPosLocation;
		GLint VertexNormalLocation;
		GLint VertexColorLocation;
	};

private:
	static const char* s_vertexShaderText;
	static const char* s_fragmentShaderText;

public:
	VertexColoredShader();

public:
	/// Returns the locations of uniforms and attributes in the shader.
	///
	/// @return Locations of uniforms and attributes in the shader.
	const VertexColoredShaderLocations& GetShaderLocations() const;

public:
	void Activate() const override;
	void Deactivate() const override;

public:
private:
	VertexColoredShaderLocations m_shaderLocations;
};
}  // namespace mwCNCSimVisualizationSamples
