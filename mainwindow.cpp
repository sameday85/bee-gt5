#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <qtimer.h>
#include "common.h"
#include "logindialog.h"

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

}

void MainWindow::createMenus()
{
    userMenu = menuBar()->addMenu(tr("&User"));
    userMenu->addAction(loginAct);
    userMenu->addAction(logoutAct);

    logoutAct->setEnabled(false);
}

//https://stackoverflow.com/questions/14356121/how-to-call-function-after-window-is-shown
void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent( event );

    QTimer::singleShot(50, this, SLOT(login()));
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
    loginAct = new QAction(tr("Log&in"), this);
    //loginAct->setShortcuts(QKeySequence::Login);
    loginAct->setStatusTip(tr("User Login"));
    connect(loginAct, &QAction::triggered, this, &MainWindow::login);

    logoutAct = new QAction(tr("Log&out"), this);
    //loginAct->setShortcuts(QKeySequence::Login);
    logoutAct->setStatusTip(tr("Logout"));
    connect(logoutAct, &QAction::triggered, this, &MainWindow::logout);

}

void MainWindow::sayHello() {
    QMessageBox msgBox;
    msgBox.setText("Hello");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard |
                              QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    msgBox.exec();
}

void MainWindow::login() {
    QString dictPath=QString::asprintf("%s/%s", szApplicationDir, FOLDER_DICT);
    QDir directory(dictPath);
    QStringList dictionaryList = directory.entryList(QStringList() << "*.txt", QDir::Files);

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

void MainWindow::onEnterKey() {
    if (mMode == MODE_NA)
        return;
    QString answer = ui->lineEditWord->text();
    if (answer.isEmpty())
        return;
    ui->lineEditWord->setText("");
    int ret = classRoom->onAnswer(answer);
    if (ret == RC_RETRY || ret == RC_SKIP) {
        Word *word = classRoom->getCurrentWord();
        ui->labelWordDetails->setText(word->getSpelling()+": "+word->getCategory() + ", " + word->getDefinition());
    }
    if (ret == RC_CORRECT || ret == RC_SKIP) {
        ret = classRoom->present();
        if (ret == RC_FINISHED_ALL) {
        }
    }
}

void MainWindow::slotOnLogin(QString& username,QString& dictionary, int &grade, int&mode) {
    mMode = mode;
    if (mMode != MODE_NA) {
        mUsername = username;
        mGrade = grade;
        mDictionary = QString::asprintf("%s/%s/%s", szApplicationDir, FOLDER_DICT, dictionary.toStdString().c_str());

        classRoom = new ClassRoom(mUsername, mDictionary, mMode);
        classRoom->prepare(mGrade);
        QTimer::singleShot(50, classRoom, SLOT(present()));
        ui->lineEditWord->setText("");
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

void MainWindow::onUpdateUi() {
    loginAct->setEnabled(mMode == MODE_NA);
    logoutAct->setEnabled(mMode != MODE_NA);

    QFileInfo fileInfo(mDictionary);
    ui->labelWelcome->setText((mMode == MODE_NA) ? "Welcome" : "Welcome " + mUsername);
    ui->labelDictionary->setText((mMode == MODE_NA) ? "" : fileInfo.fileName());
    if (mMode == MODE_NA) {
        ui->labelWordDetails->setText("");
        ui->lineEditWord->setText("");
    }
}
