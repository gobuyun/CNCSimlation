// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#include "OwnVisualizationSample.hpp"

#include "CNCSimMachineEntity.hpp"
#include "UnicoloredShader.hpp"
#include "mwCNCSimVersionInfo.hpp"
#include "mwCamera.hpp"
#include "mwMatrix.hpp"
#include "mwMatrixTransformationsFactory.hpp"
#include "mwString.hpp"
#include "mwCadcamFwdDecl.hpp"

#include "mwAxisNode.hpp"
#include "mwEndMill.hpp"
#include "mwEngine.hpp"
#include "mwException.hpp"
#include "mwFileSystem.hpp"
#include "mwMachineDefinitionNode.hpp"
#include "mwMachineGeometryNode.hpp"
#include "mwSimulationOptions.hpp"
#include "mwSimulatorVisualizer.hpp"
#include "mwToolNode.hpp"
#include "mwAutoRefineOptions.hpp"
#include "mwMotionContainer.hpp"
#include "mwFaceMill.hpp"
#include "mwLolMill.hpp"

#include <QTimer>
#include <QThread>
#include <QtConcurrent>
#include <QMutexLocker>
#include <QColor>

#include <cstdlib>
#include <iostream>
#include <thread>

using namespace cadcam;
typedef mwMatrixTransformationsFactory<float> MatrixFactory;


// 向上是+x，屏幕朝外是+y，向右是+z
namespace mwCNCSimVisualizationSamples
{
const cadcam::mwTPoint3d<float> OwnVisualizationSample::WORLD_UP(0.f, 1.f, 0.f);
const cadcam::mwTPoint3d<float> OwnVisualizationSample::WORLD_VIEW(0.f, 0.f, -1.f);
OwnVisualizationSample* OwnVisualizationSample::s_SampleInstance = nullptr;

OwnVisualizationSample::OwnVisualizationSample(ContextType contextType) : QObject{}
{
    resetView(true);
    setlocale(LC_ALL, "");
    initializeOpenGLFunctions();
}

OwnVisualizationSample::~OwnVisualizationSample()
{
    stopAndWaitAxisSportThd(); // 停止轴运动线程
    m_enginePtr->RemoveEngineListener(m_MachineEntity);
    m_enginePtr->Stop();
    m_enginePtr.Release();
    m_enginePtr = nullptr;
    m_MachineEntity = nullptr;
}

void OwnVisualizationSample::start()
{
    m_enginePtr = cncsim::CreateEngine();
    m_MachineEntity = std::make_shared<CNCSimMachineEntity>(m_enginePtr);
    m_enginePtr->AddEngineListener(m_MachineEntity);
    m_MachineEntity->RegisterOwnVisualObject(this);
    m_MachineEntity->Initialize();

    cncsim::mwSimulationOptions engineOption;
    engineOption.SetCuttingEnabled(true);
    engineOption.SetCollisionCheckingEnabled(true);
    m_enginePtr->SetOptions(engineOption);

//    switchMachine(1);
}

void OwnVisualizationSample::setViewportSize(int width, int height)
{
    m_width = width;
    m_height = height;
}

void OwnVisualizationSample::pushAxisActionData(QVector<QVector<double>> datas)
{
    if (datas.size() == 0)
        return;
    QMutexLocker locker(&m_mutex);
    if (m_sportAxisData.size() == 0)
        m_sportAxisData.resize(datas.size());
    for (auto i = 0; i<datas.size(); ++i) {
        m_sportAxisData[i].append(datas[i]);
    }
    ++m_count;
    m_notEmptyCond.wakeOne();
}

int OwnVisualizationSample::getAxisDataCount()
{
    return m_sportAxisData.size();
}

QVector<QVector<double>> OwnVisualizationSample::popAxisActionData()
{
    QMutexLocker locker(&m_mutex);
    while (m_sportAxisData.size() == 0 && this->property("axisSportStopReq").toBool() != true)
        m_notEmptyCond.wait(&m_mutex);
    m_count = 0;
    auto temp = m_sportAxisData;
    m_sportAxisData.clear();
    return temp;
}

void OwnVisualizationSample::drawFrame(int width, int height, float zoom)
{
    m_zoomDelta = zoom;
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    m_MachineEntity->PrepareForDrawing();

    // Setup camera
    cadcam::mwMatrix<float, 4, 4> projectionMatrix;
    cadcam::mwMatrix<float, 4, 4> lookatMatrix;

    GetCameraMatrices(
        width,
        height,
        m_MachineEntity->GetVisualBoundingBox(),
        projectionMatrix,
        lookatMatrix);

    // Draw the machine
    m_MachineEntity->Draw(projectionMatrix, lookatMatrix);
}

void OwnVisualizationSample::GetCameraMatrices(
    int vpWidth,
    int vpHeight,
    cadcam::mw3dBoundingBox<float> sceneBoundingBox,
    cadcam::mwHomogenousMatrix& projMatrixOut,
    cadcam::mwHomogenousMatrix& lookatMatrixOut) const
{
    if (!sceneBoundingBox.IsInitialized())
    {
        cadcam::mwTPoint3d<float> corner(100.f, 100.f, 100.f);
        sceneBoundingBox = cadcam::mw3dBoundingBox<float>(corner, -corner);
    }

    mwCNCSimVisualization::mwCamera camera(mwCNCSimVisualization::mwCamera::PT_ORTHOGRAPHIC);

    // To calculate camera matrices content scaling can be ignored here
    camera.SetSize(vpWidth, vpHeight, vpWidth, vpHeight, 1);
    static auto originBox = sceneBoundingBox;
    cadcam::mwTPoint3d<float> cameraEye(
        originBox.GetCenter() - originBox.GetDiagonalLength()*3 * m_CameraViewDir);
    cadcam::mwTPoint3d<float> cameraCenter(originBox.GetCenter());

    camera.SetEyeViewDirectionUp(cameraEye, cameraCenter - cameraEye, m_CameraUpDir);
    camera.FitNearAndFarPlane(originBox);

    // zoom
    if (m_bNeedInitFit)
    {
        m_bNeedInitFit = false;
        m_initZoom = adapterFactor(camera, originBox);
    }
    if (m_ZoomFactor == -1.0)
        m_ZoomFactor = m_initZoom;
    else if (m_zoomDelta != 0.0)
        m_ZoomFactor *= qPow(0.9, m_zoomDelta);
    camera.Zoom(m_ZoomFactor);

    // move
    auto curPos = camera.GetPosition();
    if (m_vecloityDirty)
    {
        m_vecloity = m_ZoomFactor/m_initZoom;
        m_moveOffsetHor += m_moveOffsetHorDelta*m_vecloity;
        m_moveOffsetVer += m_moveOffsetVerDelta*m_vecloity;
        m_vecloityDirty = false;
    }
    // hor
    cadcam::mwTPoint3d<float> cameraRight = m_CameraViewDir % m_CameraUpDir;
    cadcam::mwTPoint3d<float> horOffset = {
        cameraRight.x()*m_moveOffsetHor,
        cameraRight.y()*m_moveOffsetHor,
        cameraRight.z()*m_moveOffsetHor
    };
    // ver
    cadcam::mwTPoint3d<float> verOffset = {
        m_CameraUpDir.x()*m_moveOffsetVer,
        m_CameraUpDir.y()*m_moveOffsetVer,
        m_CameraUpDir.z()*m_moveOffsetVer
    };
    curPos = curPos + horOffset + verOffset;
    camera.SetPosition(curPos);
    projMatrixOut = camera.GetProjectionMatrix(mwCNCSimVisualization::mwCamera::GL_OPEN_GL);
    lookatMatrixOut = camera.GetViewMatrix();
}

float OwnVisualizationSample::adapterFactor(mwCNCSimVisualization::mwCamera& camera, cadcam::mw3dBoundingBox<float>& sceneBoundingBox) const
{
    return sceneBoundingBox.GetDiagonalLength()/camera.GetOrthographicCameraData().GetFrustumWidth();
}

void OwnVisualizationSample::updateViewVectorByPosDelta(QVector<int> posDelta, bool bIsMoveReq)
{
    mwTPoint3d<float> viewDir = m_CameraViewDir;
    mwTPoint3d<float> cameraRight = viewDir % m_CameraUpDir;

    if (bIsMoveReq)
    {
        m_moveOffsetHorDelta = posDelta[0]*5;
        m_moveOffsetVerDelta = posDelta[1]*-5;
        m_vecloityDirty = true;
    }
    else
    {
        if (m_angle != "xyz") // 非3D视图不能旋转
            return;

        mwHomogenousMatrix verticalRotation =
            MatrixFactory::Create3dRotation(cameraRight, 0.3f * posDelta[1]);
        mwHomogenousMatrix horizontalRotation =
            MatrixFactory::Create3dRotation(m_CameraUpDir, 0.3f * posDelta[0]);
        mwHomogenousMatrix completeRotation = horizontalRotation * verticalRotation;

        m_CameraViewDir.Transform(completeRotation, true);
        m_CameraViewDir.Normalize();
        m_CameraUpDir.Transform(completeRotation, true);
        m_CameraUpDir.Normalize();
    }
}

void OwnVisualizationSample::resetRotate()
{
    m_CameraViewDir = WORLD_VIEW;
    m_CameraUpDir = WORLD_UP;
    auto worldRight = WORLD_VIEW % WORLD_UP;

    mwHomogenousMatrix verticalRotation =
        MatrixFactory::Create3dRotation(worldRight, -45);
    mwHomogenousMatrix horizontalRotation =
        MatrixFactory::Create3dRotation(WORLD_UP, 30);
    mwHomogenousMatrix completeRotation = horizontalRotation * verticalRotation;

    m_CameraViewDir.Transform(completeRotation, true);
    m_CameraUpDir.Transform(completeRotation, true);
}

void OwnVisualizationSample::switchAngle(QString angle, bool force)
{
    if (force != true && m_angle == angle)
        return;

    m_angle = angle;
    m_moveOffsetHor = 0.f;
    m_moveOffsetVer = 0.f;
    resetRotate(); // 切换前都恢复默认旋转视角
    auto worldRight = WORLD_VIEW % WORLD_UP;
    if (m_angle == mapAngle2id[0]) // XYZ
    {
    }
    else if (m_angle == mapAngle2id[1]) // XY
    {
        m_CameraUpDir = worldRight;
        m_CameraViewDir = -WORLD_UP;
    }
    else if (m_angle == mapAngle2id[2]) // YZ
    {
        m_CameraUpDir = WORLD_UP;
        m_CameraViewDir = WORLD_VIEW;
    }
    else if (m_angle == mapAngle2id[3]) // XZ
    {
        m_CameraUpDir = WORLD_UP;
        m_CameraViewDir = worldRight;
    }
}

void OwnVisualizationSample::setBackgroundColor(QString clr)
{
    m_bgColor = clr;
}

void OwnVisualizationSample::resetStockMark(bool resetStock)
{
    if (resetStock)
    {
        this->resetStock();
    }
}

void OwnVisualizationSample::resetView(bool resetView)
{
    if (resetView)
    {
        // 视角
        switchAngle("xyz", true);
        // 平移
        m_moveOffsetHor = 0.f;
        m_moveOffsetVer = 0.f;
        m_vecloityDirty = true;
        // 旋转
        resetRotate();
        // 缩放
        m_ZoomFactor = -1.0f;
    }
}

void OwnVisualizationSample::setSamplePeriod(int period)
{
    m_samplePriod = period;
}

void OwnVisualizationSample::createAxisSportThd()
{
    QThreadPool::globalInstance()->start([this](){
        while (1) {
            if (this->property("axisSportStopReq").toBool())
                break;
            QString axisName = "X";
            cncsim::mwAxisNode axis = m_enginePtr->GetNodeByName(axisName.toStdString()).AsNode<cncsim::mwAxisNode>();
            m_enginePtr->SetAxis(cncsim::AxisValuePair(axis.GetId(), axis.GetValue()+1));

            axisName = "Y";
            axis = m_enginePtr->GetNodeByName(axisName.toStdString()).AsNode<cncsim::mwAxisNode>();
            m_enginePtr->SetAxis(cncsim::AxisValuePair(axis.GetId(), axis.GetValue()+1));

             axisName = "Z";
            axis = m_enginePtr->GetNodeByName(axisName.toStdString()).AsNode<cncsim::mwAxisNode>();
            m_enginePtr->SetAxis(cncsim::AxisValuePair(axis.GetId(), axis.GetValue()+1));

             axisName = "R1";
            axis = m_enginePtr->GetNodeByName(axisName.toStdString()).AsNode<cncsim::mwAxisNode>();
            m_enginePtr->SetAxis(cncsim::AxisValuePair(axis.GetId(), axis.GetValue()+1));

             axisName = "R2";
            axis = m_enginePtr->GetNodeByName(axisName.toStdString()).AsNode<cncsim::mwAxisNode>();
            m_enginePtr->SetAxis(cncsim::AxisValuePair(axis.GetId(), axis.GetValue()+1));
            QThread::msleep(10);
        }
        this->setProperty("axisSportStop", true);
    });
    return;
    QThreadPool::globalInstance()->start([this](){
        QStringList axisActiveOrder{"X", "Y", "Z", "A", "C"}; // 这个顺序与CNC协议上的axis数据顺序对应
        int curPriod = 0; // 周期计数，与m_samplePriod配合
        auto periodSetting = this->m_samplePriod; // 每次运动序列更新周期
        while (1)
        {
            if (this->property("axisSportStopReq").toBool())
                break;
            // 拉取新的运动序列
            auto asxisDatas = this->popAxisActionData();
            if (asxisDatas.size() == 0)
                continue;
            // 每次运动序列更新周期
            auto periodSettingUpdate = this->m_samplePriod;
            if (periodSetting != periodSettingUpdate)
            {
                qDebug() << "336+++++periodSetting=" << periodSetting << periodSettingUpdate;
                periodSetting = periodSettingUpdate;
                curPriod = 0;
            }

            auto supportAxisTimes = qMin(axisActiveOrder.size(), asxisDatas.size());
            auto periodsCont = asxisDatas[0].size();

            // 首帧轴数据
            if (m_lastPos.isEmpty())
            {
                for (auto axisId = 0; axisId<supportAxisTimes; ++axisId)
                {
                    auto axisData = asxisDatas[axisId];
                    auto axisName = axisActiveOrder[axisId];
                    m_lastPos.append(axisData[0]);
                    cncsim::mwAxisNode axis = m_enginePtr->GetNodeByName(axisName.toStdString()).AsNode<cncsim::mwAxisNode>();
                    auto result = m_enginePtr->SetAxis(cncsim::AxisValuePair(axis.GetId(), m_lastPos[axisId]));
                }
            }

            // 遍历每个插补周期的数据帧
            cncsim::AxesValues values;
            cncsim::mwMotionContainer container;

            for (auto period = 0; period<periodsCont; ++period)
            {
                for (auto axisId = 0; axisId<supportAxisTimes; ++axisId)
                {
//                    if (fIsEqual(m_lastPos[axisId], asxisDatas[axisId][period]))
//                        continue;
//                    m_lastPos[axisId] = asxisDatas[axisId][period];
                    auto axisName = axisActiveOrder[axisId];
                    cncsim::mwAxisNode axis = m_enginePtr->GetNodeByName(axisName.toStdString()).AsNode<cncsim::mwAxisNode>();
                    values.push_back(cncsim::AxisValuePair(axis.GetId(), asxisDatas[axisId][period]));
                }
                if (values.empty())
                    continue;
                ++curPriod;
                // 推入引擎模拟
//                qDebug() << "386+++++periodSetting=" << periodSetting << periodSettingUpdate << curPriod;
                if (curPriod == periodSetting)
                {
                    container.AddMovement(values);
                    m_axisIsActive.lock();
                    m_enginePtr->SetAxes(container);
                    m_axisIsActive.unlock();

                    curPriod = 0;
                    container.clear();
                    QThread::msleep(5);
                }
                cncsim::AxesValues clr;
                clr.reserve(values.size());
                values.swap(clr);
            }
        }
        this->setProperty("axisSportStop", true);
    });
}

void OwnVisualizationSample::stopAndWaitAxisSportThd()
{
    this->setProperty("axisSportStopReq", true);
    // optimistic wait
    while (this->property("axisSportStop").toBool() != true)
    {
        m_notEmptyCond.wakeOne();
        QThread::msleep(100);
    }
}

void OwnVisualizationSample::resetStock()
{
    cncsim::mwStockNode stock = m_enginePtr->GetNodeByName("stock").AsNode<cncsim::mwStockNode>();
    const auto length = 244/2;
    const auto width = 277/2;
    const auto height = 141;
    m_axisIsActive.lock();
    stock.SetClampingRequired(false);
    stock.SetStockCube(cadcam::mw3dBoundingBox<float>{
        cadcam::mwTPoint3d<float>(-length, -width, 0), cadcam::mwTPoint3d<float>(length, width, height)});
    m_axisIsActive.unlock();
}

void OwnVisualizationSample::switchMachine(int machineType)
{
    misc::mwstring pathToMachineXml = QCoreApplication::applicationDirPath().toStdString();
    if (machineType == 0)
    {
        pathToMachineXml += "/Resources/Model/1823/1823.MachineDefines.xml";
    }
    else if (machineType == 1)
    {
        pathToMachineXml += "/Resources/Model/P100/P100.MachineDefines.xml";
    }
    else if (machineType == 2)
    {
        pathToMachineXml += "/Resources/Model/520T/520T.MachineDefines.xml";
    }

    m_enginePtr->BeginEditMode();
    m_enginePtr->InitializeFromXml(pathToMachineXml);

    // add tool
    cncsim::mwMachineDefinitionNode machine = m_enginePtr->GetMachine();
    measures::mwUnitsFactory::Units units = measures::mwUnitsFactory::METRIC;
    cadcam::mwToolPtr endMill(new cadcam::mwEndMill(
        20.0,
        cadcam::mwRevolvedTool::HolderDefinition(units),
        150.0,
        150.0,
        cadcam::mwRevolvedTool::ArborDefinition(units),
        units));
    auto toolNode = machine.FindFirstBelow<cncsim::mwToolNode>(cncsim::SST_EXCLUDE_THIS_NODE);
    toolNode.SetTool(std::make_shared<cncsim::mwSimulationTool>(endMill));

    // add stock
    resetStock();

    m_enginePtr->EndEditMode();

    createAxisSportThd();
}

}  // namespace mwCNCSimVisualizationSamples
