#ifndef WORDEX_H
#define WORDEX_H

class WordSense {
public:
    WordSense();
    QString getDefinition() const;
    void setDefinition(const QString &definition);

    QString getExample() const;
    void setExample(const QString &example);

private:
    QString mDefinition;
    QString mExample;

};

class WordCategory {
public:
    WordCategory(QString category);
    ~WordCategory();
    void addSense(WordSense * sense);

    QString getCategory() const;

    QString getAudio() const;
    void setAudio(const QString &audio);
    QArray<WordSense*> *getSenses();

private:
    QString mCategory;
    QString mAudio;
    QArray<WordSense*> senses;
};

class WordEx
{
public:
    WordEx(QString _word);
    ~WordEx();
    void addCategory(WordCategory *category);
    QList<WordCategory *> *getCategories();

private:
    QString mSpelling;
    QList<WordCategory *>categories;


};

#endif // WORDEX_H
