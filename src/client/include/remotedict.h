#ifndef _REMOTE_DICT_H_
#define _REMOTE_DICT_H_

#include <optional>
#include <string>
#include <memory>

class RemoteDictionary
{
public:
    struct Stats
    {
        int64_t nTotalGet = 0;
        int64_t nTotalSet = 0;
        int64_t nSuccessfulGet = 0;
        int64_t nFailedGet = 0;
    };

    RemoteDictionary(const std::string& remoteAddress);
    ~RemoteDictionary();

    /**
     * @brief Remote get procedure
     * @param key key
     * @return optional value
     */
    std::optional<std::string> remoteGet(const std::string& key);

    /**
     * @brief Remote set procedure
     * @param key key
     * @param value value to set
     * @return success state
     */
    bool remoteSet(const std::string& key, const std::string& value);

    /**
     * @brief Remote get stats procedure
     */
    Stats remoteStats();

private:

    // hides dependency on grpc
    std::unique_ptr<struct RemoteDictionaryImpl> pImpl;
};

#endif // _REMOTE_DICT_H_