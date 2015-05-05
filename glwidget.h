#ifndef GLWIDGET_H
#define GLWIDGET_H
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QMouseEvent>
#include <QWheelEvent>

#include "topmanager.h"

struct LightProperties {
    bool isEnabled;
    bool isLocal;
    bool isSopt;
    QVector3D ambient;
    QVector3D color;
    QVector3D position;
    //QVector3D halfVector;
    QVector3D coneDirection;
    float spotCosCutoff;
    float spotExponent;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(TopManager *topmanager);
    ~GLWidget();
    void initializeShaderProgramsAndBuffers(LimitOrNot limitOrNot);
    void setOrder(int k);
    void setTensionParameter(double u);
    void updateTensionParameter();
    int getOrder();
    double getTensionParameter();
    CheckStatus *checkStatus;
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *);

    void renderInMultipleLights();
    void renderInSingleLight();

private:
    //void renderSphere();

    bool lighting;
    bool antialiasing;
    bool smooth;
    bool rotation;
    bool controlMesh;
    bool disVertex;
    bool disWire;
    bool disFace;
    bool directionalLight;
    bool pointLight;
    bool spotLight;
    bool multipleLights;

    QColor backColor;
    QColor meshColor;
    QColor edgeColor;
    QColor controlMeshColor;

    float xRotation;
    float yRotation;
   // float zRotation;
    float xTranslation;
    float yTranslation;
    float zTranslation;
    float xScaling;
    float yScaling;
    float zScaling;

    double alpha;
    double beta;
    double distance;
    QPoint lastMousePosition;

    QVector3D centroid;
    QVector3D transed;
    QMatrix4x4 pMatrix;
    QMatrix4x4 mMatrix;
    QOpenGLShaderProgram controlMeshProgram;
    QOpenGLShaderProgram modelShaderProgram;
    QOpenGLShaderProgram noLightingShaderProgram;
    QOpenGLShaderProgram specialVerticesShaderProgram1;
    QOpenGLShaderProgram specialVerticesShaderProgram2;
    QOpenGLShaderProgram specialEdgesShaderProgram;

    QList<int> indicates;
    QList<int> controlMeshIndicates;

    QVector<QVector3D> controlMeshVertices;
    QVector<QVector3D> controlMeshNormals;
    int numControlMeshVertices;
    int numModelVertices;
    int numSpecialVertices1;
    int numSpecialVertices2;
    int numSpecialEdges;
    QOpenGLBuffer controlMeshBuffer;
    QOpenGLBuffer modelBuffer;
    QOpenGLBuffer specialVerticesBuffer1;
    QOpenGLBuffer specialVerticesBuffer2;
    QOpenGLBuffer specialEdgesBuffer;
    double lightAngle;

    //int id;
    TopManager *manager;
    struct LightProperties lights[3];

    int k;
    double u;

    void initializeStatus();
    void initializeLights();

    void oldGLShaderProgram();
    void modernGLShaderProgram();

//private slots:
//    void timeout(void);

};

#endif // GLWIDGET_H
