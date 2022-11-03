#include <iostream>
#include <optional>
#include <map>

#include <remotedict.h>


enum class Choice
{
    GET,
    SET,
    STATS,
    EXIT
};

Choice parseCommand(const std::string& cmd)
{
    static const std::map<std::string, Choice> cmdMap = {
        {"get", Choice::GET}, 
        {"set", Choice::SET}, 
        {"stats", Choice::STATS}
    };
    if (const auto it = cmdMap.find(cmd); it != cmdMap.end())
    {
        return it->second;
    }
    std::cout << "Unknown command " << cmd;
    return Choice::EXIT;
}


int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
    std::cout << "Supported commands:" << std::endl;
    std::cout << " get KEY" << std::endl;
    std::cout << " set KEY VALUE" << std::endl;
    std::cout << " stats" << std::endl;

    Choice choice = Choice::EXIT;
    std::string cmd;
    std::string key, value;
    RemoteDictionary remoteDict{"localhost:50051"};
    do {
        std::cout << "> ";
        std::cin >> cmd;
        choice = parseCommand(cmd);
        switch (choice) 
        {
            case Choice::GET: 
            {
                std::cin >> key;
                const auto value = remoteDict.remoteGet(key);
                std::cout << (value? "Got back " + *value: "Key not found");
                break;
            }
            case Choice::SET:
            {
                std::cin >> key >> value;
                auto success = remoteDict.remoteSet(key, value);
                std::cout << (success? "Succesfully set ": "Could not set ") << key << " to " << value;
                break;
            }
            case Choice::STATS:
            {
                const auto stats = remoteDict.remoteStats();
                std::cout << "Stats:" << std::endl;
                std::cout << "  nTotalGet = " << stats.nTotalGet << std::endl;
                std::cout << "  nTotalSet = " << stats.nTotalSet << std::endl;
                std::cout << "  nSuccessfulGet = " << stats.nSuccessfulGet << std::endl;
                std::cout << "  nFailedGet = " << stats.nFailedGet << std::endl;
                break;
            }
            default:
                break;
        }
        std::cout << std::endl;
    } while (choice != Choice::EXIT);
    return 0;
}