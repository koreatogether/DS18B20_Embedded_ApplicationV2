
### 센서별 사용자 데이터(ID) 저장/조회/중복 체크 ✅ 완료

- **설계 의도:**
	- 각 DS18B20 센서에 대해 사용자가 임의의 식별자(ID)를 부여, 저장, 조회, 중복 체크할 수 있도록 한다.
	- 센서 주소와 사용자 ID의 매핑을 통해 센서 관리의 편의성과 확장성을 높인다.

- **구조 및 책임:**
	- setSensorId(index, id): 지정한 센서에 사용자 ID를 저장
	- getSensorId(index): 지정한 센서의 사용자 ID를 반환
	- isIdDuplicated(id): ID 중복 여부를 검사
	- 내부적으로 sensorIds_ 벡터(또는 map)로 주소-사용자ID 매핑 관리

- **주요 흐름:**
	1. setSensorId() 호출 시 해당 인덱스의 ID를 갱신, 중복 검사
	2. getSensorId()로 센서별 ID 조회
	3. isIdDuplicated()로 전체 ID 중복 여부 확인

- **예외 처리:**
	- 인덱스 범위 오류, 빈 문자열, 중복 ID 등은 false 반환 또는 예외 처리
	- ID가 없는 경우 빈 문자열 반환

- **구현 완료:**
	- ✅ TemperatureSensorManager 클래스에 ID 관리 메서드 구현
	- ✅ Unity 테스트 프레임워크로 정상/예외 케이스 검증
	- ✅ 테스트용 메서드(_test_setSensorIds) 추가로 유닛 테스트 지원
	- ✅ 설계 문서 및 계획서 동기화 완료

#
### 에러 처리(-127.0 등) 및 예외 상황 관리 ✅ 완료

- **설계 의도:**
	- DS18B20 센서의 온도 측정 시 발생할 수 있는 에러값(-127.0 등) 및 예외 상황을 일관되게 관리한다.
	- 센서 연결 상태, 온도값 유효성, 에러 코드/메시지 제공 등 robust한 도메인 계층을 구현한다.

- **구조 및 책임:**
	- SensorErrorCode enum: 에러 코드 정의 (NO_ERROR, SENSOR_DISCONNECTED, INVALID_TEMPERATURE 등)
	- isTemperatureValid(float temp): 온도값 유효성 검사
	- getLastErrorCode(), getErrorMessage(): 최근 에러 코드/메시지 반환
	- isSensorConnected(index): 센서 연결 상태 확인
	- clearLastError(), setLastError(): 에러 상태 관리
	- isValidIndex(index): 인덱스 범위 체크

- **주요 흐름:**
	1. readTemperature() 등에서 온도값이 에러값(-127.0 등)이면 에러 코드/메시지 설정
	2. isTemperatureValid()로 온도값의 정상 여부 판단
	3. getLastErrorCode()/getErrorMessage()로 에러 상태 조회
	4. isSensorConnected()로 센서 연결 상태 확인

- **예외 처리:**
	- 인덱스 범위 오류, 센서 미연결, 에러값(-127.0 등)은 적절한 에러 코드/메시지로 반환
	- 에러 발생 시 내부 상태에 기록, 필요시 clearLastError()로 초기화

- **구현 완료:**
	- ✅ TemperatureSensorManager에 에러 처리/예외 관리 메서드 및 enum 구현
	- ✅ 테스트 케이스(test_temperature_sensor.cpp) 및 문서 동기화 완료
	- ✅ 설계 문서 및 계획서 동기화 완료

# DS18B20 Domain Layer Core Feature Progress (계획-설계-구현-테스트-문서화-자동화 동기화)

---
## 장시간 동작 안정성(롱런) 테스트 전략 (Application/Test/QA)

- **목적:** 임베디드 시스템이 7일 이상 연속 동작 시 메모리 누수, 에러, 품질 저하 없이 안정적으로 동작하는지 검증
- **Mock 기반:** MockLongRunStabilityManager 헤더온리 mock 객체로 실제 7일 시나리오를 5분 내 반복 루프(300회)로 가속화하여 자동 검증
- **테스트 자동화:** 모든 시나리오(정상, 에러, 누수, 요약 등)는 header-only, RTTI-free, mock 기반으로 통합 테스트 파일에서 자동화
- **가상 시간 스케일:** 실제 1초를 수천 초로 가정하여 반복 횟수만큼 빠르게 품질 시나리오를 시뮬레이션
- **장점:** 실제 장시간 동작 환경을 수분 내에 반복 검증, CI/CD 및 로컬 PC에서 품질 회귀 테스트에 즉시 활용 가능

이 문서의 각 단계는 아래와 같은 일관된 워크플로우를 따릅니다:

- **설계/구현/테스트/문서화/자동화/체크리스트 동기화**
	1. 설계 및 책임 정의 → 코드/테스트/문서 파일 생성 및 수정
	2. PlatformIO 빌드/테스트로 기능 검증
	3. 관련 설계 문서 및 체크리스트(frimePlan.md) 동기화
	4. auto_commit.sh 등 자동화 스크립트로 스테이징(커밋/푸시는 수동)
	5. 각 단계별 완료 시, 문서 상단에 ✅ 및 요약 추가, 계획과 문서의 일치 보장
---

### ITemperatureSensor 인터페이스 정의  ✅ 완료

### TemperatureSensorManager 클래스 설계  ✅ 완료
> [v] TemperatureSensorManager 클래스 설계, 책임 정의, 주요 메서드 시그니처, 설계 고려사항 등 완료.

### 센서 자동 탐지 및 주소 관리 로직  ✅ 완료
> [v] 센서 자동 탐지 및 주소 관리 로직 설계, 구조, 책임, 주요 흐름, 예외 처리 방안까지 완료.

### 센서별 온도 측정/수집/갱신 메서드  ✅ 완료
> [v] 센서별 온도 측정/수집/갱신 메서드 설계, 구조, 책임, 주요 흐름, 예외 처리 방안까지 완료.

---
### 센서별 온도 측정/수집/갱신 메서드

- **설계 의도:**
	- 각 DS18B20 센서의 온도를 개별적으로 측정하고, 필요 시 전체 센서의 온도값을 일괄 수집/갱신할 수 있도록 한다.
	- 온도값은 실시간 측정 또는 주기적 갱신 방식 모두 지원 가능하도록 설계한다.

- **구조 및 책임:**
	- readTemperature(index): 지정한 센서의 현재 온도를 반환
	- updateAllTemperatures(): 모든 센서의 온도를 한 번에 측정하여 내부 캐시에 저장(옵션)
	- ITemperatureSensor 인터페이스의 read() 또는 readAll()을 활용하여 하드웨어 독립적 구현

- **주요 흐름:**
	1. readTemperature(index) 호출 시 해당 센서 주소로 ITemperatureSensor::read(address) 실행
	2. updateAllTemperatures()는 전체 주소 목록을 순회하며 온도값을 일괄 측정/저장
	3. getCachedTemperature(index)는 내부 벡터에서 최근 측정값 반환(옵션)

- **예외 처리:**
	- 인덱스 범위 오류 등은 false/NaN/특정 값 반환

---
### 센서 자동 탐지 및 주소 관리 로직
- **설계 의도:**
	- 시스템이 부팅되거나 사용자가 요청할 때, 1-Wire 버스에 연결된 모든 DS18B20 센서를 자동으로 탐지하여 고유 8바이트 주소를 수집/저장합니다.
	- 센서 주소 목록은 TemperatureSensorManager가 관리하며, 센서별 온도 측정/ID 매핑의 기준이 됩니다.

- **구조 및 책임:**
	- discoverSensors() 메서드는 ITemperatureSensor 인터페이스(실제 하드웨어 드라이버)를 통해 센서 주소를 반복적으로 검색합니다.
	- 중복 없이 주소 벡터(sensorAddresses_)에 저장하고, 센서 개수에 맞춰 sensorIds_ 벡터도 동기화합니다.
	- 센서가 추가/제거될 경우에도 일관된 주소 관리가 가능하도록 설계합니다.
- **주요 흐름:**
	1. discoverSensors() 호출 시 기존 주소/ID 목록 초기화
	2. ITemperatureSensor::search() 등 하드웨어 API로 1-Wire 버스의 모든 센서 주소 탐색
	3. 각 주소를 sensorAddresses_에 저장, sensorIds_는 빈 문자열로 초기화

- **예외 처리:**
	- 센서가 하나도 없을 경우, getSensorCount()는 0을 반환
	- 주소 중복, 버스 에러 등은 추후 예외 정책에 따라 처리 예정

---
### TemperatureSensorManager 클래스 설계

	- 여러 DS18B20 센서를 자동 탐지하고, 각 센서의 주소를 관리하며, 온도 측정/수집/갱신 기능을 제공하는 도메인 계층의 핵심 매니저 클래스입니다.
	- 각 센서별로 사용자 정의 ID를 저장/조회/중복 체크하며, 센서 에러(-127.0 등) 및 예외 상황을 일관되게 처리합니다.

- **주요 책임:**
	- 센서 자동 탐지 및 주소 목록 관리
	- 센서별 온도 측정 및 데이터 갱신
	- 센서별 사용자 데이터(ID) 저장/조회/중복 체크
	- 에러 및 예외 상황 관리
- **주요 메서드 시그니처(예시):**
	```cpp
	class TemperatureSensorManager {
	public:
			TemperatureSensorManager();
			~TemperatureSensorManager();
			void discoverSensors(); // 센서 자동 탐지
			size_t getSensorCount() const;
			bool getSensorAddress(size_t index, uint8_t* address) const;
			float readTemperature(size_t index);
			std::string getSensorId(size_t index) const;
			bool isIdDuplicated(const std::string& id) const;
			// ... 기타 예외/에러 처리 메서드
	```

- **설계 시 고려사항:**
	- 센서 주소와 사용자 ID의 매핑 구조(예: std::map 등) 활용
	- 예외 상황(센서 미연결, 에러값 등) 처리 정책 명확화







---
# 02. 핵심 기능 설계 및 구현

이 문서는 DS18B20 Embedded Application의 핵심 기능(센서 관리, 메모리 분석, 통신, 의존성 주입 등) 설계 및 구현 과정을 기록합니다.

## 목차
1. DS18B20 센서 관리 (Domain Layer)
2. 메모리 분석 시스템 (Application Layer)
3. 통신 및 명령어 처리 (Infrastructure Layer)
4. 의존성 주입 및 통합 (Main Layer)
5. 설계 의사결정/이슈/토의

---

## 1. DS18B20 센서 관리 (Domain Layer)

### ITemperatureSensor 인터페이스 정의

 #### 개발 및 자동화 과정 기록

 - **파일 생성 위치:**
	 - `src/domain/ITemperatureSensor.h` 파일을 생성하여 DS18B20 센서 추상화를 위한 인터페이스를 정의함.

 - **빌드 및 테스트:**
	 - PlatformIO 환경에서 `C:\Users\h\.platformio\penv\Scripts\platformio.exe run -e uno_r4_wifi` 명령으로 빌드.
	 - `C:\Users\h\.platformio\penv\Scripts\platformio.exe test -e native` 명령으로 유닛테스트를 수행하여 인터페이스 정의의 적합성 및 프로젝트 정상 동작을 검증함.

 - **자동화 스크립트 도입 배경:**
	 - 반복적인 빌드, 테스트, git add/commit/push 과정을 자동화하고, 각 단계별 에러 발생 시 즉시 중단 및 명확한 메시지 출력을 위해 `tools/auto_commit.sh` 스크립트를 도입함.
	 - 해당 스크립트는 빌드, 테스트, git add/commit/push를 일괄 처리하며, 변경사항이 없을 경우 커밋을 생략하고, 에러 발생 시 원인별로 종료 코드를 반환함.
	 - Windows 환경에서는 Git Bash 또는 WSL에서 `bash tools/auto_commit.sh`로 실행 가능하며, 프로젝트의 일관된 품질 관리와 개발 효율성 향상에 기여함.

 ---
 

## 2. 메모리 분석 시스템 (Application Layer)

### IMemoryAnalyzer 인터페이스 정의 ✅ 완료

- **설계 의도:**
	- 메모리 분석 시스템의 핵심 기능을 추상화하여 하드웨어 독립적인 메모리 모니터링 및 분석을 제공합니다.
	- 런타임 분석, 구조 분석, 주기적 모니터링, 추적 기능 등을 통합적으로 관리합니다.

- **주요 메서드:**
	- `periodicCheck()`: 주기적 메모리 상태 확인
	- `getRuntimeAnalysis()`: 런타임 메모리 분석 수행
	- `getStructureAnalysis()`: 메모리 구조 분석
	- `getFreeMemory()`: 현재 가용 메모리 조회
	- `toggleMonitoring()`: 모니터링 토글
	- `toggleTracking()`: 메모리 추적 토글
	- `getMemoryReport()`: 메모리 추적 리포트 생성

### IMemoryTracker 인터페이스 정의 ✅ 완료

- **설계 의도:**
	- 메모리 사용량 변화 이력을 체계적으로 추적하고 분석하는 기능을 제공합니다.
	- 메모리 스냅샷 저장, 트렌드 분석, 리포트 생성 등의 고급 기능을 지원합니다.

- **핵심 구조체:**
	```cpp
	struct MemorySnapshot {
		unsigned long timestamp;    // 측정 시간
		int freeBytes;             // 자유 메모리 바이트
		int usedBytes;             // 사용된 메모리 바이트
		std::string eventType;     // 이벤트 타입
		std::string description;   // 추가 설명
	};

	struct MemoryReport {
		int totalSnapshots;        // 총 스냅샷 수
		int minFreeBytes;          // 최소 자유 메모리
		int maxFreeBytes;          // 최대 자유 메모리
		int avgFreeBytes;          // 평균 자유 메모리
		int memoryTrend;           // 메모리 트렌드
		unsigned long duration;    // 추적 기간
		std::string summary;       // 요약 텍스트
	};
	```

- **주요 메서드:**
	- `recordSnapshot()`: 메모리 스냅샷 저장
	- `recordCurrentMemory()`: 현재 메모리 자동 측정 및 저장
	- `generateReport()`: 분석 리포트 생성
	- `exportToCsv()`: CSV 형식 내보내기
	- `exportReportToMarkdown()`: Markdown 리포트 생성

### MemoryMonitorService 클래스 구현 ✅ 완료

- **설계 의도:**
	- IMemoryAnalyzer 인터페이스를 구현하여 실제 메모리 모니터링 및 분석 기능을 제공합니다.
	- HAL 추상화를 통해 하드웨어 독립적 구현을 달성합니다.
	- 메모리 추적 기능을 통합하여 포괄적인 메모리 관리 솔루션을 제공합니다.

- **핵심 기능:**
	- 주기적 메모리 모니터링 (기본 10초 간격)
	- 런타임 메모리 스트레스 테스트
	- 메모리 구조 분석
	- CSV/Markdown 형식 로그 출력
	- 메모리 변화 추적 및 리포트 생성

### MemoryTrackerService 클래스 구현 ✅ 완료

- **설계 의도:**
	- IMemoryTracker 인터페이스의 완전한 구현체로서 메모리 이력 관리의 모든 기능을 제공합니다.
	- 원형 버퍼 구조를 사용하여 메모리 효율적인 히스토리 관리를 구현합니다.
	- 실시간 메모리 측정 및 트렌드 분석 기능을 포함합니다.

- **핵심 기능:**
	- 메모리 스냅샷 자동 수집 및 저장
	- 메모리 사용량 트렌드 분석 (-1: 감소, 0: 안정, 1: 증가)
	- 통계 분석 (최소/최대/평균 메모리 사용량)
	- CSV/Markdown 형식 데이터 내보내기
	- 히스토리 크기 제한 및 관리

### 통합 메모리 시스템 아키텍처 ✅ 완료

- **의존성 주입 구조:**
	```
	MemoryMonitorService
	├── IHal (하드웨어 추상화)
	└── IMemoryTracker (메모리 추적)
		└── MemoryTrackerService
			└── IHal (하드웨어 추상화)
	```

- **시리얼 명령어 통합:**
	- `memtrack`: 메모리 추적 기능 토글
	- `memreport`: 메모리 추적 리포트 생성
	- 기존 명령어와의 완전한 호환성 유지

### 로그 시스템 구조화 ✅ 완료

- **디렉토리 구조:**
	- `logs/memory/`: 메모리 관련 로그 파일
	- `logs/comfile/`: 컴파일 결과 로그 파일
	- `test/logs/`: 테스트 결과 로그 파일

- **로그 저장 명령어:**
	- 컴파일: `pio run -e uno_r4_wifi -v 2>&1 | tee logs/comfile/compile_results.txt`
	- 테스트: `pio test -e native -v 2>&1 | tee test/logs/test_results_clean.txt`

---

## 3. 통신 및 명령어 처리 (Infrastructure Layer)

### ICommandProcessor 인터페이스 정의 ✅ 완료

- **설계 의도:**
	- 명령어 처리 시스템의 핵심 인터페이스로서 확장 가능한 명령어 처리 구조를 제공합니다.
	- 시리얼 통신뿐만 아니라 다양한 통신 방식에 대한 추상화를 지원합니다.

### SerialCommandHandler 클래스 구현 ✅ 완료

- **설계 의도:**
	- ICommandProcessor 인터페이스를 구현하여 시리얼 통신을 통한 명령어 처리를 담당합니다.
	- 메모리 분석 시스템과의 완전한 통합을 제공하며, RTTI 없는 환경에서도 안전하게 동작합니다.

- **지원 명령어:**
	- `memtest`: 런타임 메모리 분석 수행
	- `memstress`: 메모리 스트레스 테스트 (memtest와 동일)
	- `memstruct`: 메모리 구조 분석
	- `memfree`: 현재 가용 메모리 조회
	- `memtoggle`: 주기적 메모리 모니터링 토글
	- `memtrack`: 메모리 변화 추적 토글 (신규)
	- `memreport`: 메모리 추적 리포트 생성 (신규)
	- `help`/`menu`: 사용 가능한 명령어 목록 표시

- **RTTI 대응 설계:**
	- Arduino 환경의 `-fno-rtti` 플래그로 인한 `dynamic_pointer_cast` 사용 불가 문제 해결
	- IMemoryAnalyzer 인터페이스에 `toggleTracking()`, `getMemoryReport()` 메서드 추가
	- 타입 캐스팅 없이 인터페이스를 통한 안전한 메서드 호출 구현

### 명령어 파싱 및 응답 처리 ✅ 완료

- **명령어 처리 흐름:**
	1. 시리얼 입력 수신 및 파싱
	2. 명령어별 적절한 메서드 호출
	3. 결과를 CSV/Markdown 형식으로 포맷팅
	4. 시리얼 출력으로 응답 전송

- **에러 처리:**
	- 알 수 없는 명령어에 대한 적절한 에러 메시지 반환
	- 메모리 분석 실패 시 안전한 에러 응답 제공

---

## 4. 의존성 주입 및 통합 (Main Layer)

### HAL(Hardware Abstraction Layer) 구현 ✅ 완료

- **설계 의도:**
	- 하드웨어 의존성을 완전히 분리하여 테스트 가능한 아키텍처를 구현합니다.
	- Arduino 하드웨어와 Mock 테스트 환경 모두를 지원하는 추상화 계층을 제공합니다.

- **IHal 인터페이스:**
	```cpp
	class IHal {
	public:
		virtual unsigned long millis() = 0;
		virtual int getFreeMemoryBytes() = 0;
		virtual std::string getMemoryStructureInfo() = 0;
		virtual void print(const std::string& message) = 0;
	};
	```

### 통합 의존성 주입 구조 ✅ 완료

- **의존성 주입 아키텍처:**
	```
	main.cpp
	├── ArduinoHal (실제 하드웨어)
	├── MemoryTrackerService(hal)
	├── MemoryMonitorService(hal, tracker)
	└── SerialCommandHandler(memoryAnalyzer)
	```

- **시스템 초기화 순서:**
	1. HAL 객체 생성 (ArduinoHal)
	2. MemoryTrackerService 생성 및 HAL 주입
	3. MemoryMonitorService 생성 및 HAL, Tracker 주입
	4. SerialCommandHandler 생성 및 MemoryAnalyzer 주입
	5. 시스템 동작 시작

### 컴파일 및 테스트 검증 ✅ 완료

- **Arduino UNO R4 WiFi 컴파일 성공:**
	- RAM 사용량: 22.6% (7,420 bytes / 32,768 bytes)
	- Flash 사용량: 70.1% (183,848 bytes / 262,144 bytes)
	- 모든 의존성 주입이 정상적으로 동작함

- **Header-Only Mock 테스트:**
	- 16개 테스트 케이스 모두 통과
	- MockMemoryTracker, MockMemoryMonitorService 완전 구현
	- 실제 구현과 Mock 간 100% 호환성 달성

---

## 5. 설계 의사결정/이슈/토의

### 메모리 추적 시스템 설계 결정 사항

#### 1. RTTI 없는 환경에서의 타입 캐스팅 문제 해결
- **문제:** Arduino 환경에서 `-fno-rtti` 플래그로 인해 `dynamic_pointer_cast` 사용 불가
- **해결책:** IMemoryAnalyzer 인터페이스에 메모리 추적 관련 메서드 직접 추가
- **장점:** 타입 캐스팅 없이 안전한 메서드 호출, 인터페이스 일관성 유지
- **결과:** SerialCommandHandler에서 안전하게 메모리 추적 기능 사용 가능

#### 2. Header-Only Mock 패턴 확장
- **설계 의도:** MockMemoryTracker를 Header-Only 방식으로 구현
- **장점:** 
  - 컴파일 시간 단축
  - src 폴더 의존성 완전 분리
  - 테스트 환경의 독립성 보장
- **구현:** IMemoryTracker 인터페이스의 모든 메서드를 Mock으로 완전 구현

#### 3. 메모리 추적 데이터 구조 설계
- **MemorySnapshot vs MemoryReport 분리:**
  - MemorySnapshot: 개별 시점의 메모리 상태 저장
  - MemoryReport: 전체 추적 기간의 분석 결과 제공
- **장점:** 데이터와 분석 결과의 명확한 분리, 확장성 향상

#### 4. 원형 버퍼를 활용한 메모리 효율성
- **설계:** std::deque를 사용한 히스토리 크기 제한
- **장점:** 메모리 사용량 예측 가능, 장기간 동작 시 메모리 오버플로우 방지
- **설정:** 기본 100개 스냅샷 제한, 사용자 설정 가능

#### 5. 로그 시스템 구조화
- **디렉토리 분리:**
  - `logs/memory/`: 메모리 관련 로그
  - `logs/comfile/`: 컴파일 로그
  - `test/logs/`: 테스트 로그
- **장점:** 로그 유형별 체계적 관리, 분석 도구 연동 용이

### 향후 확장 고려사항

#### 1. 실시간 메모리 시각화
- 웹 기반 대시보드를 통한 실시간 메모리 상태 모니터링
- JSON 형식 데이터 내보내기 기능 추가 예정

#### 2. 메모리 누수 탐지
- 메모리 사용량 트렌드 분석을 통한 누수 패턴 자동 탐지
- 임계값 기반 알림 시스템 구현 예정

#### 3. 다중 센서 연동
- DS18B20 센서 데이터와 메모리 사용량 상관관계 분석
- 센서 개수 증가에 따른 메모리 사용량 패턴 추적

---

## 4. 코드 품질 메트릭 모니터링 시스템 ✅ 완료

### 설계 목표 및 구현 상태

#### 자동화된 품질 분석 시스템 구축
- **목표**: 지속적인 코드 품질 모니터링 및 개선 가이드 제공
- **구현 상태**: ✅ 완료 (품질 점수 90.9/100 달성)
- **핵심 기능**: 포괄적 메트릭 수집, 트렌드 분석, 자동화 도구

### 주요 구현 컴포넌트

#### 1. 코드 품질 메트릭 수집기 (`code_metrics.py`)

**기능 개요:**
- 코드 메트릭: 라인 수, 함수/클래스 개수, 복잡도 분석
- 아키텍처 메트릭: Clean Architecture 준수도, 의존성 역전 원칙
- 테스트 메트릭: 커버리지, 성공률, 실행 시간
- 빌드 메트릭: 컴파일 상태, 메모리 사용률, 경고/오류

**핵심 클래스 구조:**
```python
class CodeMetricsCollector:
    def collect_code_metrics() -> Dict[str, Any]     # 코드 복잡도 분석
    def collect_architecture_metrics() -> Dict[str, Any]  # 아키텍처 준수도
    def collect_test_metrics() -> Dict[str, Any]     # 테스트 품질
    def collect_build_metrics() -> Dict[str, Any]    # 빌드 상태
    def calculate_quality_score() -> float          # 종합 품질 점수
```

**메트릭 가중치 시스템:**
- 아키텍처 메트릭: 30% (계층 분리, 의존성 역전, 인터페이스 사용)
- 테스트 메트릭: 25% (성공률, 커버리지)
- 빌드 메트릭: 20% (컴파일 성공, 메모리 효율성)
- 코드 메트릭: 15% (복잡도, 파일 크기)
- 문서화: 10% (문서 파일 수)

#### 2. 트렌드 분석 시스템 (`trend_analyzer.py`)

**기능 개요:**
- 과거 메트릭 데이터 로드 및 비교 분석
- 품질 점수 변화 추적 (+30.4점 향상 달성)
- 자동 권장사항 생성

**주요 분석 항목:**
```python
class QualityTrendAnalyzer:
    def load_historical_metrics() -> List[Dict]     # 과거 데이터 로드
    def analyze_trends() -> Dict[str, Any]          # 트렌드 분석
    def generate_trend_report() -> str              # 리포트 생성
```

#### 3. 자동화 스크립트

**Windows PowerShell (`monitor_quality.ps1`):**
```powershell
# 전체 품질 모니터링 파이프라인
param([switch]$SkipBuild, [switch]$SkipTest, [switch]$Verbose)

# 1. 빌드 및 테스트 실행
# 2. 품질 메트릭 수집
# 3. 트렌드 분석
# 4. 결과 요약 출력
```

**Linux/macOS Bash (`monitor_quality.sh`):**
- 동일한 기능의 Bash 스크립트 버전
- Cross-platform 지원

### 현재 품질 점수 상세 분석

#### 📊 전체 품질 점수: 90.9/100 (탁월한 품질)

**구성 요소별 점수:**
1. **아키텍처 메트릭**: 85.0/100
   - 계층 분리: 90.0/100 (Domain: 2, Application: 5, Infrastructure: 2)
   - 의존성 역전: 80.0/100 (인터페이스: 5, 구현체: 4)
   - 인터페이스 사용: 90.0/100 (Mock 파일: 6개)

2. **테스트 메트릭**: 100.0/100
   - 테스트 성공률: 100% (16/16 통과)
   - 실행 시간: 0.86초
   - 커버리지 추정: 40%

3. **빌드 메트릭**: 95.0/100
   - 컴파일: ✅ 성공
   - RAM 사용률: 22.6% (효율적)
   - Flash 사용률: 70.1% (적절)
   - 경고/오류: 0개

4. **코드 메트릭**: 100.0/100
   - 평균 복잡도: 0.6 (매우 낮음)
   - 총 라인 수: 1,222
   - 함수: 72개, 클래스: 10개

### CI/CD 통합

#### GitHub Actions 워크플로우 확장
```yaml
- name: Run code quality metrics analysis
  run: python tools/quality_metrics/code_metrics.py

- name: Upload quality metrics
  uses: actions/upload-artifact@v4
  with:
    name: quality-metrics
    path: |
      logs/quality/*.json
      logs/quality/*.md
```

#### 자동 품질 게이트
- 품질 점수 85점 이상 유지 목표
- 테스트 성공률 95% 이상 필수
- 메모리 사용률 모니터링 (RAM < 80%, Flash < 85%)

### 생성되는 보고서

#### 1. 품질 메트릭 리포트 (`quality_report_*.md`)
- 전체 품질 점수 및 구성 요소별 상세 분석
- 코드 구조 분석 (파일 수, 라인 수, 복잡도)
- 아키텍처 준수도 평가
- 테스트 및 빌드 상태 요약

#### 2. 트렌드 분석 리포트 (`trend_report_*.md`)
- 품질 점수 변화 추적
- 메트릭별 상세 변화 분석
- 자동 생성 권장사항

#### 3. JSON 데이터 (`metrics_*.json`, `trend_analysis_*.json`)
- 머신리더블 형태의 상세 메트릭 데이터
- 외부 도구 연동 및 추가 분석 지원

### 품질 개선 가이드라인

#### 개발 워크플로우 권장사항
1. **코드 변경 후**: `python tools/quality_metrics/code_metrics.py` 실행
2. **일일 체크**: PowerShell 자동화 스크립트 활용
3. **주간 리뷰**: 트렌드 분석 보고서 검토
4. **릴리스 전**: 전체 품질 점수 85점 이상 확인

#### 품질 점수 기준
- **90-100점**: 🎉 탁월한 품질 (현재 상태)
- **80-89점**: ✅ 좋은 품질
- **70-79점**: ⚠️ 수용 가능한 품질
- **0-69점**: 🚨 개선 필요

### 향후 확장 계획

#### 1. 정적 분석 도구 확장
- cppcheck 완전 통합
- clang-tidy 추가 규칙 적용
- 메모리 안전성 분석 강화

#### 2. 커버리지 분석 고도화
- gcov/lcov 통합
- 실제 코드 커버리지 측정
- 브랜치 커버리지 분석

#### 3. 성능 메트릭 추가
- 실행 시간 프로파일링
- 메모리 할당 패턴 분석
- 전력 소비 모니터링

---

### 주요 성과 요약

✅ **완료된 핵심 기능:**
- 메모리 변화 추적 및 이력 관리
- 실시간 메모리 분석 리포트 생성
- CSV/Markdown 형식 데이터 내보내기
- Arduino UNO R4 WiFi 환경 컴파일 성공
- 16개 테스트 케이스 100% 통과
- **코드 품질 메트릭 모니터링 시스템 (90.9/100 점수 달성)**

✅ **아키텍처 개선:**
- RTTI 없는 환경에서 안전한 인터페이스 설계
- Header-Only Mock 패턴으로 완전한 의존성 분리
- HAL 추상화를 통한 하드웨어 독립적 구현
- **Clean Architecture 준수도 85점 달성**

✅ **개발 효율성:**
- 자동화된 빌드/테스트/로그 시스템
- 체계적인 문서화 및 버전 관리
- CI/CD 파이프라인 통합
- **포괄적 품질 모니터링 및 트렌드 분석 도구**

✅ **품질 보증:**
- 지속적 품질 모니터링 체계 구축
- 자동화된 메트릭 수집 및 분석
- 트렌드 기반 개선 권장사항 생성
- **Cross-platform 자동화 스크립트 지원**

---

> 각 항목별로 설계/구현/예제/이슈/토의 내용을 자유롭게 추가·수정해 주세요.
