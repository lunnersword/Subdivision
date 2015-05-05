#include<stdlib.h>
#include<stdio.h>
#include "basefaces.h"

BaseVertex::BaseVertex()
{
    this->dim = 3;
    datas = new double[dim];
    for(int i=0; i < dim; i++)
    {
        datas[i] = 0;
        ndatas[i] = 0;
    }
    sharp = 0;
    alpha = 0;
}

BaseVertex::BaseVertex(int dim)
{
    if(dim < 3)
        return;//rase exception;
    this->dim = dim;
    datas = new double[dim];
    for(int i=0; i < dim; i++)
    {
        datas[i] = 0;
        ndatas[i] = 0;
    }
    sharp = 0;
    alpha = 0;

}
BaseVertex::~BaseVertex()
{
    for(int i = 0; i < dim; i++)
    {
        datas[i] = 0;
        ndatas[i] = 0;
    }
    sharp = 0;
    alpha = 0;
}

void BaseVertex::create(double x0, double y0, double z0, int sharp0)
{
    datas[0] = x0;
    datas[1] = y0;
    datas[2] = z0;
    sharp = sharp0;
    alpha = 0;
}

void BaseVertex::create(const double *datas0, int sharp0)
{
    //if(sizeof(datas0) / sizeof(double) != dim)//sizeof cannot caculate dynamic space size

    //    return;//raise exception
    for(int i = 0; i < dim; i++)
    {
        datas[i] = datas0[i];
    }
    sharp = sharp0;
    alpha = 0;
}


//insert the face which is compoused by the vertex to the vertex<int> faces
void BaseVertex::insertFace(int index)
{
    int i = 0;
    if(faces.size() == 0)
        //faces.insert(faces.end(), index);
        faces.push_back(index);
    else
    {
        //while make sure the face index is inserted by the small->big order
        while((i < faces.size()) && (faces.at(i) != index))
        {
            i++;
        }

        if(i < faces.size())
        {
            if(faces.at(i) != index)
                faces.insert(i, index);
        }
        else
        {
            if(faces.at(i-1) != index)
                faces.insert(i, index);
        }
    }
}

void BaseVertex::removeFace(int index)
{
    int i = 0;
    if(faces.size() != 0)
    {
        while((i < faces.size()) && (faces.at(i) != index))
        {
            i++;
        }
        if(i < faces.size())
            faces.erase(faces.begin()+i);
    }
}

void BaseVertex::copy(BaseVertex *v, bool keepTop)
{
    if(dim != v->dim)
        return;//raise exception
    for(int i = 0; i < dim; i++)
    {
        v->datas[i] = datas[i];
    }
    if(!keepTop)
    {

        v->sharp = sharp;
        v->faces.clear();
        v->edges.clear();
        //we didn't copy nx,ny,nz here and clear the faces and edges, because we copy the single vertex out, it's topolopy structure is meaningless.

    }
    else
    {

        for(int i = 0; i < 3; i++)
        {
            v->ndatas[i] = ndatas[i];
        }
        v->sharp = sharp;
        v->alpha = alpha;
        v->faces.clear();
        for(int i = 0; i < faces.size(); i++)
        {
            v->faces.append(faces.at(i));
        }
        v->edges.clear();
        for(int i = 0; i < edges.size(); i++)
        {
            v->edges.append(edges.at(i));
        }
    }
}


BaseEdge::BaseEdge()
{
    index1 = 0;
    index2 = 0;
}

BaseEdge::~BaseEdge()
{
    index1 = 0;
    index2 = 0;
    esharp = 0;
}

void BaseEdge::create(int ind1, int ind2)
{
    index1 = ind1;
    index2 = ind2;
    esharp = 0;
}

void BaseEdge::copy(BaseEdge *e, bool keepTop)
{
    if(!keepTop)
    {
        e->index1 = index1;
        e->index2 = index2;
        e->midpoint = midpoint;
        e->esharp = esharp;//cat has while loop doo not
        e->adjedges.clear();
        e->adjfaces.clear();
    }
    else
    {
        e->index1 = index1;
        e->index2 = index2;
        e->esharp = esharp;
        e->midpoint = midpoint;
        e->adjedges.clear();
        for(int i = 0; i < adjedges.size(); i++)
        {
            e->adjedges.append(adjedges.at(i));
        }
        e->adjfaces.clear();
        for(int i = 0; i < adjfaces.size(); i++)
        {
            e->adjfaces.append(adjfaces.at(i));
        }
    }
}
BaseFace::BaseFace()
{
    edgeoff = NULL;
}



BaseFace::~BaseFace()
{
    /*if(edgeoff != NULL)
        delete edgeoff;//here will truely delete memory edgeoff points? Needn't a loop to delete?
    // think it carefully here may cause memory leak
    */
    int i;
    for(i=0; i < edgeoff->size(); i++)
    {
        if((*edgeoff)[i] != NULL)
            delete (*edgeoff)[i];
    }
    if(edgeoff != NULL)
    delete edgeoff;
}

void BaseFace::insertEdge(int index, int ivert0, int adjface, int adjedge)
{
    BaseEdgeDesc *edge = new BaseEdgeDesc;
    edge->index = index;
    edge->ivert0 = ivert0;
    edge->adjf = adjface;
    edge->adje = adjedge;
    //edgeoff->insert(edgeoff->end(), edge);
    edgeoff->push_back(edge);
}

void BaseFace::copy(BaseFace *f)
{
    int i;
    BaseEdgeDesc *edge;
    if(f->edgeoff != NULL)
    {
        for(i=0; i < f->edgeoff->size(); i++)
        {
            if((*f->edgeoff)[i] != NULL)
                delete (*f->edgeoff)[i];
        }
        delete f->edgeoff;
    }

    f->fpoint = fpoint;
    f->edgeoff = new QList<BaseEdgeDesc*>;

    for(i = 0; i < edgeoff->size(); ++i)
    {
        edge = new BaseEdgeDesc;
        edge->index = (*edgeoff)[i]->index;
        edge->ivert0 = (*edgeoff)[i]->ivert0;
        edge->adjf = (*edgeoff)[i]->adjf;
        edge->adje = (*edgeoff)[i]->adje;

        //f->edgeoff->insert(f->edgeoff->end(), edge);
        f->edgeoff->push_back(edge);
    }
}


