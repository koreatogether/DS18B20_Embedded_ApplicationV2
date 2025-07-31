#include "SerialCommandHandler.h"
#include <sstream>
#include <map>
#include <functional>

SerialCommandHandler::SerialCommandHandler(std::shared_ptr<IMemoryAnalyzer> memoryAnalyzer)
    : _memoryAnalyzer(memoryAnalyzer) {}

SerialCommandHandler::~SerialCommandHandler() {}

std::string SerialCommandHandler::processCommand(const std::string &command)
{
    static const std::map<std::string, std::function<std::string()>> commandMap = {
        {"help", [this]()
         { return handleHelp(); }},
        {"menu", [this]()
         { return handleHelp(); }},
        {"memtest", [this]()
         { return handleMemTest(); }},
        {"memstress", [this]()
         { return handleMemStress(); }},
        {"memstruct", [this]()
         { return handleMemStruct(); }},
        {"memfree", [this]()
         { return handleMemFree(); }},
        {"memtoggle", [this]()
         { return handleMemToggle(); }},
        {"memtrack", [this]()
         { return _memoryAnalyzer->toggleTracking(); }},
        {"memreport", [this]()
         { return _memoryAnalyzer->getMemoryReport(); }}};

    auto it = commandMap.find(command);
    if (it != commandMap.end())
    {
        return it->second();
    }
    return "Unknown command: " + command;
}

std::string SerialCommandHandler::handleHelp()
{
    std::ostringstream oss;
    oss << "--- Command Menu ---\n";
    oss << "memtest   : Run memory runtime analysis.\n";
    oss << "memstress : Run memory stress test (same as memtest).\n";
    oss << "memstruct : Show memory structure analysis.\n";
    oss << "memfree   : Show current free memory.\n";
    oss << "memtoggle : Toggle periodic memory monitoring.\n";
    oss << "memtrack  : Toggle memory change tracking.\n";
    oss << "memreport : Generate memory tracking report.\n";
    oss << "help/menu : Show this menu.\n";
    oss << "--------------------";
    return oss.str();
}

std::string SerialCommandHandler::handleMemTest()
{
    return _memoryAnalyzer->getRuntimeAnalysis();
}

std::string SerialCommandHandler::handleMemStress()
{
    return _memoryAnalyzer->getRuntimeAnalysis();
}

std::string SerialCommandHandler::handleMemStruct()
{
    return _memoryAnalyzer->getStructureAnalysis();
}

std::string SerialCommandHandler::handleMemFree()
{
    return _memoryAnalyzer->getFreeMemory();
}

std::string SerialCommandHandler::handleMemToggle()
{
    return _memoryAnalyzer->toggleMonitoring();
}
