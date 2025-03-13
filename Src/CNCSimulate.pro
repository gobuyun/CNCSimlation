QT += quick quick3d gui

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    FileReader/FileReader.cpp \
    GL/StlTriangleDetect.cpp \
    MachineControl/AxisSport/AxisStepSport.cpp \
    Math/MeshMath/AABox.cpp \
    Math/MeshMath/BoundingBox.cpp \
    Math/MeshMath/OBBox.cpp \
    Math/MotionPlanning/Motor.cpp \
    Math/MotionPlanning/MotorControl.cpp \
    Math/MotionPlanning/PosGenerator.cpp \
    Math/MotionPlanning/SampleJog.cpp \
    Math/MotionPlanning/TrajPlanning.cpp \
    Math/MotionPlanning/Traj_S.cpp \
    Math/MotionPlanning/Traj_T.cpp \
    Math/kdl/frames.cpp \
    Math/kdl/frames_io.cpp \
    Math/kdl/jacobian.cpp \
    Math/kdl/jntarray.cpp \
    Math/kdl/joint.cpp \
    Math/kdl/kinfam_io.cpp \
    Math/kdl/utilities/error_stack.cxx \
    Math/kdl/utilities/svd_HH.cpp \
    Math/kdl/utilities/svd_eigen_HH.cpp \
    Math/kdl/utilities/svd_eigen_Macie.cpp \
    Math/kdl/utilities/utility.cxx \
    Math/kdl/utilities/utility_io.cxx

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#################################################### LIB ################################################################
THIRD_PARTY_LIB_PATH=$$PWD/../Lib
# assimp
LIBS += -L$${THIRD_PARTY_LIB_PATH}/assimp/lib -lassimp-vc143-mt
INCLUDEPATH += $${THIRD_PARTY_LIB_PATH}/assimp/include
DEPENDPATH += $${THIRD_PARTY_LIB_PATH}/assimp/include
# glm
INCLUDEPATH += $${THIRD_PARTY_LIB_PATH}/glm
#stb_image
INCLUDEPATH += $${THIRD_PARTY_LIB_PATH}/stb_image/include
# opengl
LIBS += -lopengl32
# eigen
INCLUDEPATH += $${THIRD_PARTY_LIB_PATH}/eigen3.4

# glu
LIBS += -L$${THIRD_PARTY_LIB_PATH}/freeglut-3.0.0/lib -lfreeglut
INCLUDEPATH += $${THIRD_PARTY_LIB_PATH}/freeglut-3.0.0/include
DEPENDPATH += $${THIRD_PARTY_LIB_PATH}/freeglut-3.0.0/include


#################################################### SOURCE ################################################################
RESOURCES += \
    $$PWD/qml.qrc

HEADERS += \
#    $$PWD/Common/ColorPickerHelper/ColorPickerHelper.h \
    $$PWD/Base/Camera.h \
    $$PWD/Base/Mesh.h \
    $$PWD/Base/Model.h \
    $$PWD/Base/shader.hpp \
    $$PWD/Common/CommonDef.hpp \
#    $$PWD/Common/CustomChart/SChartPlot.h \
#    $$PWD/Common/CustomChart/SSeries.h \
#    $$PWD/Common/LineTypePainter/LineTypePainter.h \
#    $$PWD/Common/RulerPainter/RulerPainter.h \
    $$PWD/Common/Singleton.hpp \
#    $$PWD/Common/StbImage/stb_image.h \
    # $$PWD/MachineRender/CNCSimMachineEntity.hpp \
    # $$PWD/MachineRender/GeometryNodeEntity.hpp \
    # $$PWD/MachineRender/MachineGeometryNodeEntity.hpp \
    # $$PWD/MachineRender/OwnVisualizationSample.hpp \
    # $$PWD/MachineRender/ShaderProgram.hpp \
    # $$PWD/MachineRender/SimpleMeshRenderer.hpp \
    # $$PWD/MachineRender/SimulationToolRenderer.hpp \
    # $$PWD/MachineRender/StandardToolRenderer.hpp \
    # $$PWD/MachineRender/StockNodeEntity.hpp \
    # $$PWD/MachineRender/StockTriangleGroupRenderer.hpp \
    # $$PWD/MachineRender/ToolNodeEntity.hpp \
    # $$PWD/MachineRender/UnicoloredNodeEntity.hpp \
    # $$PWD/MachineRender/UnicoloredShader.hpp \
    # $$PWD/MachineRender/VertexColoredNodeEntity.hpp \
    # $$PWD/MachineRender/VertexColoredShader.hpp \
    # $$PWD/MachineRender/WireToolRenderer.hpp \
    # $$PWD/MachineRender/mwMeshLoader.hpp \
    $$PWD/OpenglItem/OpenglItem.hpp \
    $$PWD/GL/GLCamera.h \
    $$PWD/RenderContent/RenderContent.h \
    Base/BoundingBox.hpp \
    Base/Scene.hpp \
    FileReader/FileReader.h \
    GL/GLTransCommon.h \
    GL/StlTriangleDetect.h \
    MachineControl/AxisSport/AxisStepSport.h \
    MachineControl/Common.hpp \
    MachineControl/MachineControl.hpp \
    # MachineDelegate/MachineDelegate.hpp \
    Math/MeshMath/AABox.h \
    Math/MeshMath/BoundingBox.h \
    Math/MeshMath/OBBox.h \
    Math/MotionCommon/ErrMsg.h \
    Math/MotionCommon/MotionState.h \
    Math/MotionPlanning/MathDataDefine.h \
    Math/MotionPlanning/Motor.h \
    Math/MotionPlanning/MotorControl.h \
    Math/MotionPlanning/PosGenerator.h \
    Math/MotionPlanning/SampleJog.h \
    Math/MotionPlanning/TrajPlanning.h \
    Math/MotionPlanning/Traj_S.h \
    Math/MotionPlanning/Traj_T.h \
    Math/kdl/frames.hpp \
    Math/kdl/frames.inl \
    Math/kdl/frames_io.hpp \
    Math/kdl/jacobian.hpp \
    Math/kdl/jntarray.hpp \
    Math/kdl/joint.hpp \
    Math/kdl/kdl.hpp \
    Math/kdl/kinfam.hpp \
    Math/kdl/kinfam_io.hpp \
    Math/kdl/utilities/error.h \
    Math/kdl/utilities/error_stack.h \
    Math/kdl/utilities/hash_combine.h \
    Math/kdl/utilities/kdl-config.h \
    Math/kdl/utilities/rall1d.h \
    Math/kdl/utilities/rall1d_io.h \
    Math/kdl/utilities/rall2d.h \
    Math/kdl/utilities/rall2d_io.h \
    Math/kdl/utilities/rallNd.h \
    Math/kdl/utilities/scoped_ptr.hpp \
    Math/kdl/utilities/svd_HH.hpp \
    Math/kdl/utilities/svd_eigen_HH.hpp \
    Math/kdl/utilities/svd_eigen_Macie.hpp \
    Math/kdl/utilities/traits.h \
    Math/kdl/utilities/utility.h \
    Math/kdl/utilities/utility_io.h

SOURCES += \
#    $$PWD/Common/ColorPickerHelper/ColorPickerHelper.cpp \
#    $$PWD/Common/CustomChart/SChartPlot.cpp \
#    $$PWD/Common/CustomChart/SSeries.cpp \
#    $$PWD/Common/LineTypePainter/LineTypePainter.cpp \
#    $$PWD/Common/RulerPainter/RulerPainter.cpp \
#    $$PWD/Base/Camera.cpp \
#    $$PWD/Common/StbImage/stb_image.cpp \
    # $$PWD/MachineRender/CNCSimMachineEntity.cpp \
    # $$PWD/MachineRender/GeometryNodeEntity.cpp \
    # $$PWD/MachineRender/MachineGeometryNodeEntity.cpp \
    # $$PWD/MachineRender/OwnVisualizationSample.cpp \
    # $$PWD/MachineRender/ShaderProgram.cpp \
    # $$PWD/MachineRender/SimpleMeshRenderer.cpp \
    # $$PWD/MachineRender/SimulationToolRenderer.cpp \
    # $$PWD/MachineRender/StandardToolRenderer.cpp \
    # $$PWD/MachineRender/StockNodeEntity.cpp \
    # $$PWD/MachineRender/StockTriangleGroupRenderer.cpp \
    # $$PWD/MachineRender/ToolNodeEntity.cpp \
    # $$PWD/MachineRender/UnicoloredNodeEntity.cpp \
    # $$PWD/MachineRender/UnicoloredShader.cpp \
    # $$PWD/MachineRender/VertexColoredNodeEntity.cpp \
    # $$PWD/MachineRender/VertexColoredShader.cpp \
    # $$PWD/MachineRender/WireToolRenderer.cpp \
    # $$PWD/MachineRender/mwMeshLoader.cpp \
    $$PWD/OpenglItem/OpenglItem.cpp \
    $$PWD/RenderContent/RenderContent.cpp \
    $$PWD/GL/GLCamera.cpp \
    $${THIRD_PARTY_LIB_PATH}/stb_image/lib/stb_image.cpp




