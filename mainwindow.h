#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <dbmanager.h>
#include "classroom.h"
#include "statistics.h"
#include "wordex.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void showEvent(QShowEvent *ev);
    void closeEvent(QCloseEvent *event);
    void changeEvent(QEvent *event);

public slots:
    void sayHello();
    void convert();
    void slotOnLogin(QString&,QString&, int&, int&);
    void slotOnResetStats();

private:
    Ui::MainWindow *ui;

    void createActions();
    void createMenus();
    void lookup(QString word);
    void showWord(WordEx *word);
    void showWordEx(WordEx *word);

    QMenu *userMenu;
    QAction *loginAct;
    QAction *retryAct;
    QAction *toolAct;
    QAction *aboutAct;
    QAction *statsAct;

private slots:
    void loginOrLogout();
    void login();
    void logout();
    void retry();
    void viewStatistics();
    void onEnterKey();
    void afterPresent();
    void autoAdvance();

    void alarm();

private:
    bool mMainWindowInactive;
    int  mInactiveWarnings;
    DbManager *mDbManager;
    ClassRoom *classRoom;
    QString mDictionary;
    qlonglong mDictId;
    QString mUsername;
    qlonglong mUserId;
    int mGrade, mLastGoodGrade, mMode;
    bool mDone, mAutoPopup, mPaused;
    qlonglong mStatId;
    int mPendingTimer;

    void loadSettings();
    void saveSettings();
    void saveTodayStats();
    void onStart();
    void onUpdateUi();
    void showHelp();
    void showStats(QLabel *label, Statistics *stats);
};

#endif // MAINWINDOW_H
