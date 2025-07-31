#ifndef SERIALCOMMANDHANDLER_H
#define SERIALCOMMANDHANDLER_H

#include "ICommandProcessor.h"
#include "application/IMemoryAnalyzer.h"      // IMemoryAnalyzer 인터페이스 포함
#include "application/MemoryMonitorService.h" // MemoryMonitorService 포함
#include <string>
#include <memory> // std::shared_ptr 사용

/**
 * @brief 시리얼 명령어 처리 구현체 (Infrastructure Layer)
 *        - ICommandProcessor 인터페이스 구현
 *        - 시리얼 입력 명령어 파싱 및 응답 처리
 */
class SerialCommandHandler : public ICommandProcessor
{
public:
    /**
     * @brief 생성자. Memory Analyzer에 대한 의존성을 주입받습니다.
     * @param memoryAnalyzer 메모리 분석 서비스에 대한 포인터
     */
    SerialCommandHandler(std::shared_ptr<IMemoryAnalyzer> memoryAnalyzer);
    virtual ~SerialCommandHandler();

    /**
     * @brief 명령어 문자열을 파싱 및 실행
     * @param command 입력 명령어(예: "memtest", "help", ...)
     * @return 처리 결과 메시지(응답 문자열)
     */
    std::string processCommand(const std::string &command) override;

private:
    std::shared_ptr<IMemoryAnalyzer> _memoryAnalyzer; // 메모리 분석기 인스턴스

    // 명령어별 처리 메서드
    std::string handleHelp();
    std::string handleMemTest();
    std::string handleMemStress();
    std::string handleMemStruct();
    std::string handleMemFree();
    std::string handleMemToggle();
};

#endif // SERIALCOMMANDHANDLER_H
