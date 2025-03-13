// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#pragma once
#include "ShaderProgram.hpp"

namespace mwCNCSimVisualizationSamples
{
/// A simple shader for rendering unicolored entities.
class UnicoloredShader : public ShaderProgram
{
public:
	/// Contains locations of uniforms and attributes in the shader.
	class UnicoloredShaderLocations
	{
	public:
		GLint AmbientIntensityLocation;
		GLint MVMatrixLocation;
		GLint NormalMVMatrixLocation;
		GLint ProjectionMatrixLocation;
        GLint EntityColorLocation;
        GLint OverrideColor;
        GLint EnableOverrideColor;
		GLint VertexPosLocation;
		GLint VertexNormalLocation;
		GLint NormalScale;
	};

private:
	static const char* s_vertexShaderText;
	static const char* s_fragmentShaderText;

public:
	/// Creates a basic shader program.
	///
	/// Needs to be called with an active rendering context.
	UnicoloredShader();

public:
	/// Returns the locations of uniforms and attributes in the shader.
	///
	/// @return Locations of uniforms and attributes in the shader.
	const UnicoloredShaderLocations& GetShaderLocations() const;

public:
	void Activate() const override;
	void Deactivate() const override;

private:
	UnicoloredShaderLocations m_shaderLocations;
};
}  // namespace mwCNCSimVisualizationSamples
