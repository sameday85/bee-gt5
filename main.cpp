#include "mainwindow.h"
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include "runguard.h"

char szApplicationDir[255];
char szAppCacheDir[255];

int main(int argc, char *argv[])
{
    RunGuard guard( "spelling_be_instance" );
    if ( !guard.tryToRun() )
       return 0;
    //QCoreApplication::applicationDirPath()
    QApplication a(argc, argv);
    strcpy(szApplicationDir, a.applicationDirPath().toStdString().c_str());
    QString cacheFolder = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir dir = QDir::root();
    dir.mkpath(cacheFolder);
    strcpy(szAppCacheDir, cacheFolder.toStdString().c_str());

    MainWindow w;
    w.show();

    return a.exec();
}
