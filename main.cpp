#include "mainwindow.h"
#include "defines.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(!OldGLSLVersion)
    {
        QSurfaceFormat format;
        format.setDepthBufferSize(24);

        format.setStencilBufferSize(8);
        format.setVersion(3, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);

        QSurfaceFormat::setDefaultFormat(format);
    }


    MainWindow w;
    w.resize(640, 480);
    int desktopArea = QApplication::desktop()->width() *
                     QApplication::desktop()->height();
    int widgetArea = w.width() * w.height();
    if (((float)widgetArea / (float)desktopArea) < 0.75f)
        w.show();
    else
        w.showMaximized();
    a.addLibraryPath("./plugins");



    return a.exec();
}
