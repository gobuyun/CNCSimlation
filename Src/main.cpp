#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QtQuick3D/QQuick3D>

#include "Front.hpp"
#include "MachineControl/MachineControl.hpp"
#include "FileReader/FileReader.h"
// #include "MachineDelegate/MachineDelegate.hpp"

//#define TSTE_MODE

#ifdef TSTE_MODE
    #include <Qdebug.h>
    #include "GL/GLCamera.h"
    #include "GL/StlTriangleDetect.h"
    #include "Math/MotionPlanning/MotorControl.h"
    #include "Math/MeshMath/AABox.h"
    #include "Math/MeshMath/OBBox.h"

#endif

int main(int argc, char *argv[])
{
#ifdef TSTE_MODE

    KDL::Vector p1(1,2,3);
    KDL::Vector p2(2,3,4);
    KDL::Vector p3(3,4,5);

    std::vector<KDL::Vector> p_set;
    p_set.push_back(p1);
    p_set.push_back(p2);
    p_set.push_back(p3);

    GMath::BoundingBox::Ptr bb = std::make_shared<GMath::OBBox>(p_set);
    //GMath::BoundingBox::Ptr bb = std::make_shared<GMath::AABox>(p_set);
    // std::cout<<bb->GetCenter()<<std::endl;
    std::cout<<"inside"<<bb->IsPointInside(KDL::Vector(0,0,0))<<std::endl;
    std::cout<<"inside"<<bb->IsPointInside(KDL::Vector(3,4,5))<<std::endl;

    KDL::Vector p12(2,2,2);
    KDL::Vector p22(4,4,4);
    KDL::Vector p23(5,5,5);
    std::vector<KDL::Vector> p_set2;
    p_set2.push_back(p12);
    p_set2.push_back(p22);
    p_set2.push_back(p23);
    GMath::BoundingBox::Ptr bb2 = std::make_shared<GMath::OBBox>(p_set2);
    //GMath::BoundingBox::Ptr bb2 = std::make_shared<GMath::AABox>(p_set2);
    std::cout<<"interset="<<bb->IsIntersects(bb2)<<std::endl;
    //测试stl三角形碰撞

    GMath::StlTriangleDetect stl_test;
    //传入三角形顶点和法向量提供了一个gl类型的转换函数 KDL::Vector KV2GlV(glm::vec3 in)
    KDL::Vector a1 = KDL::Vector(3,2,2);
    KDL::Vector b1 = KDL::Vector(14,6,5);
    KDL::Vector c1 = KDL::Vector(4,1,6);
    KDL::Vector n1 = (a1-b1)*(a1-c1);
    KDL::Vector a2 = KDL::Vector(3,2,4);
    KDL::Vector b2 = KDL::Vector(14,6,5);
    KDL::Vector c2 = KDL::Vector(4,1,1);
    KDL::Vector n2 = (a2-b2)*(a2-c2);
    GMath::TriangleVertex tr1 = GMath::TriangleVertex(a1,b1,c1,n1);
    GMath::TriangleVertex tr2 = GMath::TriangleVertex(a2,b2,c2,n2);

    stl_test.SetAttackTriangle(tr1);

    stl_test.SetDefenseTriangle(tr2);

    std::cout<<stl_test.Detect()<<std::endl;

    while(1);
    //位置再0,0,3,与z轴45度夹角的姿态，相机坐标系需要绕X旋转45度才是与Z夹角45°
    GLCamera test_ca(KDL::Vector(0,0,3),KDL::Vector(45,0,0));
    test_ca.MoveX(1,false);//增量位移x方向
     test_ca.MoveY(2,false);//增量位移Y方向
     test_ca.MoveZ(3);//绝对位移Z去到位置（x,y,3）
     test_ca.RotX(1,false);//增量旋转绕世界坐标轴X 1°
          std::cout<<KDL::Rotation::RPY(30*KDL::deg2rad,0,0)<<std::endl;


    qDebug()<<"in test mode";

    while (1);
#endif
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
    QSurfaceFormat::setDefaultFormat(QQuick3D::idealSurfaceFormat());
#endif

    Front::registerModule();
    Tool::registerModule();
    MachineControl::prepare();

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/Script/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
        // 机床控制模块运行
        MachineControl::start();
    }, Qt::QueuedConnection);
    engine.load(url);
    QObject::connect(&app,
                     &QGuiApplication::aboutToQuit,
                     [](){MachineControl::quit();});

    return app.exec();
}
