# DS18B20 Embedded Application 모듈 인터페이스

## Domain Layer
- `ITemperatureSensor`: 센서 추상화 인터페이스
- `TemperatureSensorManager`: 센서 관리/데이터 갱신

## Application Layer
- `IMemoryAnalyzer`: 메모리 분석 인터페이스
- `MemoryMonitorService`: 메모리 모니터링/로깅
- `IMemoryTracker`: 메모리 변화 추적
- `MemoryTrackerService`: 메모리 이력 관리

## Infrastructure Layer
- `ICommandProcessor`: 명령어 처리 인터페이스
- `SerialCommandHandler`: 시리얼 명령어 파싱/응답

## HAL Layer
- `IHal`: 하드웨어 추상화 인터페이스
- `ArduinoHal`: 실제 하드웨어 구현

## 테스트/Mock
- `MockHal`, `MockMemoryMonitorService`, `MockMemoryTracker` 등

---
각 인터페이스/클래스의 역할과 주요 메서드는 실제 코드와 동기화하여 지속적으로 업데이트하세요.
