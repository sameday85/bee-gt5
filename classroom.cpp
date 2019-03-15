#include <QFile>
#include "classroom.h"

#define PLACEMENT_CORRECT_PTG           80
#define PLACEMENT_WORDS_EACH_GRADE      10

ClassRoom::ClassRoom(QString &username, QString& dictionary, int& mode)
{
    mUsername = username;
    mDictionary = dictionary;
    mMode = mode;

    loadDictionary();
    statsLifetime.load(mUsername);
}

int ClassRoom::prepare(int forGrade) {
    started = false;
    mGrade = forGrade;
    if (!multipleGradeSupported)
        mGrade = 0;
    if (mMode == MODE_PLACE)
        mGrade = 1;
    selectedTotal = 0;
    if (mWordList.isEmpty())
        return 0;

    stats.reset();
    switch (mMode) {
    case MODE_PRACTICE:
        chooseWords(0, false);
        selected = statsLifetime.getWordIndexLastPracticed(mDictionary, mGrade);
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

void ClassRoom::chooseWords(int max, bool random) {
    for (int i = 0; i < mWordList.size(); ++i) {
        if ((mGrade == 0) || (mWordList.at(i).getGrade() == 0) || (mWordList.at(i).getGrade() == mGrade)) {
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
    selected = 0;
}

Statistics *ClassRoom::getStatisticLifetime() {
    return &statsLifetime;
}

Statistics *ClassRoom::getStatistic() {
    return &stats;
}

Word* ClassRoom::getCurrentWord() {
    return &currentWord;
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
    //repeat
    selected = selected % selectedTotal;
    int selection = index[selected];
    currentWord = mWordList.at(selection);
    currentWord.pronounceWord();

    failures = 0;
    statsLifetime.incAsked();
    stats.incAsked();
    return RC_OK;
}

int ClassRoom::onAnswer(QString answer) {
    int ret = RC_HELP;
    if (answer == "c") {
        currentWord.pronounceCategory(true);
    }
    else if (answer == "c!") {
        currentWord.pronounceCategory(false);
    }
    else if (answer == "d") {
        currentWord.pronounceDefinition(true);
    }
    else if (answer == "d!") {
        currentWord.pronounceDefinition(false);
    }
    else if (answer == "s") {
        currentWord.pronounceSample(true);
    }
    else if (answer == "s!") {
        currentWord.pronounceSample(false);
    }
    else if (answer == "r") {
        currentWord.pronounceWord();
    }
    else if (answer == "r!") {
        currentWord.pronounceWordAlt();
    }
    else if (answer == "?") {
        ret = (mMode == MODE_PRACTICE) ? RC_RETRY : RC_SKIP;
        ++failures;
    }
    else if (QString::compare(answer, currentWord.getSpelling(), Qt::CaseInsensitive) == 0) {
        ret = RC_CORRECT;
        if (failures <= 0) {
            statsLifetime.incCorrect();
            stats.incCorrect();
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
    mWordList.clear();
    if (mMode == MODE_PRACTICE)
        statsLifetime.setWordIndexLastPracticed(mDictionary, mGrade, selected);
    statsLifetime.save();
}

//load word list from given file
void ClassRoom::loadDictionary () {
    mWordList.clear();
    multipleGradeSupported = false;

    QString fullPath = QString::asprintf("%s/%s/%s", szApplicationDir, FOLDER_DICT, mDictionary.toStdString().c_str());
    QFile file(fullPath);
    if (!file.open(QIODevice::ReadOnly))
        return;

   QTextStream in(&file);
   QString line = in.readLine();
   if (line == "#Ver1.0") {
       multipleGradeSupported = true;
       loadV10(in);
   }
   else if (line == "#Ver1.1") {
       loadV11(in);
   }
   file.close();
}

void ClassRoom::loadV10 (QTextStream &in) {
    QString line;
    int grade = 1, word_sequence = 1;
    while (!in.atEnd()) {
        line = in.readLine();

        QString tmp;
        tmp.sprintf("%d.", word_sequence);
        if (line == tmp) {
            //skip the blank line
            in.readLine();
            //read the word
            line = in.readLine();
            Word word(line.trimmed());
            word.setGrade(grade);

            //skip blank line
            in.readLine();
            //read the class
            line = in.readLine();
            word.setCategory(line.trimmed());
            //skip blank line
            in.readLine();
            //read the definition
            line = in.readLine();
            word.setDefinition(line.trimmed());
            line = in.readLine();
            if (!line.isEmpty()) {
                word.setDefinition(word.getDefinition() + " " + line);
            }

            //set the sample sentence
            line = "Sorry, sample sentence is not avaiable";
            word.setSample(line);
            mWordList.append(word);

            if (word_sequence++ >= 100) {
                word_sequence = 1;
                ++grade;
            }
        }
    }
}

void ClassRoom::loadV11 (QTextStream &in) {
    QString line;
    while (!in.atEnd()) {
        line = in.readLine();
        if (line.isEmpty())
            break;
        //word
        Word word(line);
        word.setGrade(0);

        //category
        line = in.readLine();
        word.setCategory(line.trimmed());

        //definition
        line = in.readLine();
        word.setDefinition(line.trimmed());
        //example
        line = in.readLine();
        word.setSample(line.trimmed());
        //audio
        line = in.readLine();
        word.setAudio(line.trimmed());
        //skip blank line
        in.readLine();

        mWordList.append(word);
    }
}

void ClassRoom::say(QString mp3) {
    QString path = QString::asprintf("%s/%s", szApplicationDir, mp3.toStdString().c_str());
    speaker.play(path);
}
