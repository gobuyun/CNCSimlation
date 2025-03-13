// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#pragma once
#include "GeometryNodeEntity.hpp"
#include "UnicoloredShader.hpp"

namespace mwCNCSimVisualizationSamples
{
/// Base class for a unicolored node entity
class UnicoloredNodeEntity : public GeometryNodeEntity
{
public:
	bool IsVertexColored() const override;

public:
	/// Draws the entity in its current state.
	///
	/// @param shaderLocations Attribute and uniform locations of the current shader program.
	/// @param viewMatrix The current lookat matrix.
	virtual void Draw(
		const UnicoloredShader::UnicoloredShaderLocations& shaderLocations,
		const cadcam::mwHomogenousMatrix& viewMatrix) = 0;
};
}  // namespace mwCNCSimVisualizationSamples
