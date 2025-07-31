#ifndef MOCK_SERIAL_COMMAND_HANDLER_H
#define MOCK_SERIAL_COMMAND_HANDLER_H

#include "../../src/infrastructure/ICommandProcessor.h"
#include "MockMemoryMonitorService.h"
#include <string>
#include <memory>
#include <map>
#include <functional>

/**
 * @class MockSerialCommandHandler
 * @brief ICommandProcessor 인터페이스의 완전한 Header-Only Mock 구현체
 * @details 실제 SerialCommandHandler의 모든 비즈니스 로직을 포함하되,
 *          의존성은 Mock으로 대체한 완전 독립적인 테스트용 구현
 */
class MockSerialCommandHandler : public ICommandProcessor
{
private:
    std::shared_ptr<MockMemoryMonitorService> _memoryAnalyzer;
    std::map<std::string, std::string> _responses; // 테스트용 응답 저장
    using HandlerFunc = std::string (MockSerialCommandHandler::*)();
    std::map<std::string, HandlerFunc> _handlerMap;

public:
    MockSerialCommandHandler(std::shared_ptr<MockMemoryMonitorService> memoryAnalyzer)
        : _memoryAnalyzer(memoryAnalyzer)
    {
        // 핸들러 맵 초기화 (this 캡처 없이 멤버 함수 포인터 사용)
        _handlerMap["help"] = &MockSerialCommandHandler::getHelpMessageHandler;
        _handlerMap["menu"] = &MockSerialCommandHandler::getHelpMessageHandler;
        _handlerMap["memory"] = &MockSerialCommandHandler::getFreeMemoryHandler;
        _handlerMap["memory structure"] = &MockSerialCommandHandler::getStructureAnalysisHandler;
        _handlerMap["mem struct"] = &MockSerialCommandHandler::getStructureAnalysisHandler;
        _handlerMap["memory runtime"] = &MockSerialCommandHandler::getRuntimeAnalysisHandler;
        _handlerMap["mem runtime"] = &MockSerialCommandHandler::getRuntimeAnalysisHandler;
        _handlerMap["memory toggle"] = &MockSerialCommandHandler::getToggleMonitoringHandler;
        _handlerMap["mem toggle"] = &MockSerialCommandHandler::getToggleMonitoringHandler;
        _handlerMap["status"] = &MockSerialCommandHandler::getStatusHandler;
    }

    // ICommandProcessor 인터페이스 구현 (실제 비즈니스 로직과 동일)
    std::string processCommand(const std::string &command) override
    {
        std::string response;
        auto it = _handlerMap.find(command);
        if (it != _handlerMap.end())
        {
            response = (this->*it->second)();
        }
        else if (command == "status")
        {
            response = "System Status: Running";
        }
        else
        {
            response = "Unknown command: " + command + ". Type 'help' for available commands.";
        }
        _responses[command] = response;
        return response;
    }

    // Test Helper Methods
    std::string getLastResponse(const std::string &command) const
    {
        auto it = _responses.find(command);
        return (it != _responses.end()) ? it->second : "";
    }

    void clearResponses() { _responses.clear(); }

private:
    std::string getHelpMessageHandler() { return getHelpMessage(); }
    std::string getFreeMemoryHandler() { return _memoryAnalyzer ? _memoryAnalyzer->getFreeMemory() : "No analyzer"; }
    std::string getStructureAnalysisHandler() { return _memoryAnalyzer ? _memoryAnalyzer->getStructureAnalysis() : "No analyzer"; }
    std::string getRuntimeAnalysisHandler() { return _memoryAnalyzer ? _memoryAnalyzer->getRuntimeAnalysis() : "No analyzer"; }
    std::string getToggleMonitoringHandler() { return _memoryAnalyzer ? _memoryAnalyzer->toggleMonitoring() : "No analyzer"; }
    std::string getStatusHandler() { return "System Status: Running"; }

    std::string getHelpMessage()
    {
        return "Available commands:\n"
               "  help, menu          - Show this help message\n"
               "  memory              - Show current free memory\n"
               "  memory structure    - Show memory structure info\n"
               "  memory runtime      - Show runtime memory analysis\n"
               "  memory toggle       - Toggle memory monitoring\n"
               "  status              - Show system status";
    }
};

#endif // MOCK_SERIAL_COMMAND_HANDLER_H
