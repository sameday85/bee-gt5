#ifndef WORDEX_H
#define WORDEX_H

#include <QString>
#include <QList>

class WordSense {
public:
    WordSense(QString definition, QString example);
    QString getDefinition() const;
    QString getExample() const;
private:
    QString mDefinition;
    QString mExample;

};

class WordCategory {
public:
    WordCategory(QString category, QString audio);
    ~WordCategory();
    void addSense(WordSense * sense);

    QString getCategory() const;
    QString getAudio() const;
    QList<WordSense*> *getSenses();

private:
    QString mCategory;
    QString mAudio;
    QList<WordSense*> senses;
};

class WordEx
{
public:
    WordEx(QString _word);
    ~WordEx();
    QString getSpelling();
    void addCategory(WordCategory *category);
    QList<WordCategory *> *getCategories();

private:
    QString mSpelling;
    QList<WordCategory *>categories;


};

#endif // WORDEX_H
