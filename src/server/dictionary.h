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

private:

    std::shared_mutex mutex;
    robin_hood::unordered_map<std::string, std::string> mDict;

    std::unique_ptr<struct bloom_filter> pBloomFilter;
};

#endif // _DICTIONARY_H_