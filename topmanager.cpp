#include "topmanager.h"

Adapter::Adapter()
{
    steps = new QList<BaseTopology*>;
    limitSteps = new QList<BaseTopology*>;
    currentStep = -1;


}

Adapter::~Adapter()
{


    for(int i = 0; i < steps->size(); i++ )
    {
       delete steps->at(i);
    }
    delete steps;
}
QString Adapter::getName()
{
    return name;
}

QString Adapter::getFileName()
{
    return this->filename;
}

BaseTopology* Adapter::getCurrentTopolopy()
{
    //return *std::advance(steps->begin(),currentStep);
    if(currentStep >= steps->size() || currentStep < 0)
    {
        exit(100);
    }
    return steps->at(currentStep);
}

BaseTopology* Adapter::getCurrentLimitTopology()
{
    if(currentStep >= steps->size() || currentStep < 0)
    {
        exit(100);

    }
    return limitSteps->at(currentStep);
}

int Adapter::getCurrentStep()
{
    return currentStep;
}

void Adapter::setCurrentStep(int position)
{
    if(position < 0 || position >= steps->size())
        return;
    currentStep = position;
}

void Adapter::addStep(BaseTopology *top)
{
    //steps->insert(std::advance(steps->begin() , currentStep + 1), top);
    //setModeValid(top);
    steps->insert(currentStep + 1, top);
    limitSteps->insert(currentStep + 1, top);
    currentStep++;
}

void Adapter::addStep(BaseTopology *top, BaseTopology *limit)
{
    steps->insert(currentStep + 1, top);
    limitSteps->insert(currentStep + 1, limit);
    currentStep++;
}

void Adapter::setName(QString name)
{
    this->name = name;
}

void Adapter::setFileName(QString filename)
{
    this->filename = filename;
}

void Adapter::setModeValid(BaseTopology *topo0)
{
    QList<Cat_Vertex*> *verts = (QList<Cat_Vertex*>*)topo0->vertices;
    QList<Cat_Face*> *faces = (QList<Cat_Face*>*)topo0->faces;
    QList<Cat_Edge*> *edges = (QList<Cat_Edge*>*)topo0->edges;
    modeValid.CAT = true;
    modeValid.DOO = true;
    modeValid.LOOP = true;
    for(int i = 0; i < edges->size(); i++)
    {

        if((*edges)[i]->esharp == 1)
        {
            int vindex1, vindex2;
            vindex1 = edges->at(i)->index1;
            vindex2 = edges->at(i)->index2;
            if(verts->at(vindex1)->sharp != 2 || verts->at(vindex2)->sharp != 2)
            {
                modeValid.DOO = false;
                break;
            }
        }
    }

    for(int i = 0; i < faces->size(); i++)
    {
        if((*faces)[i]->edgeoff->size() > 3)
        {
            modeValid.LOOP = false;
            break;
        }
    }

}

TopManager::TopManager(QWidget *parent)
{
    this->parent = parent;
    windows = new QList<Adapter*>;
    currentWindow = -1;
   // windowSize = 0;
}
TopManager::~TopManager()
{
    currentWindow = -1;
    delete windows;//does it delete adapters in the list either?
}

void TopManager::addWindow(Adapter* adapter)
{
    //windows->insert(windows->end(), adapter);
   // windowSize++;
    windows->push_back(adapter);
}

void TopManager::removeWindow(int index)
{
    if(index >= windows->size() || index < 0 )
        return;
    if(index < currentWindow)
        currentWindow--;
    if(index == currentWindow && index == windows->size() - 1)
    {
        currentWindow = 0;
    }
    Adapter* temp =  windows->at(index);
    //windows->erase(windows->begin()+currentWindow);
    windows->removeAt(index);
    delete temp;
    //setCurrentWindow(currentWindow);
}

Adapter* TopManager::getCurrentWindow()
{
    return windows->at(currentWindow);
}
int TopManager::getCurrentWindowIndex()
{
    return currentWindow;
}

void TopManager::setCurrentWindow(int current)
{
    currentWindow = current;
}


QList<int> TopManager::makeVertList(QList<BaseFace *> *facelist, int indexf, QList<BaseEdge *> *e)
{
    int g, size;
    QList<int> V;
    BaseFace *f;
    BaseEdgeDesc *tmp;
    int vert0;

    g = 0;
    f = (*facelist)[indexf];
    size = f->edgeoff->size();

    //go through all the edgedesc of the face
    while(g < size)
    {
        tmp = (*f->edgeoff)[g];
        vert0 = tmp->ivert0;

        //always take the first vertex of the edge
        if(vert0 == 0)
            V.push_back((*e)[tmp->index]->index1);
        else
            V.push_back((*e)[tmp->index]->index2);
        g++;
    }
    return V;
}
void TopManager::mergeWithoutCommons(QList<int> &list1, QList<int> &list2)
{
    for(int i = 0; i < list2.size(); i++)
    {
        if(list1.contains(list2[i]))
        {
            list1.removeAll(list2[i]);
        }
        else
            list1.push_back(list2[i]);
    }
}


void TopManager::edgeMidpoint(BaseTopology *topold, BaseTopology *topnew)
{

    int i;
    int index1, index2;

    Cat_Vertex *v1, *v2, *newv;

    for(i = 0; i < topold->edges->size(); ++i)
    {
        //take the two vertices index of the edge
        index1 = (*topold->edges)[i]->index1;
        v1 = (Cat_Vertex*)(*topold->vertices)[index1];
        index2 = (*topold->edges)[i]->index2;
        v2 = (Cat_Vertex*)(*topold->vertices)[index2];

        //case esharp = 0 -> smooth edge
        if( (*topold->edges)[i]->esharp == 0)
        {
            //take the two face point that share the edge


            //compute the new vertex;
            newv = new Cat_Vertex(topold->dim);
            for(int indexOfDim = 0; indexOfDim < topold->dim; indexOfDim++)
            {
                newv->datas[indexOfDim] = (v1->datas[indexOfDim] + v2->datas[indexOfDim] ) / 2.0;
            }
            newv->sharp = 0;
            newv->faces.clear();
            newv->edges.clear();

            //insert the new vertex into the new topology
            topnew->vertices->push_back(newv);
            (*topnew->edges)[i]->midpoint = topnew->vertices->size() - 1;

            //split the edge into two
            installCatEdge(topnew->edges, topnew->vertices, topnew->vertices->size() - 1, index2);
            (*topnew->edges)[i]->index2 = topnew->vertices->size() - 1;

            (*topnew->vertices)[index1]->edges.push_back(i);//topnew is copy from topold, but the topology information is cleared.
            (topnew->vertices)->back()->edges.push_back(i);

            //clear
            (*topnew->edges)[i]->adjfaces.clear();
            (*topnew->edges)[i + topold->edges->size()]->adjfaces.clear();
        }

        //case esharp = 1 -> crease
        else if( (*topold->edges)[i]->esharp >= 1)
        {
            newv = new Cat_Vertex(topold->dim);

            for(int indexOfDim = 0; indexOfDim < topold->dim; indexOfDim++)
            {
                newv->datas[indexOfDim] = (v1->datas[indexOfDim] + v2->datas[indexOfDim]) / 2.0;
            }
            newv->sharp = 2;
            newv->faces.clear();
            newv->edges.clear();

            //insert the new vertex into the new topology
            topnew->vertices->push_back(newv);
            (*topnew->edges)[i]->midpoint = topnew->vertices->size() - 1;

            installCatEdge(topnew->edges, topnew->vertices, topnew->vertices->size() - 1, index2);

            (*topnew->edges).last()->esharp = 1;
            (*topnew->edges)[i]->index2 = topnew->vertices->size() - 1;
            (*topnew->edges)[i]->esharp = 1;

            //insert the edge list into the two others vertices
            (*topnew->vertices)[index1]->edges.push_back(i);
            (*topnew->vertices).last()->edges.push_back(i);

            (*topnew->edges)[i]->adjfaces.clear();
            (*topnew->edges)[i + topold->edges->size()]->adjfaces.clear();
        }
    }


}

void TopManager::evenPartNonuniformSmoothing(BaseTopology *topold, BaseTopology *topnew, double u)
{
    double N;

    double alf;
    double bta;
    double gama;

    double *vedatas, *vfdatas;
    vedatas = new double[topold->dim];
    vfdatas = new double[topold->dim];

    for(int i = 0; i < topold->vertices->size(); i++)
    {
        if(topold->vertices->at(i)->sharp == 0 || (*topold->vertices)[i]->sharp == 1)
        {
            //first compute the adjacent edge vertices' sum
            for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
            {
                vedatas[dimIndex] = 0.0;
            }
            QList<int> edges = topold->vertices->at(i)->edges;
            N = edges.size();
            alf = ((N - 4)*(1 + 2*u) + N*u*u) /(N*(1 + u)*(1 + u));
            bta = (8.0*u) / (N*N*(1+u)*(1+u));
            gama = (4.0) / (N*N*(1+u)*(1+u));
            QList<int> veindexs;
            for(int j = 0; j < edges.size(); j++)
            {
                int index;
                if(topold->edges->at(edges.at(j))->index1 == i)
                {
                    index = topold->edges->at(edges.at(j))->index2;

                }
                else
                {
                    index = topold->edges->at(edges.at(j))->index1;
                }
                veindexs.push_back(index);
                for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
                {
                    vedatas[dimIndex] += topold->vertices->at(index)->datas[dimIndex];
                }
            }

            //compute the adjacent face vertices (except Ve)
            for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
            {
                vfdatas[dimIndex] = 0.0f;
            }
            QList<int> faces = topold->vertices->at(i)->faces;
            for(int j = 0; j < faces.size(); j++)
            {
                QList<int> vlist = this->makeVertList(topold->faces, faces.at(j), topold->edges);
                for(int z = 0; z < veindexs.size(); z++)
                {
                    int index = vlist.indexOf(veindexs[z]);
                    if(index != -1)
                        vlist.removeAt(index);
                }
                vlist.removeAt(vlist.indexOf(i));
                for(int z = 0; z < vlist.size(); z++)
                {
                    for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
                    {
                        vfdatas[dimIndex] += topold->vertices->at(vlist[z])->datas[dimIndex];
                    }
                }
            }

            //update the vertex;
            for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
            {
                //topnew->vertices->at(i)->datas[dimIndex] = alf * topold->vertices->at(i)->datas[dimIndex] + bta * vedatas[dimIndex] + gama * vfdatas[dimIndex];
                (*topnew->vertices)[i]->datas[dimIndex] = alf * topold->vertices->at(i)->datas[dimIndex] + bta * vedatas[dimIndex] + gama * vfdatas[dimIndex];
            }
            (*topnew->vertices)[i]->sharp = topold->vertices->at(i)->sharp;
        }
        else if(topold->vertices->at(i)->sharp == 2)
        {
            //
            for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
            {
                vedatas[dimIndex] = 0.0;
            }
            QList<int> edges = topold->vertices->at(i)->edges;

            alf = u / (1 + u);
            bta = 1 / (2 * (1 + u));
            for(int j=0; j < edges.size(); j++)
            {
                if(topold->edges->at(edges[j])->esharp == 1)
                {
                    int index;
                    int index1 = topold->edges->at(edges[j])->index1;
                    int index2 = topold->edges->at(edges[j])->index2;
                    if(index1 == i)
                    {
                        index = index2;
                    }
                    else if(index2 == i)
                    {
                        index = index1;
                    }
                    for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
                    {
                        vedatas[dimIndex] += topold->vertices->at(index)->datas[dimIndex];
                    }
                }
            }

            for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
            {
                //topnew->vertices->at(i)->datas[dimIndex] = alf * topold->vertices->at(i)->datas[dimIndex] + bta * vedatas[dimIndex] + gama * vfdatas[dimIndex];
                (*topnew->vertices)[i]->datas[dimIndex] = alf * topold->vertices->at(i)->datas[dimIndex] + bta * vedatas[dimIndex];
            }
            (*topnew->vertices)[i]->sharp = 2;
        }
        else if(topold->vertices->at(i)->sharp == 3)
        {
            for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
            {
                (*topnew->vertices)[i]->datas[dimIndex] = topold->vertices->at(i)->datas[dimIndex];
            }
            (*topnew->vertices)[i]->sharp = topold->vertices->at(i)->sharp;
        }

    }
    delete [] vedatas;
    delete [] vfdatas;
}

void TopManager::uniformSmoothing(BaseTopology *topold, BaseTopology *topnew, bool isEven)
{
    double N;

    double alf;
    double bta;
    double gama;

    double *vedatas, *vfdatas;
    vedatas = new double[topnew->dim];
    vfdatas = new double[topnew->dim];

    for(int i = 0; i < topold->vertices->size(); i++)
    {
        if(topold->vertices->at(i)->sharp == 0 || topold->vertices->at(i)->sharp == 1 || ( topold->vertices->at(i)->sharp == 2 && !isEven))
        {
            //first compute the adjacent edge vertices' sum
            for(int dimIndex = 0; dimIndex < topnew->dim; dimIndex++)
            {
                vedatas[dimIndex] = 0.0;
            }
            QList<int> edges = topold->vertices->at(i)->edges;

            N = edges.size();

            alf = double(N - 3.0)/N;
            bta = 2.0/double(N*N);
            gama = 1.0 / double(N*N);
            QList<int> veindexs;

            for(int j = 0; j < edges.size(); j++)
            {
                int index;
                if(topold->edges->at(edges.at(j))->index1 == i)
                {
                    index = topold->edges->at(edges.at(j))->index2;

                }
                else
                {
                    index = topold->edges->at(edges.at(j))->index1;
                }
                veindexs.push_back(index);
                for(int dimIndex = 0; dimIndex < topnew->dim; dimIndex++)
                {
                    vedatas[dimIndex] += topnew->vertices->at(index)->datas[dimIndex];
                }
            }

            //compute the adjacent face vertices (except Ve)
            for(int dimIndex = 0; dimIndex < topnew->dim; dimIndex++)
            {
                vfdatas[dimIndex] = 0.0f;
            }
            QList<int> faces = topold->vertices->at(i)->faces;
            for(int j = 0; j < faces.size(); j++)
            {
                QList<int> vlist = this->makeVertList(topold->faces, faces.at(j), topold->edges);
                for(int z = 0; z < veindexs.size(); z++)
                {
                    int index = vlist.indexOf(veindexs[z]);
                    if(index != -1)
                        vlist.removeAt(index);
                }
                vlist.removeAt(vlist.indexOf(i));
                for(int z = 0; z < vlist.size(); z++)
                {
                    for(int dimIndex = 0; dimIndex < topnew->dim; dimIndex++)
                    {
                        vfdatas[dimIndex] += topnew->vertices->at(vlist[z])->datas[dimIndex];
                    }
                }
            }

            //update the vertex;
            for(int dimIndex = 0; dimIndex < topnew->dim; dimIndex++)
            {
                //topnew->vertices->at(i)->datas[dimIndex] = alf * topnew->vertices->at(i)->datas[dimIndex] + bta * vedatas[dimIndex] + gama * vfdatas[dimIndex];
                (*topnew->vertices)[i]->datas[dimIndex] = alf * topnew->vertices->at(i)->datas[dimIndex] + bta * vedatas[dimIndex] + gama * vfdatas[dimIndex];

            }
        }
        else if(topold->vertices->at(i)->sharp == 2 && isEven)
        {
            for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
            {
                vedatas[dimIndex] = 0.0;
            }
            QList<int> edges = topold->vertices->at(i)->edges;

            alf = 0.5;
            bta = 0.25;
            for(int j=0; j < edges.size(); j++)
            {
                if(topold->edges->at(edges[j])->esharp == 1)
                {
                    int index;
                    int index1 = topold->edges->at(edges[j])->index1;
                    int index2 = topold->edges->at(edges[j])->index2;
                    if(index1 == i)
                    {
                        index = index2;
                    }
                    else if(index2 == i)
                    {
                        index = index1;
                    }
                    for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
                    {
                        vedatas[dimIndex] += topold->vertices->at(index)->datas[dimIndex];
                    }
                }
            }

            for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
            {
                //topnew->vertices->at(i)->datas[dimIndex] = alf * topold->vertices->at(i)->datas[dimIndex] + bta * vedatas[dimIndex] + gama * vfdatas[dimIndex];
                (*topnew->vertices)[i]->datas[dimIndex] = alf * topold->vertices->at(i)->datas[dimIndex] + bta * vedatas[dimIndex];
            }
            (*topnew->vertices)[i]->sharp = 2;
        }

        else if(topold->vertices->at(i)->sharp == 3)
        {
            for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
            {
                (*topnew->vertices)[i]->datas[dimIndex] = topold->vertices->at(i)->datas[dimIndex];
            }
            (*topnew->vertices)[i]->sharp = topold->vertices->at(i)->sharp;
        }
    }
    delete [] vedatas;
    delete [] vfdatas;
}


void TopManager::evenGeneralizedSubdivision(BaseTopology *topold, BaseTopology *topnew, int k, double u)
{
    createCatFaceFace(topold, topnew);
    //createCatEdgeFace(topold, topnew);
    edgeMidpoint(topold, topnew);
    //createCatVertexFace(topold, topnew);
    createCatTopo(topold, topnew);
    topnew->noffaces = topnew->faces->size();
    topnew->nofvert = topnew->vertices->size();

    BaseTopology *toptemp = new BaseTopology(cat);
    topnew->copy(toptemp, true);

    evenPartNonuniformSmoothing(toptemp, topnew, u);
    if(k >= 6)
    {
        for(int i = 0; i < (k-4)/2; i++)
        {
            if(toptemp != NULL)
                delete toptemp;
            toptemp = new BaseTopology(cat);
            topnew->copy(toptemp, true);
            this->uniformSmoothing(topnew, topnew, u);
        }
    }
//    if(toptemp != NULL)
//    {
//        delete toptemp;
//    }



}
void TopManager::oddPartNonuniformSmoothing(BaseTopology *topold, BaseTopology *topnew, double u)
{
    double N;
    double alf;
    double beta;
    double gama;
    double *vedatas, *vfdatas;
    vedatas = new double[topold->dim];
    vfdatas = new double[topold->dim];
    int i, j, k = -1;
    int fsize, index, size;
    int edgeindex, sindex;
    double sum;
    QList<int> vlist;
    QList<int> vflist;
    BaseVertex  *newv;
    BaseFace *face;

    BaseTopology *toptemp = new BaseTopology(doo);

    topold->copy(toptemp, true);
    for(int i = 0; i < topold->vertices->size(); i++)
    {
        (*topold->vertices)[i]->edges.clear();
        (*topold->vertices)[i]->faces.clear();
    }
    for(int i = 0; i < topold->edges->size(); i++)
    {
        (*topold->edges)[i]->adjedges.clear();
        (*topold->edges)[i]->adjfaces.clear();
    }

    fsize = topold->faces->size();

    for(i = 0; i < fsize; ++i)
    {

        vlist = makeVertList(toptemp->faces, i, toptemp->edges);



        //compute the new vertices of the face and create a new face
        face = new BaseFace;
        face->edgeoff = new QList<BaseEdgeDesc*>;
        size = vlist.size();

        for(j = 0; j < size; ++j)
        {

            if(toptemp->vertices->at(vlist[j])->sharp == 0 || toptemp->vertices->at(vlist[j])->sharp == 1 || toptemp->vertices->at(vlist[j])->sharp == 2)
            {
                N = size;
                alf = (N*u*u + (2*N-4)*u + 1)/(N*(1+u)*(1+u));
                beta = (1+2*u)/(N*(1+u)*(1+u));
                gama = 1/(N*(1+u)*(1+u));
//                N = size;
//                alf = 0.5 + 1.0 / (N * (1.0 + u) * (1.0+u));
//                beta = 1.0/8.0 + 1.0 / (N * (1.0 + u) * (1.0 + u));
//                gama = 1.0 / (N * (1.0+u) * (1.0+u));
//                N = size;
//                alf =(2.0 * N + 1)/(4.0*N);
//                beta = (N*N + 2.0*N)/(8.0*N*N);
//                gama = 1.0 / (4.0*N);

                vflist = makeVertList(toptemp->faces, i, toptemp->edges);
                newv = new BaseVertex(toptemp->dim);

                QList<int> veindexs;
                QList<int> edges = toptemp->vertices->at(vlist[j])->edges;
                for(int dimIndex = 0; dimIndex < toptemp->dim; dimIndex++)
                {
                    vedatas[dimIndex] = 0.0;
                }
                for(int z = 0; z < edges.size(); z++)
                {
                    int index;
                    if(toptemp->edges->at(edges.at(z))->index1 == vlist[j])
                    {
                        index = toptemp->edges->at(edges.at(z))->index2;

                    }
                    else if(toptemp->edges->at(edges.at(z))->index2 == vlist[j])
                    {
                        index = toptemp->edges->at(edges.at(z))->index1;
                    }
                    if(vlist.indexOf(index) != -1 && veindexs.indexOf(index) == -1)
                    {
                        veindexs.push_back(index);

                    }
                }
                for(int z = 0; z < veindexs.size(); z++)
                {
                    for(int dimIndex = 0; dimIndex < toptemp->dim; dimIndex++)
                    {
                        vedatas[dimIndex] += toptemp->vertices->at(veindexs[z])->datas[dimIndex];
                    }
                }

                //compute the adjacent face vertices (except Ve)
                for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
                {
                    vfdatas[dimIndex] = 0.0f;
                }



                for(int z = 0; z < veindexs.size(); z++)
                {
                    int index = vflist.indexOf(veindexs[z]);
                    if(index != -1)
                        vflist.removeAt(index);
                }

                vflist.removeAt(vflist.indexOf(vlist[j]));
                for(int z = 0; z < vflist.size(); z++)
                {
                    for(int dimIndex = 0; dimIndex < toptemp->dim; dimIndex++)
                    {
                        vfdatas[dimIndex] += toptemp->vertices->at(vflist[z])->datas[dimIndex];
                    }

                }


                // the new vertex
                for(int dimIndex = 0; dimIndex < toptemp->dim; dimIndex++)
                {
                    newv->datas[dimIndex] = alf * toptemp->vertices->at(vlist[j])->datas[dimIndex] +  beta * vedatas[dimIndex] +  gama * vfdatas[dimIndex];
                }
                newv->sharp = (*toptemp->vertices)[vlist[j]]->sharp;
            }
            /*
            else if(topold->vertices->at(vlist[j])->sharp == 2)
            {
                newv = new BaseVertex(topold->dim);
                for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
                {
                    vedatas[dimIndex] = 0.0;
                }
                QList<int> edges = topold->vertices->at(vlist[j])->edges;

                alf = u / (1 + u);
                beta = 1 / (2 * (1 + u));
                for(int z=0; z < edges.size(); j++)
                {
                    if(topold->edges->at(edges[z])->esharp == 1)
                    {
                        int index;
                        int index1 = topold->edges->at(edges[z])->index1;
                        int index2 = topold->edges->at(edges[z])->index2;
                        if(index1 == vlist[j])
                        {
                            index = index2;
                        }
                        else if(index2 == vlist[j])
                        {
                            index = index1;
                        }
                        if(vlist.indexOf(index) != -1)
                        {
                            for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
                            {
                                vedatas[dimIndex] += topold->vertices->at(index)->datas[dimIndex];
                            }
                        }
                    }
                }

                for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
                {
                    //topnew->vertices->at(i)->datas[dimIndex] = alf * topold->vertices->at(i)->datas[dimIndex] + bta * vedatas[dimIndex] + gama * vfdatas[dimIndex];
                    newv->datas[dimIndex] = alf * topold->vertices->at(vlist[j])->datas[dimIndex] + beta * vedatas[dimIndex];
                }
                newv->sharp = 2;
            }*/
            else if(toptemp->vertices->at(vlist[j])->sharp >= 3)
            {
                newv = new BaseVertex(toptemp->dim);
                for(int indexOfDim = 0; indexOfDim < newv->dim; indexOfDim++)
                {
                    newv->datas[indexOfDim] = (*toptemp->vertices)[vlist[j]]->datas[indexOfDim];
                }
                newv->sharp = (*toptemp->vertices)[vlist[j]]->sharp;
            }

            // newv->faces.insert(newv->faces.end(), i);
            newv->faces.push_back(i);



            //insert the new vertex in the new topolopy
            //topnew->vertices->insert(topnew->vertices->end(), newv);
            topnew->vertices->push_back(newv);
            //insert the new edge in the new topolopy and make a new edge in the new face
            if(j > 0)
            {
                installDooEdge(topnew->edges, topnew->vertices, topnew->vertices->size()-2, topnew->vertices->size()-1);
                if(topnew->vertices->at(topnew->vertices->size() - 2)->sharp == 2 && topnew->vertices->last()->sharp == 2)
                    topnew->edges->last()->esharp = 1;
                installEdgeOfFace(face, topnew->edges->size() - 1, 0, -1, -1);
                //(*topnew->edges)[topnew->edges->size() - 1]->adjfaces.insert((*topnew->edges)[topnew->edges->size() - 1]->adjfaces.end(), i);
                (*topnew->edges)[topnew->edges->size() - 1]->adjfaces.push_back(i);
                //insert in the old edge the 2 new vertices adj
                index = (*(*topold->faces)[i]->edgeoff)[j-1]->index;
                //(*topold->edges)[index]->adjedges.insert((*topold->edges)[index]->adjedges.end(), topnew->edges->size()-1);
                (*topold->edges)[index]->adjedges.push_back(topnew->edges->size()-1);
                //insert the edge index to the two vertices// it has done in install the edge
//                (*topnew->vertices)[topnew->vertices->size() - 2]->edges.push_back(topnew->edges->size()-1);
//                (*topnew->vertices)[topnew->vertices->size() - 1]->edges.push_back(topnew->edges->size()-1);
                //(*topnew->vertices)[topnew->vertices->size() - 2]->edges.insert((*topnew->vertices)[topnew->vertices->size() -2]->edges.end(), topnew->edges->size() - 1);
                //(*topnew->vertices)[topnew->vertices->size() - 1]->edges.insert((*topnew->vertices)[topnew->vertices->size() -1]->edges.end(), topnew->edges->size() - 1);
            }

            //update the topold
//            for(int dimIndex = 0; dimIndex < toptemp->dim; dimIndex++)
//            {
//                (*toptemp->vertices)[vlist[j]]->datas[dimIndex] = newv->datas[dimIndex];
//            }

        }


        //insert the last edge
        //insert the edge in new face
        installDooEdge(topnew->edges, topnew->vertices, topnew->vertices->size() - 1, topnew->vertices->size()-size);
        if(topnew->vertices->at(topnew->vertices->size() - size)->sharp == 2 && topnew->vertices->last()->sharp == 2)
            topnew->edges->last()->esharp = 1;
        installEdgeOfFace(face, topnew->edges->size()-1,0,-1,-1);
       // (*topnew->edges)[topnew->edges->size()-1]->adjfaces.insert((*topnew->edges)[topnew->edges->size()-1]->adjfaces.end(), i);
        (*topnew->edges)[topnew->edges->size()-1]->adjfaces.push_back(i);

        //Insert in the old edge the 2 new vertices adj
        index = topold->faces->at(i)->edgeoff->at(j - 1)->index;
        (*topold->edges)[index]->adjedges.push_back(topnew->edges->size() - 1);
        //insert the edge index to the two vertices
        //(*topnew->vertices)[topnew->vertices->size() -1]->edges.insert((*topnew->vertices)[topnew->vertices->size()-1]->edges.end(), topnew->edges->size()-1);
        //(*topnew->vertices)[topnew->vertices->size() -size]->edges.insert((*topnew->vertices)[topnew->vertices->size()-size]->edges.end(), topnew->edges->size()-1);
//        (*topnew->vertices)[topnew->vertices->size() -1]->edges.push_back(topnew->edges->size()-1);
//        (*topnew->vertices)[topnew->vertices->size() -size]->edges.push_back(topnew->edges->size()-1);
        //insert the new face in the new topolopy
        //topnew->faces->insert(topnew->faces->end(), face);
        topnew->faces->push_back(face);
    }
    delete [] vedatas;
    delete [] vfdatas;
    delete toptemp;
}


void TopManager::oddGeneralizedSubdivision(BaseTopology *topold, BaseTopology *topnew, int k, double u)
{

    topnew->vertices = new QList<BaseVertex*>;
    topnew->edges = new QList<BaseEdge*>;
    topnew->faces = new QList<BaseFace*>;
    topnew->name = topold->name;
    this->oddPartNonuniformSmoothing(topold, topnew, u);
    createDooEdgeFace(topold, topnew);
    createDooVertexFace(topold, topnew);



    if( k >= 5)
    {
        for(int i = 0; i < (k-3)/2; i++)
        {
            BaseTopology *toptemp = new BaseTopology(doo);
            topnew->copy(toptemp, true);
            this->uniformSmoothing(toptemp, topnew, u);
            delete toptemp;
        }
    }

    topnew->nofvert = topnew->vertices->size();
    topnew->noffaces = topnew->faces->size();


}

void TopManager::loopMakeAlpha(BaseTopology *topo)
{
    int i, n;

    for( i = 0; i < topo->vertices->size(); i++)
    {
        Loop_Vertex *temp = (Loop_Vertex*)( (*topo->vertices)[i]);
        n = (*topo->vertices)[i]->faces.size();
        (*topo->vertices)[i]->faces.clear();
        if(n != 0)
        {
          temp ->alpha = ((0.375 + 0.25*cos(2*PI/n)) * (0.375 + 0.25*cos(2*PI/n)) + 0.375) / 2;
           for(int dimindex = 0; dimindex < topo->dim; dimindex++)
           {
               temp->datas[dimindex] = temp->alpha * 2 * temp->datas[dimindex];
           }
        }
    }
}

void TopManager::createLoopVertexFace(BaseTopology *topold, BaseTopology *topnew)
{
    int i, j, k, l, m, n, e, nv[2], edgeindex;
    QList<int> v;
    for(i = 0; i < topold->faces->size(); i++)
    {
        v = makeVertList(topold->faces, i, topold->edges);

        for(j = 0; j < v.size(); j++)
        {
            Loop_Vertex *temp = (Loop_Vertex*)((*topnew->vertices)[v[j]]);

                    if(((*topold->vertices)[v[j]]->sharp == 0) || ((*topold->vertices)[v[j]]->sharp == 1))
                    {
                        for( k = 0; k < v.size(); k++)
                        {
                            n = (*topold->vertices)[v[j]]->faces.size();
                            if(j != k)
                            {
                                for(int dimindex = 0; dimindex < topold->dim; dimindex++)
                                {
                                    (*topnew->vertices)[v[j]]->datas[dimindex] += (0.5 - temp->alpha) * (*topold->vertices)[v[k]]->datas[dimindex] / n;
                                }
                            }
                        }
                    }
                    else if((*topold->vertices)[v[j]]->sharp == 2)
                    {
                        e = (*topold->vertices)[v[j]]->edges.size();
                        m = 0;
                        for(l = 0; l < e; l++)
                        {
                            edgeindex = (*topold->vertices)[v[j]]->edges[l];
                            if((*topold->edges)[edgeindex]->esharp == 1)
                            {
                                if((*topold->edges)[edgeindex]->index1 == v[j])
                                {
                                    nv[m] = (*topold->edges)[edgeindex]->index2;
                                    m++;
                                }
                                else if((*topold->edges)[edgeindex]->index2 == v[j])
                                {
                                    nv[m] = (*topold->edges)[edgeindex]->index1;
                                    m++;
                                }
                            }
                        }
                        for(int dimindex = 0; dimindex < topold->dim; dimindex++)
                        {
                            temp->datas[dimindex] = (6 * (*topold->vertices)[v[j]]->datas[dimindex] + (*topold->vertices)[nv[0]]->datas[dimindex] + (*topold->vertices)[nv[1]]->datas[dimindex]) / 8;
                        }
                        temp->alpha = 2;
                    }
                    else if (((*topold->vertices)[v[j]]->sharp == 3) || ((*topold->vertices)[v[j]]->sharp == 4))
                    {
                        temp->sharp = 3;
                        for(int dimindex = 0; dimindex < topold->dim; dimindex++)
                        {
                            temp->datas[dimindex] = (*topold->vertices)[v[j]]->datas[dimindex];
                        }
                    }


        }
    }
}

void TopManager::createLoopEdgeFace(BaseTopology *topold, BaseTopology *topnew)
{
    int i, j;
    int index1, index2, indexold, indexnew;
    int tmp1, tmp2;
    QList<int> vlist0, vlist1;
    Loop_Vertex *v1, *v2, *v3, *v4, *newv;

    for(i = 0; i < topold->edges->size(); ++i)
    {
        index1 = (*topold->edges)[i]->index1;
        v1 = (Loop_Vertex*)(*topold->vertices)[index1];
        index2 = (*topold->edges)[i]->index2;
        v2 = (Loop_Vertex*)(*topold->vertices)[index2];

        newv = new Loop_Vertex;

        if((*topold->edges)[i]->esharp != 1)
        {
            tmp1 = (*topold->edges)[i]->adjfaces[0];
            tmp2 = (*topold->edges)[i]->adjfaces[1];
            vlist1.clear();
            vlist1 = makeVertList(topold->faces, tmp1, topold->edges);
            vlist0 = makeVertList(topold->faces, tmp2, topold->edges);
            mergeWithoutCommons(vlist1, vlist0);

            //v3 and v4 contains the other two vertices of the edge-mask
            v3 = (Loop_Vertex*)(*topold->vertices)[vlist1[0]];
            v4 = (Loop_Vertex*)(*topold->vertices)[vlist1[1]];

            for(int dimindex = 0; dimindex < topold->dim; dimindex++)
            {
                newv->datas[dimindex] = (6 * v1->datas[dimindex] + 6 * v2->datas[dimindex] + 2 * v3->datas[dimindex] + 2 * v4->datas[dimindex]) / 16;
            }
            newv->faces.clear();
            //Insert the new vertex into the new topology
            topnew->vertices->push_back(newv);
            //save the idnex of this new vertex
            indexnew = topnew->vertices->size() - 1;

            //modifiy the old edge in the new topology
            (*topnew->edges)[i]->index2 = indexnew;
            indexold = (*topnew->edges)[i]->index1;

            (*topnew->vertices)[indexold]->edges.push_back(i);
            (*topnew->vertices)[indexnew]->edges.push_back(i);
            (*topnew->edges)[i]->adjfaces.clear();
            //insert the new edge into the new topology
            installLoopEdge(topnew->edges, topnew->vertices, indexnew, index2);
        }
        else if((*topold->edges)[i]->esharp == 1)
        {
            for(int dimindex = 0; dimindex < topold->dim; dimindex++)
            {
                newv->datas[dimindex] = (v1->datas[dimindex] + v2->datas[dimindex]) / 2;
            }
            newv->sharp = 3;
            newv->faces.clear();

            topnew->vertices->push_back(newv);

            indexnew = topnew->vertices->size() - 1;
            (*topnew->edges)[i]->index2 = indexnew;
            indexold = (*topnew->edges)[i]->index1;
            (*topnew->edges)[i]->esharp = 1;

            (*topnew->vertices)[indexold]->edges.push_back(i);
            (*topnew->vertices)[indexnew]->edges.push_back(i);
            (*topnew->edges)[i]->adjfaces.clear();

            installLoopEdge(topnew->edges, topnew->vertices, indexnew, index2);
            (*topnew->edges).back()->esharp = 1;
        }
    }
}

void TopManager::createLoopFaceFace(BaseTopology *topold, BaseTopology *topnew)
{
    int i, j ,h;
    int fsize;
    int comp1, comp3, test, ivert0, adjae;
    int vindex;
    QList<int> listo, listn;
    Loop_Face *face;

    fsize = topold->faces->size();
    for( i = 0; i < fsize; i++)
    {
        listo = makeVertList(topold->faces, i, topold->edges);

        listn.clear();
        for( h = 0; h < (*topold->faces)[i]->edgeoff->size(); h++)
        {
            vindex = (*(*topold->faces)[i]->edgeoff)[h]->index;
            listn.push_back((*topnew->edges)[vindex]->index2);
        }

        for(j = 0; j < listn.size(); j++)
        {
            face = new Loop_Face;
            face->edgeoff = new QList<BaseEdgeDesc*>;

            comp1 = BaseTopology::adjacentFace((*topnew->vertices)[listo[j]], (*topnew->vertices)[listn[j]], i);
            comp3 = BaseTopology::adjacentFace((*topnew->vertices)[listn[(j+2) % listn.size()]], (*topnew->vertices)[listo[j]], i);

            (*topnew->vertices)[listo[j]]->insertFace(i + fsize + 2*i + j);
            (*topnew->vertices)[listn[j]]->insertFace(i + fsize + 2 * i + j);
            (*topnew->vertices)[listn[(j+2)%listn.size()]]->insertFace(i + fsize + 2 * i + j);

            //update the old face vertices
            (*topnew->vertices)[listn[j]]->insertFace(i);
            (*topnew->vertices)[listn[(j+2) % listn.size()]]->insertFace(i);

            if(comp1 == i)
                comp1 = -1;
            test = BaseTopology::equalEdge(topnew->edges, topnew->vertices, listo[j], listn[j]);
            if((*topnew->edges)[test]->index1 == listo[j])
                ivert0 = 0;
            else
                ivert0 = 1;
            if(comp1 != -1)
                adjae = BaseTopology::updateAdjEdge((*topnew->faces)[comp1], test, i + fsize + 2*i + j, 0);
            else
                adjae = -1;

            //update the adjacent face
            (*topnew->edges)[test]->adjfaces.push_back(i + fsize + 2*i + j);

            //insert the first edge to the new face
            installEdgeOfFace(face, test, ivert0, comp1, adjae);

            //the second edge
            test = topnew->edges->size();
            //create the second edge
            installLoopEdge(topnew->edges, topnew->vertices, listn[j], listn[(j+2)%listn.size()]);
            //update the pareameters of the second edge
            adjae = j;
            ivert0 = 0;
            (*topnew->edges)[test]->adjfaces.clear();
            (*topnew->edges)[test]->adjfaces.push_back(i + fsize + 2 * i + j);
            (*topnew->edges)[test]->adjfaces.push_back(i);

            //insert the second edge to the new face
            installEdgeOfFace(face, test, ivert0, i, adjae);
            //modify the old face (now become the face composed of the edge vertices)
            (*(*topnew->faces)[i]->edgeoff)[j]->index = test;
            (*(*topnew->faces)[i]->edgeoff)[j]->ivert0 = 1;
            (*(*topnew->faces)[i]->edgeoff)[j]->adjf = i + fsize + 2*i + j;
            (*(*topnew->faces)[i]->edgeoff)[j]->adje = 1;

            //the third edge (created in the newedge function) same as the first edge
            if(comp3 == i)
                comp3 = -1;
            test = BaseTopology::equalEdge(topnew->edges, topnew->vertices, listn[(j+2)%listn.size()], listo[j]);
            if((*topnew->edges)[test]->index1 == listn[(j+2)%listn.size()])
                ivert0 = 0;
            else
                ivert0 = 1;
            if(comp3 != -1)
                adjae = BaseTopology::updateAdjEdge((*topnew->faces)[comp3], test, i + fsize + 2*i + j, 2);
            else
                adjae = -1;
            (*topnew->edges)[test]->adjfaces.push_back(i + fsize + 2*i + j);
            installEdgeOfFace(face, test, ivert0, comp3, adjae);

            topnew->faces->push_back(face);
        }
    }
}

void TopManager::createLoopLimit(BaseTopology *topnew, BaseTopology *toplimit)
{
    int i, j, k, e, l, m, n, nv[2], edgeindex;
    double fx, fy, fz, ex, ey, ez, alpha, beta, original;
    int index, ind1, ind2, ind;
    int esize;
    int dim;
    double *Rdatas, *Qdatas;
    QList<int> fpoint, epoint;
    double *sum;
    dim = topnew->dim;
    Rdatas = new double[dim];
    Qdatas = new double[dim];
    sum = new double[dim];

    for(i = 0; i < topnew->vertices->size(); i++)
    {
        if((*topnew->vertices)[i]->sharp == 0 || (*topnew->vertices)[i]->sharp == 1)
        {
            for(int dimindex = 0; dimindex < dim; dimindex++)
            {
                sum[dimindex] = 0;
            }
            n = (*topnew->vertices)[i]->edges.size();
            alpha = (0.625 - (0.375 + 0.25 * cos(2 * PI/n)) * (0.375 + 0.25*cos(2*PI/n)))/n;
            beta = 1 / (( 3/ (8 * alpha) + n));
            original = 1 - (n * beta);

            for(j = 0; j < n; j++)
            {
                if((*topnew->edges)[(*topnew->vertices)[i]->edges[j]]->index1 == i)
                    ind = (*topnew->edges)[(*topnew->vertices)[i]->edges[j]]->index2;
                else if((*topnew->edges)[(*topnew->vertices)[i]->edges[j]]->index2 == i)
                    ind = (*topnew->edges)[(*topnew->vertices)[i]->edges[j]]->index1;

                for(int dimindex=0; dimindex < dim; dimindex++)
                {
                    sum[dimindex] += beta * (*topnew->vertices)[ind]->datas[dimindex];
                }
            }
            //compute the new value of the vertex
            for(int dimindex = 0; dimindex < dim; dimindex++)
            {
                (*toplimit->vertices)[i]->datas[dimindex] = sum[dimindex] + (original * (*topnew->vertices)[i]->datas[dimindex]);
            }
            (*toplimit->vertices)[i]->sharp = (*topnew->vertices)[i]->sharp;
        }
        else if((*topnew->vertices)[i]->sharp == 2)
        {
            e = (*toplimit->vertices)[i]->edges.size();
            m=0;
            //find the two neighbour vertices and allocate them to nv[2]
            for(l = 0; l < e; l++)
            {
                edgeindex= (*toplimit->vertices)[i]->edges[l];
                if((*toplimit->edges)[edgeindex]->esharp == 1)
                {
                    if((*toplimit->edges)[edgeindex]->index1 == i)
                    {
                        nv[m] = (*toplimit->edges)[edgeindex]->index2;
                        m++;
                    }
                    else if((*toplimit->edges)[edgeindex]->index2 == i)
                    {
                        nv[m] = (*toplimit->edges)[edgeindex]->index1;
                        m++;
                    }
                }
            }
            //update the crease vertex
            for(int dimindex = 0; dimindex < dim; dimindex++)
            {
                (*toplimit->vertices)[i]->datas[dimindex] = (4 * (*topnew->vertices)[i]->datas[dimindex] + (*topnew->vertices)[nv[0]]->datas[dimindex] + (*topnew->vertices)[nv[1]]->datas[dimindex]) / 6;
            }
            (*toplimit->vertices)[i]->sharp = 2;
        }
        else if((*topnew->vertices)[i]->sharp == 3)
        {
            for(int dimindex = 0; dimindex < dim; dimindex++)
            {
                (*toplimit->vertices)[i]->datas[dimindex] = (*toplimit->vertices)[i]->datas[dimindex];
            }
            (*toplimit->vertices)[i]->sharp = (*toplimit->vertices)[i]->sharp;
        }
    }
}

void TopManager::loopSubdivision(BaseTopology *topold, BaseTopology *topnew, BaseTopology *toplimit)
{
    loopMakeAlpha(topnew);
    createLoopVertexFace(topold, topnew);
    createLoopEdgeFace(topold, topnew);
    createLoopFaceFace(topold, topnew);
    topnew->noffaces = topnew->faces->size();
    topnew->nofvert = topnew->vertices->size();

    loopMakeAlpha(toplimit);
    createLoopVertexFace(topold, toplimit);
    createLoopEdgeFace(topold, toplimit);
    createLoopFaceFace(topold, toplimit);
    toplimit->noffaces = toplimit->faces->size();
    toplimit->nofvert = toplimit->vertices->size();

    createLoopLimit(topnew, toplimit);
}

void TopManager::createCatFaceFace(BaseTopology *topold, BaseTopology *topnew)
{
    int i, j;
    int fsize;
    double *datas;
    QList<int> list;
    Cat_Vertex *facePoint;

    datas = new double[topold->dim];
    fsize = topold->faces->size();

    for(i = 0; i < fsize; i++)
    {
        facePoint = new Cat_Vertex(topold->dim);
        list = makeVertList(topold->faces, i, topold->edges);
        // Compute the new face point
            // It's the average of all Vertices that define the Face
        for(int indexOfDim = 0; indexOfDim < topold->dim; indexOfDim++)
        {
            datas[indexOfDim] = 0;
        }
        for(j = 0; j < list.size(); j++)
        {
            for(int indexOfDim = 0; indexOfDim < topold->dim; indexOfDim++)
            {
                datas[indexOfDim] += (*topold->vertices)[list[j]]->datas[indexOfDim];
            }
        }

        for(int indexOfDim = 0; indexOfDim < topold->dim; indexOfDim++)
        {
            facePoint->datas[indexOfDim] = (datas[indexOfDim] / list.size());
        }
        facePoint->sharp = 0;
        facePoint->edges.clear();
        facePoint->faces.clear();

        topnew->vertices->push_back(facePoint);
        (*topold->faces)[i]->fpoint = topnew->vertices->size() - 1;
    }
    delete [] datas;
}

void TopManager::createCatEdgeFace(BaseTopology *topold, BaseTopology *topnew)
{
    int i;
    int index1, index2;
    int tmp1, tmp2;
    QList<int> vlist0, vlist1;
    Cat_Vertex *v1, *v2, *v3, *v4, *newv;

    for(i = 0; i < topold->edges->size(); ++i)
    {
        //take the two vertices index of the edge
        index1 = (*topold->edges)[i]->index1;
        v1 = (Cat_Vertex*)(*topold->vertices)[index1];
        index2 = (*topold->edges)[i]->index2;
        v2 = (Cat_Vertex*)(*topold->vertices)[index2];

        //case esharp = 0 -> smooth edge
        if( (*topold->edges)[i]->esharp == 0)
        {
            //take the two face point that share the edge
            tmp1 = (*topold->edges)[i]->adjfaces[0];
            tmp2 = (*topold->edges)[i]->adjfaces[1];
            v3 = (Cat_Vertex*)(*topnew->vertices)[(*topold->faces)[tmp1]->fpoint];
            v4 = (Cat_Vertex*)(*topnew->vertices)[(*topold->faces)[tmp2]->fpoint];

            //compute the new vertex;
            newv = new Cat_Vertex(topold->dim);
            for(int indexOfDim = 0; indexOfDim < topold->dim; indexOfDim++)
            {
                newv->datas[indexOfDim] = (v1->datas[indexOfDim] + v2->datas[indexOfDim] + v3->datas[indexOfDim] + v4->datas[indexOfDim]) / 4.0;
            }
            newv->sharp = 0;
            newv->faces.clear();
            newv->edges.clear();

            //insert the new vertex into the new topology
            topnew->vertices->push_back(newv);
            (*topnew->edges)[i]->midpoint = topnew->vertices->size() - 1;

            //split the edge into two
            installCatEdge(topnew->edges, topnew->vertices, topnew->vertices->size() - 1, index2);
            (*topnew->edges)[i]->index2 = topnew->vertices->size() - 1;

            (*topnew->vertices)[index1]->edges.push_back(i);//topnew is copy from topold, but the topology information is cleared.
            (topnew->vertices)->back()->edges.push_back(i);

            //clear
            (*topnew->edges)[i]->adjfaces.clear();
            (*topnew->edges)[i + topold->edges->size()]->adjfaces.clear();
        }

        //case esharp = 1 -> crease
        else if( (*topold->edges)[i]->esharp >= 1)
        {
            newv = new Cat_Vertex(topold->dim);

            for(int indexOfDim = 0; indexOfDim < topold->dim; indexOfDim++)
            {
                newv->datas[indexOfDim] = (v1->datas[indexOfDim] + v2->datas[indexOfDim]) / 2.0;
            }
            newv->sharp = 2;
            newv->faces.clear();
            newv->edges.clear();

            //insert the new vertex into the new topology
            topnew->vertices->push_back(newv);
            (*topnew->edges)[i]->midpoint = topnew->vertices->size() - 1;

            installCatEdge(topnew->edges, topnew->vertices, topnew->vertices->size() - 1, index2);

            (*topnew->edges).last()->esharp = 1;
            (*topnew->edges)[i]->index2 = topnew->vertices->size() - 1;
            (*topnew->edges)[i]->esharp = 1;

            //insert the edge list into the two others vertices
            (*topnew->vertices)[index1]->edges.push_back(i);
            (*topnew->vertices).last()->edges.push_back(i);

            (*topnew->edges)[i]->adjfaces.clear();
            (*topnew->edges)[i + topold->edges->size()]->adjfaces.clear();
        }
    }
}

void TopManager::createCatVertexFace(BaseTopology *topold, BaseTopology *topnew)
{
    int i, j, nv[2], edgeindex;
    double *fdatas, *edatas;
    int index, ind1, ind2;
    int esize;
    double *Rdatas, *Qdatas;
    QList<int> fpoint, epoint;
    fdatas = new double[topold->dim];
    edatas = new double[topold->dim];
    Rdatas = new double[topold->dim];
    Qdatas = new double[topold->dim];

    for(i = 0; i < topold->vertices->size(); i++)
    {
        for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
        {
            fdatas[dimIndex] = 0;
            edatas[dimIndex] = 0;
        }
        //case sharp == 0 or sharp == 1
        if( (*topold->vertices)[i]->sharp == 0 || (*topold->vertices)[i]->sharp == 1)
        {
            for(j = 0; j < (*topold->vertices)[i]->faces.size(); j++)
            {
                index = (*topold->vertices)[i]->faces[j];
                for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
                {
                    fdatas[dimIndex] += (*topnew->vertices)[(*topold->faces)[index]->fpoint]->datas[dimIndex];
                }
            }
                //comput the average of the midpoint of all Edges indicent on the Vertex

            for(j = 0; j < (*topold->vertices)[i]->edges.size(); j++)
            {
                index = (*topold->vertices)[i]->edges[j];
                ind1 = (*topold->edges)[index]->index1;
                ind2 = (*topold->edges)[index]->index2;

                for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
                {
                    edatas[dimIndex] += ( (*topold->vertices)[ind1]->datas[dimIndex] + (*topold->vertices)[ind2]->datas[dimIndex]) / 2.0;
                }
            }
            esize = (*topold->vertices)[i]->edges.size();
            for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
            {
                Qdatas[dimIndex] = fdatas[dimIndex] / (*topold->vertices)[i]->faces.size();
                Rdatas[dimIndex] = 2 * (edatas[dimIndex] / esize);
            }

            //update the vertex;
            for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
            {
                (*topnew->vertices)[i]->datas[dimIndex] = (Qdatas[dimIndex] + Rdatas[dimIndex] + ((esize - 3) * (*topold->vertices)[i]->datas[dimIndex])) / esize;
            }
        }
        else if ( (*topold->vertices)[i]->sharp == 2)
        {
            //find the two neighbour vertices and allocate them to nv[2];
            int m = 0;
            for(j = 0; j < (*topold->vertices)[i]->edges.size(); j++ )
            {
                edgeindex = (*topold->vertices)[i]->edges[j];
                if((*topold->edges)[edgeindex]->esharp >= 1)
                {
                    if((*topold->edges)[edgeindex]->index1 == i)
                    {
                        nv[m] = (*topold->edges)[edgeindex]->index2;
                        m++;
                    }
                    else if((*topold->edges)[edgeindex]->index2 == i)
                    {
                        nv[m] = (*topold->edges)[edgeindex]->index1;
                        m++;
                    }
                }
            }
            //update the crease vertex
           for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
           {
                (*topnew->vertices)[i]->datas[dimIndex] = (6 * (*topold->vertices)[i]->datas[dimIndex] + (*topold->vertices)[nv[0]]->datas[dimIndex] + (*topold->vertices)[nv[1]]->datas[dimIndex]) / 8;
           }
           (*topnew->vertices)[i]->sharp = 2;
        }
        else if((*topold->vertices)[i]->sharp == 3)
        {
            for(int dimIndex = 0; dimIndex < topold->dim; dimIndex++)
            {
                (*topnew->vertices)[i]->datas[dimIndex] = (*topold->vertices)[i]->datas[dimIndex];
            }
            (*topnew->vertices)[i]->sharp = (*topold->vertices)[i]->sharp;
        }
    }
}

void TopManager::createCatTopo(BaseTopology *topold, BaseTopology *topnew)
{
    int i, j, k;
    int fsize, esize, nofedges;
    int ivert0;
    int eindex1;
    int vindex1, vindex3, vindex4;
    int cpt;
    int firstedge;
    QList<int> listo, listn;
    Cat_Face *face, *hilf;

    esize = topold->edges->size();
    //delete the topnew->faces (copy of the topold->faces)
    for(i = 0; i < topnew->faces->size(); i++)
    {
        if((*topnew->faces)[i] != NULL)
            delete (*topnew->faces)[i];
    }
    if(topnew->faces != NULL)
        delete topnew->faces;
    topnew->faces = (QList<BaseFace*>*)new QList<Cat_Face*>;

    //for each face split it into lots of faces;
    fsize = topold->faces->size();
    cpt = -1;
    for( i = 0; i < fsize; i++)
    {
        vindex4 = (*topold->faces)[i]->fpoint;
        nofedges = (*topold->faces)[i]->edgeoff->size();
        firstedge = topnew->edges->size();
        for(j = 0; j < nofedges; ++j)
        {
            cpt++;
            face = new Cat_Face;
            face->edgeoff = new QList<BaseEdgeDesc*>;
            (*topnew->vertices)[vindex4]->faces.push_back(cpt);
            //create the first edge
            eindex1 = (*(*topold->faces)[i]->edgeoff)[j]->index;
            vindex1 = (*topnew->edges)[eindex1]->midpoint;
            installCatEdge(topnew->edges, topnew->vertices, vindex4, vindex1);

            //insert the edge into the face
            if(j == 0)
                installEdgeOfFace(face, topnew->edges->size() - 1, 0, cpt + ((j - 1) + nofedges) % nofedges, 3);
            else
                installEdgeOfFace(face, topnew->edges->size() - 1, 0, cpt - 1, 3);
            //update the adjacent face list
            (*topnew->edges).last()->adjfaces.push_back(cpt);
            if(j == 0)
                (*topnew->edges).last()->adjfaces.push_back(cpt + ((j-1) + nofedges) %nofedges);
            else
                (*topnew->edges).last()->adjfaces.push_back(cpt - 1);

            //insert the 2nd edge
            ivert0 = (*(*topold->faces)[i]->edgeoff)[j]->ivert0;
            if(ivert0 == 0)
            {
                //take the second edge
                //first, check if this edge has already a new adjacent face
                if( (*topnew->edges)[eindex1 + esize]->adjfaces.size() > 0)
                {
                    installEdgeOfFace(face, eindex1 + esize, 0, (*topnew->edges)[eindex1 + esize]->adjfaces[0], 2);
                    hilf = (Cat_Face*)(*topnew->faces)[(*topnew->edges)[eindex1 + esize]->adjfaces[0]];
                    k = 0;
                    while( (*hilf->edgeoff)[k]->index != eindex1 + esize)
                    {
                        k++;
                    }
                    (*hilf->edgeoff)[k]->adjf = cpt;
                    (*hilf->edgeoff)[k]->adje = 1;

                }
                else
                    installEdgeOfFace(face, eindex1 + esize, 0, -1, -1);
                //insert the face index to the edge and the two vertices
                (*topnew->edges)[eindex1 + esize]->adjfaces.push_back(cpt);
                (*topnew->vertices)[vindex1]->faces.push_back(cpt);
            }
            else
            {
                //take the first edge
                //first check if this edge has already a new adjcacent face
                if( (*topnew->edges)[eindex1]->adjfaces.size() > 0)
                {
                    installEdgeOfFace(face, eindex1, 1, (*topnew->edges)[eindex1]->adjfaces[0], 2);
                    hilf = (Cat_Face*)(*topnew->faces)[(*topnew->edges)[eindex1]->adjfaces[0]];
                    k = 0;
                    while( (*hilf->edgeoff)[k]->index != eindex1)
                    {
                        k++;
                    }
                    (*hilf->edgeoff)[k]->adjf =cpt;
                    (*hilf->edgeoff)[k]->adje = 1;
                }
                else
                    installEdgeOfFace(face, eindex1, 1, -1, -1);
                //insert the face index into the edge and the vertex
                (*topnew->edges)[eindex1]->adjfaces.push_back(cpt);
                (*topnew->vertices)[vindex1]->faces.push_back(cpt);
            }

            //insert the 3nd edge
            ivert0 = (*(*topold->faces)[i]->edgeoff)[(j + 1) % nofedges] ->ivert0;
            eindex1 = (*(*topold->faces)[i]->edgeoff)[(j + 1)%nofedges]->index;
            vindex3 = (*topnew->edges)[eindex1]->midpoint;
            if(ivert0 != 0)
            {
                //Take the second edge
                //first, check if this edge has already a new adjacent face
                if( (*topnew->edges)[eindex1 + esize]->adjfaces.size() > 0)
                {
                    installEdgeOfFace(face, eindex1 + esize, 1, (*topnew->edges)[eindex1 + esize]->adjfaces[0], 1);
                    hilf = (Cat_Face*)(*topnew->faces)[(*topnew->edges)[eindex1 + esize]->adjfaces[0]];
                    k = 0;
                    while( (*hilf->edgeoff)[k]->index != eindex1 + esize)
                    {
                        k++;
                    }
                    (*hilf->edgeoff)[k]->adje = 2;
                    (*hilf->edgeoff)[k]->adjf = cpt;
                }
                else
                    installEdgeOfFace(face, eindex1 + esize, 1, -1, -1);

                //insert the face index in the edge and the two vertices
                (*topnew->edges)[eindex1 + esize]->adjfaces.push_back(cpt);
                (*topnew->vertices)[vindex3]->faces.push_back(cpt);
                (*topnew->vertices)[(*topnew->edges)[eindex1 + esize]->index2]->faces.push_back(cpt);
            }
            else
            {
                //take the first edge
                //first, check if this edge has already a new adjacent face
                if((*topnew->edges)[eindex1]->adjfaces.size() > 0)
                {
                    installEdgeOfFace(face, eindex1, 0, (*topnew->edges)[eindex1]->adjfaces[0], 1);
                    hilf = (Cat_Face*)(*topnew->faces)[(*topnew->edges)[eindex1]->adjfaces[0]];
                    k = 0;
                    while( (*hilf->edgeoff)[k]->index != eindex1)
                    {
                        k++;
                    }
                    (*hilf->edgeoff)[k]->adje = 2;
                    (*hilf->edgeoff)[k]->adjf =cpt;
                }
                else
                    installEdgeOfFace(face, eindex1, 0, -1, -1);

                //insert the face index in the edge and in the two vertices
                (*topnew->edges)[eindex1]->adjfaces.push_back(cpt);
                (*topnew->vertices)[vindex3]->faces.push_back(cpt);
                (*topnew->vertices)[(*topnew->edges)[eindex1]->index1]->faces.push_back(cpt);
            }

            //check if it's the last new face creating in the face
            //if yes, change the reference of the last edge
            if( j == (nofedges -1))
                installEdgeOfFace(face, firstedge, 1, cpt -nofedges + 1, 0);
            else
                installEdgeOfFace(face, topnew->edges->size(), 1, cpt + 1, 0);

            //insert the new face in the new topology
            topnew->faces->push_back(face);

        }
    }
}

void TopManager::createCatLimitPosition(BaseTopology *topnew, BaseTopology *toplimit)
{
    int i, j, k, nv[2], edgeindex;
    double *fdatas, *edatas, *Rdatas, *Qdatas, *sum;
    int index, ind1, ind2, ind;
    int esize, fsize;
    QList<int> fpoint, epoint;
    QList<int> list;
    int dimIndex, dim = topnew->dim;

    fdatas = new double[dim];
    edatas = new double[dim];
    Rdatas = new double[dim];
    Qdatas = new double[dim];
    sum = new double[dim];

    for(i = 0; i < topnew->vertices->size(); i++)
    {
        for(dimIndex = 0; dimIndex < dim; dimIndex++)
        {
            fdatas[dimIndex] = 0;
        }
        //case sharp = 0 or sharp == 1
        if( (*topnew->vertices)[i]->sharp == 0 || (*topnew->vertices)[i]->sharp == 1)
        {
            for(j = 0; j < (*topnew->vertices)[i]->faces.size(); j++)
            {
                index = (*topnew->vertices)[i]->faces[j];
                list = makeVertList(topnew->faces, index, topnew->edges);
                for(dimIndex = 0; dimIndex < dim; dimIndex++)
                {
                    sum[dimIndex] = 0;
                }
                for(k = 0; k < list.size(); k++)
                {
                    for(dimIndex = 0; dimIndex < dim; dimIndex++)
                    {
                        sum[dimIndex] += (*topnew->vertices)[list[k]]->datas[dimIndex];
                    }
                }
                for(dimIndex = 0; dimIndex < dim; dimIndex++)
                {
                    sum[dimIndex] /= list.size();
                }

                for(dimIndex = 0; dimIndex < dim; dimIndex++)
                {
                    fdatas[dimIndex] += sum[dimIndex];
                }
            }
            //compute the average of the midpoint of all edges indicent on the vertex
            for(dimIndex = 0; dimIndex < dim; dimIndex++)
            {
                edatas[dimIndex] = 0;
            }
            for( j = 0; j < (*topnew->vertices)[i]->edges.size(); j++)
            {
                index = (*topnew->vertices)[i]->edges[j];
                ind1 = (*topnew->edges)[index]->index1;
                ind2 = (*topnew->edges)[index]->index2;

                if(ind1 == i)
                    ind = ind2;
                else if(ind2 == i)
                    ind = ind1;
                for(dimIndex = 0; dimIndex < dim; dimIndex++)
                {
                    edatas[dimIndex] += (*topnew->vertices)[ind]->datas[dimIndex];
                }
            }

            esize = (*topnew->vertices)[i]->edges.size();
            for(dimIndex = 0; dimIndex < dim; dimIndex++)
            {
                Qdatas[dimIndex] = fdatas[dimIndex] / esize;
                Rdatas[dimIndex] = 4 * (edatas[dimIndex] / esize);
                //compute the new value of the vertex

                (*toplimit->vertices)[i]->datas[dimIndex] = (Qdatas[dimIndex] + Rdatas[dimIndex] + ( esize * (*topnew->vertices)[i]->datas[dimIndex])) / (esize + 5);
            }
           (*toplimit->vertices)[i]->sharp = (*topnew->vertices)[i]->sharp;
        }

        //case sharp = 2
        else if( (*topnew->vertices)[i]->sharp == 2) //should be topnew
        {
            esize = (*topnew->vertices)[i]->edges.size();
            int m = 0;
            //find the two neighbour vertices  which edge's sharp >= 1 and allocate them to nv[2]
            for(int l = 0; l < esize; l++)
            {
                edgeindex = (*topnew->vertices)[i]->edges[l];
                if((*topnew->edges)[edgeindex]->esharp >= 1)
                {
                    if((*topnew->edges)[edgeindex]->index1 == i)
                    {
                        nv[m] = (*topnew->edges)[edgeindex]->index2;
                        m++;
                    }
                    else if( (*topnew->edges)[edgeindex]->index2 == i)
                    {
                        nv[m] = (*topnew->edges)[edgeindex]->index1;
                        m++;
                    }
                }
            }
            //update the crease vertex
            for(dimIndex = 0; dimIndex < dim; dimIndex++)
            {
                (*toplimit->vertices)[i]->datas[dimIndex] = ( 4 * (*topnew->vertices)[i]->datas[dimIndex] + (*topnew->vertices)[nv[0]]->datas[dimIndex] + (*topnew->vertices)[nv[1]]->datas[dimIndex]) / 6;
            }
            (*toplimit->vertices)[i]->sharp = 2;
        }
        else if((*topnew->vertices)[i]->sharp == 3)
        {
            for(dimIndex = 0; dimIndex < dim; dimIndex++)
            {
                (*toplimit->vertices)[i]->datas[dimIndex] = (*topnew->vertices)[i]->datas[dimIndex];
            }
            (*toplimit->vertices)[i]->sharp = (*topnew->vertices)[i]->sharp;
        }
    }
}

// before
void TopManager::catSubdivision(BaseTopology *topold, BaseTopology *topnew, BaseTopology *toplimit)
{
    createCatFaceFace(topold, topnew);
    createCatEdgeFace(topold, topnew);
    createCatVertexFace(topold, topnew);
    createCatTopo(topold, topnew);
    topnew->noffaces = topnew->faces->size();
    topnew->nofvert = topnew->vertices->size();

    createCatFaceFace(topold, toplimit);
    createCatEdgeFace(topold, toplimit);
    createCatVertexFace(topold, toplimit);
    createCatTopo(topold, toplimit);
    toplimit->noffaces = toplimit->faces->size();
    toplimit->nofvert = toplimit->vertices->size();
    createCatLimitPosition(topnew, toplimit);
}


void TopManager::createDooFaceFace(BaseTopology *topold, BaseTopology *topnew)
{
    int i, j, k = -1;
    int fsize, index, size;
    int edgeindex, sindex;
    double sum;
    QList<int> vlist;
    BaseVertex *facePoint, *newv;
    BaseFace *face;

    fsize = topold->faces->size();

    for(i = 0; i < fsize; ++i)
    {
        facePoint = new BaseVertex(topold->dim);
        vlist = makeVertList(topold->faces, i, topold->edges);

        //compute the new face point
        for(int indexOfDim = 0; indexOfDim < topold->dim; indexOfDim++)
        {
            sum = 0;
            for(j = 0; j < vlist.size(); ++j)
            {
                sum += (*topold->vertices)[vlist[j]]->datas[indexOfDim];
            }
            facePoint->datas[indexOfDim] = sum / vlist.size();
        }

        //compute the new vertices of the face and create a new face
        face = new BaseFace;
        face->edgeoff = new QList<BaseEdgeDesc*>;
        size = vlist.size();

        for(j = 0; j < size; ++j)
        {
            newv = new BaseVertex(topold->dim);

            if((*topold->vertices)[vlist[j]]->sharp >= 3)
            {
                for(int indexOfDim = 0; indexOfDim < newv->dim; indexOfDim++)
                {
                    newv->datas[indexOfDim] = (*topold->vertices)[vlist[j]]->datas[indexOfDim];
                }
                newv->sharp = (*topold->vertices)[vlist[j]]->sharp;
            }

//            else if((*topold->vertices)[vlist[j]]->sharp == 2)
//            {
//                int s = topold->edges->size();
//                for (edgeindex = 0; edgeindex < s; edgeindex++)
//                {
//                    if((*topold->edges)[edgeindex]->esharp == 1)
//                    {
//                        if ((*topold->edges)[edgeindex]->index1 == vlist[j])
//                            k = (*topold->edges)[edgeindex]->index2;
//                        else if ((*topold->edges)[edgeindex]->index2 == vlist[j])
//                            k = (*topold->edges)[edgeindex]->index1;

//                        if (k != -1)
//                        {
//                            for(int p = 0; p < vlist.size(); p++)
//                            {
//                                if(k == vlist[p])
//                                    sindex = k;
//                            }
//                        }
//                    }
//                }
//                for(int indexOfDim = 0; indexOfDim < newv->dim; indexOfDim++)
//                {
//                    newv->datas[indexOfDim] = 0.75*(*topold->vertices)[vlist[j]]->datas[indexOfDim] + 0.25*(*topold->vertices)[sindex]->datas[indexOfDim];
//                }

//              newv->sharp = 2;
//            }
            else if(topold->vertices->at(vlist[j])->sharp == 2)
            {
                for(int indexOfDim = 0; indexOfDim < newv->dim; indexOfDim++)
                {
                    newv->datas[indexOfDim] = ( (*topold->vertices)[vlist[(j-1 + size) % size]]->datas[indexOfDim] + (*topold->vertices)[vlist[(j+1)%size]]->datas[indexOfDim] + 4*(*topold->vertices)[vlist[j]]->datas[indexOfDim] + 2*facePoint->datas[indexOfDim]) / 8;
                       // {(j + j-1)/2 + (j + j+1)/2 + j +fp}/4;
                }
                newv->sharp = 2;
            }
            else
            {
                for(int indexOfDim = 0; indexOfDim < newv->dim; indexOfDim++)
                {
                    newv->datas[indexOfDim] = ( (*topold->vertices)[vlist[(j-1 + size) % size]]->datas[indexOfDim] + (*topold->vertices)[vlist[(j+1)%size]]->datas[indexOfDim] + 4*(*topold->vertices)[vlist[j]]->datas[indexOfDim] + 2*facePoint->datas[indexOfDim]) / 8;
                       // {(j + j-1)/2 + (j + j+1)/2 + j +fp}/4;
                }
                newv->sharp = (*topold->vertices)[vlist[j]]->sharp;
            }

           // newv->faces.insert(newv->faces.end(), i);
            newv->faces.push_back(i);

            //insert the new vertex in the new topolopy
            //topnew->vertices->insert(topnew->vertices->end(), newv);
            topnew->vertices->push_back(newv);
            //insert the new edge in the new topolopy and make a new edge in the new face
            if(j > 0)
            {
                installDooEdge(topnew->edges, topnew->vertices, topnew->vertices->size()-2, topnew->vertices->size()-1);
                if(topnew->vertices->at(topnew->vertices->size() - 2)->sharp == 2 && topnew->vertices->last()->sharp == 2)
                    topnew->edges->last()->esharp = 1;
                installEdgeOfFace(face, topnew->edges->size() - 1, 0, -1, -1);
                //(*topnew->edges)[topnew->edges->size() - 1]->adjfaces.insert((*topnew->edges)[topnew->edges->size() - 1]->adjfaces.end(), i);
                (*topnew->edges)[topnew->edges->size() - 1]->adjfaces.push_back(i);
                //insert in the old edge the 2 new vertices adj
                index = (*(*topold->faces)[i]->edgeoff)[j-1]->index;
                //(*topold->edges)[index]->adjedges.insert((*topold->edges)[index]->adjedges.end(), topnew->edges->size()-1);
                (*topold->edges)[index]->adjedges.push_back(topnew->edges->size()-1);
                //insert the edge index to the two vertices
//                (*topnew->vertices)[topnew->vertices->size() - 2]->edges.push_back(topnew->edges->size()-1);
//                (*topnew->vertices)[topnew->vertices->size() - 1]->edges.push_back(topnew->edges->size()-1);
                //(*topnew->vertices)[topnew->vertices->size() - 2]->edges.insert((*topnew->vertices)[topnew->vertices->size() -2]->edges.end(), topnew->edges->size() - 1);
                //(*topnew->vertices)[topnew->vertices->size() - 1]->edges.insert((*topnew->vertices)[topnew->vertices->size() -1]->edges.end(), topnew->edges->size() - 1);
            }

        }

        delete facePoint;
        //insert the last edge
        //insert the edge in new face
        installDooEdge(topnew->edges, topnew->vertices, topnew->vertices->size() - 1, topnew->vertices->size()-size);
        if(topnew->vertices->at(topnew->vertices->size() - size)->sharp == 2 && topnew->vertices->last()->sharp == 2)
            topnew->edges->last()->esharp = 1;
        installEdgeOfFace(face, topnew->edges->size()-1,0,-1,-1);
       // (*topnew->edges)[topnew->edges->size()-1]->adjfaces.insert((*topnew->edges)[topnew->edges->size()-1]->adjfaces.end(), i);
        (*topnew->edges)[topnew->edges->size()-1]->adjfaces.push_back(i);

        //Insert in the old edge the 2 new vertices adj
        index = topold->faces->at(i)->edgeoff->at(j - 1)->index;
        (*topold->edges)[index]->adjedges.push_back(topnew->edges->size() - 1);
        //insert the edge index to the two vertices
        //(*topnew->vertices)[topnew->vertices->size() -1]->edges.insert((*topnew->vertices)[topnew->vertices->size()-1]->edges.end(), topnew->edges->size()-1);
        //(*topnew->vertices)[topnew->vertices->size() -size]->edges.insert((*topnew->vertices)[topnew->vertices->size()-size]->edges.end(), topnew->edges->size()-1);
//        (*topnew->vertices)[topnew->vertices->size() -1]->edges.push_back(topnew->edges->size()-1);
//        (*topnew->vertices)[topnew->vertices->size() -size]->edges.push_back(topnew->edges->size()-1);
        //insert the new face in the new topolopy
        //topnew->faces->insert(topnew->faces->end(), face);
        topnew->faces->push_back(face);
    }
}

void TopManager::createDooEdgeFace(BaseTopology *topold, BaseTopology *topnew)
{
    int i,j;
    int vindex1, vindex2, vindex3, vindex4;
    int ovindex1, ovindex2;
    int eindex1, eindex2;
    int findex;
    BaseFace* face;

    for(i = 0; i < topold->edges->size(); ++i)
    {
        if( (*topold->edges)[i]->esharp != 1)
        {
            //take the two adjacent new edge index
            eindex1 = (*topold->edges)[i]->adjedges[0];
            eindex2 = (*topold->edges)[i]->adjedges[1];

            //take the two old vertex index of the edge
            ovindex1 = (*topold->edges)[i]->index1;
            ovindex2 = (*topold->edges)[i]->index2;
            //1.72232	-1.22232	0.353553
            BaseVertex *temp = topold->vertices->at(ovindex1);
            if(temp->datas[0] == 1.72232 || temp->datas[1] == -1.22232 || temp->datas[2] == 0.353553)
            {
                printf("value is 39");
            }
            temp = topold->vertices->at(ovindex2);
            if(temp->datas[0] == 1.72232 || temp->datas[1] == -1.22232 || temp->datas[2] == 0.353553)
            {
                printf("value is 39");
            }
            if(ovindex1 == 39 || ovindex2 == 39) {
                printf(" here for debug");
            }

            else if(ovindex1 == 41 || ovindex2 == 41) {
                printf(" here for debug");
            }
            else if(ovindex1 == 43 || ovindex2 == 43) {
                printf(" here for debug");
            }
            else if(ovindex1 == 48 || ovindex2 == 48) {
                printf(" here for debug");
            }
            else if(ovindex1 == 50 || ovindex2 == 50) {
                printf(" here for debug");
            }
            else if(ovindex1 == 52 || ovindex2 == 52) {
                printf(" here for debug");
            }
            //take the 4 vertices
            vindex1 = (*topnew->edges)[eindex1]->index1;
            vindex2 = (*topnew->edges)[eindex1]->index2;
            vindex3 = (*topnew->edges)[eindex2]->index1;
            vindex4 = (*topnew->edges)[eindex2]->index2;
            //insert the current creating face's index into it's vertices
            (*topnew->vertices)[vindex1]->faces.insert((*topnew->vertices)[vindex1]->faces.end(), topnew->faces->size());
            (*topnew->vertices)[vindex2]->faces.insert((*topnew->vertices)[vindex2]->faces.end(), topnew->faces->size());
            (*topnew->vertices)[vindex3]->faces.insert((*topnew->vertices)[vindex3]->faces.end(), topnew->faces->size());
            (*topnew->vertices)[vindex4]->faces.insert((*topnew->vertices)[vindex4]->faces.end(), topnew->faces->size());


            //create the first new edges
            installDooEdge(topnew->edges, topnew->vertices, vindex2, vindex3);
            if(topnew->vertices->at(vindex2)->sharp == 2 && topnew->vertices->at(vindex3)->sharp == 2)
                topnew->edges->last()->esharp = 1;
            (*topnew->edges)[topnew->edges->size()-1]->adjfaces.insert((*topnew->edges)[topnew->edges->size()-1]->adjfaces.end(), topnew->faces->size());
//            //insert the new edge's index into the new face's four vertices's edges
//            (*topnew->vertices)[vindex1]->edges.insert((*topnew->vertices)[vindex1]->edges.end(), topnew->edges->size()-1);
//            (*topnew->vertices)[vindex2]->edges.insert((*topnew->vertices)[vindex2]->edges.end(), topnew->edges->size()-1);
//            (*topnew->vertices)[vindex3]->edges.insert((*topnew->vertices)[vindex3]->edges.end(), topnew->edges->size()-1);
//            (*topnew->vertices)[vindex4]->edges.insert((*topnew->vertices)[vindex4]->edges.end(), topnew->edges->size()-1);

            //create the second new edge
            installDooEdge(topnew->edges, topnew->vertices, vindex4,vindex1);
            if(topnew->vertices->at(vindex4)->sharp == 2 && topnew->vertices->at(vindex1)->sharp == 2)
                topnew->edges->last()->esharp = 1;
            (*topnew->edges)[topnew->edges->size()-1]->adjfaces.insert((*topnew->edges)[topnew->edges->size()-1]->adjfaces.end(), topnew->faces->size());
//            //insert the new edge's index into the new face's four vertices's edges
//            (*topnew->vertices)[vindex1]->edges.insert((*topnew->vertices)[vindex1]->edges.end(), topnew->edges->size()-1);
//            (*topnew->vertices)[vindex2]->edges.insert((*topnew->vertices)[vindex2]->edges.end(), topnew->edges->size()-1);
//            (*topnew->vertices)[vindex3]->edges.insert((*topnew->vertices)[vindex3]->edges.end(), topnew->edges->size()-1);
//            (*topnew->vertices)[vindex4]->edges.insert((*topnew->vertices)[vindex4]->edges.end(), topnew->edges->size()-1);

            // add the creating face's index into the two edges created in face-face
            (*topnew->edges)[eindex1]->adjfaces.insert((*topnew->edges)[eindex1]->adjfaces.end(), topnew->faces->size());
            (*topnew->edges)[eindex2]->adjfaces.insert((*topnew->edges)[eindex2]->adjfaces.end(), topnew->faces->size());

            //create the new face
            face = new BaseFace;
            face->edgeoff = new QList<BaseEdgeDesc*>;

            //insert the first edge into the new face
            findex = (*topnew->edges)[eindex1]->adjfaces[0];
            j = 0;
            //find the first age's index in another face's edgeoff, as the argument  adje
            while( (*(*topnew->faces)[findex]->edgeoff)[j]->index != eindex1)
            {
                j++;
            }

            installEdgeOfFace(face, eindex1, 1, findex, j);

            //update information of the adjacent face
            (*(*topnew->faces)[findex]->edgeoff)[j]->adjf = topnew->faces->size();
            (*(*topnew->faces)[findex]->edgeoff)[j]->adje = 0;

            //Insert new edges into the 2 old vertices ( will be used in createVertexFace)
            //Test if the sense is the same or not
            if ( (*(*topnew->faces)[findex]->edgeoff)[j]->ivert0 == (*(*topold->faces)[findex]->edgeoff)[j]->ivert0 )
            {
                (*topold->vertices)[ovindex1]->edges.insert((*topold->vertices)[ovindex1]->edges.end(), topnew->edges->size()-1);
                (*topold->vertices)[ovindex2]->edges.insert((*topold->vertices)[ovindex2]->edges.end(), topnew->edges->size()-2);
            }
            else
            {
                (*topold->vertices)[ovindex1]->edges.insert((*topold->vertices)[ovindex1]->edges.end(), topnew->edges->size()-2);
                (*topold->vertices)[ovindex2]->edges.insert((*topold->vertices)[ovindex2]->edges.end(), topnew->edges->size()-1);

            }

            //insert the second edge to the new face
            installEdgeOfFace(face, topnew->edges->size() - 1, 1, -1, -1);

            //insert the third edge into the new face
            findex = (*topnew->edges)[eindex2]->adjfaces[0];
            j=0;
            while ( (*(*topnew->faces)[findex]->edgeoff)[j]->index != eindex2 )
            {
              j++;
            }

            installEdgeOfFace(face, eindex2, 1, findex, j);
            // Update information of the adjacent face
            (*(*topnew->faces)[findex]->edgeoff)[j]->adjf = topnew->faces->size();
            (*(*topnew->faces)[findex]->edgeoff)[j]->adje = 2;

            // Insert the last edge to the new face
            installEdgeOfFace( face, topnew->edges->size() - 2 , 1, -1, -1);

            // Insert the new face to the new Topology
            topnew->faces->insert (topnew->faces->end(), face);
        }
    }
}

void TopManager::createDooVertexFace(BaseTopology *topold, BaseTopology *topnew)
{
    int i, j, k, h, tmp;
    int eindex;//the index of the edge which compouses the new vertex face
    int findex;// the edge 's adjacent face which is create in edge face
    BaseFace *face;
    //BaseVertex *oldVertex;
    QList<int> a, b, c, index;
    for(i = 0; i < topold->vertices->size(); i++)
    {
       if(topold->vertices->at(i)->sharp != 2)
       {
           //oldVertex = topold->vertices->at(i);
           face = new BaseFace;
           face->edgeoff = new QList<BaseEdgeDesc*>;
           a.clear();
           b.clear();
           c.clear();
           index.clear();

           for(j = 0; j < (*topold->vertices)[i]->edges.size(); ++j)
           {
               //insert the first edge into the new face
               eindex = (*topold->vertices)[i]->edges[j];
               findex = (*topnew->edges)[eindex]->adjfaces[0];
               (*topnew->edges)[eindex]->adjfaces.insert((*topnew->edges)[eindex]->adjfaces.end(), topnew->faces->size());
               //search the edge of eindex 's index in the face of findex

               k = 0;
               while( (*(*topnew->faces)[findex]->edgeoff)[k]->index != eindex)
               {
                   k++;
               }

               if(j == 0)
               {
                   installEdgeOfFace(face, eindex, 0, findex, k);
                   //update information of the adjacent face
                   (*(*topnew->faces)[findex]->edgeoff)[k]->adjf = topnew->faces->size();
                   (*(*topnew->faces)[findex]->edgeoff)[k]->adje = j;
                   index.insert(index.end(), (*topnew->edges)[eindex]->index1);
                   index.insert(index.end(), (*topnew->edges)[eindex]->index2);

               }

               else
               {
                   //save all the values of each verctices
                   a.insert(a.end(), eindex);
                   b.insert(b.end(), findex);
                   c.insert(c.end(), k);

                   index.insert(index.end(), (*topnew->edges)[eindex]->index1);
                   index.insert(index.end(), (*topnew->edges)[eindex]->index2);

               }
           }
           //insert the edges in the right order
           tmp = 1;
           j = 2;
           for(h = 0; h < a.size(); ++h)
           {
               while((index[j] != index[tmp]) || (j == tmp))
               {
                   j = (j + 2)%index.size();
               }

               installEdgeOfFace(face, a[j/2 - 1], 0, b[j/2 - 1], c[j/2 - 1]);// why j/2 -1 ? because index has inserted extra two vertices of the first edge
               //update information of the adjacent face
               (*(*topnew->faces)[b[j/2 -1]]->edgeoff)[c[j/2 -1]]->adjf = topnew->faces->size();
               (*(*topnew->faces)[b[j/2 -1]]->edgeoff)[c[j/2 -1]]->adje = h + 1;
               tmp = j+1;
               j = 2;
           }
           //insert the new face into the new topolopy
           if(face->edgeoff->size() == 0)
           {
               printf("0 edge face\n");
               printf("%d ", i);
           }
           else {
               topnew->faces->insert(topnew->faces->end(), face);
           }

       }
    }
}

void TopManager::dooSubdivision(BaseTopology *topold, BaseTopology *topnew)
{
    int i;

    //delete the old values
    for(i = 0; i < topold->vertices->size(); ++i)
    {
        (*topold->vertices)[i]->edges.clear();
        (*topold->vertices)[i]->faces.clear();
    }

    for(i = 0; i < topold->edges->size(); ++i)
    {
        (*topold->edges)[i]->adjfaces.clear();
        (*topold->edges)[i]->adjedges.clear();
    }

    topnew->vertices = new QList<BaseVertex*>;
    topnew->edges = new QList<BaseEdge*>;
    topnew->faces = new QList<BaseFace*>;
    topnew->name = topold->name;

    createDooFaceFace(topold, topnew);
    createDooEdgeFace(topold, topnew);
    createDooVertexFace(topold, topnew);

    topnew->nofvert = topnew->vertices->size();
    topnew->noffaces = topnew->faces->size();

//    for(i = 0; i < topnew->edges->size(); i++)
//    {
//        index1 = (*topnew->edges)[i]->index1;
//        index2 = (*topnew->edges)[i]->index2;

//        if(((*topnew->vertices)[index1]->sharp == 2) && ((*topnew->vertices)[index2]->sharp == 2))
//                  (*topnew->edges)[i]->esharp = 1;
//    }
}


void TopManager::installLoopVertex(QList<Loop_Vertex *> *v,int dim, const double *dbuf, int sharp0)
{
    //int dim = sizeof(dbuf)/sizeof(double);
    Loop_Vertex *vert;
    vert = new Loop_Vertex(dim);
    vert->create(dbuf,sharp0);
    //v->insert(v->end(), vert);
    v->push_back(vert);
}
void TopManager::installDooVertex(QList<Doo_Vertex *> *v, int dim, const double *dbuf, int sharp0)
{
    Doo_Vertex *vert;
    vert = new Doo_Vertex(dim);
    vert->create(dbuf,sharp0);
    //v->insert(v->end(), vert);
    v->push_back(vert);
}
void TopManager::installCatVertex(QList<Cat_Vertex *> *v, int dim, const double *dbuf, int sharp0)
{
    Cat_Vertex *vert;
    vert = new Cat_Vertex(dim);
    vert->create(dbuf,sharp0);
    //v->insert(v->end(), vert);
    v->push_back(vert);
}
void TopManager::installLoopEdge(QList<BaseEdge *> *e, QList<BaseVertex *> *v, int index1, int index2)
{
    Loop_Edge *edge;
    edge = new Loop_Edge;
    edge->create(index1, index2);
    //e->insert(e->end(),edge);
    //(*v)[index1]->edges.insert((*v)[index1]->edges.end(), e->size()-1);
    //(*v)[index2]->edges.insert((*v)[index2]->edges.end(), e->size()-1);
    e->push_back(edge);
    (*v)[index1]->edges.push_back(e->size() - 1);
    (*v)[index2]->edges.push_back(e->size() - 1);
}
void TopManager::installDooEdge(QList<BaseEdge *> *e, QList<BaseVertex *> *v, int index1, int index2)
{
    Doo_Edge *edge;
    edge = new Doo_Edge;
    edge->create(index1, index2);
    //e->insert(e->end(),edge);
    //(*v)[index1]->edges.insert((*v)[index1]->edges.end(), e->size()-1);
    //(*v)[index2]->edges.insert((*v)[index2]->edges.end(), e->size()-1);
    e->push_back(edge);
    (*v)[index1]->edges.push_back(e->size() - 1);
    (*v)[index2]->edges.push_back(e->size() - 1);
}
void TopManager::installCatEdge(QList<BaseEdge *> *e, QList<BaseVertex *> *v, int index1, int index2)
{
    Cat_Edge *edge;
    edge = new Cat_Edge;
    edge->create(index1, index2);
    //e->insert(e->end(),edge);
    //(*v)[index1]->edges.insert((*v)[index1]->edges.end(), e->size()-1);
    //(*v)[index2]->edges.insert((*v)[index2]->edges.end(), e->size()-1);
    e->push_back(edge);
    (*v)[index1]->edges.push_back(e->size() - 1);
    (*v)[index2]->edges.push_back(e->size() - 1);
}


void TopManager::installEdgeOfFace(BaseFace  *face, int index, int ivert0, int adjface, int adjedge)
{
    BaseEdgeDesc *edge = new BaseEdgeDesc;
    edge->index = index;
    edge->ivert0 = ivert0;
    edge->adjf = adjface;
    edge->adje = adjedge;
    //face->edgeoff->insert(face->edgeoff->end(), edge);
    face->edgeoff->push_back(edge);
}

void TopManager::composeTopology(QTextStream *input, Adapter *adapter, BaseTopology *topology)
{
    int ibuf;
    double *dbuf;
    int *buf;
    int rid1,rid2;
    int adjface, adje;
    BaseFace *face;
    int nofvert;
    int nofedge;
    int noffaces;
    int test;
    int ivert0;
    int nofsvertex;
    int nofsedge;
    int edgeindex;
    int vertexindex1;
    int vertexindex2;
    int vertexid;
    int vertexsharpid;

    if(topology->dim < 3)
        return ;
    dbuf = new double[topology->dim];

    input->skipWhiteSpace();
    *input >> ibuf;
    nofvert = ibuf;
    topology->nofvert = nofvert;
    for(rid1 = 0; rid1 < nofvert; rid1++)
    {
        for(rid2 = 0; rid2 < topology->dim; rid2++)
        {
            input->skipWhiteSpace();
            *input >> dbuf[rid2];
        }
        switch(topology->mode)
        {
        case cat:
            installCatVertex((QList<Cat_Vertex*>*)(topology->vertices),topology->dim, dbuf, 0);
            break;
        case doo:
            installDooVertex((QList<Doo_Vertex*>*)(topology->vertices),topology->dim, dbuf, 0);
            break;
        case loop:
            installLoopVertex((QList<Loop_Vertex*>*)(topology->vertices),topology->dim, dbuf, 0);
            break;
        }


    }

    input->skipWhiteSpace();
    *input >> ibuf;
    noffaces = ibuf;
    topology->noffaces = noffaces;
    for(rid1 = 0; rid1 < noffaces; rid1++)
    {
        switch(topology->mode)
        {
        case doo:
            face = new Doo_Face;
            break;
        case cat:
            face = new Cat_Face;
            break;
        case loop:
            face = new Loop_Face;
            break;
        default:
            face = new BaseFace;
        }


        face->edgeoff = new QList<BaseEdgeDesc*>;
        input->skipWhiteSpace();
        *input >> nofedge;
        buf = new int[nofedge];

        for(rid2 = 0; rid2 < nofedge; rid2++)
        {
            input->skipWhiteSpace();
            *input >> buf[rid2];
        }

        for(rid2 = 0; rid2 < nofedge; rid2++)
        {
            adjface = topology->adjacentFace( (*topology->vertices)[buf[rid2]], (*topology->vertices)[buf[(rid2+1)%nofedge]], rid1);

            (*topology->vertices)[buf[rid2]]->insertFace(rid1);
            (*topology->vertices)[buf[(rid2+1)%nofedge]]->insertFace(rid1);

            if(adjface == rid1)
                adjface = -1;
            test = topology->equalEdge(topology->edges, topology->vertices, buf[rid2], buf[(rid2+1)%nofedge]);
            if(test == topology->edges->size())
            {
                ivert0 = 0;
                switch(topology->mode)
                {
                case doo:
                    installDooEdge(topology->edges, topology->vertices, buf[rid2], buf[(rid2+1)%nofedge]);
                    break;
                case loop:
                    installLoopEdge(topology->edges, topology->vertices, buf[rid2], buf[(rid2+1)%nofedge]);
                    break;
                case cat:
                    installCatEdge(topology->edges, topology->vertices, buf[rid2], buf[(rid2+1)%nofedge]);
                    break;
                }

                //installEdge(topology->edges, topology->vertices, buf[rid2], buf[(rid2+1)%nofedge]);
            }
            else
            {
                if((*topology->edges)[test]->index1 == buf[rid2])
                    ivert0 = 0;
                else
                    ivert0 = 1;
            }

            if(adjface != -1)
                adje = topology->updateAdjEdge((*topology->faces)[adjface], test, rid1, rid2);
            else
                adje = -1;

           // (*topology->edges)[adjface]->adjfaces.insert((*topology->edges)[adjface]->adjfaces.end(), rid1);
            (*topology->edges)[test]->adjfaces.push_back(rid1);
            installEdgeOfFace(face, test, ivert0, adjface, adje);

        }
        //topology->faces->insert(topology->faces->end(), face);
        topology->faces->push_back(face);
    }

    //set the number of sharp feature edges
    input->skipWhiteSpace();
    *input >> ibuf;
    nofsedge = ibuf;
    for(rid1 = 0; rid1 < nofsedge; rid1++)
    {
        input->skipWhiteSpace();
        *input >> vertexindex1;
        input->skipWhiteSpace();
        *input >> vertexindex2;

        for(rid2 = 0; rid2 < (*topology->vertices)[vertexindex1]->edges.size(); rid2++)
        {
            edgeindex = (*topology->vertices)[vertexindex1]->edges[rid2];
            if(((*topology->edges)[edgeindex]->index1 == vertexindex1) && ((*topology->edges)[edgeindex]->index2 == vertexindex2))
            {
                (*topology->edges)[edgeindex]->esharp = 1;
            }
            else if(((*topology->edges)[edgeindex]->index1 == vertexindex2) && ((*topology->edges)[edgeindex]->index2 == vertexindex1))
            {
                (*topology->edges)[edgeindex]->esharp = 1;
            }
        }
    }

    input->skipWhiteSpace();
    *input >> ibuf;
    nofsvertex = ibuf;
    for(rid1 = 0; rid1 < nofsvertex; rid1++)
    {
        input->skipWhiteSpace();
        *input >> vertexid;
        input->skipWhiteSpace();
        *input >> vertexsharpid;

        (*topology->vertices)[vertexid]->sharp = vertexsharpid;
    }
    delete [] buf;
    //insert the topology into the adapter...
    //adapter->setModeValid(topology);
    adapter->addStep(topology);
}

void TopManager::parseSurfaceBase(QTextStream *input, Adapter *adapter, TopMode topMode)
{

    //char *name;
    QString name;
    int dim = 3;

    BaseTopology *topology = new BaseTopology(topMode);
    topology->mode = topMode;
    topology->initLists();

    input->skipWhiteSpace();
    //input->get(cbuf, MAXBUFSIZE,'\n');
    //name = new char[strlen(cbuf) + 1];
    //strcpy(name,cbuf);
    //adapter->setName(input->readLine());
    name = input->readLine();
    //*input >> name; //will break when counter white space, name may contains white space
    topology->name = name;

    input->skipWhiteSpace();
    *input>>dim;
    topology->dim = dim;


    //skip the #ORIGINAL line
    input->skipWhiteSpace();
    input->readLine();

    composeTopology(input, adapter, topology);
    adapter->setModeValid(topology);

    topology = new BaseTopology(topMode);
    topology->mode = topMode;
    topology->initLists();
    topology->dim = dim;
    topology->name = name;

    input->skipWhiteSpace();
    *input >> name;

    if(name.compare("#CURRENT") == 0)
    {
        input->skipWhiteSpace();
        *input >> adapter->stepIndicate;
        composeTopology(input, adapter, topology);
    }
    else if(name.compare("#END") == 0)
    {
        adapter->stepIndicate = 0;
        return;
    }

//    input->skipWhiteSpace();
//    name = input->readLine();

}

void TopManager::drawCatSurface(BaseTopology *surface, float r, float g, float b)
{
    QList<BaseVertex*> *verts = surface->vertices;
    QList<BaseFace*> *faces = surface->faces;
    QList<BaseEdge*> *edges = surface->edges;
    QList<int> list;
    int dim = surface->dim;
    GLfloat d1[dim],d2[dim],norm[3];

    for(int i = 0; i < surface->faces->size(); i++)
    {
        list = makeVertList(faces, i, edges);

        //take three vertices and calculate the normal vertex of the face norm[3];

        int vindex1 = list[0];
        int vindex2 = list[1];
        int vindex3 = list[2];

        for(int dimindex = 0; dimindex < dim; dimindex++)
        {
            d1[dimindex] = (*verts)[vindex1]->datas[dimindex] - (*verts)[vindex2]->datas[dimindex];
            d2[dimindex] = (*verts)[vindex2]->datas[dimindex] - (*verts)[vindex3]->datas[dimindex];
        }

        norm[0] = d1[1] * d2[2] - d1[2] * d2[1];
        norm[1] = d1[2] * d2[0] - d1[0] * d2[2];
        norm[2] = d1[0] * d2[1] - d1[1] * d2[0];

        for(int j=0; j < list.size(); j++)
        {
            for(int index=0; index < 3; index++)
            {
                (*verts)[list[j]]->ndatas[index] = (*verts)[list[j]]->ndatas[index] + norm[index];
            }
        }
    }

    for(int i = 0; i < surface->faces->size(); i++)
    {
        QList<BaseEdgeDesc*> *edgedesc = (*faces)[i]->edgeoff;
        int edgeindex;
        int edgesize = (*faces)[i]->edgeoff->size();

        list = makeVertList(faces, i, edges);

        glBegin(GL_POLYGON);
        for(int j = 0; j < list.size(); j++)
        {
            glColor4f(r, b, b, 0.9f);
            glNormal3f((*verts)[list[j]]->ndatas[0], (*verts)[list[j]]->ndatas[1], (*verts)[list[j]]->ndatas[2]);
            glVertex3f((*verts)[list[j]]->datas[0], (*verts)[list[j]]->datas[1], (*verts)[list[j]]->datas[2]);
        }
        glEnd();

        GLint fill[2];
        glGetIntegerv(GL_POLYGON_MODE, fill);

        if(fill[0] != GL_FILL)
        {
            //check the corner /cusp / crease vertices and draw them in different colors
            for(int j = 0; j < list.size(); j++)
            {
                if(((*verts)[list[j]]->sharp == 3) || ((*verts)[list[j]]->sharp == 4))
                {
                    glPointSize(8);
                    glBegin(GL_POINTS);
                    {
                        glColor3f(0, 0, 1);
                        glVertex3f((*verts)[list[j]]->datas[0], (*verts)[list[j]]->datas[1], (*verts)[list[j]]->datas[2]);

                    }
                    glEnd();
                    glPointSize(1);
                }
                else if((*verts)[list[j]]->sharp == 2)
                {
                    glPointSize(5);
                    glBegin(GL_POINTS);
                    {
                        glColor3f(0, 1, 0);
                        glVertex3f((*verts)[list[j]]->datas[0], (*verts)[list[j]]->datas[1], (*verts)[list[j]]->datas[2]);
                    }
                    glEnd();
                    glPointSize(1);
                }
            }
            //check the crease and draw them in red color
            for(int j = 0; j < edgesize; j++)
            {
                edgeindex = (*edgedesc)[j]->index;
                if((*edges)[edgeindex]->esharp >=1)
                {
                    int vindex1 = (*edges)[edgeindex]->index1;
                    int vindex2 = (*edges)[edgeindex]->index2;

                    glLineWidth(3);
                    glBegin(GL_LINES);
                    {
                        glColor3f(1, 0, 0);
                        glVertex3f((*verts)[vindex1]->datas[0], (*verts)[vindex1]->datas[1], (*verts)[vindex1]->datas[2]);
                        glVertex3f((*verts)[vindex2]->datas[0], (*verts)[vindex2]->datas[1], (*verts)[vindex2]->datas[2]);
                    }
                    glEnd();
                    glLineWidth(1);
                }
            }
        }
    }
}

void TopManager::drawDooSurface(BaseTopology *surface, float r, float g, float b)
{
    QList<BaseVertex*> *verts = surface->vertices;
    QList<BaseFace*> *faces = surface->faces;
    QList<BaseEdge*> *edges = surface->edges;

    QList<int> list;
    int dim = surface->dim;

    GLfloat d1[dim], d2[dim], norm[3];

    for(int i = 0; i < surface->faces->size(); i++)
    {
        list = makeVertList(faces, i, edges);
        //take three vertices and calculate the normal vertex of the face norm[3];

        int vindex1 = list[0];
        int vindex2 = list[1];
        int vindex3 = list[2];

        for(int dimindex = 0; dimindex < dim; dimindex++)
        {
            d1[dimindex] = (*verts)[vindex1]->datas[dimindex] - (*verts)[vindex2]->datas[dimindex];
            d2[dimindex] = (*verts)[vindex2]->datas[dimindex] - (*verts)[vindex3]->datas[dimindex];
        }

        norm[0] = d1[1] * d2[2] - d1[2] * d2[1];
        norm[1] = d1[2] * d2[0] - d1[0] * d2[2];
        norm[2] = d1[0] * d2[1] - d1[1] * d2[0];

        for(int j=0; j < list.size(); j++)
        {
            for(int index=0; index < 3; index++)
            {
                (*verts)[list[j]]->ndatas[index] = (*verts)[list[j]]->ndatas[index] + norm[index];
            }
        }
    }

    for(int i = 0; i < surface->faces->size(); i++)
    {
        list = makeVertList(faces, i, edges);
        QList<BaseEdgeDesc*> *edgedesc = (*faces)[i]->edgeoff;

        int edgesize = (*faces)[i]->edgeoff->size();

        //draw every polygon using OpenGL
        glBegin(GL_POLYGON);
        for(int j=0; j < list.size(); j++)
        {
            glColor4f(r, g, b, 0.9f);
            glNormal3f((*verts)[list[j]]->ndatas[0], (*verts)[list[j]]->ndatas[1], (*verts)[list[j]]->ndatas[2]);
            glVertex3f((*verts)[list[j]]->datas[0], (*verts)[list[j]]->datas[1], (*verts)[list[j]]->datas[2]);
        }
        glEnd();

        GLint fill[2];
        glGetIntegerv(GL_POLYGON_MODE, fill);
        if(fill[0] != GL_FILL)
        {
            for(int j = 0; j < list.size(); j++)
            {
                if((*verts)[list[j]]->sharp == 3)
                {
                    glPointSize(8);
                    glBegin(GL_POINTS);
                        glColor3f(0, 0, 1);
                        glVertex3f((*verts)[list[j]]->datas[0], (*verts)[list[j]]->datas[1], (*verts)[list[j]]->datas[2]);
                    glEnd();
                    glPointSize(1);
                }
            }
        }

    }
}

void TopManager::drawLoopSurface(BaseTopology *surface, float r, float g, float b)
{
    QList<BaseVertex*> *verts = surface->vertices;
    QList<BaseEdge*> *edges = surface->edges;
    QList<BaseFace*> *faces = surface->faces;

    QList<int> list;
    int dim = surface->dim;
    GLfloat d1[dim], d2[dim],norm[3];
    for(int i = 0; i < surface->faces->size(); i++)
    {
        list = makeVertList(faces, i, edges);
        //take three vertices and calculate the normal vertex of the face norm[3];

        int vindex1 = list[0];
        int vindex2 = list[1];
        int vindex3 = list[2];

        for(int dimindex = 0; dimindex < dim; dimindex++)
        {
            d1[dimindex] = (*verts)[vindex1]->datas[dimindex] - (*verts)[vindex2]->datas[dimindex];
            d2[dimindex] = (*verts)[vindex2]->datas[dimindex] - (*verts)[vindex3]->datas[dimindex];
        }

        norm[0] = d1[1] * d2[2] - d1[2] * d2[1];
        norm[1] = d1[2] * d2[0] - d1[0] * d2[2];
        norm[2] = d1[0] * d2[1] - d1[1] * d2[0];

        for(int j=0; j < list.size(); j++)
        {
            for(int index=0; index < 3; index++)
            {
                (*verts)[list[j]]->ndatas[index] = (*verts)[list[j]]->ndatas[index] + norm[index];
            }
        }
    }

    for(int i = 0; i < surface->faces->size(); i++)
    {
        QList<BaseEdgeDesc*> *edgedesc = (*faces)[i]->edgeoff;
        int edgeindex;
        int edgesize = (*faces)[i]->edgeoff->size();

        list = makeVertList(faces, i, edges);

        glBegin(GL_POLYGON);
        for(int j = 0; j < 3; j++)
        {
            glColor4f(r, b, b, 0.9f);
            glNormal3f((*verts)[list[j]]->ndatas[0], (*verts)[list[j]]->ndatas[1], (*verts)[list[j]]->ndatas[2]);
            glVertex3f((*verts)[list[j]]->datas[0], (*verts)[list[j]]->datas[1], (*verts)[list[j]]->datas[2]);
        }
        glEnd();

        GLint fill[2];
        glGetIntegerv(GL_POLYGON_MODE, fill);

        if(fill[0] != GL_FILL)
        {
            //check the corner /cusp / crease vertices and draw them in different colors
            for(int j = 0; j < list.size(); j++)
            {
                if(((*verts)[list[j]]->sharp == 3) || ((*verts)[list[j]]->sharp == 4))
                {
                    glPointSize(8);
                    glBegin(GL_POINTS);
                    {
                        glColor3f(0, 0, 1);
                        glVertex3f((*verts)[list[j]]->datas[0], (*verts)[list[j]]->datas[1], (*verts)[list[j]]->datas[2]);

                    }
                    glEnd();
                    glPointSize(1);
                }
                else if((*verts)[list[j]]->sharp == 2)
                {
                    glPointSize(5);
                    glBegin(GL_POINTS);
                    {
                        glColor3f(0, 1, 0);
                        glVertex3f((*verts)[list[j]]->datas[0], (*verts)[list[j]]->datas[1], (*verts)[list[j]]->datas[2]);
                    }
                    glEnd();
                    glPointSize(1);
                }
            }
            //check the crease and draw them in red color
            for(int j = 0; j < edgesize; j++)
            {
                edgeindex = (*edgedesc)[j]->index;
                if((*edges)[edgeindex]->esharp ==1)
                {
                    int vindex1 = (*edges)[edgeindex]->index1;
                    int vindex2 = (*edges)[edgeindex]->index2;

                    glLineWidth(2);
                    glBegin(GL_LINES);
                    {
                        glColor3f(1, 0, 0);
                        glVertex3f((*verts)[vindex1]->datas[0], (*verts)[vindex1]->datas[1], (*verts)[vindex1]->datas[2]);
                        glVertex3f((*verts)[vindex2]->datas[0], (*verts)[vindex2]->datas[1], (*verts)[vindex2]->datas[2]);
                    }
                    glEnd();
                    glLineWidth(1);
                }
            }
        }
    }
}

void TopManager::drawSurface(BaseTopology *surface, float r, float g, float b)
{
    switch(surface->mode)
    {
    case cat:
        drawCatSurface(surface, r, g, b);
        break;
    case doo:
        drawDooSurface(surface, r, g, b);
        break;
    case loop:
        drawLoopSurface(surface, r, g, b);
        break;

    }
}

void TopManager::getOriginalModelVerticesAndNormals(QVector<QVector3D> *vertices, QVector<QVector3D> *normals, QList<int> *indicates)
{
    BaseTopology *top;
    top = getCurrentWindow()->steps->first();
    QList<int> faceVertices;
    int dim = top->dim;
    GLfloat d1[dim], d2[dim], norm[3];

    for(int i = 0; i < top->faces->size(); i++)
    {
        faceVertices = this->makeVertList(top->faces, i, top->edges);

        int vindex1 = faceVertices[0];
        int vindex2 = faceVertices[1];
        int vindex3 = faceVertices[2];

        for(int dimIndex = 0; dimIndex < top->dim; dimIndex++)
        {
            d1[dimIndex] = (*top->vertices)[vindex1]->datas[dimIndex] - (*top->vertices)[vindex2]->datas[dimIndex];
            d2[dimIndex] = (*top->vertices)[vindex2]->datas[dimIndex] - (*top->vertices)[vindex3]->datas[dimIndex];
        }

        norm[0] = d1[1] * d2[2] - d1[2] * d2[1];
        norm[1] = d1[2] * d2[0] - d1[0] * d2[2];
        norm[2] = d1[0] * d2[1] - d1[1] * d2[0];
        //after calculate the normal of the face, we split the face into several triangles (fan)
        for(int j = 0; j < faceVertices.size(); j++)
        {
            int index = faceVertices[j];
            qreal x = top->vertices->at(index)->datas[0];
            qreal y = top->vertices->at(index)->datas[1];
            qreal z = top->vertices->at(index)->datas[2];
            QVector3D vertex(x, y, z);
            vertices->push_back(vertex);
            normals->push_back(QVector3D(norm[0], norm[1], norm[2]));
        }
        indicates->push_back(faceVertices.size());
    }


}

void TopManager::getCurrentModelVerticesAndNormals(QVector<QVector3D> *vertices, QVector<QVector3D> *normals, QList<int> *indicates, LimitOrNot limitOrNot)
{
    BaseTopology *top;
    if(limitOrNot == LIMIT)
    {
        top = getCurrentLimitTopology();
    }
    else
    {
        top = getCurrentTopology();
    }


    QList<int> faceVertices;
    int dim = top->dim;
    GLfloat d1[dim], d2[dim], norm[3];

    for(int i = 0; i < top->faces->size(); i++)
    {

        faceVertices = this->makeVertList(top->faces, i, top->edges);
        int vindex1 = faceVertices[0];
        int vindex2 = faceVertices[1];       
        int vindex3 = faceVertices[2];


        for(int dimIndex = 0; dimIndex < top->dim; dimIndex++)
        {
            d1[dimIndex] = (*top->vertices)[vindex1]->datas[dimIndex] - (*top->vertices)[vindex2]->datas[dimIndex];
            d2[dimIndex] = (*top->vertices)[vindex2]->datas[dimIndex] - (*top->vertices)[vindex3]->datas[dimIndex];
        }

        norm[0] = d1[1] * d2[2] - d1[2] * d2[1];
        norm[1] = d1[2] * d2[0] - d1[0] * d2[2];
        norm[2] = d1[0] * d2[1] - d1[1] * d2[0];
        //after calculate the normal of the face, we split the face into several triangles (fan)
        for(int j = 0; j < faceVertices.size(); j++)
        {
            int index = faceVertices[j];

            qreal x = top->vertices->at(index)->datas[0];
            qreal y = top->vertices->at(index)->datas[1];
            qreal z = top->vertices->at(index)->datas[2];
            QVector3D vertex(x, y, z);
            vertices->push_back(vertex);
            normals->push_back(QVector3D(norm[0], norm[1], norm[2]));
        }

        indicates->push_back(faceVertices.size());

    }


}

void TopManager::getCurrentSpecialVerticesAndEdges(QVector<QVector3D> *vertices1, QVector<QVector3D> *vertices2, QVector<QVector3D> *edges, LimitOrNot limitOrNot)
{

    BaseTopology *top;
    QList<int> faceVertices;

    if(limitOrNot == LIMIT)
    {
        top = getCurrentLimitTopology();
    }
    else
    {
        top = getCurrentTopology();
    }

    for(int i = 0; i < top->faces->size(); i++)
    {
        QList<BaseEdgeDesc*> *edgedesc = top->faces->at(i)->edgeoff;
        int edgeindex;
        int edgesize = edgedesc->size();

        faceVertices = makeVertList(top->faces, i, top->edges);
        for(int j = 0; j < faceVertices.size(); j++)
        {
            if(top->vertices->at(faceVertices[j])->sharp == 3 || top->vertices->at(faceVertices[j])->sharp == 4)
            {
                qreal x = top->vertices->at(faceVertices[j])->datas[0];
                qreal y = top->vertices->at(faceVertices[j])->datas[1];
                qreal z = top->vertices->at(faceVertices[j])->datas[2];
                vertices1->push_back(QVector3D(x, y, z));

            }
            else if(top->vertices->at(faceVertices[j])->sharp == 2)
            {
                qreal x = top->vertices->at(faceVertices[j])->datas[0];
                qreal y = top->vertices->at(faceVertices[j])->datas[1];
                qreal z = top->vertices->at(faceVertices[j])->datas[2];
                vertices2->push_back(QVector3D(x, y, z));
            }
        }
        for(int j = 0; j < edgesize; j++)
        {
            edgeindex = edgedesc->at(j)->index;
            if(top->edges->at(edgeindex)->esharp >= 1)
            {
                int vindex1 = top->edges->at(edgeindex)->index1;
                int vindex2 = top->edges->at(edgeindex)->index2;
                qreal x = top->vertices->at(vindex1)->datas[0];
                qreal y = top->vertices->at(vindex1)->datas[1];
                qreal z = top->vertices->at(vindex1)->datas[2];
                edges->push_back(QVector3D(x, y, z));

                x = top->vertices->at(vindex2)->datas[0];
                y = top->vertices->at(vindex2)->datas[1];
                z = top->vertices->at(vindex2)->datas[2];
                edges->push_back(QVector3D(x, y, z));
            }
        }
    }

}

BaseTopology* TopManager::getCurrentTopology()
{
   return windows->at(currentWindow)->getCurrentTopolopy();
}

BaseTopology* TopManager::getCurrentLimitTopology()
{
    return windows->at(currentWindow)->getCurrentLimitTopology();
}

bool TopManager::onOpen()
{
    QString filename = openAFile();

    //do file operations
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QFile::Text))
    {
        QMessageBox::information(parent, QObject::tr("Unable to open file"), file.errorString());
        return false;
    }
    Adapter* adapter = new Adapter;
    adapter->setFileName(filename);
    QString name = QFileInfo(file.fileName()).fileName();
    adapter->setName(name);

    QTextStream in(&file);
    try
    {
        parseSurfaceBase(&in,adapter,cat);
    }
    catch(QException e)
    {
        QMessageBox::warning(parent, parent->tr("Subdivision"),
                             parent->tr("Cannot parse file %1:\n%2.")
                             .arg(filename)
                             .arg(file.errorString()));
        return false;
    }

    //windows->insert(windows->end(), adapter);
    windows->push_back(adapter);
    //windowSize++;
    currentWindow = windows->size() - 1;//windowSize - 1;
    //do something to display the window
    return true;
}

QString TopManager::openAFile()
{
    QString filename;
    filename = QFileDialog::getOpenFileName(parent, QObject::tr("Open a raw data file"), "", QObject::tr("Text Files(*.txt);;Subdivision File(*.subd)"));
    return filename;
}

void TopManager::saveToFile(Adapter *adapter)
{
    QString fileName = adapter->filename;
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(parent, QObject::tr("Unable to open file"), file.errorString());
        return;
    }


    BaseTopology *original = adapter->steps->front();

    if(original == NULL)
        return;
    QTextStream out(&file);
    //BaseTopology *topology = adapter->getCurrentTopolopy();
    QString filename = QFileInfo(file.fileName()).fileName();
    original->name = filename;
    out << original->name;
    out << "\n";
    out << original->dim;
    out << "\n";
    out << "#ORIGINAL\n";

    saveDataToFile(original, &out);
    if(adapter->getCurrentStep() > 0)
    {
        BaseTopology *current = adapter->getCurrentTopolopy();
        int stepSpan = adapter->stepIndicate + adapter->getCurrentStep() - 1;

        out << "\n";
        out << "#CURRENT\t";
        out << stepSpan;
        out << "\n";

        saveDataToFile(current, &out);
    }

    out << "\n";
    out << "#END";

}
void TopManager::saveDataToFile(BaseTopology *topology, QTextStream *out)
{
    int i,j;
    //BaseTopology* topolopy = adapter->getCurrentTopolopy();
    //BaseTopology* topology = adapter->getCurrentTopolopy();
    if(topology == NULL)
        return;

    *out << topology->nofvert;
    *out << "\n";

    for(i = 0; i < topology->nofvert; i++)
    {
        //for(j = 0; j < dim; j++)
            //out << *((*topology->vertices)[i] + j);
        for(int j = 0; j < topology->dim; j++)
        {
            *out << (*topology->vertices)[i]->datas[j];
            *out << "\t";
        }
        *out << "\n";
    }

    *out << topology->noffaces;
    *out << "\n";
//    for(i = 0; i < topology->noffaces; i++)
//    {
//        QList<BaseEdgeDesc*> *edges = (*topology->faces)[i]->edgeoff;
//        out << edges->size();
//        out << "\t";
//        for(j = 0; j < edges->size(); j++)
//        {
//            out << (* topology->edges)[(*edges)[j]->index]->index1;
//            out << "\t";
//        }
//        out << "\n";
//    }
    for(i = 0; i < topology->noffaces; i++)
    {
        QList<int> vlist = makeVertList(topology->faces, i, topology->edges);

        *out << vlist.size() << "\t";
        for(j = 0; j < vlist.size(); j++)
        {
            *out << vlist[j] << "\t";
        }
        *out << "\n";
    }
    int count = 0;
    for(i = 0; i < topology->edges->size(); i++)
    {
        if((*topology->edges)[i]->esharp >= 1)
            count++;
    }
    *out << count;
    *out << "\n";

    for(i = 0; i < topology->edges->size(); i++)
    {
        if((*topology->edges)[i]->esharp >= 1)
        {
            *out << (*topology->edges)[i]->index1;
            *out << "\t";
            *out << (*topology->edges)[i]->index2;
//            out << "\t";
//            out << (*topology->edges)[i]->esharp;
            *out << "\n";
        }
    }

    count = 0;
    for(i = 0; i < topology->vertices->size(); i++)
    {
        if((*topology->vertices)[i]->sharp > 1)
            count++;
    }
    *out << count << "\n";

    for(i = 0; i < topology->vertices->size(); i++)
    {
        if((*topology->vertices)[i]->sharp > 1)
        {
            *out<<i<<"\t";
            *out<<(*topology->vertices)[i]->sharp<<"\n";
        }
    }
}

void TopManager::saveAsToFile(Adapter *adapter)
{
    QString fileName = QFileDialog::getSaveFileName(parent, QObject::tr("Save Topolopy"), "", QObject::tr("Text Files(*.txt);;Subdivision File(*.subd)"));

    if(fileName.isEmpty())
        return;
    else
    {
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(parent, QObject::tr("Unable to open file"), file.errorString());
            return;
        }

        BaseTopology *original = adapter->steps->front();

        if(original == NULL)
            return;
        QTextStream out(&file);
        //BaseTopology *topology = adapter->getCurrentTopolopy();
        QString filename = QFileInfo(file.fileName()).fileName();
        original->name = filename;
        out << original->name;
        out << "\n";
        out << original->dim;
        out << "\n";
        out << "#ORIGINAL\n";

        saveDataToFile(original, &out);

        if(adapter->getCurrentStep() > 0)
        {
            BaseTopology *current = adapter->getCurrentTopolopy();
            int stepSpan = 0;
            if(adapter->stepIndicate == 0)
                stepSpan = adapter->getCurrentStep();
            else
                stepSpan = adapter->stepIndicate + adapter->getCurrentStep() - 1;

            out << "\n";
            out << "#CURRENT\t";
            out << stepSpan;
            out << "\n";

            saveDataToFile(current, &out);
        }

        out << "\n";
        out << "#END";
    }


}


void TopManager::onSaveAs()
{
    saveAsToFile(windows->at(currentWindow));
}

void TopManager::onCatSubdivision()
{
    if(getCurrentWindow()->steps->size() < MAXSUBTIME)
    {
        BaseTopology *topold, *topnew, *toplimit;
        topnew = new BaseTopology(cat);
        toplimit = new BaseTopology(cat);
        topold = getCurrentTopology();
        topold->copy(topnew, false);
        topold->copy(toplimit, false);
        catSubdivision(topold, topnew, toplimit);
        getCurrentWindow()->addStep(topnew, toplimit);
    }

}

void TopManager::onDooSubdivision()
{
    if(getCurrentWindow()->steps->size() < MAXSUBTIME)
    {
        BaseTopology *topold, *topnew;
        topold = windows->at(currentWindow)->getCurrentTopolopy();
        topnew = new BaseTopology(doo);
        topnew->dim = topold->dim;
        dooSubdivision(topold, topnew);
        windows->at(currentWindow)->addStep(topnew);
    }
}

void TopManager::onLoopSubdivision()
{
    if(getCurrentWindow()->steps->size() < MAXSUBTIME)
    {
        BaseTopology *topold, *topnew, *toplimit;
        topnew = new BaseTopology(loop);
        toplimit = new BaseTopology(loop);
        topold = getCurrentTopology();
        topold->copy(topnew, false);
        topold->copy(toplimit, false);
        loopSubdivision(topold, topnew, toplimit);
        getCurrentWindow()->addStep(topnew, toplimit);
    }
}

void TopManager::onGeneralizedSubdivision(int k, double u)
{
    if(getCurrentWindow()->steps->size() < MAXSUBTIME)
    {
        BaseTopology *topold, *topnew;


        topold = getCurrentTopology();


        if(k >= 4 && k % 2 == 0)
        {
            topnew = new BaseTopology(cat);
            topold->copy(topnew, false);
            evenGeneralizedSubdivision(topold, topnew, k, u);
        }
        else if(k >= 3 && k % 2 == 1)
        {
            topnew = new BaseTopology(doo);
            topnew->dim = topold->dim;
            oddGeneralizedSubdivision(topold, topnew, k, u);
        }
        getCurrentWindow()->addStep(topnew);
    }
}
