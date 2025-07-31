#ifndef ICOMMANDPROCESSOR_H
#define ICOMMANDPROCESSOR_H

#include <string>

/**
 * @brief 통신 명령어 처리용 추상 인터페이스 (Infrastructure Layer)
 *        - 시리얼/네트워크 등 다양한 입력 소스의 명령어 파싱 및 실행을 추상화
 *        - DIP 원칙에 따라 상위 계층(애플리케이션/도메인)과의 결합도 최소화
 */
class ICommandProcessor
{
public:
    virtual ~ICommandProcessor() {}

    /**
     * @brief 명령어 문자열을 파싱 및 실행
     * @param command 입력 명령어(예: "memtest", "help", ...)
     * @return 처리 결과 메시지(응답 문자열)
     */
    virtual std::string processCommand(const std::string &command) = 0;
};

#endif // ICOMMANDPROCESSOR_H
