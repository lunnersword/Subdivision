#include "doo_faces.h"

Doo_Vertex::Doo_Vertex()
{
}

Doo_Vertex::Doo_Vertex(int dim):BaseVertex(dim)
{

}

Doo_Vertex::~Doo_Vertex()
{
    for(int i = 0; i < dim; i++)
    {
        datas[i] = 0;
        ndatas[i] = 0;
    }
    sharp = 0;
}

Doo_Edge::Doo_Edge()
{
}
Doo_Edge::~Doo_Edge()
{
    index1 = 0;
    index2 = 0;
    esharp = 0;
}

Doo_Face::Doo_Face()
{

}
Doo_Face::~Doo_Face()
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
