#include "modelshader.h"

ModelShader::ModelShader(TopManager *topmanager)
{
    vertexColor = QColor::fromRgb(154, 117, 167);
    wireColor = QColor(Qt::white);
    faceColor = QColor::fromRgb(167, 154, 205);
    controlMeshColor = QColor::fromRgb(100, 178, 178);
    specialPoint1Color = QColor(Qt::blue);
    specialPoint2Color = QColor(Qt::green);
    specialEdgeColor = QColor(Qt::red);

    pointSize = 1.0f;
    lineWidth = 1.0f;
    meshLineWidth = 2.0f;
    specialEdgeWidth = 3.0f;
    specialPoint1Size = 5.0f;
    specialPoint2Size = 3.0f;

    xRotation = 0.0f;
    yRotation = 0.0f;
    xTranslation = 0.0f;
    yTranslation = 0.0f;
    zTranslation = 0.0f;
    xScaling = 1.0f;
    yScaling = 1.0f;
    zScaling = 1.0f;

    //effects = {true,true,true,false,true, effects.Wire};
    effects.lighting = true;
    effects.antialiasing = true;
    effects.smooth = true;
    effects.rotation = false;
    effects.controlMesh = true;
    effects.displayMode = effects.Wire;

    manager = topmanager;
}
void ModelShader::initializeShaderProgramsAndBuffers(LimitOrNot limitOrNot)
{
    if(!modelShaderProgram.isLinked())
    {
        modelShaderProgram.addShaderFromSourceFile(QGLShader::Vertex,":/GLSL/modelVertexShader.vsh");
        modelShaderProgram.addShaderFromSourceFile(QGLShader::Fragment, ":/GLSL/modelFragmentShader.fsh");
        modelShaderProgram.link();
    }
    if(!noLightingShaderProgram.isLinked())
    {
        noLightingShaderProgram.addShaderFromSourceFile(QGLShader::Vertex, ":/GLSL/specialVertexShader.vsh");
        noLightingShaderProgram.addShaderFromSourceFile(QGLShader::Fragment, ":/GLSL/specialFragmentShader.fsh");
        noLightingShaderProgram.link();
    }
    if(!specialVerticesShaderProgram1.isLinked())
    {
        specialVerticesShaderProgram1.addShaderFromSourceFile(QGLShader::Vertex, ":/GLSL/specialVertexShader.vsh");
        specialVerticesShaderProgram1.addShaderFromSourceFile(QGLShader::Fragment, ":/GLSL/specialFragmentShader.fsh");
        specialVerticesShaderProgram1.link();
    }

    if(!specialVerticesShaderProgram2.isLinked())
    {
        specialVerticesShaderProgram2.addShaderFromSourceFile(QGLShader::Vertex, ":/GLSL/specialVertexShader.vsh");
        specialVerticesShaderProgram2.addShaderFromSourceFile(QGLShader::Fragment, ":/GLSL/specialFragmentShader.fsh");
        specialVerticesShaderProgram2.link();
    }


    if(!specialEdgesShaderProgram.isLinked())
    {
        specialEdgesShaderProgram.addShaderFromSourceFile(QGLShader::Vertex, ":/GLSL/specialVertexShader.vsh");
        specialEdgesShaderProgram.addShaderFromSourceFile(QGLShader::Fragment, ":/GLSL/specialFragmentShader.fsh");
        specialEdgesShaderProgram.link();
    }


    QVector<QVector3D> modelVertices;
    QVector<QVector3D> modelNormals;

    indicates.clear();
    manager->getCurrentModelVerticesAndNormals(&modelVertices, &modelNormals, &indicates, limitOrNot);
    numModelVertices = modelVertices.size();

    if(modelBuffer.isCreated())
    {
        modelBuffer.destroy();
    }
    modelBuffer.create();
    modelBuffer.bind();
    modelBuffer.allocate(modelVertices.size() * ( 3 + 3) * sizeof(GLfloat));
    int offset = 0;
    modelBuffer.write(offset, modelVertices.constData(), modelVertices.size() * 3 * sizeof(GLfloat));
    offset += modelVertices.size() * 3 * sizeof(GLfloat);
    modelBuffer.write(offset, modelNormals.constData(), modelNormals.size() * 3 * sizeof(GLfloat));
    modelBuffer.release();

    QVector<QVector3D> specialVertices1;
    QVector<QVector3D> specialVertices2;
    QVector<QVector3D> specialEdges;

    manager->getCurrentSpecialVerticesAndEdges(&specialVertices1, &specialVertices2, &specialEdges, limitOrNot);
    numSpecialVertices1 = specialVertices1.size();
    numSpecialVertices2 = specialVertices2.size();
    numSpecialEdges = specialEdges.size();

    if(specialVerticesBuffer1.isCreated())
    {
        specialVerticesBuffer1.destroy();
    }
    specialVerticesBuffer1.create();
    specialVerticesBuffer1.bind();
    specialVerticesBuffer1.allocate(numSpecialVertices1 * 3 * sizeof(GLfloat));
    specialVerticesBuffer1.write(0, specialVertices1.constData(), numSpecialVertices1 * 3 * sizeof(GLfloat));
    specialVerticesBuffer1.release();

    if(specialVerticesBuffer2.isCreated())
    {
        specialVerticesBuffer2.destroy();
    }
    specialVerticesBuffer2.create();
    specialVerticesBuffer2.bind();
    specialVerticesBuffer2.allocate(numSpecialVertices2 * 3 * sizeof(GLfloat));
    specialVerticesBuffer2.write(0, specialVertices2.constData(), numSpecialVertices2 * 3 * sizeof(GLfloat));
    specialVerticesBuffer2.release();

    if(specialEdgesBuffer.isCreated())
    {
        specialEdgesBuffer.destroy();
    }
    specialEdgesBuffer.create();
    specialEdgesBuffer.bind();
    specialEdgesBuffer.allocate(numSpecialEdges * 3 * sizeof(GLfloat));
    specialEdgesBuffer.write(0, specialEdges.constData(), numSpecialEdges * 3 * sizeof(GLfloat));
    specialEdgesBuffer.release();
}
