#include "basetopology.h"

BaseTopology::BaseTopology(TopMode mode)
{
    this->mode = mode;
    vertices = NULL;
    edges = NULL;
    faces = NULL;
    centroid = NULL;
}

BaseTopology::~BaseTopology()
{
    int i;
    for(i = 0; i < vertices->size(); i++)
    {
        if((*vertices)[i] != NULL)
            delete (*vertices)[i];
    }
    if(vertices != NULL)
        delete vertices;
    for(i = 0; i < edges->size(); i++)
    {
        if((*edges)[i] != NULL)
            delete (*edges)[i];
    }
    if(edges != NULL)
        delete edges;

    for(i = 0; i < faces->size(); i++)
    {
        //(*faces)[i]->destroy();
        if((*faces)[i] != NULL)
            delete (*faces)[i];
    }
    if(faces != NULL)
        delete faces;
    if(centroid != NULL)
        delete centroid;

}
QVector3D BaseTopology::getCentroid()
{
    if(centroid == NULL)
        calculateCentroid();
    QVector3D cent(centroid->datas[0], centroid->datas[1], centroid->datas[2]);
    return cent;
}

void BaseTopology::calculateCentroid()
{
    if(vertices == NULL)
        return;
    if(centroid != NULL)
        return;
    centroid = new BaseVertex(this->dim);
    double *temp = new double[this->dim];

    for(int i = 0; i < this->dim; i++)
        temp[i] = 0;

    for(int i = 0,j=1; i < vertices->size();i++, j = (j+1)%100)
    {
        for(int k = 0; k <this->dim; k++)
            temp[k] += vertices->at(i)->datas[k];
        if(j % 100 == 0)
        {
            j = 1;
            for(int k = 0; k < this->dim; k++)
            {
                centroid->datas[k] += temp[k] / 100;
                temp[k] = 0;
            }
        }
    }
}

void BaseTopology::initLists()
{
    switch(mode)
    {
    case cat:
        vertices = (QList<BaseVertex*>*)new QList<Cat_Vertex*>;
        edges = (QList<BaseEdge*>*)new QList<Cat_Edge*>;
        faces = (QList<BaseFace*>*)new QList<Cat_Face*>;
        break;
    case loop:
        vertices = (QList<BaseVertex*>*) new QList<Loop_Vertex*>;
        edges = (QList<BaseEdge*>*)new QList<Loop_Edge*>;
        faces = (QList<BaseFace*>*)new QList<Loop_Face*>;
        break;
    case doo:
        vertices = (QList<BaseVertex*>*)new QList<Doo_Vertex*>;
        edges = (QList<BaseEdge*>*)new QList<Doo_Edge*>;
        faces = (QList<BaseFace*>*)new QList<Doo_Face*>;
        break;
    default:
        vertices = new QList<BaseVertex*>;
        edges = new QList<BaseEdge*>;
        faces = new QList<BaseFace*>;
        break;
    }
}

void BaseTopology::copy(BaseTopology *dst, bool keepTop)
{
    int i;

    dst->nofvert = nofvert;
    dst->noffaces = noffaces;
    dst->name = name;
    dst->dim = dim;
    dst->mode = mode;

    switch (mode)
    {
    case cat:
    {
        Cat_Vertex *catvert;
        Cat_Edge *catedge;
        Cat_Face *catface;
        dst->vertices = (QList<BaseVertex*>*)new QList<Cat_Vertex*>;
        for(i = 0; i < vertices->size(); i++)
        {
            catvert = new Cat_Vertex(dim);
            (*vertices)[i]->copy(catvert, keepTop);
            dst->vertices->push_back(catvert);
        }

        dst->edges = (QList<BaseEdge*>*)new QList<Cat_Edge*>;
        for(i = 0; i < edges->size(); i++)
        {
            catedge = new Cat_Edge;
            (*edges)[i]->copy(catedge, keepTop);
            dst->edges->push_back(catedge);
        }

        dst->faces = (QList<BaseFace*>*)new QList<Cat_Face*>;
        for(i = 0; i < faces->size(); i++)
        {
            catface = new Cat_Face;
            (*faces)[i]->copy(catface);
            dst->faces->push_back(catface);
        }
        break;
    }
    case loop:
    {
        Loop_Vertex *loopvert;
        Loop_Edge *loopedge;
        Loop_Face *loopface;

        dst->vertices = (QList<BaseVertex*>*) new QList<Loop_Vertex*>;
        for(i = 0; i < vertices->size(); i++)
        {
            loopvert = new Loop_Vertex(dim);
            (*vertices)[i]->copy(loopvert, keepTop);
            dst->vertices->push_back(loopvert);
        }

        dst->edges = (QList<BaseEdge*>*)new QList<Loop_Edge*>;
        for(i = 0; i < edges->size(); i++)
        {
            loopedge = new Loop_Edge;
            (*edges)[i]->copy(loopedge, keepTop);
            dst->edges->push_back(loopedge);
        }

        dst->faces = (QList<BaseFace*>*)new QList<Loop_Face*>;
        for(i = 0; i < faces->size(); i++)
        {
            loopface = new Loop_Face;
            (*faces)[i]->copy(loopface);
            dst->faces->push_back(loopface);
        }
        break;
    }
    case doo:
    {
        Doo_Vertex *doovert;
        Doo_Edge *dooedge;
        Doo_Face *dooface;
        dst->vertices = (QList<BaseVertex*>*)new QList<Doo_Vertex*>;
        for(i = 0; i < vertices->size(); i++)
        {
            doovert = new Doo_Vertex(dim);
            (*vertices)[i]->copy(doovert, keepTop);
            dst->vertices->push_back(doovert);
        }

        dst->edges = (QList<BaseEdge*>*)new QList<Doo_Edge*>;
        for(i = 0; i < edges->size(); i++)
        {
            dooedge = new Doo_Edge;
            (*edges)[i]->copy(dooedge, keepTop);
            dst->edges->push_back(dooedge);
        }

        dst->faces = (QList<BaseFace*>*)new QList<Doo_Face*>;
        for(i = 0; i < faces->size(); i++)
        {
            dooface = new Doo_Face;
            (*faces)[i]->copy(dooface);
            dst->faces->push_back(dooface);
        }
        break;
    }
    default:
        break;
    }

    /*
    dst->vertices = new QList<BaseVertex*>;
    for(i = 0; i < vertices->size(); i++)
    {
        vert = new BaseVertex;
        (*vertices)[i]->copy(vert);
        dst->vertices->push_back(vert);
    }

    dst->edges = new QList<BaseEdge*>;
    for(i = 0; i < edges->size(); i++)
    {
        edge = new BaseEdge;
        (*edges)[i]->copy(edge);
        dst->edges->push_back(edge);
    }

    dst->faces = new QList<BaseFace*>;
    for(i = 0; i < faces->size(); i++)
    {
        face = new BaseFace;
        (*faces)[i]->copy(face);
        dst->faces->push_back(face);
    }*/

}

 int BaseTopology::adjacentFace(BaseVertex *a, BaseVertex *b, int face)
{
    // serarch the first element of a->face which is equalivant to any element of b->face and it's value is not face
        //寻找第一个a->face中的元素，它的值与b->face中某个值相等 但不等于face，因为a->face,b->face元素都是从小到大排列的，所以不用每次都set j = 0;
      int i, j;
      i = 0;
      j = 0;

      if ((a->faces.size() == 0) || (b->faces.size() == 0))
      {
        return -1;
      }
      else
      {
        do
        {
          while ( (b->faces.size() > j) && (a->faces[i] > b->faces[j]) ) {
            j++;
          }
          if ( j != b->faces.size() && (a->faces[i] == b->faces[j]) && (a->faces[i] != face) ){
            return a->faces[i];
          }
          i++;
        }
        while ((b->faces.size() > j) && (a->faces.size() > i));
        return -1;
      }
}

 int BaseTopology::equalEdge(QList<BaseEdge *> *e, QList<BaseVertex *> *v, int index1, int index2)
{
    //顶点集v中的两个顶点index1,index2.  寻找与index1相邻边中第一条与index2相邻边相等的边
    //whether the (*v)[index]->edge 也是从小到大的呢？  如果是这里就有改进的地方， 不过这个应该影响不大，因为点的相邻边一般不会特别多
      int i, j;
      QList<int> a = (*v)[index1]->edges;
      QList<int> b = (*v)[index2]->edges;

      for (i = 0; i < a.size(); ++i) {
        for (j = 0; j < b.size(); ++j) {
          if ( a[i] == b[j] )
        return a[i];
        }
      }
      return e->size();
}

 int BaseTopology::updateAdjEdge(BaseFace *f, int index, int find, int eind)
{
    int i;

      for (i = f->edgeoff->size()-1; i >= 0; i--)
      {
        if ((*f->edgeoff)[i]->index == index)
        {
          (*f->edgeoff)[i]->adjf = find;
          (*f->edgeoff)[i]->adje = eind;
          return i;
        }
      }
      return -1;
}
