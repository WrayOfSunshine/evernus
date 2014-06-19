#include "APIResponseCache.h"

namespace Evernus
{
    bool APIResponseCache::hasChracterListData(Key::IdType key) const
    {
        const auto it = mCharacterListCache.find(key);
        if (it == std::end(mCharacterListCache))
            return false;

        if (QDateTime::currentDateTimeUtc() > it->second.mCachedUntil)
        {
            mCharacterListCache.erase(it);
            return false;
        }

        return true;
    }

    APIResponseCache::CharacterList APIResponseCache::getCharacterListData(Key::IdType key) const
    {
        const auto it = mCharacterListCache.find(key);
        Q_ASSERT(it != std::end(mCharacterListCache));

        return it->second.mData;
    }
}
