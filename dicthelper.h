#ifndef DICTHELPER_H
#define DICTHELPER_H

#include <QString>
#include "wordex.h"

class DictHelper
{
public:
    DictHelper(QString word);
    WordEx* download();

    QString getCategory() const;
    QString getDefinitions() const;
    QString getExample() const;
    QString getAudio() const;

private:
    QString mApiId;
    QString mApiKey;
    QString mApiBase;

    QString mWord;

    void loadCredentials();
    bool downloadOnline(WordEx *);
    bool downloadByApi(WordEx *);
    QString parseCategory(QString content);
    QString parseDefinitions(QString content, QString word);
    QString parseExample(QString content);
    QString parseAudio(QString content);
};

#endif // DICTHELPER_H
