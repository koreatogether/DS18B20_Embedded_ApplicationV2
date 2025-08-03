# DS18B20 Embedded Temperature Monitoring System

Arduino Uno R4 WiFi 기반 다중 DS18B20 온도 센서 모니터링 및 제어 시스템

## 🎯 주요 기능
- **다중 센서 관리**: 최대 8개 DS18B20 센서 동시 모니터링
- **동적 임계값 설정**: 센서별 개별 상/하한 온도 임계값 설정
- **측정 주기 조정**: 10초~30일 범위에서 1초 단위 설정 가능
- **EEPROM 영구 저장**: 모든 설정값 전원 차단 후에도 유지
- **실시간 상태 모니터링**: 센서별 온도, 임계값 초과 상태 실시간 표시
- **직관적 메뉴 시스템**: 시리얼 인터페이스 기반 사용자 친화적 제어

## 🏗️ 프로젝트 구조
```
├── src/                                    # 소스 코드
│   ├── DS18B20_Embedded_ApplicationV2.ino  # 메인 Arduino 파일
│   ├── application/                        # 애플리케이션 계층
│   │   ├── MenuController.cpp/.h           # 메뉴 제어
│   │   ├── SensorController.cpp/.h         # 센서 관리
│   │   └── ...                             # 기타 서비스들
│   ├── domain/                             # 도메인 계층
│   │   └── TemperatureSensorManager.cpp/.h # 센서 관리 도메인
│   └── infrastructure/                     # 인프라 계층
│       ├── DS18B20Sensor.cpp/.h            # 센서 하드웨어 인터페이스
│       ├── ArduinoMemoryManager.cpp/.h     # 메모리 관리
│       └── SerialLogger.cpp/.h             # 로깅
├── docs/                                   # 문서
├── platformio.ini                          # PlatformIO 설정
└── README.md                               # 프로젝트 개요
```

## 🚀 빠른 시작

### 하드웨어 연결
```
Arduino Uno R4 WiFi
├── Digital Pin 2 ──── DS18B20 Data (with 4.7kΩ pullup to 5V)
├── 5V ──────────────── DS18B20 VDD
└── GND ─────────────── DS18B20 GND
```

### 소프트웨어 설치
```bash
# PlatformIO 사용
pio run --target upload

# Arduino IDE 사용
# 라이브러리 설치: DallasTemperature, OneWire
```

### 기본 사용법
1. 시리얼 모니터 연결 (115200 baud)
2. `menu` 입력으로 설정 메뉴 진입
3. 센서별 임계값 설정
4. 측정 주기 조정
5. 실시간 모니터링 시작

## 📊 시스템 사양
- **지원 센서**: DS18B20 (최대 8개)
- **온도 범위**: -55°C ~ +125°C
- **온도 정확도**: ±0.5°C
- **측정 주기**: 10초 ~ 30일 (1초 단위 설정)
- **메모리 사용량**: Flash 29.7%, RAM 10.4%, EEPROM 1.7%
- **통신**: OneWire 프로토콜
- **인터페이스**: 시리얼 (115200 baud)

## 🛠️ 개발 환경

### 요구사항
- Arduino Uno R4 WiFi
- PlatformIO 또는 Arduino IDE
- DS18B20 온도 센서
- 4.7kΩ 풀업 저항

### 라이브러리 의존성
```ini
lib_deps = 
    DallasTemperature@^4.0.4
    OneWire@^2.3.8
```

### 빌드 및 업로드
```bash
# PlatformIO
pio run --target upload

```

## 🎮 사용 예시

### 기본 모니터링
```
DS18B20 온도 센서 시스템 시작...
센서 검색 중: 3개 센서 발견
현재 측정 주기: 15초

| 번호 | ID  | 현재 온도 | 상한임계값 | 하한임계값 | 센서상태 |
| ---- | --- | --------- | ---------- | ---------- | -------- |
| 1    | 1   | 25.3°C    | 30.0°C     | 20.0°C     | 정상     |
| 2    | 2   | 23.8°C    | 30.0°C     | 20.0°C     | 정상     |
| 3    | 3   | 28.1°C    | 30.0°C     | 20.0°C     | 정상     |
```

### 설정 변경
```
> menu
===== 센서 제어 메뉴 =====
1. 센서 ID 조정
2. 상/하한 온도 조정  
3. 센서 측정 주기 조정
4. 취소 / 상태창으로 돌아가기

> 2
센서 1 임계값 설정
상한 온도 (현재: 30.0°C): 35
하한 온도 (현재: 20.0°C): 15
✅ 센서 1 임계값 설정 완료: TH=35.0°C, TL=15.0°C
💾 EEPROM 저장 완료
```

## 🔧 주요 기능

### 센서 관리
- 자동 센서 검색 및 등록
- 센서별 논리 ID 할당
- 실시간 연결 상태 모니터링

### 임계값 시스템
- 센서별 개별 상/하한 임계값 설정
- 복수 센서 일괄 설정
- 실시간 임계값 초과 감지

### 데이터 저장
- EEPROM 영구 저장
- 중복 쓰기 방지로 수명 보호
- 전원 차단 후에도 설정 유지

---

## 📄 라이선스
이 프로젝트는 MIT 라이선스 하에 배포됩니다.

## 🤝 기여하기
1. 이 저장소를 포크합니다
2. 기능 브랜치를 생성합니다 (`git checkout -b feature/AmazingFeature`)
3. 변경사항을 커밋합니다 (`git commit -m 'Add some AmazingFeature'`)
4. 브랜치에 푸시합니다 (`git push origin feature/AmazingFeature`)
5. Pull Request를 생성합니다

## 📞 지원
문제가 발생하거나 질문이 있으시면 [Issues](../../issues)를 통해 문의해 주세요.