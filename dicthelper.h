#ifndef DICTHELPER_H
#define DICTHELPER_H

#include <QString>

class DictHelper
{
public:
    DictHelper(QString word);
    bool download();

    QString getCategory() const;
    QString getDefinitions() const;
    QString getExample() const;
    QString getAudio() const;

private:
    QString mWord;
    QString mCategory;
    QString mDefinitions;
    QString mExample;
    QString mAudio;

    QString parseCategory(QString content);
    QString parseDefinitions(QString content, QString word);
    QString parseExample(QString content);
    QString parseAudio(QString content);
};

#endif // DICTHELPER_H
