#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include "topmanager.h"
#include "glwidget.h"
#include "defines.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);

    


private slots:
     void onOpenAction();
     void onSaveAction();
     void onSaveAsAction();
     void onCloseAllAction();
     void onExitAction();

     void onUndoAction();
     void onCutAction();
     void onCopyAction();
     void onPasteAction();

     void onAntialiasingAction();
     void onLightingAction();
     void onShadingAction();
     void onEffectsGroup(QAction *action);

     void onZoomInAction();
     void onZoomOutAction();
     void onRotateAction();

     void onCatmullAction();
     void onCatmullLimitAction();
     void onDooAction();
     void onLoopAction();
     void onLoopLimitAction();
     void onGeneralizedSubAction();
     void onPreviousStepAction();
     void onNextStepAction();

     void onVertexAction();
     void onWireframeAction();
     void onFaceAction();
     void onDisplayGroup(QAction *action);

     void onControlMeshAction();
     void onSharpFeatureAction();

     void onNewWindowAction();
     void onCascadeAction();
     void onTileAction();
     void onArrangeIconsAction();

     void onToolbarAction();
     void onStatusBarAction();

     void onAboutAction();

     void onCurrentChanged(int newIndex);
     void onTabCloseRequested(int index);

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBars();

    void subModeModified(TopMode mode);

    void statusWithNoTabs();
    void statusWithTabs();

    void storeStatus(CheckStatus *checkStatus);
    void setStatusSynchronized(CheckStatus *checkStatus);
    //TopManager manager;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *openGLMenu;
    QMenu *zoomMenu;
    QMenu *subdivisionMenu;
    QMenu *displayMenu;
    QMenu *windowMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *openGLToolBar;
    QToolBar *zoomToolBar;
    QToolBar *subdivisionToolBar;
    QToolBar *displayToolBar;

    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *closeAllAct;
    QAction *exitAct;

    QAction *undoAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;

    QAction *antialiasingAct;
    QAction *lightingAct;
    QAction *shadingAct;
    QActionGroup *effectsGroup;

    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *rotateAct;

    QAction *catmullAct;
    QAction *dooAct;
    QAction *loopAct;
    QAction *catmullLimitAct;
    QAction *loopLimitAct;
    QAction *generalizedSubAct;
    QAction *previousStepAct;
    QAction *nextStepAct;

    QAction *vertexAct;
    QAction *wireframeAct;
    QAction *faceAct;
    QActionGroup *displayGroup;

    QAction *controlMeshAct;

    QAction *sharpFeatureAct;

    QAction *newWindowAct;
    QAction *cascadeAct;
    QAction *tileAct;
    QAction *arrangeIconsAct;

    QAction *toolbarAct;
    QAction *statusBarAct;

    QAction *aboutAct;
    struct _SubMode
    {
        bool CAT, CAT_LIMIT, DOO, LOOP, LOOP_LIMIT, GENERAL;
    }SubMode;

    bool isTabClosing;

    TopManager *manager;
    QTabWidget *tabWidget;
    int lastTabIndex;

};

#endif // MAINWINDOW_H
