# [업데이트: 2025-07-31]
- PlatformIO native 환경에서 31개 유닛테스트 모두 성공적으로 통과.
- 대형 Mock/헬퍼 코드가 `test/mocks/utils/` 등으로 분리되어 가독성 및 유지보수성 대폭 향상.
- 모든 테스트는 `test/test_header_only_complete_system.cpp`에서 통합 관리.
- 최신 테스트/모듈화 현황은 `docs/needFix/test_validity_analysis.md` 참고.
# DS18B20_Embedded_Application

Arduino UNO R4 WiFi 기반 DS18B20 온도 센서 + 메모리 분석 통합 시스템

## 주요 특징
- **다중 DS18B20 온도 센서**: 자동 탐지 및 실시간 데이터 수집
- **메모리 분석 시스템**: 실시간 사용량 분석, 누수 탐지, 스트레스 테스트
- **시리얼 명령어 인터페이스**: 대화형 메모리 테스트 및 모니터링
- **로그 출력**: CSV/마크다운 형식의 구조화된 데이터 로깅
- **클린 아키텍처**: SOLID 원칙과 DIP(의존성 역전) 기반 계층적 구조
- **테스트 자동화**: Unity 프레임워크 기반 유닛/통합 테스트
- **크로스 플랫폼**: PlatformIO 및 Arduino IDE 호환

## 아키텍처 구조

### 클린 아키텍처 계층
```
┌─────────────────────────────────────────┐
│           Infrastructure Layer          │  ← 시리얼 명령어 처리
├─────────────────────────────────────────┤
│            Application Layer            │  ← 메모리 분석 서비스
├─────────────────────────────────────────┤
│              Domain Layer               │  ← 온도 센서 관리
├─────────────────────────────────────────┤
│                HAL Layer                │  ← 하드웨어 추상화
└─────────────────────────────────────────┘
```

### 폴더 구조
```
├── .gitignore
├── .pio/                        # PlatformIO 빌드 산출물
├── .vscode/                     # VSCode 설정
├── docs/                        # 설계, 분석, 가이드 문서
│   ├── adviceAi/               # AI 기반 개발 가이드
│   ├── exProject/              # 프로젝트 예제 및 가이드
│   ├── Functions/              # 기능 명세서
│   └── Plan/                   # 개발 계획서
├── include/                     # 전역 헤더 파일
├── lib/                         # 외부 라이브러리
├── platformio.ini               # PlatformIO 설정
├── README.md                    # 프로젝트 개요(본 파일)
├── src/                         # 메인 소스 코드 (클린 아키텍처)
│   ├── application/            # 애플리케이션 계층
│   │   ├── IMemoryAnalyzer.h   # 메모리 분석 인터페이스
│   │   ├── MemoryMonitorService.* # 메모리 모니터링 서비스
│   │   └── MemoryTrackerService.* # 메모리 추적 서비스
│   ├── domain/                 # 도메인 계층
│   │   ├── ITemperatureSensor.h # 온도 센서 인터페이스
│   │   └── TemperatureSensorManager.* # 센서 관리자
│   ├── hal/                    # 하드웨어 추상화 계층
│   │   ├── IHal.h             # HAL 인터페이스
│   │   └── ArduinoHal.h       # Arduino HAL 구현
│   ├── infrastructure/         # 인프라 계층
│   │   ├── ICommandProcessor.h # 명령어 처리 인터페이스
│   │   └── SerialCommandHandler.* # 시리얼 명령어 핸들러
│   └── main.cpp               # 메인 애플리케이션
└── test/                       # 테스트 코드
    ├── etc/                   # 기타 테스트
    ├── integration/           # 통합 테스트
    ├── interfaces/            # 테스트용 인터페이스
    ├── logs/                  # 테스트 로그
    ├── mocks/                 # Mock 객체
    │   ├── MockHal.h         # HAL Mock
    │   ├── MockMemoryMonitorService.h
    │   ├── MockSerialCommandHandler.h
    │   └── Mock*.h           # 기타 Mock 객체들
    └── test_header_only_complete_system.cpp # 통합 테스트
```

## 시리얼 명령어 인터페이스

### 메모리 분석 명령어
- `memtest` - 런타임 메모리 분석 및 스트레스 테스트
- `memstress` - 메모리 스트레스 테스트 실행
- `memstruct` - 메모리 구조 분석 (힙, 스택 정보)
- `memfree` - 현재 가용 메모리 조회
- `memtoggle` - 주기적 메모리 모니터링 활성화/비활성화

### 시스템 명령어
- `help` / `menu` - 사용 가능한 명령어 목록 표시

### 출력 형식
- **CSV 형식**: `타임스탬프,이벤트타입,값`
- **Markdown 형식**: `| 타임스탬프 | 이벤트타입 | 값 |`

## 핵심 기능

### 메모리 분석 시스템
- **실시간 모니터링**: 주기적(10초) 메모리 상태 체크
- **누수 탐지**: 메모리 사용 패턴 분석을 통한 누수 감지
- **스트레스 테스트**: 다양한 시나리오의 메모리 부하 테스트
- **구조 분석**: 힙, 스택, 전역 변수 영역 분석

### 온도 센서 관리
- **자동 탐지**: OneWire 버스의 DS18B20 센서 자동 검색
- **다중 센서**: 여러 센서 동시 관리 및 데이터 수집
- **센서 ID**: 각 센서의 고유 ID 기반 관리

## 개발 환경 설정

### PlatformIO (권장)
```bash
# 프로젝트 빌드
pio run

# 업로드
pio run --target upload

# 시리얼 모니터
pio device monitor

# 테스트 실행 (PC 환경)
pio test -e native
```

### Arduino IDE
1. Arduino IDE에서 프로젝트 폴더 열기
2. 보드: Arduino UNO R4 WiFi 선택
3. 라이브러리 설치:
   - DallasTemperature (^3.9.0)
   - OneWire (^2.3.7)
   - ArduinoJson (^6.21.3)
4. 컴파일 및 업로드

## 테스트 시스템

### 테스트 환경
- **Unity 테스트 프레임워크**: C 기반 유닛 테스트
- **Mock 객체**: 하드웨어 독립적인 테스트
- **PC 환경 테스트**: Arduino 없이 로컬에서 실행 가능
- ** 클라우드 codacy 플랫폼 사용 보안 , 코드 품질 등등 총체적 분석중

### 테스트 실행
```bash
# 모든 테스트 실행
pio test -e native

# 특정 테스트 실행
pio test -e native --filter test_memory_service_initialization
```

### 테스트 커버리지
- MemoryMonitorService 테스트
- SerialCommandHandler 테스트  
- Memory Tracker 테스트
- Memory Leak Detection 테스트
- Stress Test Manager 테스트

## 의존성 주입 패턴

시스템은 의존성 역전 원칙(DIP)을 따라 설계되었습니다:

```cpp
// main.cpp에서의 의존성 주입 예시
hal = std::make_shared<ArduinoHal>();
memoryAnalyzer = std::make_shared<MemoryMonitorService>(hal);
commandProcessor = std::make_unique<SerialCommandHandler>(memoryAnalyzer);
```

이를 통해 각 계층이 추상화에 의존하여 테스트 용이성과 유지보수성을 확보했습니다.

## 참고 문서
- [프로젝트 계획서](docs/Plan/frimePlan.md)
- [환경 설정 가이드](docs/Plan/01_environment_setup_guide.md)
- [핵심 기능 설계](docs/Plan/02_core_feature_design.md)
- [저장소 분석 및 리팩토링 계획](docs/adviceAi/repository_analysis_and_refactoring_plan.md)



---
