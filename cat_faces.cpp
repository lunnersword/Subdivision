#include "cat_faces.h"
Cat_Vertex::Cat_Vertex():BaseVertex()
{

}
Cat_Vertex::Cat_Vertex(int dim):BaseVertex(dim)
{

}

Cat_Vertex::~Cat_Vertex()
{
    for(int i = 0; i < dim; i++)
    {
        datas[i] = 0;
        ndatas[i] = 0;
    }
    sharp = 0;
}


Cat_Edge::~Cat_Edge()
{
    index1 = 0;
    index2 = 0;
    esharp = 0;
}

Cat_Face::~Cat_Face()
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

