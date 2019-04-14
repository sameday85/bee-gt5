#include <QStringList>
#include "urlconnection.h"
#include "dicthelper.h"

DictHelper::DictHelper(QString word)
{
    mWord = word;
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

bool DictHelper::download() {
    QString url = QString::asprintf("https://en.oxforddictionaries.com/definition/%s", mWord.toStdString().c_str());
    UrlConnection urlConnection;
    QString page = urlConnection.download2String(url);
    mCategory = parseCategory(page);
    mDefinitions = parseDefinitions(page, mWord);
    mExample = parseExample(page);
    mAudio = parseAudio(page);

    return !mCategory.isEmpty() || !mDefinitions.isEmpty() || !mDefinitions.isEmpty();
}

QString DictHelper::getCategory() const
{
    return mCategory;
}

QString DictHelper::getDefinitions() const
{
    return mDefinitions;
}

QString DictHelper::getExample() const
{
    return mExample;
}

QString DictHelper::getAudio() const
{
    return mAudio;
}
