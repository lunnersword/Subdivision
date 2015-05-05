#ifndef DOO_FACES_H
#define DOO_FACES_H
#include "basefaces.h"
#include <QList>
/*
//This type EdgeDesc defines an edge in a face
typedef struct _Doo_EdgeDesc
{
    int index;//the edge's index in edge vector
    int ivert0;
    int adjf;//adjacent face's index
    int adje;//this edge's index in the adjacent face
}Doo_EdgeDesc;
*/
//definition of the class Vertex
class Doo_Vertex: public BaseVertex
{
public:
    /*QList<int> faces;//the faces is compoused by this vertex, sotred by index
    QList<int> edges;*/
    Doo_Vertex();
    Doo_Vertex(int dim);
    ~Doo_Vertex();
};
class Doo_Edge: public BaseEdge
{
public:
    Doo_Edge();
    ~Doo_Edge();
};

class Doo_Face: public BaseFace
{
public:
    Doo_Face();
    ~Doo_Face();
};

#endif // DOO_FACES_H
