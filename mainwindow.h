#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "classroom.h"

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
    QAction *logoutAct;

private slots:
    void login();
    void logout();
    void onEnterKey();


private:
    ClassRoom *classRoom;
    QString mDictionary;
    QString mUsername;
    int mGrade, mMode;

    void loadSettings();
    void saveSettings();
    void onUpdateUi();
};

#endif // MAINWINDOW_H
