#ifndef BaseTOPOLOGY_H
#define BaseTOPOLOGY_H

#include <fstream>
#include <QVector3D>
#include "basefaces.h"
#include "cat_faces.h"
#include "doo_faces.h"
#include "loop_faces.h"
using namespace std;
#include <qstring.h>

enum TopMode{cat,doo,loop, general};


class BaseTopology
{
public:
    BaseTopology(TopMode mode);
    ~BaseTopology();

    void destroy();
    QVector3D getCentroid();

    virtual void initLists();
    virtual void copy(BaseTopology *dst, bool keepTop = false);
   static int adjacentFace(BaseVertex *a, BaseVertex *b, int face);
   static int equalEdge(QList<BaseEdge*> *e, QList<BaseVertex*> *v, int index1, int index2);
   static int updateAdjEdge(BaseFace *f, int index, int find, int eind);//return the edge index in f
public:
   // vector<BaseVertex*> *vertices;
    //vector<BaseEdge*> *edges;
    //vector<BaseFace*> *faces;
    QList<BaseVertex*> *vertices;
    QList<BaseEdge*> *edges;
    QList<BaseFace*> *faces;

    BaseVertex *centroid;

    TopMode mode;
    QString name;
    int nofvert;
    int noffaces;
    int dim;
private:
      void calculateCentroid();

};

#endif // DOO_TOPOLOGY_H
