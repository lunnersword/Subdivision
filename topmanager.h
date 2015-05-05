#ifndef DOOMANAGER_H
#define DOOMANAGER_H
#ifndef MAXBUFSIZE
#define MAXBUFSIZE 1024
#endif
#include "basetopology.h"
#include "loop_faces.h"
#include "cat_faces.h"
#include "doo_faces.h"
#include "defines.h"
#include <iostream>
#include <list>
#include <QtWidgets>
#include <QWidget>
#include <QObject> // tr function is static function inside QObject and most of the time QObject is inherited in other widget they directly use tr, but in my case it has to be QObject::tr works
using namespace std;




typedef struct _ModeValid
{
    bool CAT = true;
    bool LOOP = false;
    bool DOO = false;
}ModeValid;

class Adapter
{
public:
    //list<BaseTopolopy*> *steps;
   // list<BaseTopology*> *steps;
    QList<BaseTopology*> *steps;
    QList<BaseTopology*> *limitSteps;
    ModeValid modeValid; // if for the first top's subdivision valid mode
    int currentStep;
    QString name;//the file's name
    QString filename;//the file's full path
   // QString name;

    int stepIndicate; // a indicate, if == 0, operation from the topology of index 0; if >0 from index 1, and the steps between top0 and top1 is stepIndicate
public:
    Adapter();
    ~Adapter();
    void destroy();
    int getCurrentStep(void);
    void setCurrentStep(int position);

    void moveAhead();
    void moveBack();
    //void pushOneStep(BaseTopology* topolopy);
    void clear();

    QString getName();
    QString getFileName();

    //void setName(QString name);
    void setName(QString name);
    void setFileName(QString filename);
    void setModeValid(BaseTopology *topo0);

    void addStep(BaseTopology *top);
    void addStep(BaseTopology *top, BaseTopology *limit);
    BaseTopology* getCurrentTopolopy();
    BaseTopology* getCurrentLimitTopology();

};

class TopManager
{
public:
    //list<Adapter*> *windows; while using list I counted some problems so I choose QList

    QList<Adapter*> *windows;
    QWidget *parent;
    int currentWindow;
    //int windowSize;
    TopManager(QWidget *parent);
    ~TopManager();



    void edgeMidpoint(BaseTopology *topold, BaseTopology *topnew);
    void evenPartNonuniformSmoothing(BaseTopology *topold, BaseTopology *topnew, double u);

    void uniformSmoothing(BaseTopology *topold, BaseTopology *topnew, bool isEven);

    void evenGeneralizedSubdivision(BaseTopology *topold, BaseTopology *topnew, int k, double u);

    void oddPartNonuniformSmoothing(BaseTopology *topold, BaseTopology *topnew, double u);

    //void oddPartUniformSmoothing(BaseTopology *topold, BaseTopology *topnew, double u);

    void oddGeneralizedSubdivision(BaseTopology *topold, BaseTopology *topnew, int k, double u);

    void mergeWithoutCommons(QList<int> &List1, QList<int> &list2);
    void loopMakeAlpha(BaseTopology *topo);
    void createLoopVertexFace(BaseTopology *topold, BaseTopology *topnew);
    void createLoopEdgeFace(BaseTopology * topold, BaseTopology *topnew);
    void createLoopFaceFace(BaseTopology *topold, BaseTopology *topnew);
    void createLoopLimit(BaseTopology *topold, BaseTopology *topnew);

    void createDooFaceFace(BaseTopology *topold, BaseTopology *topnew);
    void createDooEdgeFace(BaseTopology *topold, BaseTopology *topnew);
    void createDooVertexFace(BaseTopology *topold, BaseTopology *topnew);

    void createCatFaceFace(BaseTopology *topold, BaseTopology *topnew);
    void createCatEdgeFace(BaseTopology *topold, BaseTopology *topnew);
    void createCatVertexFace(BaseTopology *topold, BaseTopology *topnew);
    void createCatTopo(BaseTopology *topold, BaseTopology* topnew);
    void createCatLimitPosition(BaseTopology *topnew, BaseTopology* toplimit);


    void installLoopVertex(QList<Loop_Vertex*> *v,int dim, const double *dbuf,int sharp0);
    void installDooVertex(QList<Doo_Vertex*> *v,int dim, const double *dbuf,int sharp0);
    void installCatVertex(QList<Cat_Vertex*> *v,int dim, const double *dbuf,int sharp0);
    void installLoopEdge(QList<BaseEdge*> *e, QList<BaseVertex*> *v, int index1, int index2);
    void installDooEdge(QList<BaseEdge*> *e, QList<BaseVertex*> *v, int index1, int index2);
    void installCatEdge(QList<BaseEdge*> *e, QList<BaseVertex*> *v, int index1, int index2);
    void installEdgeOfFace(BaseFace *face, int index, int ivert0, int adjface, int adjedge);

    void loopSubdivision(BaseTopology *topold, BaseTopology *topnew, BaseTopology *toplimit);
    void dooSubdivision(BaseTopology *topold, BaseTopology *topnew);
    void catSubdivision(BaseTopology *topold, BaseTopology *topnew, BaseTopology *toplimit);
    void loopSubdivision(BaseTopology *topold, BaseTopology *topnew);
    void generalizedSubdivision(BaseTopology *topold, BaseTopology *topnew);

    void composeTopology(QTextStream *input, Adapter *adapter, BaseTopology *topology);
    void parseSurfaceBase(QTextStream *input, Adapter *adapter, TopMode topMode);

    void drawCatSurface(BaseTopology *surface, float r, float g, float b);
    void drawDooSurface(BaseTopology *surface, float r, float g, float b);
    void drawLoopSurface(BaseTopology *surface, float r, float g, float b);
    void drawSurface(BaseTopology *surface, float r, float g, float b);

    void getOriginalModelVerticesAndNormals(QVector<QVector3D> *vertices, QVector<QVector3D> *normals, QList<int> *indicates);
    void getCurrentModelVerticesAndNormals(QVector<QVector3D> *vertices, QVector<QVector3D> *normals, QList<int> *indicates, LimitOrNot limitOrNot);
    void getCurrentSpecialVerticesAndEdges(QVector<QVector3D> *vertices1, QVector<QVector3D> *vertices2, QVector<QVector3D> *edges, LimitOrNot limitOrNot);
    void onNew(char* filename);
    bool onOpen();
    void onSave();
    void onSaveAs();
    void onCatSubdivision();
    void onDooSubdivision();
    void onLoopSubdivision();
    void onGeneralizedSubdivision(int k, double u);


    void addWindow(Adapter* adapter);
    void removeWindow(int index);
    Adapter* getCurrentWindow();
    int getCurrentWindowIndex();
    BaseTopology* getCurrentTopology();
    BaseTopology* getCurrentLimitTopology();
    void setCurrentWindow(int current);
    QString openAFile();

    QList<int> makeVertList(QList<BaseFace*> *facelist, int indexf, QList<BaseEdge*> *e);
    void saveToFile(Adapter *adapter);
    void saveAsToFile(Adapter *adapter);
    void saveDataToFile(BaseTopology *topology, QTextStream *out);



};

#endif // DOOMANAGER_H
