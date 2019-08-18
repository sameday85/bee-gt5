#include <QStringList>
#include "urlconnection.h"
#include "dicthelper.h"
#include "common.h"

DictHelper::DictHelper(QString word)
{
    mWord = word.toLower();
}

void DictHelper::loadCredentials() {
    QString fullPath = QString::asprintf("%s/oxford.txt", szApplicationDir);
    QFile file(fullPath);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
       QString line = in.readLine();
       int pos = line.indexOf("=");
       if (pos > 0) {
           QString key = line.left(pos);
           QString value = line.mid(pos+1).trimmed();
           if (key == "id") {
               mApiId = value;
           }
           else if (key == "key") {
               mApiKey = value;
           }
           else if (key == "base") {
               mApiBase = value;
           }
       }
   }
   file.close();
}

WordEx * DictHelper::download() {
    if (mWord.isEmpty())
        return nullptr;
    WordEx *theWord = new WordEx(mWord);
    this->loadCredentials();
    if (!mApiKey.isEmpty() && !mApiId.isEmpty()) {
        if (!downloadByApi(theWord))
            downloadOnline(theWord);
    }
    return theWord;
}

bool DictHelper::downloadByApi(WordEx *word) {
    //sprintf(message, "GET /api/v2/entries/en/%s HTTP/1.1\r\nHost: od-api.oxforddictionaries.com\r\nConnection: close\r\nAccept: application/json\r\napp_id: %s\r\napp_key: %s\r\n\r\n", word, szApiId, szApiKey);
    QString url = mApiBase + "/entries/en/" + mWord;
    QNetworkRequest request(url);
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader(QString("app_id").toUtf8(), mApiId.toUtf8());
    request.setRawHeader(QString("app_key").toUtf8(), mApiKey.toUtf8());
    UrlConnection urlConnection;
    QNetworkReply *reply = urlConnection.doGet(request);
    if (urlConnection.isError()) {
        qDebug() << reply->errorString();
        return false;
    }
    QString json = urlConnection.saveToString(reply);
    if (json.indexOf("Authentication failed") >= 0)
        return false;
    QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
    QJsonArray results = document.object().value("results").toArray();
    for (int i = 0; i < results.count(); ++i) {
        QJsonArray lexicalEntries = results.at(i).toObject().value("lexicalEntries").toArray();
        for (int j = 0; j < lexicalEntries.count(); ++j) {
            QString category = lexicalEntries.at(j).toObject().value("lexicalCategory").toString();
            //pronunciations, array, audioFile
            QString audio = lexicalEntries.at(j).toObject().value("pronunciations").toArray().at(0).toObject().value("audioFile").toString();
            WordCategory *theCategory = new WordCategory(category, audio);
            word->addCategory(theCategory);

            QJsonArray entries = lexicalEntries.at(j).toObject().value("entries").toArray();
            for (int m = 0; m < entries.count(); ++m) {
                QJsonArray senses = entries.at(m).toObject().value("senses").toArray();
                for (int n = 0; n < senses.count(); ++n) {
                    //definitions, array
                    QString definition = senses.at(n).toObject().value("definitions").toArray().at(0).toString();
                    //examples, array
                    QString example = senses.at(n).toObject().value("examples").toArray().at(0).toObject().value("text").toString();
                    theCategory->addSense(new WordSense(definition, example));
                }
            }
        }
    }
/*
    QFile file("/tmp/word.json");
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out << json;
        file.close();
    }
*/
    return true;
}

bool DictHelper::downloadOnline(WordEx *word) {
    QString url = QString::asprintf("https://www.lexico.com/en/definition/%s", mWord.toStdString().c_str());
    UrlConnection urlConnection;
    QString page = urlConnection.download2String(url);
    QString category = parseCategory(page);
    QString definition = parseDefinitions(page, mWord);
    QString example = parseExample(page);
    QString audio = parseAudio(page);

    if (category.isEmpty() && definition.isEmpty())
        return false;
    WordCategory *wordCategory = new WordCategory(category, audio);
    wordCategory->addSense(new WordSense(definition, example));
    word->addCategory(wordCategory);

    return true;
}

QString DictHelper::parseCategory(QString content) {
    QString category = "";

    QString tag = "<span class=\"pos\">";
    int pos = content.indexOf(tag);
    if (pos > 0) {
        pos += tag.length();
        int pos2 = content.indexOf("</span>", pos);
        category = content.mid(pos, pos2 - pos).trimmed();
    }
    if (category.isEmpty())
        return category;

    QStringList list = category.split(" ");
    return list.at(0).trimmed();
}

QString DictHelper::parseDefinitions(QString content, QString word) {
    QString definitions = "";

    QString tag = QString::asprintf("Definition of %s - ", word.toStdString().c_str());
    int pos = content.indexOf(tag);
    if (pos > 0) {
        pos += tag.length();
        int pos2 = content.indexOf("\"", pos);
        definitions = content.mid(pos, pos2 - pos);
    }
    definitions = definitions.replace("&#39;", "'");
    return definitions;
}

QString DictHelper::parseExample(QString content) {
    QString example;

    QString tag="<div class=\"exg\">";
    int pos = content.indexOf(tag);
    if (pos > 0) {
        pos = content.indexOf("&lsquo;", pos);
        if (pos > 0) {
            pos += 7;
            int pos2 = content.indexOf("&rsquo;", pos);
            if (pos2 > pos) {
                example = content.mid(pos, pos2 - pos);
            }
        }
    }
    return example;
}

QString DictHelper::parseAudio(QString content) {
    QString audio;

    QString tag = "<audio src='";
    int pos = content.indexOf(tag);
    if (pos > 0) {
        pos += tag.length();
        int pos2 = content.indexOf("\'", pos);
        audio = content.mid(pos, pos2 - pos);
    }

    return audio;
}
