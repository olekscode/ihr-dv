#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QMap>
#include <QString>
#include <QList>
#include <QSet>
#include <QPair>

#include <map>
#include <string>
#include <list>
#include <utility>

class Dictionary
{
    QMap<QString, QPair<QList<QString>, QSet<float> > > mDict;
    static Dictionary* mInstance;

    QMap<QString, bool> tempVerified;

public:
    static Dictionary* instance();

    void add(const QString &word,
             const QList<QString> &translations,
             const QSet<float> &usages,
             bool verified = true); // should be false by default

    void addTranslation(const QString &word,
                        const QString &transl);

    void setTranslations(const QString &word,
                         const QList<QString> &translations,
                         bool verified = true);

    void addUsage(const QString &word,
                  const float &usage);

    void remove(const QString &word);

    QList<QString> translations(const QString &word) const;
    QSet<float> usages(const QString &word) const;

    bool contains(const QString &word) const;
    bool isVerified(const QString &word) const;

    int size() const;

    std::map<QString, QPair<QList<QString>, QSet<float> > >
    toStdMap() const;

private:
    Dictionary();
    Dictionary(const Dictionary&);
    ~Dictionary();
};

#endif // DICTIONARY_H
