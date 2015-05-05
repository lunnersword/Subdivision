#ifndef LOOP_FACES_H
#define LOOP_FACES_H
#include "basefaces.h"
class Loop_Vertex: public BaseVertex
{
public:

    Loop_Vertex();
    Loop_Vertex(int dim);
    ~Loop_Vertex();
    void create(const double *datas0, int sharp0);
    void create(double x0, double y0, double z0, int sharp0);
    void insertFace(int index);
    void removeFace(int index);
    virtual void copy(Loop_Vertex *v, bool keepTop);
};

class Loop_Edge: public BaseEdge
{
public:
    Loop_Edge();
    ~Loop_Edge();

    void copy(Loop_Edge *e, bool keepTop);
};

class Loop_Face: public BaseFace
{
public:
    Loop_Face();
    ~Loop_Face();

};


#endif // LOOP_FACES_H
