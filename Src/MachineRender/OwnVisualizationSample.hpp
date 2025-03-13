// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#pragma once

#include "mw3dBoundingBox.hpp"
#include "mwTPoint2d.hpp"
#include "mwTPoint3d.hpp"
#include "mwCamera.hpp"
#include "mwEngine.hpp"

#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include <memory>


namespace mwCNCSimVisualizationSamples
{
class CNCSimMachineEntity;
class OwnVisualizationSample;


class SPSpinLock {
private:
    std::atomic_flag lock_ = ATOMIC_FLAG_INIT;

public:
    SPSpinLock(){}
    void lock() {
        // 使用忙等待（自旋）来尝试获取锁
        while (lock_.test_and_set(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
    }

    void unlock() {
        lock_.clear(std::memory_order_release);
    }

    // 禁止拷贝和赋值，因为锁应该是唯一的
    SPSpinLock(const SPSpinLock&) = delete;
    SPSpinLock& operator=(const SPSpinLock&) = delete;
};


/// A sample showing how an own visualization can be created.
///
/// This sample uses GLFW for creating an OpenGL visualization.
/// Simple camera interaction is possible via mouse dragging and scrolling.
/// @note The various parts of the sample are distributed over various files.
class OwnVisualizationSample
        : public QObject
        , protected QOpenGLFunctions
{
public:
	enum class ContextType
	{
		OpenGL,
		OpenGLES
	};
	/// Creates a new sample which displays a simulation in an OpenGLWindow.
    OwnVisualizationSample(ContextType contextType);
	~OwnVisualizationSample();

public:

    void start();

    void setViewportSize(int width, int height);
    void drawFrame(int width, int height, float zoom = 1.0f); // 画一帧
    void pushAxisActionData(QVector<QVector<double>> datas);
    int getAxisDataCount();
    QVector<QVector<double>> popAxisActionData();

    float adapterFactor(mwCNCSimVisualization::mwCamera& camera, cadcam::mw3dBoundingBox<float> &sceneBoundingBox) const;

    void updateViewVectorByPosDelta(QVector<int> posDelta, bool bIsMoveReq);
    void switchAngle(QString angle, bool force = false);
    void setBackgroundColor(QString clr);
    void switchMachine(int machineType);
    void resetStockMark(bool);
    void resetView(bool resetView);
    void setSamplePeriod(int period);

    bool renderStart() {return m_renderStartFlag;}
    bool renderStop() {return !m_renderStartFlag;}
    void setRenderStop() {m_renderStartFlag = false;}
    void setRenderStart() {m_renderStartFlag = true;}

    QString m_angle;

    QString m_bgColor;
    bool m_bResetStockMark = false;

    volatile int m_samplePriod = 1;

    // 同步轴运动的乐观锁
    SPSpinLock m_axisIsActive;

private:

	/// Gets camera matrices for the current camera position and scene bounding box.
	///
	/// @param vpWidth The viewport width.
	/// @param vpHeight The viewport height.
	/// @param sceneBoundingBox The bounding box that should be visible.
	/// @param projMatrixOut Output parameter for the projection matrix.
	/// @param lookatMatrixOut Output parameter for the view (lookat) matrix.
    void GetCameraMatrices(int vpWidth,
        int vpHeight,
        cadcam::mw3dBoundingBox<float> sceneBoundingBox,
        cadcam::mwHomogenousMatrix& projMatrixOut,
        cadcam::mwHomogenousMatrix& lookatMatrixOut) const;

    void createAxisSportThd();
    void stopAndWaitAxisSportThd();
    void resetStock();
    void resetRotate();

private:
	/// Contains logic of the actual machine visualization
	std::shared_ptr<CNCSimMachineEntity> m_MachineEntity;

	/// The window in which the visualization will be drawn
	ContextType m_contextType;
	/// True if the mouse is currently pressed on the samples window.
	bool m_MouseDown;
	/// The last mouse position on the samples window.
	cadcam::mwTPoint2d<int> m_LastMousePos;
	/// The current view direction (on the scene bounding box center).
	cadcam::mwTPoint3d<float> m_CameraViewDir;
	/// The current camera up direction.
	cadcam::mwTPoint3d<float> m_CameraUpDir;

	/// The current zoom factor of the camera.
    mutable float m_ZoomFactor = -1.0;
    float m_zoomDelta = 0.0;

    mutable float m_moveOffsetHor = 0.0f;
    float m_moveOffsetHorDelta = 0.0f;

    mutable float m_moveOffsetVer = 0.0f;
    float m_moveOffsetVerDelta = 0.0f;

    misc::mwAutoPointer<cncsim::mwEngine> m_enginePtr;

    int m_width;
    int m_height;

    mutable bool m_bNeedInitFit = true;
    bool m_renderStartFlag = false;
    mutable float m_initZoom = 1.0;
    mutable float m_vecloity = 1.0f;
    mutable bool m_vecloityDirty = true;

    QMutex m_mutex;
    QWaitCondition m_notEmptyCond;

    QVector<QVector<double>> m_sportAxisData;
    QVector<double> m_lastPos;

    int m_count = 0;

private:
	/// The world up direction. Depending on your machine orientation this needs to be changed.
	static const cadcam::mwTPoint3d<float> WORLD_UP;
    static const cadcam::mwTPoint3d<float> WORLD_VIEW;
	/// The instance of the sample. Only one sample is allowed
	static OwnVisualizationSample* s_SampleInstance;

    const QStringList mapAngle2id = {"xyz", "xy", "yz", "xz"};
};
}  // namespace mwCNCSimVisualizationSamples
