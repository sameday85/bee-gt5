#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include "classroom.h"
#include "statistics.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void showEvent(QShowEvent *ev);
    void closeEvent(QCloseEvent *event);

public slots:
    void sayHello();
    void slotOnLogin(QString&,QString&, int&, int&);

private:
    Ui::MainWindow *ui;

    void createActions();
    void createMenus();

    QMenu *userMenu;
    QAction *loginAct;
    QAction *retryAct;
    QAction *aboutAct;

private slots:
    void loginOrLogout();
    void login();
    void logout();
    void retry();
    void onEnterKey();

private:
    ClassRoom *classRoom;
    QString mDictionary;
    QString mUsername;
    int mGrade, mMode;
    bool mDone;

    void loadSettings();
    void saveSettings();
    void onStart();
    void onUpdateUi();
    void showStats(QLabel *label, Statistics *stats);
    void showPlaceResult(QLabel *label, int finishedGrade);
};

#endif // MAINWINDOW_H
