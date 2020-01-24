#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QProgressDialog>
#include <QFileDialog>
#include <QSettings>
#include <QDir>
#include <qtimer.h>
#include "common.h"
#include "logindialog.h"
#include "statisticsdialog.h"
#include "dicthelper.h"

#define DEFAULT_DICT        "2018.xml"
#define AUTO_DELAY          10000   //ms

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

    mAutoPopup = mPaused = false;
    mMode = MODE_NA;

    QString dbPath=QString::asprintf("%s/bee.db", szAppCacheDir);
    mDbManager = new DbManager(dbPath);
    mDbManager->createDbIf();
    mStatId = mUserId = mDictId = 0;
    mPendingTimer = 0;

    mMainWindowInactive = false;
    mInactiveWarnings = 0;
}

void MainWindow::createMenus()
{
    userMenu = menuBar()->addMenu(tr("&User"));
    userMenu->addAction(loginAct);
    userMenu->addAction(statsAct);
    userMenu->addAction(retryAct);
    userMenu->addSeparator();
    userMenu->addAction(toolAct);
    userMenu->addSeparator();
    userMenu->addAction(aboutAct);

    ui->mainToolBar->addAction(loginAct);
    ui->mainToolBar->addAction(statsAct);
}

//https://stackoverflow.com/questions/14356121/how-to-call-function-after-window-is-shown
void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent( event );

    onUpdateUi();
    ui->textEdit->setReadOnly(true);
    if (!mAutoPopup) {
        mAutoPopup = true;
        showHelp();
        QTimer::singleShot(50, this, SLOT(login()));
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    logout();
    QMainWindow::closeEvent(event);
}

void MainWindow::changeEvent(QEvent *event) {
    QWidget::changeEvent(event);
    if (event->type() == QEvent::ActivationChange) {
        mMainWindowInactive =  !this->isActiveWindow();
        if (mMainWindowInactive && (mMode != MODE_NA)) {
            ++mInactiveWarnings;
            QTimer::singleShot(500, this, SLOT(warning()));
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mDbManager;
}

void MainWindow::createActions()
{
    QIcon icon(":/SpellingBee.png");
    loginAct = new QAction(tr("Log&in..."), this);
    //loginAct->setShortcuts(QKeySequence::Login);
    loginAct->setStatusTip(tr("User Login/Logout"));
    loginAct->setIcon(icon);
    connect(loginAct, &QAction::triggered, this, &MainWindow::loginOrLogout);

    QIcon icon2(":/piechart.png");
    statsAct = new QAction(tr("&Statistics..."), this);
    statsAct->setStatusTip(tr("Statistics"));
    statsAct->setIcon(icon2);
    connect(statsAct, &QAction::triggered, this, &MainWindow::viewStatistics);

    retryAct = new QAction(tr("&Restart"), this);
    retryAct->setStatusTip(tr("Try again"));
    connect(retryAct, &QAction::triggered, this, &MainWindow::retry);

    toolAct = new QAction(tr("Tool..."), this);
    toolAct->setStatusTip(tr("Conversion tool"));
    connect(toolAct, &QAction::triggered, this, &MainWindow::convert);

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

    QGridLayout* layout = dynamic_cast<QGridLayout*>(msgBox.layout());
    QSpacerItem* horizontalSpacer = new QSpacerItem(460, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    msgBox.exec();
}
//convert work list txt file to dictionary xml file
void MainWindow::convert() {
    QString fileName = QFileDialog::getOpenFileName(this,
                            tr("Word List Text File"), "", tr("Text Files (*.txt)"));
    //User cancelled
    if (fileName == nullptr)
        return;
    //file does not exit
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    //load in all words
    QStringList words;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            break;
        words.append(line);
    }
    file.close();

    QString msg = QString::asprintf("Total words %d. Converting, please wait ...", words.count());
    QProgressDialog progress(msg, "Cancel", 0, words.count(), this);
    progress.setWindowModality(Qt::WindowModal);

    QString dictPath=QString::asprintf("%s/%s/", szApplicationDir, FOLDER_DICT);
    QString outputFileName = dictPath + "/" + QFileInfo(fileName).completeBaseName() + ".xml";
    //create output file
    QFile outputFile(outputFileName);
    if (outputFile.open(QIODevice::Truncate|QIODevice::WriteOnly)) {
        outputFile.write(QString("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n").toUtf8());
        outputFile.write(QString("<words>\r\n").toUtf8());
        for (int i = 0; i < words.count(); ++i) {
            QString line = words.at(i);
            WordEx word(line);
            DictHelper *helper = new DictHelper();
            bool ret = helper->download(&word, nullptr);
            delete helper;
            if (ret) {
                QString xmlSlice=QString::asprintf("<word idx=\"%d\" grade=\"%d\">\r\n", i+1, 0);
                xmlSlice += "  <spelling>" + word.getSpelling().trimmed() + "</spelling>\r\n";
                xmlSlice += "  <category>" + word.getCategory().trimmed() + "</category>\r\n";
                xmlSlice += "  <definitions>" + word.getDefinition().trimmed() + "</definitions>\r\n";
                xmlSlice += "  <example>" + word.getSample().trimmed() + "</example>\r\n";
                xmlSlice += "  <audio>" + word.getAudio().trimmed() + "</audio>\r\n";
                xmlSlice += "</word>\r\n";
                outputFile.write(xmlSlice.toUtf8());
            }
            progress.setValue(i+1);
            if (progress.wasCanceled())
                break;
        }
        outputFile.write(QString("</words>\r\n").toUtf8());
        outputFile.close();
    }
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
    loginDialog->setGrade(mGrade > 0 ? mGrade : mLastGoodGrade);

    connect( loginDialog,
     SIGNAL (acceptLogin(QString&,QString&,int&, int&)),
     this,
     SLOT (slotOnLogin(QString&,QString&, int&, int&)));
    loginDialog->exec();
}

void MainWindow::viewStatistics() {
    if ((mMode == MODE_NA) || (classRoom == nullptr))
        return;

    saveTodayStats();
    Statistics *perDict = mDbManager->findDictStatsBy(mUserId, mDictId);
    Statistics *lifetime= mDbManager->findLifetimeStatsBy(mUserId);
    StatisticsDialog* statsDialog = new StatisticsDialog( this,
                                                          (mMode == MODE_LEARNING) ? nullptr : classRoom->getStatistic(), //today stats
                                                          perDict,
                                                          lifetime,
                                                          mDbManager->findDailyStatsBy(mUserId));
    connect( statsDialog,
     SIGNAL (resetStats()),
     this,
     SLOT (slotOnResetStats()));

    mPaused = true;
    statsDialog->exec();
    mPaused = false;
}

void MainWindow::logout() {
    if (classRoom) {
        saveTodayStats();
        classRoom->dismiss();

        delete classRoom;
        classRoom = nullptr;
    }
    if (mMode != MODE_NA) {
        mMode = MODE_NA;
        saveSettings();
        onUpdateUi();
    }
    mStatId = mUserId = mDictId = 0;
}

void MainWindow::retry() {
    if (!mDone)
        return;

    onStart();
    onUpdateUi();
}

void MainWindow::onEnterKey() {
    QString answer = (mMode == MODE_LEARNING) ? "?" : ui->lineEditWord->text();
    if (answer.isEmpty())
       return;

    ui->lineEditWord->setText("");
    if (mMode == MODE_NA || mDone) {
        lookup(answer);
    }
    else {
        int ret = classRoom->onAnswer(answer);
        if (ret == RC_RETRY || ret == RC_SKIP) {
            showWord(classRoom->getCurrentWord());
        }
        if (ret == RC_CORRECT || ret == RC_SKIP) {
            ret = classRoom->present();
            if (mMode == MODE_LEARNING)
                afterPresent();
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
    mDone = mPaused = false;
    mPendingTimer = 0;
    if (mMode != MODE_NA) {
        mUsername = username;
        mGrade = grade;
        mDictionary = dictionary;
        mUserId = mDbManager->insertUserIf(mUsername);
        mDictId = mDbManager->insertDictIf(mDictionary);
        mStatId = 0;
        if (mMode == MODE_PLACE)
            mDictionary=DEFAULT_DICT;
        classRoom = new ClassRoom(mUsername, mDictionary, mMode);
        classRoom->setDbManger(mDbManager);
        onStart();
    }
    onUpdateUi();
}

void MainWindow::slotOnResetStats() {
    mDbManager->resetDictStatsBy(mUserId, mDictId);
}

void MainWindow::loadSettings() {
   QSettings settings(ORG_NAME, APP_NAME);

   settings.beginGroup("User");
   mUsername=settings.value("UserName").toString();
   mDictionary=settings.value("Dictionary").toString();
   mGrade = mLastGoodGrade = settings.value("Grade").toInt();
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
    if (mGrade > 0)
        settings.setValue("Grade", mGrade);
    settings.endGroup();

    settings.beginGroup("Window");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

void MainWindow::saveTodayStats() {
    if (classRoom == nullptr)
        return;
    if (mMode != MODE_PRACTICE && mMode != MODE_QUIZ && mMode != MODE_LEARNING)
        return;

    if (mMode == MODE_PRACTICE || mMode == MODE_QUIZ) {
        Statistics *stats = classRoom->getStatistic();
        if (mStatId > 0) {
            mDbManager->updateStat(mStatId, stats->getAsked(), stats->getAnswered());
        }
        else {
            mStatId = mDbManager->insertStat(mUserId, mDictId, stats->getAsked(), stats->getAnswered());
        }
    }
    if (mMode == MODE_PRACTICE || mMode == MODE_LEARNING)
        mDbManager->updatePositionBy(mUserId, mDictId, mGrade, mMode, classRoom->getProgress());
}

void MainWindow::onStart() {
    mGrade = classRoom->adjustGrade(mGrade);
    int progress = (mMode == MODE_PRACTICE || mMode == MODE_LEARNING) ? mDbManager->getPositionBy(mUserId, mDictId, mGrade, mMode, 0) : 0;
    mDone = classRoom->prepare(mGrade, progress) <= 0;
    if (mGrade > 0)
        mLastGoodGrade = mGrade;

    ui->progressBar->setRange(0, classRoom->getTotalWordsSelected());
    ui->progressBar->setValue(classRoom->getProgress());

    if (!mDone) {
        QTimer::singleShot(50, classRoom, SLOT(present()));
        if (mMode == MODE_LEARNING)
            QTimer::singleShot(100, this, SLOT(afterPresent()));
    }
}

void MainWindow::afterPresent() {
    if (mMode == MODE_LEARNING) {
        showWord(classRoom->getCurrentWord());
        ++mPendingTimer;
        QTimer::singleShot(AUTO_DELAY, this, SLOT(autoAdvance()));
    }
}

void MainWindow::autoAdvance() {
    if (mMode != MODE_LEARNING)
        return;
    if (--mPendingTimer > 0)
        return;
    if (mPaused) {
        ++mPendingTimer;
        QTimer::singleShot(200, this, SLOT(autoAdvance()));
    }
    else {
        onEnterKey();
    }
}

void MainWindow::onUpdateUi() {
    loginAct->setText(mMode == MODE_NA ? "Log&in..." : "Log&out");
    statsAct->setEnabled(mMode != MODE_NA);
    retryAct->setEnabled(mDone && (mMode == MODE_QUIZ || mMode == MODE_PLACE));

    ui->labelWelcome->setText((mMode == MODE_NA) ? "Welcome" : "Welcome " + mUsername);
    ui->labelDictionary->setText((mMode == MODE_NA) ? "" : QString::asprintf("%s. Grade %d. Total %d/%d words", mDictionary.toStdString().c_str(), mGrade, classRoom->getTotalWordsSelected(), classRoom->getTotalWords()));
    ui->lineEditWord->setText("");
    ui->labelStats->setText("");

    ui->progressBar->setVisible((mMode == MODE_NA) ? false : true);
    ui->textEdit->setText("");
}

void MainWindow::showStats(QLabel *label, Statistics *stats) {
    QString info = QString::asprintf("Total: %d, Correct: %d, Correct Percentage: %.2f%%", stats->getAsked(), stats->getAnswered(), stats->getCorrectPercentage());
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

void MainWindow::lookup(QString spelling) {
    WordEx *theWord = new WordEx(spelling);

    DictHelper helper;
    bool ret = helper.download(theWord, mDbManager);
    if (ret) {
        showWordEx(theWord);
    }
    else {
        ui->textEdit->setText(spelling);
    }
    delete theWord;

    Speaker speaker;
    speaker.read_sentence_online(spelling);
}

void MainWindow::showHelp() {
    //QString help = "c-category,d-definition, s-sample sentence, r-read again";
    //ui->textEdit->setText(help);
}

void MainWindow::showWord(WordEx *word) {
    QString spelling = word->getSpelling();
    if (!spelling.isEmpty()) {
        spelling.replace(0, 1, spelling[0].toUpper());
    }
    QString category = word->getCategory();
    int pos1 = category.indexOf("(");
    int pos2 = category.indexOf(")");
    if (pos2 > pos1 && pos1 >= 0)
        category = category.mid(pos1+1, pos2-pos1-1);

    QString openingTag="<p>", closingTag="</p>";
    QString content = "<h1>" + spelling + "</h1>";
    content += openingTag + category + closingTag;
    content += openingTag + word->getDefinition() + closingTag;
    content += openingTag + + "<i>" + word->getSample() + + "</i>" + closingTag;
    ui->textEdit->setText(content);
}

void MainWindow::showWordEx(WordEx *word) {
    QString spelling = word->getSpelling();
    if (!spelling.isEmpty()) {
        spelling.replace(0, 1, spelling[0].toUpper());
    }

    QString openingTag="<p>", closingTag="</p>";
    QString content = "<h1>" + spelling + "</h1>";
    QList<WordCategory *>*categories = word->getCategories();
    for (int i = 0; i < categories->count(); ++i) {
        content += openingTag + "<font color='orange'>" + categories->at(i)->getCategory() + +"</font>" + closingTag;
        QList<WordSense*>* senses = categories->at(i)->getSenses();
        for (int j = 0; j < senses->count(); ++j) {
            WordSense *sense = senses->at(j);
            content += openingTag + QString::number(j+1) + ". " + sense->getDefinition() + closingTag;
            if (!sense->getExample().isEmpty())
                content += openingTag + "<i>'" + sense->getExample() + "'</i>" + closingTag;
        }
    }
    ui->textEdit->setText(content);
}

void MainWindow::warning() {
    int thisWarning = mInactiveWarnings;
    while (mMainWindowInactive && mMode != MODE_NA && !mPaused && (thisWarning == mInactiveWarnings)) {
        Speaker speaker;
        QString path = QString::asprintf("%s/%s", szApplicationDir, "warning.mp3");
        speaker.play_offline(path);
    }
}
