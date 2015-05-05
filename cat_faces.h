#ifndef CAT_FACES_H
#define CAT_FACES_H
#include "basefaces.h"
class Cat_Vertex: public BaseVertex
{
public:
    Cat_Vertex();
    Cat_Vertex(int dim);
    ~Cat_Vertex();
};

class Cat_Edge: public BaseEdge
{
public:

    Cat_Edge(){}
    ~Cat_Edge();
};

class Cat_Face: public BaseFace
{
public:
    Cat_Face(){}
    ~Cat_Face();
};
#endif // CAT_FACES_H
