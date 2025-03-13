THIRD_PARTY_LIB_PATH=$$PWD/../../Lib
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

RESOURCES += \
    $$PWD/qml.qrc


INCLUDEPATH += \
    $$PWD \
    $$PWD/..


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
    $$PWD/Common/StbImage/stb_image.h \
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
#    $$PWD/../GL/GLCamera.h \
    $$PWD/RenderContent/RenderContent.h

SOURCES += \
#    $$PWD/Common/ColorPickerHelper/ColorPickerHelper.cpp \
#    $$PWD/Common/CustomChart/SChartPlot.cpp \
#    $$PWD/Common/CustomChart/SSeries.cpp \
#    $$PWD/Common/LineTypePainter/LineTypePainter.cpp \
#    $$PWD/Common/RulerPainter/RulerPainter.cpp \
    $$PWD/Base/Camera.cpp \
    $$PWD/Common/StbImage/stb_image.cpp \
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
#    $$PWD/../GL/GLCamera.cpp \
    $${THIRD_PARTY_LIB_PATH}/stb_image/lib/stb_image.cpp
