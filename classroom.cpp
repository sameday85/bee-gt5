#include <QFile>
#include <QXmlStreamReader>
#include "classroom.h"
#include "dicthelper.h"

#define WORDS_PER_GRADE                 100
#define MAX_GRADE                       8

ClassRoom::ClassRoom(QString &username, QString& dictionary, int& mode)
{
    mUsername = username;
    mDictionary = dictionary;
    mMode = mode;
    selected = 0;
    dbManager = nullptr;
    mCaseSensitive = true;

    loadDictionary();
}

void ClassRoom::setDbManger(DbManager *manager) {
    dbManager = manager;
}

void ClassRoom::setCaseSensitive(bool b) {
    mCaseSensitive = b;
}

int ClassRoom::prepare(int forGrade, int progress) {
    started = tried = false;
    mGrade = forGrade;
    if (!multipleGradeSupported)
        mGrade = 0;
    selectedTotal = 0;
    if (mWordList.isEmpty())
        return 0;

    stats.reset();
    switch (mMode) {
    case MODE_PRACTICE:
    case MODE_LEARNING:
        chooseWords(0, false);
        selected = progress;
        if (selected < 0 || selected >= selectedTotal)
            selected = 0;
        break;
    case MODE_QUIZ:
        chooseWords(0, true);
        break;
    }
    return mWordList.size();
}

int ClassRoom::adjustGrade(int grade) {
    if (grade > mMaxGrade)
        grade = mMaxGrade;
    return multipleGradeSupported ? grade : 0;
}

void ClassRoom::chooseWords(int max, bool random) {
    selectedTotal = selected = 0;
    for (int i = 0; i < mWordList.size(); ++i) {
        if ((mGrade == 0) || (mWordList.at(i)->getGrade() == 0) || (mWordList.at(i)->getGrade() == mGrade)) {
            index[selectedTotal++]=i;
        }
    }
    if (max > 0 && selectedTotal > max)
        selectedTotal = max;
    if (random) {
        qsrand(QDateTime::currentMSecsSinceEpoch() / 1000);
        for (int i = 0; i < selectedTotal; ++i) {
            int pos1 = qrand() % selectedTotal;
            int pos2 = qrand() % selectedTotal;

            int tmp = index[pos1];
            index[pos1]=index[pos2];
            index[pos2]=tmp;
        }
    }
}

Statistics *ClassRoom::getStatistic() {
    return &stats;
}

WordEx* ClassRoom::getCurrentWord() {
    return currentWord;
}

int ClassRoom::getTotalWordsSelected() {
    return selectedTotal;
}

int ClassRoom::getTotalWords() {
    return mWordList.size();
}

int ClassRoom::getFinishedGrade() {
    return mGrade;
}

int ClassRoom::getProgress() {
    return selected;
}

int ClassRoom::present() {
    if (mWordList.size() <= 0)
        return RC_FINISHED_ALL;

    if (started) {
        //move to next word
        ++selected;
    }
    else {
        started = true;
    }
    if (selected >= selectedTotal) {
        selected = 0;
        if (mMode == MODE_QUIZ)
            return RC_FINISHED_ALL;
    }
    currentWord = mWordList.at(index[selected]);
    this->downloadWordOnlineIf(currentWord);
    if (mMode != MODE_LEARNING)
        currentWord->pronounceWord();

    failures = 0; tried = false;
    return RC_OK;
}

int ClassRoom::onAnswer(QString answer) {
    int ret = RC_HELP;
    if (!tried) {
        tried = true;

        stats.incAsked();
    }
    if (currentWord == nullptr) {
    }
    else if (answer == "c") {
        currentWord->pronounceCategory(true);
    }
    else if (answer == "c!") {
        currentWord->pronounceCategory(false);
    }
    else if (answer == "d") {
        currentWord->pronounceDefinition(true);
    }
    else if (answer == "d!") {
        currentWord->pronounceDefinition(false);
    }
    else if (answer == "s") {
        currentWord->pronounceSample(true);
    }
    else if (answer == "s!") {
        currentWord->pronounceSample(false);
    }
    else if (answer == "r") {
        currentWord->pronounceWord();
    }
    else if (answer == "r!") {
        currentWord->pronounceWordAlt();
    }
    else if (answer == "?") {
        ret = (mMode == MODE_PRACTICE) ? RC_RETRY : RC_SKIP;
        ++failures;
    }
    else if (QString::compare(answer, currentWord->getSpelling(), mCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive) == 0) {
        ret = RC_CORRECT;
        currentWord = nullptr;
        if (failures <= 0) {
            stats.incAnswered();
        }
        say((failures <= 0) ? "perfect.wav" : "pass.wav");
    }
    else {
        ++failures;
        ret = (mMode == MODE_PRACTICE) ? RC_RETRY : RC_SKIP;
        say("sorry.wav");
    }
    return ret;
}

void ClassRoom::dismiss() {
    currentWord = nullptr;
    for (int i = 0; i < mWordList.size(); ++i)
        delete mWordList.at(i);
    mWordList.clear();
}

//load word list from given file
void ClassRoom::loadDictionary () {
    mWordList.clear();
    multipleGradeSupported = false;

    QString fullPath = QString::asprintf("%s/%s/%s", szApplicationDir, FOLDER_DICT, mDictionary.toStdString().c_str());
    QFile file(fullPath);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QXmlStreamReader *xmlReader = new QXmlStreamReader(&file);
    //Parse the XML until we reach end of it
    QString spelling, category, definition, example, audio;
    mMaxGrade = 0;
    int grade = 0;
    while(!xmlReader->atEnd() && !xmlReader->hasError()) {
            // Read next element
            QXmlStreamReader::TokenType token = xmlReader->readNext();
            if(token == QXmlStreamReader::StartElement) {
                if(xmlReader->name() == "word") {
                    grade = 0;
                    spelling = category = definition = example = audio = "";

                    foreach(const QXmlStreamAttribute &attr, xmlReader->attributes()) {
                        if (attr.name().toString() == "grade") {
                            grade = attr.value().toInt();
                            if (grade > 1)
                                multipleGradeSupported = true;
                        }
                    }
                }
                else if (xmlReader->name() == "spelling") {
                    spelling = xmlReader->readElementText();
                }
                else if (xmlReader->name() == "category") {
                    category = xmlReader->readElementText();
                }
                else if (xmlReader->name() == "definitions") {
                    definition = xmlReader->readElementText();
                }
                else if (xmlReader->name() == "example") {
                    example = xmlReader->readElementText();
                }
                else if (xmlReader->name() == "audio") {
                    audio = xmlReader->readElementText();
                }
            }
            else if (token == QXmlStreamReader::EndElement) {
                if(xmlReader->name() == "word") {
                    if (!spelling.isEmpty()) {
                        bool found = false;
                        for (int w = 0; w < mWordList.size(); ++w) {
                            WordEx *one = mWordList.at(w);
                            if (one->getSpelling().compare(spelling, Qt::CaseInsensitive) == 0) {
                                found = true;
                                break;
                            }
                        }
                        if (!found) {
                            if (grade <= 0) {
                                //set default grade base on the word index
                                grade = (mWordList.size() / WORDS_PER_GRADE) + 1;
                                if (grade > MAX_GRADE)
                                    grade = MAX_GRADE;
                            }
                            WordEx * word = new WordEx(spelling);
                            word->setGrade(grade);

                            WordCategory *wordCategory = new WordCategory(category, audio);
                            wordCategory->addSense(new WordSense(definition, example));
                            word->addCategory(wordCategory);

                            mWordList.append(word);

                            if (grade > mMaxGrade)
                                mMaxGrade = grade;
                        }
                    }
                }
            }
    }
    delete xmlReader;
    multipleGradeSupported = true;
    file.close();
}

void ClassRoom::say(QString mp3) {
    QString path = QString::asprintf("%s/%s", szApplicationDir, mp3.toStdString().c_str());
    speaker.play_offline(path);
}

void ClassRoom::downloadWordOnlineIf(WordEx *word) {
    if (word->getTriedDownload())
        return;
    word->setTriedDownload(true);
    if (word->getDefinition().isEmpty() || word->getCategory().isEmpty()) {
        DictHelper *helper = new DictHelper();
        helper->download(word, dbManager);
        delete helper;
    }
}
