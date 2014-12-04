#include "unknownwords.h"

UnknownWords* UnknownWords::mInstance = nullptr;

UnknownWords::UnknownWords() {}

UnknownWords* UnknownWords::instance()
{
    if (mInstance == nullptr)
        mInstance = new UnknownWords();

    return mInstance;
}

void UnknownWords::add(const QString &word, const QSet<float> &usages)
{
    if (!contains(word))
    {
        mUnk.append(QPair<QString, QSet<float> > (word, usages));
    }
    else
    {
        for (auto it = mUnk.begin(); it != mUnk.end(); ++it)
        {
            if (it->first == word)
            {
                foreach (float u, usages)
                {
                    it->second.insert(u);
                }
                break;
            }
        }
    }
}

void UnknownWords::addUsage(const QString &word, const float &usage)
{
    for (auto it = mUnk.begin(); it != mUnk.end(); ++it)
    {
        if (it->first == word)
        {
            it->second.insert(usage);
            break;
        }
    }
}

void UnknownWords::remove(const QString &word)
{
    foreach (auto p, mUnk)
    {
        if (p.first == word)
        {
            mUnk.removeOne(p);
            break;
        }
    }
}

QSet<float> UnknownWords::usages(const QString &word) const
{
    foreach (auto p, mUnk)
    {
        if (p.first == word)
        {
            return p.second;
        }
    }
}

bool UnknownWords::contains(const QString &word) const
{
    bool res = false;

    foreach (auto p, mUnk)
    {
        if (p.first == word)
        {
            res = true;
            break;
        }
    }
    return res;
}

int UnknownWords::size() const
{
    return mUnk.size();
}

QList<QPair<QString, QSet<float> > > UnknownWords::toQList() const
{
    return mUnk;
}
