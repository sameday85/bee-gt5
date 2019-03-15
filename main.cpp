#include "mainwindow.h"
#include <QApplication>
#include <QStandardPaths>
#include <QDir>

char szApplicationDir[255];
char szAppCacheDir[255];

int main(int argc, char *argv[])
{
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
