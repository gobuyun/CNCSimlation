// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#include "UnicoloredShader.hpp"

namespace mwCNCSimVisualizationSamples
{
const char* UnicoloredShader::s_vertexShaderText =
    "#version 100\n"
    "precision mediump float;\n"
    "uniform highp float uNormalScale;\n"
    "uniform highp float uAmbientIntensity;\n"
    "uniform highp mat4 uModelViewMatrix;\n"
    "uniform highp mat4 uNormalModelViewMatrix;\n"
    "uniform highp mat4 uProjectionMatrix;\n"
    "uniform highp vec3 uEntityColor;\n"
    "uniform highp vec3 uOverrideColor;\n"
    "uniform int uEnableOverrideColor;\n"
    "\n"
    "attribute vec3 aPos;\n"
    "attribute vec3 aNormal;\n"
    "\n"
    "varying vec3 vColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   vec3 useColor = uEntityColor;"
    "   if (uEnableOverrideColor == 1) useColor = uOverrideColor;\n"
    "   vec4 eyeSpacePosition = uModelViewMatrix * vec4(aPos,1);\n"
    "   vec3 eyeSpaceNormal = uNormalScale * normalize((uNormalModelViewMatrix * "
    "vec4(aNormal,0)).xyz);\n"
    "   gl_Position = uProjectionMatrix * eyeSpacePosition;\n"
    "   \n"
    "   // Assuming that ambient light is white\n"
    "   vec3 ambient = uAmbientIntensity * useColor;\n"
    "   \n"
    "   // Assuming that light is white and at camera position\n"
    "   // Also use full ambient reflectivity\n"
    "   vec3 diffuse = useColor * abs(dot(eyeSpaceNormal, vec3(0,0,1)));\n"
    "   \n"
    "   vColor = ambient + diffuse;\n"
    "}";
const char* UnicoloredShader::s_fragmentShaderText =
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec3 vColor;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = vec4(vColor, 1.0);\n"
    "}\n";

UnicoloredShader::UnicoloredShader() : ShaderProgram(s_vertexShaderText, s_fragmentShaderText)
{
    // Store attribute positions
    m_shaderLocations.AmbientIntensityLocation =
        glGetUniformLocation(m_shaderProgram, "uAmbientIntensity");
    m_shaderLocations.MVMatrixLocation = glGetUniformLocation(m_shaderProgram, "uModelViewMatrix");
    m_shaderLocations.NormalMVMatrixLocation =
        glGetUniformLocation(m_shaderProgram, "uNormalModelViewMatrix");
    m_shaderLocations.ProjectionMatrixLocation =
        glGetUniformLocation(m_shaderProgram, "uProjectionMatrix");
    m_shaderLocations.EntityColorLocation = glGetUniformLocation(m_shaderProgram, "uEntityColor");
    m_shaderLocations.OverrideColor = glGetUniformLocation(m_shaderProgram, "uOverrideColor");
    m_shaderLocations.EnableOverrideColor = glGetUniformLocation(m_shaderProgram, "uEnableOverrideColor");
    m_shaderLocations.NormalScale = glGetUniformLocation(m_shaderProgram, "uNormalScale");
    m_shaderLocations.VertexPosLocation = glGetAttribLocation(m_shaderProgram, "aPos");
    m_shaderLocations.VertexNormalLocation = glGetAttribLocation(m_shaderProgram, "aNormal");
}

const UnicoloredShader::UnicoloredShaderLocations& UnicoloredShader::GetShaderLocations() const
{
    return m_shaderLocations;
}

void UnicoloredShader::Activate() const
{
    ShaderProgram::Activate();
    (const_cast<UnicoloredShader*>(this))->glEnableVertexAttribArray(m_shaderLocations.VertexPosLocation);
    (const_cast<UnicoloredShader*>(this))->glEnableVertexAttribArray(m_shaderLocations.VertexNormalLocation);
}

void UnicoloredShader::Deactivate() const
{
    ShaderProgram::Deactivate();
    (const_cast<UnicoloredShader*>(this))->glDisableVertexAttribArray(m_shaderLocations.VertexPosLocation);
    (const_cast<UnicoloredShader*>(this))->glDisableVertexAttribArray(m_shaderLocations.VertexNormalLocation);
}








//const char* UnicoloredShader::s_vertexShaderText =
//    "#version 100\n"
//    "precision mediump float;\n"
//    "attribute vec3 aPos;\n"
//    "void main()\n"
//    "{\n"
//    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
//    "}";
//const char* UnicoloredShader::s_fragmentShaderText =
//    "#version 100\n"
//    "precision mediump float;\n"
//    "void main()\n"
//    "{\n"
//    "    gl_FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);\n"
//    "}\n";


//UnicoloredShader::UnicoloredShader() : ShaderProgram(s_vertexShaderText, s_fragmentShaderText)
//{
//    // Store attribute positions
//    m_shaderLocations.VertexPosLocation =
//        glGetAttribLocation(m_shaderProgram, "aPos");
//}

//const UnicoloredShader::UnicoloredShaderLocations& UnicoloredShader::GetShaderLocations() const
//{
//    return m_shaderLocations;
//}

//void UnicoloredShader::Activate() const
//{
//    ShaderProgram::Activate();
//    (const_cast<UnicoloredShader*>(this))->glEnableVertexAttribArray(m_shaderLocations.VertexPosLocation);
////    (const_cast<UnicoloredShader*>(this))->glEnableVertexAttribArray(m_shaderLocations.VertexNormalLocation);
//}

//void UnicoloredShader::Deactivate() const
//{
//    ShaderProgram::Deactivate();
//    (const_cast<UnicoloredShader*>(this))->glDisableVertexAttribArray(m_shaderLocations.VertexPosLocation);
////    (const_cast<UnicoloredShader*>(this))->glDisableVertexAttribArray(m_shaderLocations.VertexNormalLocation);
//}

}  // namespace mwCNCSimVisualizationSamples
