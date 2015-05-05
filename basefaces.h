#ifndef BASEFACES_H
#define BASEFACES_H
#include <QList>
// this base Type BaseEdgeDesc defines an Edge in a face
typedef struct _BaseEdgeDesc
{
    int index;//the index of edge in topology.edges
    int ivert0;//0 : the direction is right, 1: the direction is reversed
    int adjf;
    int adje;

}BaseEdgeDesc;

class BaseVertex
{
public:
    int dim;
    int sharp;
    double alpha;
    double *datas;
    double ndatas[3];
    QList<int> faces;
    QList<int> edges;
    BaseVertex();
    BaseVertex(int dim);
    ~BaseVertex();
    void create(const double *datas0, int sharp0);
    void create(double x0, double y0, double z0, int sharp0);
    void insertFace(int index);
    void removeFace(int index);
    virtual void copy(BaseVertex *v, bool keepTop);
};

class BaseEdge
{
public:
    int index1;
    int index2;
    int esharp;
    int midpoint;

    QList<int> adjfaces;
    QList<int> adjedges;

    BaseEdge();
    ~BaseEdge();
    void create(int ind1, int ind2);
    virtual void copy(BaseEdge *e, bool keepTop);
};

class BaseFace
{
public:
    QList<BaseEdgeDesc*> *edgeoff;
    int fpoint;

    BaseFace();
    ~BaseFace();

    void insertEdge(int index, int ivert0, int adjface, int adjedge);
    virtual void copy(BaseFace *f);

};

#endif // BASEFACE_H
