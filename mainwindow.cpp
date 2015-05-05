#include "mainwindow.h"
#include "mainwindow.h"
#include <QInputDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{

    //openglWidget = new ..
    //setCentralWidget(openglWidget);

    createActions();
    createMenus();
    createStatusBars();

    tabWidget = new QTabWidget;
    QToolButton *newTabButton = new QToolButton(tabWidget);
    newTabButton->setCursor(Qt::ArrowCursor);
    newTabButton->setAutoRaise(true);
    newTabButton->setIcon(QIcon(":/images/newTab32.png"));
    newTabButton->setToolTip(tr("Add page"));
    tabWidget->setCornerWidget(newTabButton, Qt::TopRightCorner);
    tabWidget->setTabsClosable(true);

    connect(newTabButton, SIGNAL(clicked()), this, SLOT(onOpenAction()));
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested(int)));
    setCentralWidget(tabWidget);

   // glWindows = new QList<GLWidget*>;
    manager = new TopManager(this);
    //SubMode = {false, false, false, false, false, true};
    SubMode.CAT = false;
    SubMode.CAT_LIMIT = false;
    SubMode.DOO = false;
    SubMode.LOOP = false;
    SubMode.LOOP_LIMIT = false;
    SubMode.GENERAL = true;
    lastTabIndex = -1;
    isTabClosing = false;
    //GLWidget *glWidget = new GLWidget(manager, -1);
   // setCentralWidget(glWidget);
}



void MainWindow::createActions()
{
    //newAct = new QAction(QIcon(":/images/new.png"), tr("&New"),this);
    //newAct->setShortcuts(QKeySequence::New);
    openAct = new QAction(QIcon(":images/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(onOpenAction()));

    saveAct = new QAction(QIcon(":images/save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save to file"));
   // connect(saveAct, SIGNAL(triggered()), this, SLOT(temp()));
    saveAct->setEnabled(false);

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the topology under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(onSaveAsAction()));
    saveAsAct->setEnabled(false);

    closeAllAct = new QAction(tr("Close All"), this);
    //connect(closeAllAct, SIGNAL(triggered()), this, SLOT(onCloseAllAction()));
    closeAllAct->setEnabled(false);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
//! [20]
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(onExitAction()));


//! [21]
    cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
//! [21]
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
   // connect(cutAct, SIGNAL(triggered()), this, SLOT(temp()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    //connect(copyAct, SIGNAL(triggered()), this, SLOT(temp()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    //connect(pasteAct, SIGNAL(triggered()), this, SLOT(temp()));

    undoAct = new QAction(tr("Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("go back to previous step"));
    undoAct->setEnabled(false);
   // connect(undoAct, SIGNAL(triggered()), this, SLOT(temp()));

    antialiasingAct = new QAction(tr("Antialiasing"), this);
    antialiasingAct->setCheckable(true);
    antialiasingAct->setChecked(true);
    //connect(antialiasingAct, SIGNAL(triggered()), this, SLOT(temp()));
    antialiasingAct->setEnabled(false);

    lightingAct = new QAction(tr("Lighting"), this);
    lightingAct->setCheckable(true);
    lightingAct->setChecked(true);
   // connect(lightingAct, SIGNAL(triggered()), this, SLOT(temp()));
    lightingAct->setEnabled(false);

    shadingAct = new QAction(tr("Shading"), this);
    shadingAct->setCheckable(true);
    shadingAct->setChecked(false);
   // connect(shadingAct, SIGNAL(triggered()), this, SLOT(temp()));
    shadingAct->setEnabled(false);

    effectsGroup = new QActionGroup(this);
    effectsGroup->addAction(antialiasingAct);
    effectsGroup->addAction(lightingAct);
    effectsGroup->addAction(shadingAct);
    effectsGroup->setExclusive(false);
    connect(effectsGroup, SIGNAL(triggered(QAction*)), this, SLOT(onEffectsGroup(QAction*)));

    zoomInAct = new QAction(tr("Zoom In"), this);
    zoomInAct->setShortcuts(QKeySequence::ZoomIn);
    //connect(zoomInAct, SIGNAL(triggered()), this, SLOT(temp()));
    zoomInAct->setEnabled(false);

    zoomOutAct = new QAction(tr("Zoom Out"), this);
    zoomOutAct->setShortcuts(QKeySequence::ZoomOut);
    //connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(temp()));
    zoomOutAct->setEnabled(false);

    rotateAct = new QAction(tr("Rotate"), this);
    //connect(rotateAct, SIGNAL(triggered()), this, SLOT(temp()));
    rotateAct->setEnabled(false);

    catmullAct = new QAction(tr("Catmull-Clark"), this);
    catmullAct->setEnabled(false);
    connect(catmullAct, SIGNAL(triggered()), this, SLOT(onCatmullAction()));

    catmullLimitAct = new QAction(tr("Catmull-Clark Limit Position"), this);
    catmullLimitAct->setCheckable(true);
    catmullLimitAct->setChecked(false);
    //checkStatus.CAT_LIMIT = false;
    catmullLimitAct->setEnabled(false);
    connect(catmullLimitAct, SIGNAL(triggered()), this, SLOT(onCatmullLimitAction()));

    dooAct = new QAction(tr("Doo-Sabin"), this);
    dooAct->setEnabled(false);
    connect(dooAct, SIGNAL(triggered()), this, SLOT(onDooAction()));

    loopAct = new QAction(tr("Loop"), this);
    loopAct->setEnabled(false);
    connect(loopAct, SIGNAL(triggered()), this, SLOT(onLoopAction()));

    loopLimitAct = new QAction(tr("Loop Limit Position"), this);
    loopLimitAct->setCheckable(true);
    loopLimitAct->setChecked(false);
    //checkStatus.LOOP_LIMIT = false;
    loopLimitAct->setEnabled(false);
    connect(loopLimitAct, SIGNAL(triggered()), this, SLOT(onLoopLimitAction()));

    generalizedSubAct = new QAction(tr("Generalized Subdivision"), this);
    generalizedSubAct->setEnabled(false);
    connect(generalizedSubAct, SIGNAL(triggered()), this, SLOT(onGeneralizedSubAction()));

    previousStepAct = new QAction(tr("Previous Step"), this);
    previousStepAct->setEnabled(false);
    connect(previousStepAct, SIGNAL(triggered()), this, SLOT(onPreviousStepAction()));
    nextStepAct = new QAction(tr("Next Step"), this);
    nextStepAct->setEnabled(false);
    connect(nextStepAct, SIGNAL(triggered()), this, SLOT(onNextStepAction()));

    vertexAct = new QAction(tr("Vertex"), this);
    vertexAct->setCheckable(true);
    vertexAct->setChecked(false);
    //connect(vertexAct, SIGNAL(triggered()), this, SLOT(temp()));
    vertexAct->setEnabled(false);

    wireframeAct = new QAction(tr("Wireframe"), this);
    wireframeAct->setCheckable(true);
    wireframeAct->setChecked(true);
   // connect(wireframeAct, SIGNAL(triggered()), this, SLOT(temp()));
    wireframeAct->setEnabled(false);

    faceAct = new QAction(tr("Face"), this);
    faceAct->setCheckable(true);
    faceAct->setChecked(false);
    //connect(faceAct, SIGNAL(triggered()), this, SLOT(temp()));
    faceAct->setEnabled(false);

    displayGroup = new QActionGroup(this);
    displayGroup->addAction(vertexAct);
    displayGroup->addAction(wireframeAct);
    displayGroup->addAction(faceAct);
    wireframeAct->setChecked(true);
    connect(displayGroup, SIGNAL(triggered(QAction*)), this, SLOT(onDisplayGroup(QAction*)));

    controlMeshAct = new QAction(tr("Control"), this);
    controlMeshAct->setCheckable(true);
    controlMeshAct->setChecked(false);
    connect(controlMeshAct, SIGNAL(triggered()), this, SLOT(onControlMeshAction()));
    controlMeshAct->setEnabled(false);

    sharpFeatureAct = new QAction(tr("Show Sharp Features"), this);
    sharpFeatureAct->setCheckable(true);
    sharpFeatureAct->setChecked(false);
    connect(sharpFeatureAct, SIGNAL(triggered()), this, SLOT(onSharpFeatureAction()));
    sharpFeatureAct->setEnabled(false);

    newWindowAct = new QAction(tr("New Window"), this);
    //connect(newWindowAct, SIGNAL(triggered()), this, SLOT(temp()));

    cascadeAct = new QAction(tr("Cascade"), this);
    //connect(cascadeAct, SIGNAL(triggered()), this, SLOT(temp()));

    tileAct = new QAction(tr("Tile"), this);
    //connect(tileAct, SIGNAL(triggered()), this, SLOT(temp()));

    arrangeIconsAct = new QAction(tr("Arrange Icons"), this);
    //connect(arrangeIconsAct, SIGNAL(triggered()), this, SLOT(temp()));

    toolbarAct = new QAction(tr("Toolbar"), this);
    toolbarAct->setCheckable(true);
    toolbarAct->setChecked(true);
    //connect(toolbarAct, SIGNAL(triggered()), this, SLOT(temp()));


    statusBarAct = new QAction(tr("Status Bar"), this);
    statusBarAct->setCheckable(true);
    statusBarAct->setChecked(true);
    //connect(statusBarAct, SIGNAL(triggered()), this, SLOT(temp()));

    aboutAct = new QAction(tr("About"), this);
    //connect(aboutAct, SIGNAL(triggered()), this, SLOT(temp()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(closeAllAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    openGLMenu = menuBar()->addMenu(tr("&OpenGL"));
    openGLMenu->addAction(antialiasingAct);
    openGLMenu->addAction(lightingAct);
    openGLMenu->addAction(shadingAct);


    zoomMenu = menuBar()->addMenu(tr("&Zoom"));
    zoomMenu->addAction(zoomInAct);
    zoomMenu->addAction(zoomOutAct);
    zoomMenu->addSeparator();
    zoomMenu->addAction(rotateAct);

    subdivisionMenu = menuBar()->addMenu(tr("&Subdivision"));
    subdivisionMenu->addAction(catmullAct);
    subdivisionMenu->addAction(catmullLimitAct);
    subdivisionMenu->addSeparator();
    subdivisionMenu->addAction(dooAct);
    subdivisionMenu->addSeparator();
    subdivisionMenu->addAction(loopAct);
    subdivisionMenu->addAction(loopLimitAct);
    subdivisionMenu->addSeparator();
    subdivisionMenu->addAction(generalizedSubAct);
    subdivisionMenu->addSeparator();
    subdivisionMenu->addAction(previousStepAct);
    subdivisionMenu->addAction(nextStepAct);

    displayMenu = menuBar()->addMenu(tr("&Display"));
    displayMenu->addAction(vertexAct);
    displayMenu->addAction(wireframeAct);
    displayMenu->addAction(faceAct);
    displayMenu->addSeparator();
    displayMenu->addAction(controlMeshAct);
    displayMenu->addSeparator();
    displayMenu->addAction(sharpFeatureAct);

    windowMenu = menuBar()->addMenu(tr("&Window"));
    windowMenu->addAction(newWindowAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addAction(tileAct);
    windowMenu->addAction(arrangeIconsAct);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(toolbarAct);
    viewMenu->addAction(statusBarAct);

    helpMenu = menuBar()->addMenu(tr("&About"));
    helpMenu->addAction(aboutAct);
}

void MainWindow::createStatusBars()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
   // fileToolBar->addAction(newAct);
//! [29] //! [31]
    fileToolBar->addAction(openAct);
//! [31]
    fileToolBar->addAction(saveAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
}

void MainWindow::subModeModified(TopMode mode)
{
    bool subdivided = manager->getCurrentWindow()->getCurrentStep() > 0 ? true : false;
    switch(mode)
    {
    case cat:
        SubMode.CAT = true;
        SubMode.CAT_LIMIT = true;
        SubMode.DOO = false;
        SubMode.LOOP = false;
        SubMode.LOOP_LIMIT = false;
        SubMode.GENERAL = false;
        break;
    case doo:
        SubMode.CAT = false;
        SubMode.CAT_LIMIT = false;
        SubMode.DOO = true;
        SubMode.LOOP = false;
        SubMode.LOOP_LIMIT = false;
        SubMode.GENERAL = false;
        break;
    case loop:
        SubMode.CAT = false;
        SubMode.CAT_LIMIT = false;
        SubMode.DOO = false;
        SubMode.LOOP = true;
        SubMode.LOOP_LIMIT = true;
        SubMode.GENERAL = false;
        break;
    default:
        SubMode.CAT = false;
        SubMode.CAT_LIMIT = false;
        SubMode.DOO = false;
        SubMode.LOOP = false;
        SubMode.LOOP_LIMIT = false;
        SubMode.GENERAL = true;
        break;

    }
    if(manager->getCurrentWindow()->steps->isEmpty())
    {
        catmullAct->setEnabled(false);
        catmullLimitAct->setEnabled(false);
        dooAct->setEnabled(false);
        loopAct->setEnabled(false);
        loopLimitAct->setEnabled(false);
        generalizedSubAct->setEnabled(false);

    }
    else if(manager->getCurrentWindow()->steps->size() >= MAXSUBTIME)
    {
        if(SubMode.CAT)
            catmullAct->setEnabled(false);
        if(SubMode.DOO)
            dooAct->setEnabled(false);
        if(SubMode.LOOP)
            loopAct->setEnabled(false);
        if(SubMode.GENERAL)
            generalizedSubAct->setEnabled(false);
    }
    else
    {
        catmullAct->setEnabled(SubMode.CAT);
        catmullLimitAct->setEnabled(SubMode.CAT_LIMIT&&subdivided);
        dooAct->setEnabled(SubMode.DOO);
        loopAct->setEnabled(SubMode.LOOP);
        loopLimitAct->setEnabled(SubMode.LOOP_LIMIT&&subdivided);
        generalizedSubAct->setEnabled(SubMode.GENERAL);
    }
}

void MainWindow::statusWithNoTabs()
{
    saveAct->setEnabled(false);
    saveAsAct->setEnabled(false);
    closeAllAct->setEnabled(false);

    antialiasingAct->setEnabled(false);
    lightingAct->setEnabled(false);
    shadingAct->setEnabled(false);

    zoomInAct->setEnabled(false);
    zoomOutAct->setEnabled(false);
    rotateAct->setEnabled(false);

    vertexAct->setEnabled(false);
    wireframeAct->setEnabled(false);
    faceAct->setEnabled(false);
    controlMeshAct->setEnabled(false);
    sharpFeatureAct->setEnabled(false);
}

void MainWindow::statusWithTabs()
{
    saveAct->setEnabled(true);
    saveAsAct->setEnabled(true);
    closeAllAct->setEnabled(true);

    antialiasingAct->setEnabled(true);
    lightingAct->setEnabled(true);
    shadingAct->setEnabled(true);

    zoomInAct->setEnabled(true);
    zoomOutAct->setEnabled(true);
    rotateAct->setEnabled(true);

    vertexAct->setEnabled(true);
    wireframeAct->setEnabled(true);
    faceAct->setEnabled(true);
    controlMeshAct->setEnabled(true);
    sharpFeatureAct->setEnabled(true);
}

void MainWindow::storeStatus(CheckStatus *status)
{
    status->ANTIALIALIASING_CHECKED = antialiasingAct->isChecked();
    status->LIGHTING_CHECKED = lightingAct->isChecked();
    status->SHADING_CHECKED = shadingAct->isChecked();
    status->ROTATE_CHECKED = rotateAct->isChecked();
    status->CAT = catmullAct->isEnabled();
    status->CAT_LIMIT = catmullLimitAct->isEnabled();
    status->CAT_LIMIT_CHECKED = catmullLimitAct->isChecked();
    status->DOO = dooAct->isEnabled();
    status->LOOP = loopAct->isEnabled();
    status->LOOP_LIMIT = loopLimitAct->isEnabled();
    status->LOOP_LIMIT_CHECKED = loopLimitAct->isChecked();
    status->PREVIOUS = previousStepAct->isEnabled();
    status->NEXT = nextStepAct->isEnabled();
    status->VERTEX_CHECKED = vertexAct->isChecked();
    status->WIREFRAME_CHECKED = wireframeAct->isChecked();
    status->FACE_CHECKED = faceAct->isChecked();
    status->CONTROL_CHECKED = controlMeshAct->isChecked();
    status->SharpFeature_CHECKED = sharpFeatureAct->isChecked();
    status->GENERAL = generalizedSubAct->isEnabled();
}

void MainWindow::setStatusSynchronized(CheckStatus *status)
{
    antialiasingAct->setChecked(status->ANTIALIALIASING_CHECKED);
    lightingAct->setChecked(status->LIGHTING_CHECKED);
    shadingAct->setChecked(status->SHADING_CHECKED);
    rotateAct->setChecked(status->ROTATE_CHECKED);
    catmullAct->setEnabled(status->CAT);
    catmullLimitAct->setChecked(status->CAT_LIMIT_CHECKED);
    catmullLimitAct->setEnabled(status->CAT_LIMIT);
    dooAct->setEnabled(status->DOO);
    loopAct->setEnabled(status->LOOP);
    loopLimitAct->setChecked(status->LOOP_LIMIT_CHECKED);
    loopLimitAct->setEnabled(status->LOOP_LIMIT);
    previousStepAct->setEnabled(status->PREVIOUS);
    nextStepAct->setEnabled(status->NEXT);
    vertexAct->setChecked(status->VERTEX_CHECKED);
    wireframeAct->setChecked(status->WIREFRAME_CHECKED);
    faceAct->setChecked(status->FACE_CHECKED);
    controlMeshAct->setChecked(status->CONTROL_CHECKED);
    sharpFeatureAct->setChecked(status->SharpFeature_CHECKED);
    generalizedSubAct->setEnabled(status->GENERAL);
}

void MainWindow::onOpenAction()
{
    GLWidget *newWindow;
    if(manager->onOpen())
    {
        statusWithTabs();
        newWindow = new GLWidget(manager);
        newWindow->setWindowFilePath(manager->getCurrentWindow()->getFileName());
        //glWindows->push_back(newWindow);
        //1 method
        QString filename = QFileInfo(QFile(manager->getCurrentWindow()->filename).fileName()).fileName();
        //2 method
        /*
        QString filePath = QFile(manager->getCurrentWindow()->filename).fileName();
        QStringList parts = filePath.split("/");
        QString lastBit = parts.last();
        */
        tabWidget->addTab(newWindow, filename);
        //setCentralWidget(newWindow);
        //set Doo LOOP chechk item...
        //tabWidget->c
        //int index = tabWidget->indexOf(newWindow);
        //tabWidget->setCurrentIndex(index);
        if(tabWidget->currentWidget() != newWindow)
            tabWidget->setCurrentWidget(newWindow);
        if(manager->getCurrentWindow()->modeValid.CAT)
        {
            SubMode.CAT = true;
            catmullAct->setEnabled(true);
        }
        else
        {
            SubMode.CAT = false;
            catmullAct->setEnabled(false);
        }
        if(manager->getCurrentWindow()->modeValid.DOO)
        {
            SubMode.DOO = true;
            dooAct->setEnabled(true);
        }
        else
        {
            SubMode.DOO = false;
            dooAct->setEnabled(false);
        }
        if(manager->getCurrentWindow()->modeValid.LOOP)
        {
            SubMode.LOOP = true;
            loopAct->setEnabled(true);
        }
        else
        {
            SubMode.LOOP = false;
            loopAct->setEnabled(false);
        }
        generalizedSubAct->setEnabled(true);


    }




}

void MainWindow::onSaveAction()
{
    manager->onSaveAs();
}

void MainWindow::onSaveAsAction()
{
    manager->onSaveAs();
}

void MainWindow::onCloseAllAction()
{

}

void MainWindow::onExitAction()
{

}

void MainWindow::onUndoAction()
{

}



void MainWindow::onCutAction()
{

}

void MainWindow::onCopyAction()
{

}

void MainWindow::onPasteAction()
{

}

void MainWindow::onAntialiasingAction()
{

}

void MainWindow::onLightingAction()
{

}

void MainWindow::onShadingAction()
{

}

void MainWindow::onEffectsGroup(QAction *action)
{
    CheckStatus *status = ((GLWidget*)tabWidget->currentWidget())->checkStatus;
    if(action == antialiasingAct)
    {
       status->ANTIALIALIASING_CHECKED = !status->ANTIALIALIASING_CHECKED;
    }
    else if(action == lightingAct)
    {
        status->LIGHTING_CHECKED = !status->LIGHTING_CHECKED;
    }
    else
    {
        status->SHADING_CHECKED = !status->SHADING_CHECKED;
    }

    ((GLWidget*)tabWidget->currentWidget())->update();

}

void MainWindow::onZoomInAction()
{

}

void MainWindow::onZoomOutAction()
{

}

void MainWindow::onRotateAction()
{

}

void MainWindow::onCatmullAction()
{

    if(manager->getCurrentWindow()->getCurrentStep() < manager->getCurrentWindow()->steps->size() - 1)
    {
       onNextStepAction();
       return;
    }
    manager->onCatSubdivision();
    CheckStatus *status = ((GLWidget*)(tabWidget->currentWidget()))->checkStatus;

    //(*glWindows)[manager->currentWindow]->initializeShaderProgramsAndBuffers(status->limitOrNot);
    //(*glWindows)[manager->currentWindow]->updateGL();
    ((GLWidget*)tabWidget->currentWidget())->initializeShaderProgramsAndBuffers(status->limitOrNot);
    ((GLWidget*)tabWidget->currentWidget())->update();
    this->subModeModified(cat);
    previousStepAct->setEnabled(true);
    //checkStatus.PREVIOUS = true;
}

void MainWindow::onCatmullLimitAction()
{
    CheckStatus *status = ((GLWidget*)tabWidget->currentWidget())->checkStatus;

    if(status->limitOrNot == NORLIMIT)
    {
        status->limitOrNot = LIMIT;
        //glWindows->at(manager->currentWindow)->initializeShaderProgramsAndBuffers(LIMIT);
        //glWindows->at(manager->currentWindow)->updateGL();
        ((GLWidget*)tabWidget->currentWidget())->initializeShaderProgramsAndBuffers(LIMIT);
        ((GLWidget*)tabWidget->currentWidget())->update();
        catmullLimitAct->setChecked(true);
    }
    else
    {
        status->limitOrNot = NORLIMIT;
//        glWindows->at(manager->currentWindow)->initializeShaderProgramsAndBuffers(NORLIMIT);
//        glWindows->at(manager->currentWindow)->updateGL();
        ((GLWidget*)tabWidget->currentWidget())->initializeShaderProgramsAndBuffers(NORLIMIT);
        ((GLWidget*)tabWidget->currentWidget())->update();
        catmullLimitAct->setChecked(false);
    }

}

void MainWindow::onDooAction()
{
    if(manager->getCurrentWindow()->getCurrentStep() < manager->getCurrentWindow()->steps->size() - 1)
    {
       onNextStepAction();
       return;
    }
    manager->onDooSubdivision();
//    (*glWindows)[manager->currentWindow]->initializeShaderProgramsAndBuffers(NORLIMIT);
//    (*glWindows)[manager->currentWindow]->updateGL();
    //manager->onSaveAs();
    ((GLWidget*)tabWidget->currentWidget())->initializeShaderProgramsAndBuffers(NORLIMIT);
    ((GLWidget*)tabWidget->currentWidget())->update();
    this->subModeModified(doo);
    previousStepAct->setEnabled(true);
}

void MainWindow::onLoopAction()
{
    if(manager->getCurrentWindow()->getCurrentStep() < manager->getCurrentWindow()->steps->size() - 1)
    {
       onNextStepAction();
       return;
    }
    manager->onLoopSubdivision();
    CheckStatus *status = ((GLWidget*)tabWidget->currentWidget())->checkStatus;

//    (*glWindows)[manager->currentWindow]->initializeShaderProgramsAndBuffers(status->limitOrNot);
//    (*glWindows)[manager->currentWindow]->updateGL();
    ((GLWidget*)tabWidget->currentWidget())->initializeShaderProgramsAndBuffers(status->limitOrNot);
    ((GLWidget*)tabWidget->currentWidget())->update();

    this->subModeModified(loop);
    previousStepAct->setEnabled(true);
}

void MainWindow::onLoopLimitAction()
{
    CheckStatus *status = ((GLWidget*)tabWidget->currentWidget())->checkStatus;

    if(status->limitOrNot == NORLIMIT)
    {
        status->limitOrNot = LIMIT;
//        glWindows->at(manager->currentWindow)->initializeShaderProgramsAndBuffers(LIMIT);
//        glWindows->at(manager->currentWindow)->updateGL();
        ((GLWidget*)tabWidget->currentWidget())->initializeShaderProgramsAndBuffers(LIMIT);
        ((GLWidget*)tabWidget->currentWidget())->update();
        loopLimitAct->setChecked(true);
    }
    else
    {
        status->limitOrNot = NORLIMIT;
//        glWindows->at(manager->currentWindow)->initializeShaderProgramsAndBuffers(NORLIMIT);
//        glWindows->at(manager->currentWindow)->updateGL();
        ((GLWidget*)tabWidget->currentWidget())->initializeShaderProgramsAndBuffers(NORLIMIT);
        ((GLWidget*)tabWidget->currentWidget())->update();
        loopLimitAct->setChecked(false);
    }

}

void MainWindow::onGeneralizedSubAction()
{
    if(manager->getCurrentWindow()->getCurrentStep() < manager->getCurrentWindow()->steps->size() - 1)
    {
       onNextStepAction();
       return;
    }
    //if have no gerneral subdivison happen alog prompt user to set k u
    //set GLwidget k u ;
    GLWidget *widget = (GLWidget*)tabWidget->currentWidget();
    if(widget->getOrder() < 3)
    {
        bool ok1, ok2;
        int k;
        double u;
        /*
        QDialog dialog(this);
        // Use a layout allowing to have a label next to each field
        QFormLayout form(&dialog);

        // Add some text above the fields
        form.addRow(new QLabel("The question ?"));

        // Add the lineEdits with their respective labels
        QList<QLineEdit *> fields;
        for(int i = 0; i < 4; ++i) {
            QLineEdit *lineEdit = new QLineEdit(&dialog);
            QString label = QString("Value %1").arg(i + 1);
            form.addRow(label, lineEdit);

            fields << lineEdit;
        }

        // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                   Qt::Horizontal, &dialog);
        form.addRow(&buttonBox);
        QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
        QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

        // Show the dialog as modal
        if (dialog.exec() == QDialog::Accepted) {
            // If the user didn't dismiss the dialog, do something with the fields
            foreach(QLineEdit * lineEdit, fields) {
                qDebug() << lineEdit->text();
            }
        }*/
        input:
        QDialog dialog(this);
        // Use a layout allowing to have a label next to each field
        QFormLayout form(&dialog);

        // Add some text above the fields
        form.addRow(new QLabel("Parameters"));

        // Add the lineEdits with their respective labels
        QList<QLineEdit *> fields;

        QLineEdit *lineEdit;
        QString label;
        lineEdit = new QLineEdit(&dialog);
        label = QString("Order");
        form.addRow(label, lineEdit);
        fields << lineEdit;

        lineEdit = new QLineEdit(&dialog);
        label = QString("Tension");
        form.addRow(label, lineEdit);
        fields << lineEdit;


        // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                   Qt::Horizontal, &dialog);
        form.addRow(&buttonBox);
        QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
        QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

        // Show the dialog as modal
        if (dialog.exec() == QDialog::Accepted) {
            // If the user didn't dismiss the dialog, do something with the fields
            QLineEdit *lineEdit = fields.at(0);
            if(lineEdit->text().isEmpty())
                k = 4;
            else
                k = lineEdit->text().toInt();
            lineEdit = fields.at(1);
            if(lineEdit->text().isEmpty())
                u = 1.0f;
            else
                u = lineEdit->text().toDouble();
            if(k >= 3 && u >= 0)
            {
                widget->setOrder(k);
                widget->setTensionParameter(u);
            }
            else
            {
                QMessageBox msgBox;
                msgBox.setText("The parameters is setted correctly");
                msgBox.setInformativeText("Do you want to reset?");
                msgBox.setDetailedText("Order should be an integer greater than 2.\n Tension is a  non-negative double.\n If you choose No we just arbot this subdivion.");
                msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
                msgBox.setDefaultButton(QMessageBox::Yes);
                int ret = msgBox.exec();
                switch(ret)
                {
                case QMessageBox::Yes:
                    goto input;
                    break;
                case QMessageBox::No:
                    return;
                    break;
                default:
                    break;
                }
            }
        }
        else
        {
            return;
        }
    }
    manager->onGeneralizedSubdivision(widget->getOrder(), widget->getTensionParameter());
//    (*glWindows)[manager->currentWindow]->initializeShaderProgramsAndBuffers(NORLIMIT);
//    (*glWindows)[manager->currentWindow]->updateGL();
    widget->initializeShaderProgramsAndBuffers(NORLIMIT);
    widget->update();
    widget->updateTensionParameter();
    this->subModeModified(general);
    previousStepAct->setEnabled(true);
}

void MainWindow::onPreviousStepAction()
{
    int step = manager->getCurrentWindow()->getCurrentStep();
    step--;
    if(step < 0)
    {
        previousStepAct->setEnabled(false);
        //checkStatus.PREVIOUS = false;
        return;
    }
    manager->getCurrentWindow()->setCurrentStep(step);
    CheckStatus *status = ((GLWidget*)tabWidget->currentWidget())->checkStatus;

//    glWindows->at(manager->currentWindow)->initializeShaderProgramsAndBuffers(status->limitOrNot);
//    glWindows->at(manager->currentWindow)->updateGL();
    ((GLWidget*)tabWidget->currentWidget())->initializeShaderProgramsAndBuffers(status->limitOrNot);
    ((GLWidget*)tabWidget->currentWidget())->update();
    if(step == 0)
    {
        previousStepAct->setEnabled(false);
        //checkStatus.PREVIOUS = false;
    }
    nextStepAct->setEnabled(true);
   // checkStatus.NEXT = true;
}

void MainWindow::onNextStepAction()
{
    int step = manager->getCurrentWindow()->getCurrentStep();
    step++;
    if(step >= manager->getCurrentWindow()->steps->size())
    {
        nextStepAct->setEnabled(false);
        //checkStatus.PREVIOUS = false;
        return;
    }
    manager->getCurrentWindow()->setCurrentStep(step);
    CheckStatus *status = ((GLWidget*)tabWidget->currentWidget())->checkStatus;
//    glWindows->at(manager->currentWindow)->initializeShaderProgramsAndBuffers(status->limitOrNot);
//    glWindows->at(manager->currentWindow)->updateGL();
    ((GLWidget*)tabWidget->currentWidget())->initializeShaderProgramsAndBuffers(status->limitOrNot);
    ((GLWidget*)tabWidget->currentWidget())->update();
    if(step == manager->getCurrentWindow()->steps->size()-1)
    {
        nextStepAct->setEnabled(false);
        //checkStatus.NEXT = false;
    }
    previousStepAct->setEnabled(true);
    //checkStatus.PREVIOUS = true;
}

void MainWindow::onVertexAction()
{

}

void MainWindow::onWireframeAction()
{

}

void MainWindow::onFaceAction()
{

}

void MainWindow::onDisplayGroup(QAction *action)
{
    CheckStatus *status = ((GLWidget*)tabWidget->currentWidget())->checkStatus;
    if(action == vertexAct)
    {
        status->VERTEX_CHECKED = true;
        status->WIREFRAME_CHECKED = false;
        status->FACE_CHECKED = false;
    }
    else if(action == wireframeAct)
    {
        status->VERTEX_CHECKED = false;
        status->WIREFRAME_CHECKED = true;
        status->FACE_CHECKED = false;
    }
    else
    {
        status->VERTEX_CHECKED = false;
        status->WIREFRAME_CHECKED = false;
        status->FACE_CHECKED = true;
    }
    ((GLWidget*)tabWidget->currentWidget())->update();
}

void MainWindow::onControlMeshAction()
{
    CheckStatus *status = ((GLWidget*)(tabWidget->currentWidget()))->checkStatus;
    status->CONTROL_CHECKED = !status->CONTROL_CHECKED;
    ((GLWidget*)(tabWidget->currentWidget()))->update();
    controlMeshAct->setChecked(status->CONTROL_CHECKED);
}

void MainWindow::onSharpFeatureAction()
{
    CheckStatus *status = ((GLWidget*)(tabWidget->currentWidget()))->checkStatus;
    status->SharpFeature_CHECKED = !status->SharpFeature_CHECKED;
    ((GLWidget*)(tabWidget->currentWidget()))->update();
    sharpFeatureAct->setChecked(status->SharpFeature_CHECKED);
}

void MainWindow::onNewWindowAction()
{

}

void MainWindow::onCascadeAction()
{

}

void MainWindow::onTileAction()
{

}

void MainWindow::onArrangeIconsAction()
{

}

void MainWindow::onToolbarAction()
{

}

void MainWindow::onStatusBarAction()
{

}

void MainWindow::onAboutAction()
{

}

void MainWindow::onCurrentChanged(int newIndex)
{
    if(lastTabIndex >= 0 && !isTabClosing)
        storeStatus(((GLWidget*)tabWidget->widget(lastTabIndex))->checkStatus);

        manager->setCurrentWindow(newIndex);

    //tabWidget->setCurrentIndex(newIndex);//because it is already be
    if(lastTabIndex >= 0 && newIndex != -1)
        setStatusSynchronized(((GLWidget*)tabWidget->currentWidget())->checkStatus);
    lastTabIndex = newIndex;
    isTabClosing = false;
}

void MainWindow::onTabCloseRequested(int index)
{
    isTabClosing = true;
    manager->removeWindow(index);
    QOpenGLWidget *glWidget =(QOpenGLWidget*)(tabWidget->widget(index));
    tabWidget->removeTab(index);
    delete glWidget;
    //tabWidget->setCurrentIndex(manager->getCurrentWindowIndex());
    if(tabWidget->count() <=0 )
    {
        statusWithNoTabs();
        {
            bool flag = false;
            catmullAct->setEnabled(flag);
            catmullLimitAct->setEnabled(flag);
            dooAct->setEnabled(flag);
            loopAct->setEnabled(flag);
            loopLimitAct->setEnabled(flag);
            generalizedSubAct->setEnabled(flag);
            previousStepAct->setEnabled(flag);
            nextStepAct->setEnabled(flag);
        }
    }


}
