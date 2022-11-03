#include "dictionary.h"

#include <mutex>
#include <iostream>


Dictionary::Dictionary(bool useBloomFilter)
{
    if (useBloomFilter)
    {
        std::cout << "Bloom filter not yet supported" << std::endl;
        // TODO: use something like the following public library: https://github.com/ArashPartow/bloom/blob/master/bloom_filter.hpp
    }
}



std::optional<std::string> Dictionary::get(const std::string& key)
{
    std::shared_lock<std::shared_mutex> lock{mutex};

    auto it = mDict.find(key);
    mStats.nTotalGet++;
    if (it != mDict.end())
    {   
        mStats.nSuccessfulGet++;
        return it->second;
    }
    mStats.nFailedGet++;
    return {};
}



void Dictionary::set(const std::string& key, const std::string& value)
{
    std::unique_lock<std::shared_mutex> lock{mutex};
    mDict[key] = value;
    mStats.nTotalSet++;
}



Dictionary::Stats Dictionary::stats()
{
    std::shared_lock<std::shared_mutex> lock{mutex};
    return mStats;
}
