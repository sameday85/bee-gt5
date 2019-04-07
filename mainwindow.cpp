#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <qtimer.h>
#include "common.h"
#include "logindialog.h"
#include "statisticsdialog.h"

#define DEFAULT_DICT        "2018.xml"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createActions();
    createMenus();
    loadSettings();

    connect( ui->lineEditWord,
             SIGNAL (returnPressed()),
             this,
             SLOT (onEnterKey())
             );

    mAutoPopup = false;
    mMode = MODE_NA;
}

void MainWindow::createMenus()
{
    userMenu = menuBar()->addMenu(tr("&User"));
    userMenu->addAction(loginAct);
    userMenu->addAction(statsAct);
    userMenu->addAction(retryAct);
    userMenu->addSeparator();
    userMenu->addAction(aboutAct);

    ui->mainToolBar->addAction(loginAct);
}

//https://stackoverflow.com/questions/14356121/how-to-call-function-after-window-is-shown
void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent( event );

    onUpdateUi();
    if (!mAutoPopup) {
        mAutoPopup = true;
        QTimer::singleShot(50, this, SLOT(login()));
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    logout();
    QMainWindow::closeEvent(event);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createActions()
{
    QIcon icon(":/SpellingBee.png");
    loginAct = new QAction(tr("Log&in..."), this);
    //loginAct->setShortcuts(QKeySequence::Login);
    loginAct->setStatusTip(tr("User Login/Logout"));
    loginAct->setIcon(icon);
    connect(loginAct, &QAction::triggered, this, &MainWindow::loginOrLogout);

    statsAct = new QAction(tr("&Statistics..."), this);
    statsAct->setStatusTip(tr("Statistics"));
    connect(statsAct, &QAction::triggered, this, &MainWindow::viewStatistics);

    retryAct = new QAction(tr("&Restart"), this);
    retryAct->setStatusTip(tr("Try again"));
    connect(retryAct, &QAction::triggered, this, &MainWindow::retry);

    aboutAct = new QAction(tr("About..."), this);
    aboutAct->setStatusTip(tr("About Everyday Spelling Bee"));
    connect(aboutAct, &QAction::triggered, this, &MainWindow::sayHello);
}

void MainWindow::sayHello() {
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("About Everyday Spelling Bee");
    msgBox.setText("Everyday Spelling Bee");
    msgBox.setInformativeText("Version 1.0.0\r\nCopyright(c) 2019. All rights reserved.");
    msgBox.setStandardButtons(QMessageBox::Ok);

    QGridLayout* layout = (QGridLayout*)msgBox.layout();
    QSpacerItem* horizontalSpacer = new QSpacerItem(460, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    msgBox.exec();
}

void MainWindow::loginOrLogout() {
    if (mMode == MODE_NA)
        login();
    else
        logout();
}

void MainWindow::login() {
    QString dictPath=QString::asprintf("%s/%s", szApplicationDir, FOLDER_DICT);
    QDir directory(dictPath);
    QStringList dictionaryList = directory.entryList(QStringList() << "*.xml", QDir::Files);

    LoginDialog* loginDialog = new LoginDialog( this );
    loginDialog->setDictionaryList(dictionaryList);
    loginDialog->setUsername(mUsername);
    loginDialog->setDictionary(mDictionary);
    loginDialog->setGrade(mGrade);

    connect( loginDialog,
     SIGNAL (acceptLogin(QString&,QString&,int&, int&)),
     this,
     SLOT (slotOnLogin(QString&,QString&, int&, int&)));
    loginDialog->exec();
}

void MainWindow::viewStatistics() {
    if ((mMode == MODE_NA) || (classRoom == NULL))
        return;

    StatisticsDialog* statsDialog = new StatisticsDialog( this, classRoom->getStatistic(), classRoom->getStatisticLifetime());
    statsDialog->exec();
}

void MainWindow::logout() {
    if (classRoom) {
        classRoom->dismiss();

        delete classRoom;
        classRoom = NULL;
    }
    if (mMode != MODE_NA) {
        mMode = MODE_NA;
        saveSettings();
        onUpdateUi();
     }
}

void MainWindow::retry() {
    if (!mDone)
        return;

    onStart();
    onUpdateUi();
}

void MainWindow::onEnterKey() {
    QString answer = ui->lineEditWord->text();
    if (answer.isEmpty())
        return;
    ui->lineEditWord->setText("");
    if (mMode == MODE_NA || mDone) {
        Speaker speaker;
        speaker.read_sentence_online(answer);
    }
    else {
        int ret = classRoom->onAnswer(answer);
        if (ret == RC_RETRY || ret == RC_SKIP) {
            Word *word = classRoom->getCurrentWord();
            ui->labelWordDetails->setText(word->getSpelling()+": "+word->getCategory() + ", " + word->getDefinition());
        }
        if (ret == RC_CORRECT || ret == RC_SKIP) {
            ret = classRoom->present();
            ui->progressBar->setValue(classRoom->getProgress());
            if (ret == RC_FINISHED_ALL) {
                mDone = true;
                if (mMode == MODE_QUIZ) {
                    retryAct->setEnabled(true);
                    showStats(ui->labelStats, classRoom->getStatistic());
                }
                else if (mMode == MODE_PLACE) {
                    retryAct->setEnabled(true);
                    showPlaceResult(ui->labelStats, classRoom->getFinishedGrade());
                }
            }
        }
    }
}

void MainWindow::slotOnLogin(QString& username,QString& dictionary, int &grade, int&mode) {
    mMode = mode;
    mDone = false;
    if (mMode != MODE_NA) {
        mUsername = username;
        mGrade = grade;
        mDictionary = dictionary;
        if (mMode == MODE_PLACE)
            mDictionary=DEFAULT_DICT;
        classRoom = new ClassRoom(mUsername, mDictionary, mMode);
        onStart();
    }
    onUpdateUi();
}

void MainWindow::loadSettings() {
   QSettings settings(ORG_NAME, APP_NAME);

   settings.beginGroup("User");
   mUsername=settings.value("UserName").toString();
   mDictionary=settings.value("Dictionary").toString();
   mGrade = settings.value("Grade").toInt();
   if (mUsername.isEmpty())
       mUsername="Guest";
   settings.endGroup();

   settings.beginGroup("Window");
   resize(settings.value("size", QSize(640, 480)).toSize());
   move(settings.value("pos", QPoint(200, 200)).toPoint());
   settings.endGroup();
}

void MainWindow::saveSettings() {
    QSettings settings(ORG_NAME, APP_NAME);

    settings.beginGroup("User");
    settings.setValue("UserName", mUsername);
    settings.setValue("Dictionary", mDictionary);
    settings.setValue("Grade", mGrade);
    settings.endGroup();

    settings.beginGroup("Window");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

void MainWindow::onStart() {
    mDone = classRoom->prepare(mGrade) <= 0;
    ui->progressBar->setRange(0, classRoom->getTotalWordsSelected());
    ui->progressBar->setValue(classRoom->getProgress());

    if (!mDone) {
        QTimer::singleShot(50, classRoom, SLOT(present()));
    }
}

void MainWindow::onUpdateUi() {
    loginAct->setText(mMode == MODE_NA ? "Log&in..." : "Log&out");
    statsAct->setEnabled(mMode != MODE_NA);
    retryAct->setEnabled(mDone && (mMode == MODE_QUIZ || mMode == MODE_PLACE));

    ui->labelWelcome->setText((mMode == MODE_NA) ? "Welcome" : "Welcome " + mUsername);
    ui->labelDictionary->setText((mMode == MODE_NA) ? "" : mDictionary);
    ui->labelWordDetails->setText("");
    ui->lineEditWord->setText("");
    ui->labelStats->setText("");

    ui->progressBar->setVisible((mMode == MODE_NA) ? false : true);
}

void MainWindow::showStats(QLabel *label, Statistics *stats) {
    QString info = QString::asprintf("Total: %d, Correct: %d, Correct Percentage: %d%%", stats->getAsked(), stats->getCorrect(), stats->getCorrect() * 100 / stats->getAsked());
    label->setText(info);
}

void MainWindow::showPlaceResult(QLabel *label, int finishedGrade) {
    QString info;
    if (finishedGrade <= 8) {
        info = QString::asprintf("Your placement is grade %d.", finishedGrade);
    }
    else {
        info = "You passed all grades! You should go to the next list.";
    }
    label->setText(info);
}
