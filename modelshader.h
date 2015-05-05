#ifndef MODELSHADER_H
#define MODELSHADER_H

#include <QGLShaderProgram>
#include <QGLBuffer>
#include "topmanager.h"

class ModelShader
{
public:
    ModelShader(TopManager *topmanager);
    ~ModelShader();
    void initializeShaderProgramsAndBuffers(LimitOrNot limitOrNot);
private:
    struct
    {
      bool lighting;
      bool antialiasing;
      bool smooth;
      bool rotation;
      bool controlMesh;
      enum DisplayMode{Vertex, Wire, Face}displayMode;
    }effects;

    QColor vertexColor;
    QColor wireColor;
    QColor faceColor;
    QColor controlMeshColor;
    QColor specialPoint1Color;
    QColor specialPoint2Color;
    QColor specialEdgeColor;

    float pointSize;
    float lineWidth;
    float meshLineWidth;
    float specialEdgeWidth;
    float specialPoint1Size;
    float specialPoint2Size;

    float xRotation;
    float yRotation;
    float xTranslation;
    float yTranslation;
    float zTranslation;
    float xScaling;
    float yScaling;
    float zScaling;

    QMatrix4x4 mMatrix;
    QMatrix3x3 normalMatrix;

    QGLShaderProgram controlMeshProgram;
    QGLShaderProgram modelShaderProgram;
    QGLShaderProgram noLightingShaderProgram;
    QGLShaderProgram specialVerticesShaderProgram1;
    QGLShaderProgram specialVerticesShaderProgram2;
    QGLShaderProgram specialEdgesShaderProgram;

    QList<int> indicates;
    QList<int> controlMeshIndicates;

    QVector<QVector3D> controlMeshVertices;
    QVector<QVector3D> controlMeshNormals;
    int numControlMeshVertices;
    int numModelVertices;
    int numSpecialVertices1;
    int numSpecialVertices2;
    int numSpecialEdges;
    QGLBuffer controlMeshBuffer;
    QGLBuffer modelBuffer;
    QGLBuffer specialVerticesBuffer1;
    QGLBuffer specialVerticesBuffer2;
    QGLBuffer specialEdgesBuffer;

    TopManager *manager;
};

#endif // MODELSHADER_H
