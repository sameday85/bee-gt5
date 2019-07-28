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
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void showEvent(QShowEvent *ev);
    void closeEvent(QCloseEvent *event);

public slots:
    void sayHello();
    void slotOnLogin(QString&,QString&, int&, int&);
    void slotOnResetStats();

private:
    Ui::MainWindow *ui;

    void createActions();
    void createMenus();
    void lookup(QString word);
    void showWord(Word *word);
    void showWordEx(WordEx *word);

    QMenu *userMenu;
    QAction *loginAct;
    QAction *retryAct;
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

private:
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
    void showPlaceResult(QLabel *label, int finishedGrade);
};

#endif // MAINWINDOW_H
