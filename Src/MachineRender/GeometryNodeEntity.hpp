// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#include <QOpenGLFunctions>

#include "UnicoloredShader.hpp"
#include "mw3dBoundingBox.hpp"
#pragma once
namespace mwCNCSimVisualizationSamples
{
/// [GeometryNodeEntity]
/// An abstract class representing the entity of a cncsim::mwGeometryNode in a scene.
///
/// Nodes that need to be rendered will have corresponding specialized entities.
class GeometryNodeEntity
{
public:
	GeometryNodeEntity();
	virtual ~GeometryNodeEntity();

public:
	/// Gets the current visual bounding box.
	///
	/// The visual bounding box is updated when calling PrepareForDrawing.
	/// @return The current visual bounding box.
	virtual cadcam::mw3dBoundingBox<float> GetVisualBoundingBox() const = 0;

public:
	/// Invalidates the visual representation (colors, ...)
	///
	/// The actual update might only take place when calling PrepareForDrawing.
	virtual void NotifyVisualRepresentationChanged() = 0;

public:
	/// Prepares the entity for drawing.
	///
	/// Must be called with a valid render context before drawing.
	/// This method will update the entity to the current visual state of the node,
	/// which also includes the bounding box.
	virtual void PrepareForDrawing() = 0;

public:
	/// Returns whether the should be drawn vertex colored.
	///
	/// @return True if the should be drawn vertex colored, false if unicolored.
	virtual bool IsVertexColored() const = 0;
};
/// [GeometryNodeEntity]
}  // namespace mwCNCSimVisualizationSamples
