#ifndef _REMOTE_DICT_H_
#define _REMOTE_DICT_H_

#include <utility>
#include <string>
#include <memory>

class RemoteDictionary
{
public:
    RemoteDictionary(const std::string& remoteAddress);
    ~RemoteDictionary();

    /**
     * @brief Remote get procedure
     * @param key key
     * @return pair of success boolean, and string of value if success, else error message
     */
    std::pair<bool, std::string> remoteGet(const std::string& key);

    /**
     * @brief Remote set procedure
     * @param key key
     * @param value value to set
     * @return pair of success boolean, and error message in case of failure
     */
    std::pair<bool, std::string> remoteSet(const std::string& key, const std::string& value);

private:

    // hides dependency on grpc
    std::unique_ptr<struct RemoteDictionaryImpl> pImpl;
};

#endif // _REMOTE_DICT_H_