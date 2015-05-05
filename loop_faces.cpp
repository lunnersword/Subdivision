#include "loop_faces.h"

Loop_Vertex::Loop_Vertex()
{
    //because when nothing specified: call default constructor
    alpha = 0;
}
Loop_Vertex::Loop_Vertex(int dim):BaseVertex(dim)
{
    alpha = 0;
}

Loop_Vertex::~Loop_Vertex()
{
    //deconstruct will not been called
    for(int i = 0; i < dim; i++)
    {
        datas[i] = 0;
        ndatas[i] = 0;
    }
    alpha = 0;
    sharp = 0;
}

void Loop_Vertex::create(double x0, double y0, double z0, int sharp0)
{
    BaseVertex::create(x0, y0, z0, sharp0);
    alpha = 0;
}

void Loop_Vertex::create(const double *datas0, int sharp0)
{
    BaseVertex::create(datas0, sharp0);
    alpha = 0;
}

void Loop_Vertex::copy(Loop_Vertex *v, bool keepTop)
{
    v->alpha = alpha;
    BaseVertex::copy(v, keepTop);
    //Loop,  copy the faces not edges
    v->faces.append(faces);

}

Loop_Edge::Loop_Edge()
{

}
Loop_Edge::~Loop_Edge()
{
    index1 = 0;
    index2 = 0;
    esharp = 0;
}

void Loop_Edge::copy(Loop_Edge *e, bool keepTop)
{
    BaseEdge::copy(e, keepTop);
    e->adjfaces.append(adjfaces);
}

Loop_Face::Loop_Face()
{

}

Loop_Face::~Loop_Face()
{
    int i;
    for(i=0; i < edgeoff->size(); i++)
    {
        if((*edgeoff)[i] != NULL)
            delete (*edgeoff)[i];
    }
    if(edgeoff != NULL)
        delete edgeoff;
}
