// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#pragma once
#include "GeometryNodeEntity.hpp"
#include "VertexColoredShader.hpp"

namespace mwCNCSimVisualizationSamples
{
/// An entity for a per vertex colored node.
class VertexColoredNodeEntity : public GeometryNodeEntity
{
public:
	bool IsVertexColored() const override;

public:
	/// Draws the entity in its current state.
	///
	/// @param shaderLocations Attribute and uniform locations of the current shader program.
	/// @param viewMatrix The current lookat matrix.
	virtual void Draw(
		const VertexColoredShader::VertexColoredShaderLocations& shaderLocations,
		const cadcam::mwHomogenousMatrix& viewMatrix) = 0;
};
}  // namespace mwCNCSimVisualizationSamples
