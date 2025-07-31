# DS18B20 Embedded Application 명령어 참조

이 문서는 시스템에서 지원하는 모든 명령어와 사용법을 정리한 참조 문서입니다.

## 명령어 목록

| 명령어     | 설명                          | 사용 예시         |
| ---------- | ----------------------------- | ----------------- |
| help/menu  | 명령어 목록 및 간단 설명 출력 | help              |
| memtest    | 메모리 테스트 실행            | memtest           |
| memstress  | 스트레스 테스트 실행          | memstress         |
| memstruct  | 메모리 구조 정보 출력         | memstruct         |
| memfree    | 사용 가능한 메모리 확인       | memfree           |
| memtoggle  | 메모리 추적 토글              | memtoggle         |
| memtrack   | 메모리 추적 시작/중지         | memtrack          |
| memreport  | 메모리 리포트 출력            | memreport         |
| sensorlist | 연결된 센서 목록 출력         | sensorlist        |
| tempget    | 센서별 온도 측정값 출력       | tempget           |
| idset      | 센서별 사용자 ID 등록         | idset 0x28xxxx... |
| idget      | 센서별 사용자 ID 조회         | idget 0x28xxxx... |

## 사용 예시
- `memtest` → 메모리 테스트 실행
- `memtrack` → 메모리 추적 시작, `memreport`로 결과 확인
- `idset 0x28xxxx...` → 특정 센서에 사용자 ID 등록

---
자세한 사용법은 [user_guide.md](user_guide.md) 참고
