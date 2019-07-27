#include <QFile>
#include <QXmlStreamReader>
#include "classroom.h"

#define PLACEMENT_CORRECT_PTG           80
#define PLACEMENT_WORDS_EACH_GRADE      10

ClassRoom::ClassRoom(QString &username, QString& dictionary, int& mode)
{
    mUsername = username;
    mDictionary = dictionary;
    mMode = mode;

    loadDictionary();
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
        chooseWords(0, false);
        selected = progress;
        if (selected < 0 || selected >= selectedTotal)
            selected = 0;
        break;
    case MODE_QUIZ:
        chooseWords(0, true);
        break;
    case MODE_PLACE:
        mGrade = 1;
        chooseWords(PLACEMENT_WORDS_EACH_GRADE, true);
        break;
    }
    return mWordList.size();
}

int ClassRoom::getGrade() {
    return multipleGradeSupported ? mGrade : 0;
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

Word* ClassRoom::getCurrentWord() {
    return currentWord;
}

int ClassRoom::getTotalWordsSelected() {
    return selectedTotal;
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
        if (mMode == MODE_PRACTICE)
            selected = 0; //restart
        else if (mMode == MODE_QUIZ)
            return RC_FINISHED_ALL;
        else if (mMode == MODE_PLACE) {
            if (stats.getCorrectPercentage() < PLACEMENT_CORRECT_PTG)
                return RC_FINISHED_ALL;
            if (++mGrade <= 8) {
                chooseWords(PLACEMENT_WORDS_EACH_GRADE, true);
                stats.reset();
            }
            else {
                return RC_FINISHED_ALL;
            }
        }
    }
    currentWord = mWordList.at(index[selected]);
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
    if (answer == "c") {
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
    else if (QString::compare(answer, currentWord->getSpelling(), Qt::CaseInsensitive) == 0) {
        ret = RC_CORRECT;
        if (failures <= 0) {
            stats.incAnswered();
        }
        say((failures <= 0) ? "perfect.wav" : "pass.wav");
    }
    else {
        ++failures;
        ret = (mMode == MODE_PRACTICE) ? RC_FAILED : RC_SKIP;
        say("sorry.wav");
    }
    return ret;
}

void ClassRoom::dismiss() {
    currentWord = NULL;
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
    Word *ptrWord;
    while(!xmlReader->atEnd() && !xmlReader->hasError()) {
            // Read next element
            QXmlStreamReader::TokenType token = xmlReader->readNext();
            if(token == QXmlStreamReader::StartElement) {
                if(xmlReader->name() == "word") {
                    ptrWord = new Word();
                    mWordList.append(ptrWord);
                    foreach(const QXmlStreamAttribute &attr, xmlReader->attributes()) {
                        if (attr.name().toString() == "grade") {
                            int grade = attr.value().toInt();
                            ptrWord->setGrade(grade);
                            if (grade > 1)
                                multipleGradeSupported = true;
                        }
                    }
                }
                else if (xmlReader->name() == "spelling") {
                    ptrWord->setSpelling(xmlReader->readElementText());
                }
                else if (xmlReader->name() == "category") {
                    ptrWord->setCategory(xmlReader->readElementText());
                }
                else if (xmlReader->name() == "definitions") {
                    ptrWord->setDefinition(xmlReader->readElementText());
                }
                else if (xmlReader->name() == "example") {
                    ptrWord->setSample(xmlReader->readElementText());
                }
                else if (xmlReader->name() == "audio") {
                    ptrWord->setAudio(xmlReader->readElementText());
                }
            }
    }
    delete xmlReader;

    file.close();
}

void ClassRoom::say(QString mp3) {
    QString path = QString::asprintf("%s/%s", szApplicationDir, mp3.toStdString().c_str());
    speaker.play_offline(path);
}
