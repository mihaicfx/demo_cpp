#include "dictionary.h"

#include <mutex>

#include <spdlog/spdlog.h>
#include <bloom_filter.hpp>


Dictionary::Dictionary(bool useBloomFilter)
{
    if (useBloomFilter)
    {
        // Default settings tweaked from https://github.com/ArashPartow/bloom/blob/master/bloom_filter.hpp
        bloom_parameters parameters;

        // How many elements roughly do we expect to insert?
        parameters.projected_element_count = 100000;

        // Maximum tolerable false positive probability? (0,1)
        parameters.false_positive_probability = 0.0001; // 1 in 10000

        if (!parameters)
        {
            spdlog::info("Error - Invalid set of bloom filter parameters!");
            return;
        }

        parameters.compute_optimal_parameters();

        //Instantiate Bloom Filter
        pBloomFilter = std::make_unique<bloom_filter>(parameters);
    }
}



Dictionary::~Dictionary() = default;



std::optional<std::string> Dictionary::get(const std::string& key)
{
    std::shared_lock<std::shared_mutex> lock{mutex};
    mStats.nTotalGet++;

    if (!pBloomFilter || pBloomFilter->contains(key))
    {
        auto it = mDict.find(key);
        if (it != mDict.end())
        {   
            mStats.nSuccessfulGet++;
            return it->second;
        }
    }
    mStats.nFailedGet++;
    return {};
}



void Dictionary::set(const std::string& key, const std::string& value)
{
    std::unique_lock<std::shared_mutex> lock{mutex};

    if (pBloomFilter)
    {
        pBloomFilter->insert(key);
    }

    mDict[key] = value;
    mStats.nTotalSet++;
}



Dictionary::Stats Dictionary::stats()
{
    std::shared_lock<std::shared_mutex> lock{mutex};
    return mStats;
}
