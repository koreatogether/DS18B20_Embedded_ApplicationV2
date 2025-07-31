#include <Arduino.h>
#include "infrastructure/ICommandProcessor.h"
#include "infrastructure/SerialCommandHandler.h"
#include "application/IMemoryAnalyzer.h"
#include "application/MemoryMonitorService.h"
#include "hal/IHal.h"
#include "hal/ArduinoHal.h"
#include <memory>

// 링커 오류 해결을 위해 __brkval 심볼을 main.cpp에 정의
#ifdef __cplusplus
extern "C"
{
#endif
  char *sbrk(int incr);
  char *__brkval = (char *)sbrk(0);
#ifdef __cplusplus
}
#endif

// 각 인터페이스를 가리키는 스마트 포인터 선언
std::shared_ptr<IHal> hal;
std::shared_ptr<IMemoryAnalyzer> memoryAnalyzer;
std::unique_ptr<ICommandProcessor> commandProcessor;

// put function declarations here:

void setup()
{
  // 시리얼 통신을 115200 속도로 초기화
  Serial.begin(115200);
  // 시리얼 포트가 연결될 때까지 대기
  while (!Serial)
    ;

  // --- 의존성 주입 (Dependency Injection) ---
  // 1. HAL (Hardware Abstraction Layer) 객체 생성
  hal = std::make_shared<ArduinoHal>();

  // 2. Application Layer의 MemoryMonitorService 객체 생성 시 HAL 주입
  memoryAnalyzer = std::make_shared<MemoryMonitorService>(hal);

  // 3. Infrastructure Layer의 SerialCommandHandler 객체 생성 시,
  //    Application Layer의 객체(memoryAnalyzer)를 주입
  commandProcessor = std::make_unique<SerialCommandHandler>(memoryAnalyzer);

  Serial.println("\n========================================");
  Serial.println(" DS18B20 Embedded System Initialized");
  Serial.println("========================================");
  Serial.println("Type 'help' or 'menu' for a list of commands.");

  // 초기화 메시지 후 첫 메모리 상태 출력
  memoryAnalyzer->getFreeMemory();
}

void loop()
{
  // 주기적인 메모리 상태 확인
  memoryAnalyzer->periodicCheck();

  // 시리얼 버퍼에 수신된 데이터가 있는지 확인
  if (Serial.available() > 0)
  {
    // 개행 문자('\n')를 만날 때까지 문자열을 읽어옴
    String commandStr = Serial.readStringUntil('\n');

    // 앞뒤 공백 제거
    commandStr.trim();

    // 읽어온 명령어가 비어있지 않다면 처리
    if (commandStr.length() > 0)
    {
      Serial.print("> ");
      Serial.println(commandStr);

      // Arduino String을 std::string으로 변환하여 명령어 처리기에 전달
      std::string response = commandProcessor->processCommand(commandStr.c_str());

      // 처리 결과를 시리얼 모니터에 출력
      Serial.println(response.c_str());
    }
  }
}

// put function definitions here:
// 더 이상 사용하지 않는 예제 함수 제거