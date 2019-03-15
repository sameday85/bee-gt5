#include <QFile>
#include "classroom.h"
#

ClassRoom::ClassRoom(QString &username, QString& dictionary, int& mode)
{
    mUsername = username;
    mDictionary = dictionary;
    mMode = mode;
}

int ClassRoom::prepare(int forGrade) {
    loadDictionary();
    selectedTotal = 0;

    if (mWordList.isEmpty())
        return 0;

    switch (mMode) {
    case MODE_PRACTICE:
        for (int i = 0; i < mWordList.size(); ++i) {
            if ((forGrade == 0) || (mWordList.at(i).getGrade() == 0) || (mWordList.at(i).getGrade() == forGrade)) {
                index[selectedTotal++]=i;
            }
        }
        if (selected < 0 || selected >= selectedTotal)
            selected = 0;
        break;
    }
    return mWordList.size();
}

Word* ClassRoom::getCurrentWord() {
    return &currentWord;
}

int ClassRoom::present() {
    if (selected >= selectedTotal)
        return RC_FINISHED_ALL;
    int selection = index[selected++];
    currentWord = mWordList.at(selection);
    currentWord.pronounceWord();

    failures = 0;
    return RC_OK;
}

int ClassRoom::onAnswer(QString answer) {
    int ret = RC_HELP;
    if (answer == "c") {
        currentWord.pronounceCategory();
    }
    else if (answer == "d") {
        currentWord.pronounceDefinition();
    }
    else if (answer == "s") {
        currentWord.pronounceSample();
    }
    else if (answer == "r") {
        currentWord.pronounceWord();
    }
    else if (answer == "?") {
        ret = (mMode == MODE_PRACTICE) ? RC_RETRY : RC_SKIP;
        ++failures;
    }
    else if (answer == currentWord.getSpelling()) {
        ret = RC_CORRECT;
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
}

//load word list from given file
void ClassRoom::loadDictionary () {
    mWordList.clear();

    QFile file(mDictionary);
    if (!file.open(QIODevice::ReadOnly))
        return;

   QTextStream in(&file);
   QString line = in.readLine();
   if (line == "#Ver1.0") {
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
