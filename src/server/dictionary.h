#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

#include <optional>
#include <cstdint>
#include <shared_mutex>
#include <memory>

// Using robin_hood map instead of std unordered map because it's proven to be faster in most cases
// https://github.com/martinus/robin-hood-hashing
#include <robin_hood.h>

class Dictionary
{
public:
    struct Stats
    {
        int64_t nTotalGet = 0;
        int64_t nTotalSet = 0;
        int64_t nSuccessfulGet = 0;
        int64_t nFailedGet = 0;
    };

    Dictionary(bool useBloomFilter);
    ~Dictionary();

    /**
     * @brief Get a value from the dictionary thread-safe
     * @param key key
     */
    std::optional<std::string> get(const std::string& key);

    /**
     * @brief Set a value into the dictionary thread-safe
     * @param key key
     * @param value value to set
     */
    void set(const std::string& key, const std::string& value);

    /**
     * @brief Atomically return stats
     */
    Stats stats();

private:

    std::shared_mutex mutex;
    robin_hood::unordered_map<std::string, std::string> mDict;

    Stats mStats;

    std::unique_ptr<struct bloom_filter> pBloomFilter;
};

#endif // _DICTIONARY_H_