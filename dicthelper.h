#ifndef DICTHELPER_H
#define DICTHELPER_H

#include <QString>
#include "wordex.h"
#include "word.h"
#include "dbmanager.h"

class DictHelper
{
public:
    DictHelper();
    bool download(WordEx*, DbManager *manager);

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
    bool downloadByApi(WordEx *);
    bool downloadOnline(WordEx *);
    QString parseCategory(QString content);
    QString parseDefinitions(QString content, QString word);
    QString parseExample(QString content);
    QString parseAudio(QString content);

    QString sanitize(QString str);
};

#endif // DICTHELPER_H
