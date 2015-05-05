#include "glwidget.h"
#include <math.h>

#ifdef WIN32
    #include <glext.h>
    PFNGLACTIVETEXTUREPROC pGlActiveTexture = NULL;
    #define glActiveTexture pGlActiveTexture
#endif //WIN32


GLWidget::GLWidget( TopManager *topmanager)
    : QOpenGLWidget(
         /* QGLFormat(
              QOpenGL::AlphaChannel |
              QGL::DoubleBuffer |
              QGL::DepthBuffer |
              QGL::Rgba |
              QGL::SampleBuffers |
              QGL::StencilBuffer |
              QGL::StereoBuffers
              )*/)
{
    manager = topmanager;
    //id = ind;

    //backColor = QColor::fromRgbF(0.0f, 0.0f, 0.0f);
    //backColor = QColor::fromRgbF(1.0f, 1.0f, 1.0f);
    //backColor = QColor::fromRgbF(0.6784313725490196, 0.6039215686274509, 0.5607843137254902);
    backColor = QColor::fromRgb(192, 179, 164);
    meshColor = QColor::fromRgb(167, 154, 200);
    //meshColor = QColor::fromRgb(124, 102, 83);
    //meshColor = QColor::fromRgb(127, 127, 127);
    //edgeColor = QColor(Qt::white);
   //edgeColor = QColor::fromRgb(91, 83, 75);
    //edgeColor = QColor::fromRgb(234, 222, 210);
    edgeColor = QColor(Qt::black);
    controlMeshColor = QColor::fromRgb(100, 178, 178);

    xRotation = 0.0f;
    yRotation = 0.0f;
    xTranslation = 0.0f;
    yTranslation = 0.0f;
    zTranslation = 0.0f;
    xScaling = 1.0f;
    yScaling = 1.0f;
    zScaling = 1.0f;

    alpha = 25;
    beta = -25;
    distance = 25;

    lightAngle = 0;

    checkStatus = new CheckStatus;
    initializeStatus();
    if(MultipleLights)
    {
        initializeLights();
    }


//    mMatrix.setToIdentity();
//    transed.setX(0);
//    transed.setY(0);
//    transed.setZ(0);
//    centroid = manager->getCurrentTopology()->getCentroid();
    k = 0;
}
GLWidget::~GLWidget()
{
    delete checkStatus;
}

void GLWidget::initializeLights() {
    //directional light
    lights[0].isEnabled = false;
    lights[0].isLocal = false;
    lights[0].isSopt = false;
    lights[0].ambient = QVector3D(1.0, 1.0, 1.0);
    lights[0].color = QVector3D(1.0, 1.0, 1.0);//QVector3D(&QColor(Qt::white));
    lights[0].position = QVector3D(10.0, 10.0, 10.0);
    lights[0].coneDirection = QVector3D(-0.0, -0.0, -0.0);
    lights[0].spotCosCutoff = 0.0;
    lights[0].spotExponent = 1.0;
    lights[0].constantAttenuation = 1.0;
    lights[0].linearAttenuation = 1.0;
    lights[0].quadraticAttenuation = 1.0;

    //Point light
    lights[1].isEnabled = false;
    lights[1].isLocal = true;
    lights[1].isSopt = false;
    lights[1].ambient = QVector3D(1.0, 1.0, 1.0);
    lights[1].color = QVector3D(1.0, 1.0, 1.0);
    lights[1].position = QVector3D(10.0, 10.0, 10.0);
    lights[1].coneDirection = QVector3D(0.0, 0.0, 0.0);
    lights[1].spotCosCutoff = 0.0;
    lights[1].spotExponent = 1.0;
    lights[1].constantAttenuation = 1.0;
    lights[1].linearAttenuation = 1.0;
    lights[1].quadraticAttenuation = 0.5;

    //spot light
    lights[2].isEnabled = false;
    lights[2].isLocal = true;
    lights[2].isSopt = true;
    lights[2].ambient = QVector3D(1.0, 1.0, 1.0);
    lights[2].color = QVector3D(1.0, 1.0, 1.0);
    lights[2].position = QVector3D(0, 15, 0);
    lights[2].coneDirection = QVector3D(0, -15, 0);
    lights[2].spotCosCutoff = 0.98; //around 0.99
    lights[2].spotExponent = 0.99;
    lights[2].constantAttenuation = 2.0;
    lights[2].linearAttenuation = 1.0;
    lights[2].quadraticAttenuation = 0.5;

}

void GLWidget::initializeStatus()
{

    checkStatus->ANTIALIALIASING_CHECKED = true;
    checkStatus->LIGHTING_CHECKED = true;
    checkStatus->SHADING_CHECKED = false;
    checkStatus->ROTATE_CHECKED = false;
    checkStatus->CAT = false;
    checkStatus->CAT_LIMIT = false;
    checkStatus->CAT_LIMIT_CHECKED = false;
    checkStatus->DOO = false;
    checkStatus->LOOP = false;
    checkStatus->LOOP_LIMIT = false;
    checkStatus->LOOP_LIMIT_CHECKED = false;
    checkStatus->PREVIOUS = false;
    checkStatus->NEXT = false;
    checkStatus->VERTEX_CHECKED = false;
    checkStatus->WIREFRAME_CHECKED = true;
    checkStatus->FACE_CHECKED = false;
    checkStatus->CONTROL_CHECKED = false;
    checkStatus->SharpFeature_CHECKED = false;
    checkStatus->limitOrNot = NORLIMIT;
    checkStatus->GENERAL = true;
    checkStatus->DirectionalLight_Checked = true;
    checkStatus->PointLight_Checked = false;
    checkStatus->SpotLight_Checked = false;
    checkStatus->MultipleLights_Checked = false;
}

void GLWidget::setOrder(int k)
{
    this->k = k;
}
void GLWidget::setTensionParameter(double u)
{
    this->u = u;
}
void GLWidget::updateTensionParameter()
{
    u = sqrt((1 + u) / 2);
}
int GLWidget::getOrder()
{
    return k;
}
double GLWidget::getTensionParameter()
{
    return u;
}
void GLWidget::modernGLShaderProgram()
{
    if(MultipleLights)
    {
        if(!modelShaderProgram.isLinked())
        {
            modelShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,":/GLSL/multipleLights330.vert");
            modelShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/GLSL/multipleLights330.frag");
            modelShaderProgram.link();
        }

    }
    else
    {
        if(!modelShaderProgram.isLinked())
        {
            modelShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,":/GLSL/modelVertexShader330.vert");
            modelShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/GLSL/modelFragmentShader330.frag");
            modelShaderProgram.link();
        }

    }
    if(!noLightingShaderProgram.isLinked())
    {
        noLightingShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/GLSL/specialVertexShader330.vert");
        noLightingShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/GLSL/specialFragmentShader330.frag");
        noLightingShaderProgram.link();
    }
    if(!specialVerticesShaderProgram1.isLinked())
    {
        specialVerticesShaderProgram1.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/GLSL/specialVertexShader330.vert");
        specialVerticesShaderProgram1.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/GLSL/specialFragmentShader330.frag");
        specialVerticesShaderProgram1.link();
    }

    if(!specialVerticesShaderProgram2.isLinked())
    {
        specialVerticesShaderProgram2.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/GLSL/specialVertexShader330.vert");
        specialVerticesShaderProgram2.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/GLSL/specialFragmentShader330.frag");
        specialVerticesShaderProgram2.link();
    }


    if(!specialEdgesShaderProgram.isLinked())
    {
        specialEdgesShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/GLSL/specialVertexShader330.vert");
        specialEdgesShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/GLSL/specialFragmentShader330.frag");
        specialEdgesShaderProgram.link();
    }
}

void GLWidget::oldGLShaderProgram()
{
    if(MultipleLights)
    {
        if(!modelShaderProgram.isLinked())
        {
            modelShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,":/GLSL/multipleLights.vert");
            modelShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/GLSL/multipleLights.frag");
            modelShaderProgram.link();
        }

    }
    else
    {
        if(!modelShaderProgram.isLinked())
        {
            modelShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,":/GLSL/modelVertexShader.vert");
            modelShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/GLSL/modelFragmentShader.frag");
            modelShaderProgram.link();
        }

    }
    if(!noLightingShaderProgram.isLinked())
    {
        noLightingShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/GLSL/specialVertexShader.vert");
        noLightingShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/GLSL/specialFragmentShader.frag");
        noLightingShaderProgram.link();
    }
    if(!specialVerticesShaderProgram1.isLinked())
    {
        specialVerticesShaderProgram1.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/GLSL/specialVertexShader.vert");
        specialVerticesShaderProgram1.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/GLSL/specialFragmentShader.frag");
        specialVerticesShaderProgram1.link();
    }

    if(!specialVerticesShaderProgram2.isLinked())
    {
        specialVerticesShaderProgram2.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/GLSL/specialVertexShader.vert");
        specialVerticesShaderProgram2.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/GLSL/specialFragmentShader.frag");
        specialVerticesShaderProgram2.link();
    }


    if(!specialEdgesShaderProgram.isLinked())
    {
        specialEdgesShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/GLSL/specialVertexShader.vert");
        specialEdgesShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/GLSL/specialFragmentShader.frag");
        specialEdgesShaderProgram.link();
    }
}

void GLWidget::initializeShaderProgramsAndBuffers(LimitOrNot limitOrNot)
{
    if(OldGLSLVersion)
    {
        oldGLShaderProgram();
    }
    else
    {
        modernGLShaderProgram();
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

void GLWidget::initializeGL()
{
    #ifdef WIN32
        glActiveTexture = (PFNGLACTIVETEXTUREPROC) wglGetProcAddress((LPCSTR) "glActiveTexture");
    #endif


    //this is very important!
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //glClearColor(backColor);
    glClearColor(backColor.redF(), backColor.greenF(), backColor.blueF(), backColor.alpha());

    //
    if(OldGLSLVersion)
    {
        controlMeshProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/GLSL/specialVertexShader.vert");
        controlMeshProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/GLSL/specialFragmentShader.frag");
    }
    else
    {
        controlMeshProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/GLSL/specialVertexShader330.vert");
        controlMeshProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/GLSL/specialFragmentShader330.frag");

    }

    controlMeshProgram.link();

    manager->getOriginalModelVerticesAndNormals(&controlMeshVertices, &controlMeshNormals, &controlMeshIndicates);
    numControlMeshVertices = controlMeshVertices.size();

    controlMeshBuffer.create();
    controlMeshBuffer.bind();
    controlMeshBuffer.allocate(numControlMeshVertices * (3 + 3) * sizeof(GLfloat));
    int offset = 0;
    controlMeshBuffer.write(offset, controlMeshVertices.constData(), numControlMeshVertices * 3 * sizeof(GLfloat));
    offset += numControlMeshVertices * 3 * sizeof(GLfloat);
    controlMeshBuffer.write(offset, controlMeshNormals.constData(), numControlMeshVertices * 3 * sizeof(GLfloat));
    controlMeshBuffer.release();

    this->initializeShaderProgramsAndBuffers(NORLIMIT);
/*
    //default mode
    glPolygonMode(GL_FRONT, GL_LINE);
    glPolygonMode(GL_BACK, GL_LINE);
    glShadeModel(GL_FLAT);
    glEnable(GL_NORMALIZE);

    //lights, material properties
    GLfloat lightAmbient[] = {0.0f, 0.7f, 0.7f, 1.0f};
    GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightPosition[] = {0.1, 0.2, 0.5, 0.0};

    glClearDepth(1.0f);

    glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    GLfloat	ambient[]  = {0.0f,0.0f,0.0f,1.0f};
    GLfloat	diffuse[]  = {0.0f,0.0f,0.0f,1.0f};
    GLfloat	specular[]  = {0.0f,0.0f,0.0f,1.0f};
    GLfloat	emission[]  = {0.3f,0.3f,0.3f,1.0f};
    GLfloat	shininess[] = {0.0f};

    // Ambient
    ambient[0] = 0.24725f;
    ambient[1] = 0.2245f;
    ambient[2] = 0.0645f;
    ambient[3] = 1.0f;
    // Diffuse
    diffuse[0] = 0.34615f;
    diffuse[1] = 0.3143f;
    diffuse[2] = 0.0903f;
    diffuse[3] = 1.0f;
    // Specular
    specular[0] = 0.797357f;
    specular[1] = 0.723991f;
    specular[2] = 0.208006f;
    specular[3] = 1.0f;
    // Shininess
    shininess[0] = 83.2f;

    glMaterialfv( GL_FRONT, GL_AMBIENT,   ambient);
    glMaterialfv( GL_FRONT, GL_DIFFUSE,   diffuse);
    glMaterialfv( GL_FRONT, GL_SPECULAR,  specular);
    glMaterialfv( GL_FRONT, GL_SHININESS, shininess);
    glMaterialfv( GL_FRONT, GL_EMISSION,  emission);

    GLfloat	back[]  = {0.0f,0.25f,0.0f,1.0f};
    glMaterialfv( GL_BACK, GL_AMBIENT, back);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.5f);
    */

}
void GLWidget::paintGL()
{
    if(MultipleLights)
    {
        renderInMultipleLights();
    }
    else
    {
        renderInSingleLight();
    }
}

void GLWidget::renderInMultipleLights()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //draw model
    //set up mMatrix;

    //setup vMatrix;
    QMatrix4x4 mMatrix;
    QMatrix4x4 vMatrix;

    QMatrix4x4 cameraTransformation;
//    cameraTransformation.translate(transed);
    cameraTransformation.rotate(alpha, 0, 1, 0);
    cameraTransformation.rotate(beta, 1, 0, 0);

    QVector3D cameraPosition = cameraTransformation * QVector3D(0, 0, distance);
    //cameraTransformation.translate(transed);
    QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);
    vMatrix.lookAt(cameraPosition, QVector3D(0, 0, 0), cameraUpDirection);

    mMatrix.setToIdentity();
    QMatrix4x4 mvMatrix;

    mvMatrix = vMatrix * mMatrix;
    QMatrix3x3 normalMatrix;
    normalMatrix = mvMatrix.normalMatrix();
    glLineWidth(1.5f);
    QVector3D lightPosition = QVector3D(0, 200, 200);
    //lightPosition.
    if(checkStatus->CONTROL_CHECKED)
    {
        controlMeshProgram.bind();
        controlMeshProgram.setUniformValue("mvpMatrix", pMatrix * mvMatrix);
        //controlMeshProgram.setUniformValue("mvMatrix", mvMatrix);
        //controlMeshProgram.setUniformValue("normalMatrix", normalMatrix);
       // controlMeshProgram.setUniformValue("lightPosition", lightPosition);

        controlMeshProgram.setUniformValue("color", this->controlMeshColor/*QColor::fromRgb(100, 178, 178)*/);
        controlMeshProgram.setUniformValue("pointSize", 3);
//        controlMeshProgram.setUniformValue("ambientColor", QColor(32, 32, 32));
//        controlMeshProgram.setUniformValue("diffuseColor", QColor(128, 128, 128));
//        controlMeshProgram.setUniformValue("specularColor", QColor(255, 255, 255));

//        controlMeshProgram.setUniformValue("ambientReflection", (GLfloat) 1.0);
//        controlMeshProgram.setUniformValue("diffuseReflection", (GLfloat) 1.0);
//        controlMeshProgram.setUniformValue("specularReflection", (GLfloat) 1.0);
//        controlMeshProgram.setUniformValue("shininess", (GLfloat) 200.0);

        controlMeshBuffer.bind();
        int offset = 0;
        controlMeshProgram.setAttributeBuffer("vertex", GL_FLOAT, offset, 3, 0);
        controlMeshProgram.enableAttributeArray("vertex");
//        offset += numControlMeshVertices * 3 * sizeof(GLfloat);
//        controlMeshProgram.setAttributeBuffer("normal", GL_FLOAT, offset, 3, 0);
//        controlMeshProgram.enableAttributeArray("normal");
        controlMeshBuffer.release();

        offset = 0;
        glLineWidth(2.0f);
        for(int i = 0; i < controlMeshIndicates.size(); i++)
        {
            glDrawArrays(GL_LINE_LOOP, offset, controlMeshIndicates[i]);
            offset += controlMeshIndicates[i];
        }
        glLineWidth(1.5f);
        controlMeshProgram.disableAttributeArray("vertex");
        //controlMeshProgram.disableAttributeArray("normal");
        controlMeshProgram.release();
    }
    if(checkStatus->LIGHTING_CHECKED)
    {


        this->lights[0].isEnabled = this->checkStatus->DirectionalLight_Checked;
        this->lights[1].isEnabled = this->checkStatus->PointLight_Checked;
        this->lights[2].isEnabled = this->checkStatus->SpotLight_Checked;
        //printf("fuck %d, %d, %d", lights[0].isEnabled, lights[1].isEnabled, lights[2].isEnabled);

        modelShaderProgram.bind();

        //set lights
        int location = modelShaderProgram.uniformLocation("Lights");
        printf("location: %d", location);
        for(int lightIndex = 0; lightIndex < 3; lightIndex++)
        {
            modelShaderProgram.setUniformValue(location + lightIndex*12 + 0, this->lights[lightIndex].isEnabled);
            modelShaderProgram.setUniformValue(location + lightIndex*12 + 1, this->lights[lightIndex].isLocal);
            modelShaderProgram.setUniformValue(location + lightIndex*12 + 2, this->lights[lightIndex].isSopt);
            modelShaderProgram.setUniformValue(location + lightIndex*12 + 3, this->lights[lightIndex].ambient);
            modelShaderProgram.setUniformValue(location + lightIndex*12 + 4, this->lights[lightIndex].color);
            modelShaderProgram.setUniformValue(location + lightIndex*12 + 5, this->lights[lightIndex].position);
            modelShaderProgram.setUniformValue(location + lightIndex*12 + 6, this->lights[lightIndex].coneDirection);
            modelShaderProgram.setUniformValue(location + lightIndex*12 + 7, this->lights[lightIndex].spotCosCutoff);
            modelShaderProgram.setUniformValue(location + lightIndex*12 + 8, this->lights[lightIndex].spotExponent);
            modelShaderProgram.setUniformValue(location + lightIndex*12 + 9, this->lights[lightIndex].constantAttenuation);
            modelShaderProgram.setUniformValue(location + lightIndex*12 + 10, this->lights[lightIndex].linearAttenuation);
            modelShaderProgram.setUniformValue(location + lightIndex*12 + 11, this->lights[lightIndex].quadraticAttenuation);
        }


        modelShaderProgram.setUniformValue("mvpMatrix", pMatrix * mvMatrix);
        modelShaderProgram.setUniformValue("mvMatrix", mvMatrix);
        modelShaderProgram.setUniformValue("normalMatrix", normalMatrix);
        //modelShaderProgram.setUniformValue("lightPosition", lightPosition);
        modelShaderProgram.setUniformValue("viewerPosition", cameraPosition);


        modelShaderProgram.setUniformValue("Shininess", (GLfloat) 20.0);
        modelShaderProgram.setUniformValue("Strength", (GLfloat) 10.0);

        if(checkStatus->FACE_CHECKED)
            modelShaderProgram.setUniformValue("VertexColor", this->meshColor/*QColor::fromRgb(167, 154, 205)*/);
        else if(checkStatus->WIREFRAME_CHECKED)
            modelShaderProgram.setUniformValue("VertexColor",this->edgeColor);
        else
            modelShaderProgram.setUniformValue("VertexColor", QColor::fromRgb(154, 117, 167));
        modelBuffer.bind();
        int offset = 0;
        modelShaderProgram.setAttributeBuffer("VertexPosition", GL_FLOAT, offset, 3, 0);
        modelShaderProgram.enableAttributeArray("VertexPosition");
        offset += numModelVertices * 3 *sizeof(GLfloat);
        modelShaderProgram.setAttributeBuffer("Normal", GL_FLOAT, offset, 3, 0);
        modelShaderProgram.enableAttributeArray("Normal");
        modelBuffer.release();


        //glEnable(GL_LINE);
        if(checkStatus->WIREFRAME_CHECKED)
        {
            offset = 0;
            for(int i = 0; i < indicates.size(); i++)
            {
                glDrawArrays(GL_LINE_LOOP, offset, indicates[i]);
                offset += indicates[i];
            }
        }
        else if(checkStatus->FACE_CHECKED)
        {
            offset = 0;
            for(int i = 0; i < indicates.size(); i++)
            {
                glDrawArrays(GL_TRIANGLE_FAN, offset, indicates[i]);
                offset += indicates[i];
            }
        }
        else if(checkStatus->VERTEX_CHECKED)
        {
            //glPointSize(6.5f);//noEffect
            offset = 0;
            for(int i = 0; i < indicates.size(); i++)
            {
                glDrawArrays(GL_POINTS, offset, indicates[i]);
                offset += indicates[i];
            }
            //glPointSize(1.0f);
        }

        modelShaderProgram.disableAttributeArray("VertexPosition");
        modelShaderProgram.disableAttributeArray("Normal");
        modelShaderProgram.release();
    }
    else
    {
        noLightingShaderProgram.bind();
        noLightingShaderProgram.setUniformValue("mvpMatrix", pMatrix * mvMatrix);
        noLightingShaderProgram.setUniformValue("pointSize", 2.0f);
        if(checkStatus->FACE_CHECKED)
            noLightingShaderProgram.setUniformValue("color", this->meshColor/*QColor::fromRgb(167, 154, 205)*/);
        else if(checkStatus->WIREFRAME_CHECKED)
            noLightingShaderProgram.setUniformValue("color", this->edgeColor);
        else
            noLightingShaderProgram.setUniformValue("color", QColor::fromRgb(154, 117, 167));
        modelBuffer.bind();
        int offset = 0;
        noLightingShaderProgram.setAttributeBuffer("vertex", GL_FLOAT, offset, 3, 0);
        noLightingShaderProgram.enableAttributeArray("vertex");
        modelBuffer.release();


        //glEnable(GL_LINE);
        if(checkStatus->WIREFRAME_CHECKED)
        {
            offset = 0;
            for(int i = 0; i < indicates.size(); i++)
            {
                glDrawArrays(GL_LINE_LOOP, offset, indicates[i]);
                offset += indicates[i];
            }
        }
        else if(checkStatus->FACE_CHECKED)
        {
            offset = 0;
            for(int i = 0; i < indicates.size(); i++)
            {
                glDrawArrays(GL_TRIANGLE_FAN, offset, indicates[i]);
                offset += indicates[i];
            }
        }
        else if(checkStatus->VERTEX_CHECKED)
        {
            //glPointSize(6.5f);//noEffect
            offset = 0;
            for(int i = 0; i < indicates.size(); i++)
            {
                glDrawArrays(GL_POINTS, offset, indicates[i]);
                offset += indicates[i];
            }
            //glPointSize(1.0f);
        }

        modelShaderProgram.disableAttributeArray("vertex");
        //modelShaderProgram.disableAttributeArray("normal");
        modelShaderProgram.release();
    }




    //draw special points and edges
    //GLint fill[2];
    //glGetIntegerv(GL_POLYGON_MODE, fill);
   // if(fill[0] != GL_FILL)
    //{
        //check the corner/cusp/crease vervices and draw them in different colors
        glEnable(GL_POINT_SPRITE);
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
        specialVerticesShaderProgram1.bind();
        specialVerticesShaderProgram1.setUniformValue("mvpMatrix", pMatrix * vMatrix * mMatrix);
        specialVerticesShaderProgram1.setUniformValue("pointSize", 3.0f);
        specialVerticesShaderProgram1.setUniformValue("color", QColor(Qt::blue));
        specialVerticesBuffer1.bind();

        specialVerticesShaderProgram1.setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);
        specialVerticesShaderProgram1.enableAttributeArray("vertex");
        specialVerticesBuffer1.release();

       // glPointSize(8);
        //glEnable(GL_POINT);
        glDrawArrays(GL_POINTS, 0, numSpecialVertices1);

        specialVerticesShaderProgram1.disableAttributeArray("vertex");
        specialVerticesShaderProgram1.release();

        specialVerticesShaderProgram2.bind();
        specialVerticesShaderProgram2.setUniformValue("mvpMatrix", pMatrix * vMatrix * mMatrix);
        specialVerticesShaderProgram2.setUniformValue("pointSize", 4.0f);
        specialVerticesShaderProgram2.setUniformValue("color", QColor(Qt::green));
        specialVerticesBuffer2.bind();

        specialVerticesShaderProgram2.setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);
        specialVerticesShaderProgram2.enableAttributeArray("vertex");
        specialVerticesBuffer2.release();

        //glPointSize(5);
        glDrawArrays(GL_POINTS, 0, numSpecialVertices2);

        specialVerticesShaderProgram2.disableAttributeArray("vertex");
        specialVerticesShaderProgram2.release();

        specialEdgesShaderProgram.bind();
        specialEdgesShaderProgram.setUniformValue("mvpMatrix", pMatrix * vMatrix * mMatrix);
        specialEdgesShaderProgram.setUniformValue("pointSize", 2.5f);
        specialEdgesShaderProgram.setUniformValue("color", QColor(Qt::red));
        specialEdgesBuffer.bind();

        specialEdgesShaderProgram.setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);
        specialEdgesShaderProgram.enableAttributeArray("vertex");
        specialEdgesBuffer.release();

        glLineWidth(2.5f);
        glDrawArrays(GL_LINES, 0, numSpecialEdges);
        glLineWidth(1.5f);
        specialEdgesShaderProgram.disableAttributeArray("vertex");
        specialEdgesShaderProgram.release();

    glLineWidth(1.0f);
}


void GLWidget::renderInSingleLight()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       //draw model
       //set up mMatrix;

       //setup vMatrix;
       QMatrix4x4 mMatrix;
       QMatrix4x4 vMatrix;

       QMatrix4x4 cameraTransformation;
   //    cameraTransformation.translate(transed);
       cameraTransformation.rotate(alpha, 0, 1, 0);
       cameraTransformation.rotate(beta, 1, 0, 0);

       QVector3D cameraPosition = cameraTransformation * QVector3D(0, 0, distance);
       //cameraTransformation.translate(transed);
       QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);
       vMatrix.lookAt(cameraPosition, QVector3D(0, 0, 0), cameraUpDirection);

       mMatrix.setToIdentity();
       QMatrix4x4 mvMatrix;

       mvMatrix = vMatrix * mMatrix;
       QMatrix3x3 normalMatrix;
       normalMatrix = mvMatrix.normalMatrix();

       QVector3D lightPosition = QVector3D(0, 200, 200);



       if(checkStatus->CONTROL_CHECKED)
       {
           glLineWidth(1.5);
           controlMeshProgram.bind();
           controlMeshProgram.setUniformValue("mvpMatrix", pMatrix * mvMatrix);
           //controlMeshProgram.setUniformValue("mvMatrix", mvMatrix);
           //controlMeshProgram.setUniformValue("normalMatrix", normalMatrix);
          // controlMeshProgram.setUniformValue("lightPosition", lightPosition);

           controlMeshProgram.setUniformValue("color", this->controlMeshColor);
           controlMeshProgram.setUniformValue("pointSize", 3);
   //        controlMeshProgram.setUniformValue("ambientColor", QColor(32, 32, 32));
   //        controlMeshProgram.setUniformValue("diffuseColor", QColor(128, 128, 128));
   //        controlMeshProgram.setUniformValue("specularColor", QColor(255, 255, 255));

   //        controlMeshProgram.setUniformValue("ambientReflection", (GLfloat) 1.0);
   //        controlMeshProgram.setUniformValue("diffuseReflection", (GLfloat) 1.0);
   //        controlMeshProgram.setUniformValue("specularReflection", (GLfloat) 1.0);
   //        controlMeshProgram.setUniformValue("shininess", (GLfloat) 200.0);

           controlMeshBuffer.bind();
           int offset = 0;
           controlMeshProgram.setAttributeBuffer("vertex", GL_FLOAT, offset, 3, 0);
           controlMeshProgram.enableAttributeArray("vertex");
   //        offset += numControlMeshVertices * 3 * sizeof(GLfloat);
   //        controlMeshProgram.setAttributeBuffer("normal", GL_FLOAT, offset, 3, 0);
   //        controlMeshProgram.enableAttributeArray("normal");
           controlMeshBuffer.release();

           offset = 0;
           if(checkStatus->ANTIALIALIASING_CHECKED)
           {
               glDisable(GL_DEPTH_TEST);
               glEnable(GL_LINE_SMOOTH);
               glEnable(GL_BLEND);
               glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
               glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

           }
           for(int i = 0; i < controlMeshIndicates.size(); i++)
           {
               glDrawArrays(GL_LINE_LOOP, offset, controlMeshIndicates[i]);
               offset += controlMeshIndicates[i];
           }
           if(checkStatus->ANTIALIALIASING_CHECKED)
           {
                glEnable(GL_DEPTH_TEST);
                glDisable(GL_LINE_SMOOTH);
                glDisable(GL_BLEND);
           }

           controlMeshProgram.disableAttributeArray("vertex");
           //controlMeshProgram.disableAttributeArray("normal");
           controlMeshProgram.release();
           glLineWidth(1.0f);
       }
       if(checkStatus->LIGHTING_CHECKED)
       {
           modelShaderProgram.bind();
           modelShaderProgram.setUniformValue("mvpMatrix", pMatrix * mvMatrix);
           modelShaderProgram.setUniformValue("mvMatrix", mvMatrix);
           modelShaderProgram.setUniformValue("normalMatrix", normalMatrix);
           modelShaderProgram.setUniformValue("lightPosition", lightPosition);
           modelShaderProgram.setUniformValue("viewerPosition", cameraPosition);

           modelShaderProgram.setUniformValue("ambientColor", QColor(51, 51, 51));
           modelShaderProgram.setUniformValue("diffuseColor", QColor(255, 255, 255));
           modelShaderProgram.setUniformValue("specularColor", QColor(200, 200, 200));

           modelShaderProgram.setUniformValue("ambientReflection", (GLfloat) 1.0);
           modelShaderProgram.setUniformValue("diffuseReflection", (GLfloat) 1.0);
           modelShaderProgram.setUniformValue("specularReflection", (GLfloat) 1.0);
           modelShaderProgram.setUniformValue("shininess", (GLfloat) 200.0);

           if(checkStatus->FACE_CHECKED)
               modelShaderProgram.setUniformValue("color", this->meshColor/*QColor::fromRgb(167, 154, 205)*/);
           else if(checkStatus->WIREFRAME_CHECKED)
               modelShaderProgram.setUniformValue("color", this->edgeColor);
           else
               modelShaderProgram.setUniformValue("color", QColor::fromRgb(154, 117, 167));
           modelBuffer.bind();
           int offset = 0;
           modelShaderProgram.setAttributeBuffer("vertex", GL_FLOAT, offset, 3, 0);
           modelShaderProgram.enableAttributeArray("vertex");
           offset += numModelVertices * 3 *sizeof(GLfloat);
           modelShaderProgram.setAttributeBuffer("normal", GL_FLOAT, offset, 3, 0);
           modelShaderProgram.enableAttributeArray("normal");
           modelBuffer.release();


           //glEnable(GL_LINE);

           if(checkStatus->WIREFRAME_CHECKED)
           {
               if(checkStatus->ANTIALIALIASING_CHECKED)
               {
                   glDisable(GL_DEPTH_TEST);
                   glEnable(GL_LINE_SMOOTH);
                   glEnable(GL_BLEND);
                   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                   glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

               }

               offset = 0;
               for(int i = 0; i < indicates.size(); i++)
               {
                   glDrawArrays(GL_LINE_LOOP, offset, indicates[i]);
                   offset += indicates[i];
               }

               if(checkStatus->ANTIALIALIASING_CHECKED)
               {
                    glEnable(GL_DEPTH_TEST);
                    glDisable(GL_LINE_SMOOTH);
                    glDisable(GL_BLEND);
               }
           }


           else if(checkStatus->FACE_CHECKED)
           {
               if(checkStatus->ANTIALIALIASING_CHECKED)
               {
                    glDisable(GL_DEPTH_TEST);
                    glEnable(GL_POLYGON_SMOOTH);
                    glEnable(GL_BLEND);
                    //glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
               }
               offset = 0;
               for(int i = 0; i < indicates.size(); i++)
               {
                   glDrawArrays(GL_TRIANGLE_FAN, offset, indicates[i]);
                   offset += indicates[i];
               }

               if(checkStatus->ANTIALIALIASING_CHECKED)
               {
                    glEnable(GL_DEPTH_TEST);
                    glDisable(GL_POLYGON_SMOOTH);
                    glDisable(GL_BLEND);
               }
           }
           else if(checkStatus->VERTEX_CHECKED)
           {
               //glPointSize(6.5f);//noEffect
               offset = 0;
               for(int i = 0; i < indicates.size(); i++)
               {
                   glDrawArrays(GL_POINTS, offset, indicates[i]);
                   offset += indicates[i];
               }
               //glPointSize(1.0f);
           }

           modelShaderProgram.disableAttributeArray("vertex");
           modelShaderProgram.disableAttributeArray("normal");
           modelShaderProgram.release();
       }
       else
       {
           noLightingShaderProgram.bind();
           noLightingShaderProgram.setUniformValue("mvpMatrix", pMatrix * mvMatrix);
           noLightingShaderProgram.setUniformValue("pointSize", 2.0f);
           if(checkStatus->FACE_CHECKED)
               noLightingShaderProgram.setUniformValue("color", this->meshColor/*QColor::fromRgb(167, 154, 205)*/);
           else if(checkStatus->WIREFRAME_CHECKED)
               noLightingShaderProgram.setUniformValue("color", this->edgeColor);
           else
               noLightingShaderProgram.setUniformValue("color", QColor::fromRgb(154, 117, 167));
           modelBuffer.bind();
           int offset = 0;
           noLightingShaderProgram.setAttributeBuffer("vertex", GL_FLOAT, offset, 3, 0);
           noLightingShaderProgram.enableAttributeArray("vertex");
           modelBuffer.release();


           //glEnable(GL_LINE);
           if(checkStatus->WIREFRAME_CHECKED)
           {
               if(checkStatus->ANTIALIALIASING_CHECKED)
               {
                   glDisable(GL_DEPTH_TEST);
                   glEnable(GL_LINE_SMOOTH);
                   glEnable(GL_BLEND);
                   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                   glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

               }
               offset = 0;
               for(int i = 0; i < indicates.size(); i++)
               {
                   glDrawArrays(GL_LINE_LOOP, offset, indicates[i]);
                   offset += indicates[i];
               }
               if(checkStatus->ANTIALIALIASING_CHECKED)
               {
                    glEnable(GL_DEPTH_TEST);
                    glDisable(GL_LINE_SMOOTH);
                    glDisable(GL_BLEND);
               }
           }
           else if(checkStatus->FACE_CHECKED)
           {
               if(checkStatus->ANTIALIALIASING_CHECKED)
               {
                    glDisable(GL_DEPTH_TEST);
                    glEnable(GL_POLYGON_SMOOTH);
                    glEnable(GL_BLEND);
                    //glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
               }
               offset = 0;
               for(int i = 0; i < indicates.size(); i++)
               {
                   glDrawArrays(GL_TRIANGLE_FAN, offset, indicates[i]);
                   offset += indicates[i];
               }
               if(checkStatus->ANTIALIALIASING_CHECKED)
               {
                    glEnable(GL_DEPTH_TEST);
                    glDisable(GL_POLYGON_SMOOTH);
                    glDisable(GL_BLEND);
               }
           }
           else if(checkStatus->VERTEX_CHECKED)
           {
               //glPointSize(6.5f);//noEffect
               offset = 0;
               for(int i = 0; i < indicates.size(); i++)
               {
                   glDrawArrays(GL_POINTS, offset, indicates[i]);
                   offset += indicates[i];
               }
               //glPointSize(1.0f);
           }

           modelShaderProgram.disableAttributeArray("vertex");
           //modelShaderProgram.disableAttributeArray("normal");
           modelShaderProgram.release();
       }




       //draw special points and edges
       //GLint fill[2];
       //glGetIntegerv(GL_POLYGON_MODE, fill);
      // if(fill[0] != GL_FILL)
       //{
           //check the corner/cusp/crease vervices and draw them in different colors
       if(checkStatus->SharpFeature_CHECKED)
       {
           glEnable(GL_POINT_SPRITE);
           glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
           specialVerticesShaderProgram1.bind();
           specialVerticesShaderProgram1.setUniformValue("mvpMatrix", pMatrix * vMatrix * mMatrix);
           specialVerticesShaderProgram1.setUniformValue("pointSize", 4.0f);
           specialVerticesShaderProgram1.setUniformValue("color", QColor(Qt::blue));
           specialVerticesBuffer1.bind();

           specialVerticesShaderProgram1.setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);
           specialVerticesShaderProgram1.enableAttributeArray("vertex");
           specialVerticesBuffer1.release();

          // glPointSize(8);
           //glEnable(GL_POINT);
           glDrawArrays(GL_POINTS, 0, numSpecialVertices1);

           specialVerticesShaderProgram1.disableAttributeArray("vertex");
           specialVerticesShaderProgram1.release();

           specialVerticesShaderProgram2.bind();
           specialVerticesShaderProgram2.setUniformValue("mvpMatrix", pMatrix * vMatrix * mMatrix);
           specialVerticesShaderProgram2.setUniformValue("pointSize", 3.0f);
           specialVerticesShaderProgram2.setUniformValue("color", QColor(Qt::green));
           specialVerticesBuffer2.bind();

           specialVerticesShaderProgram2.setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);
           specialVerticesShaderProgram2.enableAttributeArray("vertex");
           specialVerticesBuffer2.release();

           //glPointSize(5);
           glDrawArrays(GL_POINTS, 0, numSpecialVertices2);

           specialVerticesShaderProgram2.disableAttributeArray("vertex");
           specialVerticesShaderProgram2.release();

           specialEdgesShaderProgram.bind();
           specialEdgesShaderProgram.setUniformValue("mvpMatrix", pMatrix * vMatrix * mMatrix);
           specialEdgesShaderProgram.setUniformValue("pointSize", 2.0f);
           specialEdgesShaderProgram.setUniformValue("color", QColor(Qt::red));
           specialEdgesBuffer.bind();

           specialEdgesShaderProgram.setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);
           specialEdgesShaderProgram.enableAttributeArray("vertex");
           specialEdgesBuffer.release();

           if(checkStatus->ANTIALIALIASING_CHECKED)
           {
               glDisable(GL_DEPTH_TEST);
               glEnable(GL_LINE_SMOOTH);
               glEnable(GL_BLEND);
               glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
               glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

           }
           glLineWidth(2.0);
           glDrawArrays(GL_LINES, 0, numSpecialEdges);
           glLineWidth(1);
           if(checkStatus->ANTIALIALIASING_CHECKED)
           {
                glEnable(GL_DEPTH_TEST);
                glDisable(GL_LINE_SMOOTH);
                glDisable(GL_BLEND);
           }
           specialEdgesShaderProgram.disableAttributeArray("vertex");
           specialEdgesShaderProgram.release();
       }

}

void GLWidget::resizeGL(int width, int height)
{
    if (height == 0) {
        height = 1;
    }

    pMatrix.setToIdentity();
    pMatrix.perspective(60.0, (float) width / (float) height, 0.001, 1000);

    glViewport(0, 0, width, height);

/*
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, aspect, 0.001, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDrawBuffer(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_TEXTURE_2D);
    glViewport(0, 0, size.width(), size.height());*/
}

//void GLWidget::renderSphere()
//{
//    // Default rendering

//    glColor3ub(0,128,255);
//    GLUquadricObj*	q = gluNewQuadric();
//    gluQuadricDrawStyle(q, GLU_FILL);
//    gluQuadricNormals(q, GLU_SMOOTH);
//    gluSphere(q, 1.0, 20, 20);
//    gluDeleteQuadric(q);
//}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastMousePosition = event->pos();

    event->accept();
    update();
}
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    /*
    if(event->buttons() & Qt::LeftButton)
    {*/
        int deltaX = event->x() - lastMousePosition.x();
        int deltaY = event->y() - lastMousePosition.y();

        if (event->buttons() & Qt::LeftButton) {
            alpha -= deltaX;
            while (alpha < 0) {
                alpha += 360;
            }
            while (alpha >= 360) {
                alpha -= 360;
            }

            beta -= deltaY;
            if (beta < -90) {
                beta = -90;
            }
            if (beta > 90) {
                beta = 90;
            }
        }

        lastMousePosition = event->pos();

        //mMatrix.rotate(alpha/200, 0, 1, 0);
        //mMatrix.rotate(beta/200, 1, 0, 0);

        event->accept();
        update();
        /*
    }

    else if(event->buttons() & Qt::RightButton)
    {
        //move topology through matrix
        QPoint currentMousePosition = event->pos();
        int spanX = currentMousePosition.x() - lastMousePosition.x();
        int spanY = currentMousePosition.y() - lastMousePosition.y();
        QVector3D temp(currentMousePosition - lastMousePosition);
        transed += temp;
        lastMousePosition = currentMousePosition;
        mMatrix.translate(spanX / 15, -spanY / 15, 0);
        event->accept();
        updateGL();
    }*/
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta();

    if (event->orientation() == Qt::Vertical) {
        if (delta < 0) {
            distance *= 1.1;
        } else if (delta > 0) {
            distance *= 0.9;
        }
    }

    event->accept();
    update();
}

//void GLWidget::timeout(void)
//{
//    lightAngle += 1;
//    while (lightAngle >= 360) {
//        lightAngle -= 360;
//    }

//    updateGL();
//}
