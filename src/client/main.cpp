#include <iostream>
#include <optional>
#include <map>
#include <getopt.h>

#include <spdlog/spdlog.h>

#include <remotedict.h>


enum class Choice
{
    GET,
    SET,
    STATS,
    EXIT
};

/**
 * @brief Turn a command string into enum
 * @param cmd command string
 */
Choice parseCommand(const std::string& cmd)
{
    static const std::map<std::string, Choice> cmdMap = {
        {"get", Choice::GET}, 
        {"set", Choice::SET}, 
        {"stats", Choice::STATS},
        {"exit", Choice::EXIT}
    };
    if (const auto it = cmdMap.find(cmd); it != cmdMap.end())
    {
        return it->second;
    }
    spdlog::error("Unknown command: {}", cmd);
    return Choice::EXIT;
}



int consoleQueryLoop(RemoteDictionary& remoteDict)
{
    spdlog::info("Supported commands:");
    spdlog::info(" get KEY");
    spdlog::info(" set KEY VALUE");
    spdlog::info(" stats");

    Choice choice = Choice::EXIT;
    std::string cmd;
    std::string key, value;
    do {
        std::cout << "> ";
        std::cin >> cmd;
        choice = parseCommand(cmd);
        switch (choice) 
        {
            case Choice::GET: 
            {
                std::cin >> key;
                const auto [success, value] = remoteDict.remoteGet(key);
                spdlog::info((success? "Got back '{}'": "Could not get: {}"), value);
                break;
            }
            case Choice::SET:
            {
                std::cin >> key >> value;
                const auto [success, errorMsg] = remoteDict.remoteSet(key, value);
                spdlog::info((success? "Succesfully set '{}' to '{}'": "Could not set '{}' to '{}'"), key, value);
                if (!success)
                {
                    spdlog::info("Because: {}", errorMsg);
                }
                break;
            }
            case Choice::STATS:
            {
                const auto stats = remoteDict.remoteStats();
                spdlog::info("Stats:");
                spdlog::info("  nTotalGet = {}", stats.nTotalGet);
                spdlog::info("  nTotalSet = {}", stats.nTotalSet);
                spdlog::info("  nSuccessfulGet = {}", stats.nSuccessfulGet);
                spdlog::info("  nFailedGet = {}", stats.nFailedGet);
                break;
            }
            case Choice::EXIT:
                break;
            default:
                return 1;
        }
    } while (choice != Choice::EXIT);
    return 0;
}



int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
    spdlog::set_level(spdlog::level::info);

    static struct option long_options[] =
    {
        {"addr", required_argument, NULL, 'a'},
        {"silent", no_argument, NULL, 's'},
        {NULL, 0, NULL, 0}
    };

    std::string address = "localhost:50051";
    char ch;
    while ((ch = getopt_long(argc, argv, "b", long_options, NULL)) != -1)
    {
        switch (ch)
        {
            case 'a':
                address = optarg;
                break;
            case 's':
                spdlog::set_level(spdlog::level::err);
                break;
            default:
                return 1;
        }
    }
    RemoteDictionary remoteDict{address};
    return consoleQueryLoop(remoteDict);
}