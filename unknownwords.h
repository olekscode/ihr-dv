#ifndef UNKNOWNWORDS_H
#define UNKNOWNWORDS_H

#include <QList>
#include <QPair>
#include <QSet>
#include <QString>

class UnknownWords
{
    QList<QPair<QString, QSet<float> > > mUnk;
    static UnknownWords* mInstance;

public:
    static UnknownWords* instance();

    void add(const QString &word,
             const QSet<float> &usages);

    void addUsage(const QString &word,
                  const float &usage);

    void remove(const QString &word);

    QSet<float> usages(const QString &word) const;

    bool contains(const QString &word) const;

    int size() const;

    QList<QPair<QString, QSet<float> > > toQList() const;

private:
    UnknownWords();
    UnknownWords(const UnknownWords&);
    ~UnknownWords();
};

#endif // UNKNOWNWORDS_H
