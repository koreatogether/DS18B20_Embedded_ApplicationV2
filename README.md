# [업데이트: 2025-08-03]
- **실시간 테스트 자동화 시스템 완성**: 7개 시나리오 모든 테스트 통과
- **센서 ID 관리 시스템 구현**: 개별/복수 센서 ID 변경, 자동 할당, 상태 모니터링
- **대화형 메뉴 시스템**: 시리얼 인터페이스 기반 센서 제어 및 설정
- **포괄적 테스트 커버리지**: 기본 기능, 에러 처리, 경계값, 엣지 케이스 모든 검증
- **문제 해결 문서화**: 개발 과정의 모든 이슈와 해결 방법 상세 기록

# DS18B20_Embedded_Application

Arduino UNO R4 WiFi 기반 DS18B20 다중 온도 센서 관리 시스템

## 주요 특징
- **다중 DS18B20 온도 센서**: 최대 8개 센서 자동 탐지 및 실시간 데이터 수집
- **센서 ID 관리 시스템**: 개별/복수 센서 ID 변경, 주소순 자동 할당, 전체 초기화
- **대화형 메뉴 시스템**: 시리얼 인터페이스 기반 직관적인 센서 제어
- **실시간 상태 모니터링**: 15초 간격 센서 상태 테이블 자동 출력
- **포괄적 에러 처리**: 입력 검증, 중복 ID 방지, 연결 상태 확인
- **자동화 테스트 시스템**: Python 기반 실시간 테스트 프레임워크
- **클린 아키텍처**: 계층적 구조와 의존성 주입 패턴
- **크로스 플랫폼**: PlatformIO 및 Arduino IDE 호환

## 아키텍처 구조

### 시스템 아키텍처
```
┌─────────────────────────────────────────┐
│         Application Layer               │  ← 센서/메뉴 컨트롤러
├─────────────────────────────────────────┤
│           Domain Layer                  │  ← 센서 관리 로직
├─────────────────────────────────────────┤
│       Infrastructure Layer             │  ← DS18B20 하드웨어
├─────────────────────────────────────────┤
│            HAL Layer                    │  ← OneWire 통신
└─────────────────────────────────────────┘
```

### 프로젝트 구조
```
├── src/                         # 메인 펌웨어 소스 코드
│   ├── application/            # 애플리케이션 계층
│   │   ├── SensorController.*  # 센서 관리 컨트롤러
│   │   └── MenuController.*    # 메뉴 시스템 컨트롤러
│   ├── domain/                 # 도메인 계층 (기존 구조 유지)
│   ├── infrastructure/         # 인프라 계층 (기존 구조 유지)
│   └── DS18B20_Embedded_ApplicationV2.ino # 메인 아두이노 파일
├── tools/realTimeTest/         # 실시간 테스트 시스템
│   ├── scenarioJson/          # 테스트 시나리오 (7개)
│   │   ├── 01_sensor_individual_id_change_flow.json
│   │   ├── 02_multi_id_change_scenario.json
│   │   ├── 03_individual_sensor_id_change_complex_scenario.json
│   │   ├── 04_multi_sensor_id_change_complex_scenario.json
│   │   ├── 05_boundary_value_test.json
│   │   ├── 06_edge_case_test.json
│   │   └── 07_dynamic_sensor_test.json
│   ├── pyTestStart.py         # 테스트 실행기
│   ├── test_json_driven_copy.py # JSON 기반 테스트 엔진
│   ├── conftest.py           # pytest 설정
│   ├── howFixExtreamError.md  # 극한 에러 해결 기록
│   ├── howFix05-06-07.md     # 05-07번 시나리오 문제 해결 기록
│   └── logs/                 # 테스트 실행 로그
├── test/                       # 유닛 테스트 (기존 구조 유지)
├── docs/                       # 프로젝트 문서
└── platformio.ini             # PlatformIO 설정
```

## 센서 제어 인터페이스

### 메뉴 시스템
- **메인 메뉴**: `menu` 또는 `m` 입력으로 접근
- **센서 제어 메뉴**: 센서 ID 조정, 온도 임계값 설정
- **센서 ID 조정 메뉴**: 개별/복수 센서 ID 관리

### 센서 ID 관리 기능
- **개별 센서 ID 변경**: 특정 센서의 ID를 1~8 범위에서 변경
- **복수 센서 ID 변경**: 여러 센서의 ID를 한 번에 변경
- **주소순 자동 ID 할당**: 센서 주소 순서대로 ID 자동 할당
- **전체 ID 초기화**: 모든 센서 ID를 기본값으로 리셋
- **상태창 모니터링**: 실시간 센서 상태 테이블 출력

### 시스템 명령어
- `reset` - 시스템을 Normal 상태로 강제 리셋
- `menu` / `m` - 센서 제어 메뉴 진입

### 상태 출력 형식
```
| 번호 | ID  | 센서 주소           | 현재 온도 | 상한설정온도 | 상한초과상태 | 하한설정온도 | 하한초과상태 | 센서상태 |
| ---- | --- | ------------       | ---------  | ------------  | ------------ | ------------ | ------------ | -------- |
| 1    | 1   | 0x28FF1234567890AB | 25.3°C     | 30.0°C        | 정상         | 20.0°C       | 정상         | 정상     |
```

## 핵심 기능

### 센서 관리 시스템
- **자동 센서 탐지**: OneWire 버스의 DS18B20 센서 자동 검색 및 등록
- **다중 센서 지원**: 최대 8개 센서 동시 관리 및 실시간 모니터링
- **논리적 ID 관리**: 물리적 주소와 독립적인 1~8 범위의 논리적 ID 할당
- **상태 검증**: 센서 연결 상태, ID 중복, 유효성 실시간 검증

### 대화형 메뉴 시스템
- **계층적 메뉴 구조**: 직관적인 네비게이션과 명확한 상태 전환
- **입력 검증**: 범위 체크, 형식 검증, 에러 메시지 제공
- **취소 기능**: 모든 단계에서 안전한 취소 및 이전 단계 복귀
- **디버그 모드**: 상태 변화와 입력 처리 과정 실시간 출력

### 실시간 모니터링
- **주기적 상태 출력**: 15초 간격으로 센서 상태 테이블 자동 갱신
- **온도 임계값 모니터링**: 상한/하한 온도 설정 및 초과 상태 감지
- **센서 상태 추적**: 연결 상태, 온도 읽기 성공/실패 모니터링

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

### 실시간 테스트 자동화
- **Python 기반 테스트 프레임워크**: pytest를 활용한 실제 하드웨어 테스트
- **JSON 기반 시나리오**: 선언적 테스트 시나리오 정의 및 실행
- **시리얼 통신 테스트**: 실제 Arduino와 시리얼 통신을 통한 E2E 테스트
- **자동 보드 리셋**: DTR 신호를 통한 하드웨어 리셋 및 상태 초기화

### 테스트 시나리오 (7개)
1. **01_sensor_individual_id_change_flow**: 개별 센서 ID 변경 기본 플로우
2. **02_multi_id_change_scenario**: 복수 센서 ID 변경 기본 플로우
3. **03_individual_sensor_id_change_complex_scenario**: 개별 센서 복합 시나리오
4. **04_multi_sensor_id_change_complex_scenario**: 복수 센서 복합 시나리오
5. **05_boundary_value_test**: 경계값 테스트 (최소/최대 ID, 센서 번호)
6. **06_edge_case_test**: 엣지 케이스 (중복 입력, ID 중복, 범위 초과)
7. **07_dynamic_sensor_test**: 동적 센서 감지 및 안전한 테스트

### 테스트 실행
```bash
# 특정 시나리오 실행
python tools/realTimeTest/pyTestStart.py 01_sensor_individual_id_change_flow.json

# 모든 시나리오 순차 실행
for i in {01..07}; do
    python tools/realTimeTest/pyTestStart.py ${i}_*.json
done
```

### 테스트 커버리지
- **기본 기능**: 개별/복수 센서 ID 변경, 자동 할당, 초기화
- **에러 처리**: 잘못된 입력, 중복 ID, 연결되지 않은 센서
- **경계값**: ID 1~8, 센서 번호 1~8의 최소/최대값
- **사용자 인터페이스**: 메뉴 네비게이션, 취소 기능, 상태 전환
- **엣지 케이스**: 중복 번호 입력, 범위 초과, 특수 문자 입력

## 문제 해결 문서화

개발 과정에서 발생한 모든 문제와 해결 과정을 상세히 문서화했습니다:

### 주요 문제 해결 기록
- **[howFixExtreamError.md](tools/realTimeTest/howFixExtreamError.md)**: 극한 상황 에러 해결
  - 보드 상태 초기화 문제
  - 시리얼 버퍼 관리
  - 상태 전환 로직 버그
- **[howFix05-06-07.md](tools/realTimeTest/howFix05-06-07.md)**: 05-07번 시나리오 문제 해결
  - 개별/복수 모드 구분 문제
  - 하드웨어 환경 의존성 문제
  - 입력 처리 로직 문제

### 교훈 및 개선사항
- 펌웨어 동작 파악의 중요성
- 실제 하드웨어 환경 고려
- 상태 전환 로직의 복잡성 관리
- 반복적 개선을 통한 품질 향상

## 참고 문서

### 개발 문서
- [프로젝트 계획서](docs/Plan/frimePlan.md)
- [환경 설정 가이드](docs/Plan/01_environment_setup_guide.md)
- [핵심 기능 설계](docs/Plan/02_core_feature_design.md)

### 테스트 문서
- [실시간 테스트 가이드](tools/realTimeTest/README.md)
- [메뉴 플로우 다이어그램](tools/realTimeTest/menuFlow.md)
- [pytest 핵심 포인트](tools/realTimeTest/keyPointPyTest.md)

### 문제 해결 기록
- [극한 에러 해결 기록](tools/realTimeTest/howFixExtreamError.md)
- [05-07번 시나리오 문제 해결](tools/realTimeTest/howFix05-06-07.md)
- [03번 시나리오 수정 기록](tools/realTimeTest/howToFix_03-secenario.md)



---
