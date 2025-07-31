# DS18B20 Embedded Application 아키텍처 설계

## 계층 구조
- **Domain Layer**: 센서 관리, 비즈니스 로직 (`src/domain/`)
- **Application Layer**: 메모리 분석, 서비스 로직 (`src/application/`)
- **Infrastructure Layer**: 하드웨어/통신 인터페이스 (`src/infrastructure/`)
- **HAL Layer**: 하드웨어 추상화 (`src/hal/`)
- **Main Layer**: 의존성 주입 및 시스템 통합 (`src/main.cpp`)

## 주요 설계 원칙
- Clean Architecture, SOLID, DIP 적용
- Mock 기반 테스트 환경(헤더 온리)
- 계층 간 의존성 주입

## 시스템 구성도
```
[센서] <-> [Domain] <-> [Application] <-> [Infrastructure] <-> [HAL] <-> [하드웨어]
```

## 데이터 흐름
1. 센서 데이터 수집 → 도메인 로직 처리
2. 메모리 상태 분석/로깅 → 서비스/리포트 생성
3. 명령어 입력 → 파싱/실행 → 결과 응답

---
