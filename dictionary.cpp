#include "dictionary.h"

Dictionary* Dictionary::mInstance = nullptr;

Dictionary::Dictionary() {}

Dictionary* Dictionary::instance()
{
    if (mInstance == nullptr)
        mInstance = new Dictionary();

    return mInstance;
}

void Dictionary::add(const QString &word, const QList<QString> &translations, const QSet<float> &usages)
{
    QPair<QList<QString>, QSet<float>> pair;
    pair.first = translations;
    pair.second = usages;

    mDict.insert(word, pair);
}

void Dictionary::addTranslation(const QString &word, const QString &transl)
{
    mDict[word].first.append(transl);
}

void Dictionary::addUsage(const QString &word, const float &usage)
{
    mDict[word].second.insert(usage);
}

void Dictionary::remove(const QString &word)
{
    mDict.remove(word);
}

QList<QString> Dictionary::translations(const QString &word) const
{
    return mDict[word].first;
}

QSet<float> Dictionary::usages(const QString &word) const
{
    return mDict[word].second;
}

bool Dictionary::contains(const QString &word) const
{
    return mDict.contains(word);
}

int Dictionary::size() const
{
    return mDict.size();
}

std::map<QString, QPair<QList<QString>, QSet<float> > >
Dictionary::toStdMap() const
{
    return mDict.toStdMap();
}
