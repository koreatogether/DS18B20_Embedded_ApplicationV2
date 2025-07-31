생성일자 : 25.07.30

## Codacy Issue Summary

---

## 주요 Critical(치명적) 위험 항목 목록

 **strncpy 사용 취약점 (CWE-120, Insecure Modules Libraries) [해결됨]**
	 - null 종료 미보장, 잘못된 포인터 체크 미흡, MS-banned 함수
	 - 해결 방법: 모든 strncpy 사용 코드를 직접 반복문 복사 방식으로 대체하여 null 종료와 오버플로우를 명확히 보장. Codacy 및 모든 정적 분석기 경고 제거.
	 - 위치:
		  - `test/mocks/MockMemoryLeakDetector.h:33, 36, 386` (모두 safeCopyString 함수로 대체)

 **strcpy/strncpy 사용 탐지 (Insecure Modules Libraries) [해결됨]**
	 - strcpy, strncpy 사용 시 무조건 경고 발생
	 - 해결 방법: 모든 strncpy 사용 코드를 반복문 복사 방식으로 대체하여 경고 발생 원천 차단.
	 - 위치: `test/mocks/MockMemoryLeakDetector.h:386` (safeCopyString 함수로 대체)

 **Input Validation 취약점 (CWE-126) [해결됨]**
	 - null 종료되지 않은 문자열 처리 미흡, 오버리드 위험
	 - 해결 방법: 테스트 코드에서 strlen 사용 전 summary 버퍼의 마지막 바이트가 항상 '\0'임을 assert로 명확히 체크하여 null 종료 보장. 정적 분석기 오탐 방지.
	 - 위치: `test/integration/test_memory_leak_detection.cpp:41` (assert 추가)

**Input Validation 취약점 (CWE-126)** [현재로썬 조치 필요 없음 ]
    - null 종료되지 않은 문자열에 strlen 사용 시 오버리드 및 크래시 위험
    - 예시 코드: `TEST_ASSERT_GREATER_THAN(0, strlen(analysis.summary));`
    - 위치: `test/integration/test_memory_leak_detection.cpp:43`
    - 조치 필요: summary 버퍼의 null-termination 보장 또는 안전한 문자열 처리 필요
---

### HIGH 등급 Codacy 이슈 체크리스트

 [v] 항상 true인 조건문 (src/domain/TemperatureSensorManager.cpp:43)
	- 코드: `if (!isSensorConnected(index))`
 [v] 사용되지 않는 import (F401) (tools/quality_metrics/trend_analyzer.py:12)
	- 코드: `from datetime import datetime, timedelta`
 [v] 사용되지 않는 import (F401) (tools/quality_metrics/trend_analyzer.py:9)
	- 코드: `import os`
 [v] 사용되지 않는 import (F401) (tools/quality_metrics/code_metrics.py:20)
	- 코드: `from typing import Dict, List, Tuple, Any`
 [v] 사용되지 않는 import (F401) (tools/quality_metrics/code_metrics.py:17)
	- 코드: `import subprocess`
 [v] 사용되지 않는 import (F401) (tools/quality_metrics/code_metrics.py:14)
	- 코드: `import os`
 [v] f-string에 placeholder 없음 (F541) (tools/quality_metrics/trend_analyzer.py:148)
	- 코드: `print(f"📊 Trend analysis saved:")`
 [v] f-string에 placeholder 없음 (F541) (tools/quality_metrics/code_metrics.py:371)
	- 코드: `print(f"✅ Compilation: SUCCESS")`
 [v] f-string에 placeholder 없음 (F541) (tools/quality_metrics/code_metrics.py:373)
	- 코드: `print(f"❌ Compilation: FAILED")`
 [v] f-string에 placeholder 없음 (F541) (tools/quality_metrics/code_metrics.py:267)
	- 코드: `print(f"❌ Could not decode test log file")`
 [v] f-string에 placeholder 없음 (F541) (tools/quality_metrics/code_metrics.py:352)
	- 코드: `print(f"❌ Could not decode build log file")`
 [v] try, except, continue 사용 (tools/quality_metrics/code_metrics.py:178)
	- 코드: `except:`
 [v] except에 예외 타입 미지정 (tools/quality_metrics/code_metrics.py:95)
	- 코드: `except:`
- [v] try, except, continue 사용 (tools/quality_metrics/code_metrics.py:176)
    - 코드: `except Exception:`
    - 문제: 예외 발생 시 아무런 처리 없이 무시하고 계속 진행하면, 중요한 오류를 놓칠 수 있음. 최소한 로그 출력 또는 예외 재발생 필요

#### code complexity 

- [1] MockStressTestManager::generateComprehensiveReport - 67 lines (limit 50), cyclomatic complexity 15 (limit 8)
	- 위치: test/mocks/MockStressTestManager.h:440
- [2] calculate_quality_score - cyclomatic complexity 12 (limit 8)
	- 위치: tools/quality_metrics/code_metrics.py:388
- [3] generate_trend_report - cyclomatic complexity 9 (limit 8)
	- 위치: tools/quality_metrics/trend_analyzer.py:96
- [4] SerialCommandHandler::processCommand - cyclomatic complexity 10 (limit 8)
	- 위치: src/infrastructure/SerialCommandHandler.cpp:9
- [5] MockStressTestManager::runCpuStressTest - 56 lines (limit 50), cyclomatic complexity 11 (limit 8)
	- 위치: test/mocks/MockStressTestManager.h:242
- [6] collect_build_metrics - cyclomatic complexity 10 (limit 8), 51 lines (limit 50)
	- 위치: tools/quality_metrics/code_metrics.py:320
- [7] collect_test_metrics - cyclomatic complexity 13 (limit 8), 67 lines (limit 50)
	- 위치: tools/quality_metrics/code_metrics.py:229
- [8] MockStressTestManager::runMemoryStressTest - 59 lines (limit 50), cyclomatic complexity 15 (limit 8)
	- 위치: test/mocks/MockStressTestManager.h:163
- [v][9] generate_report - 61 lines (limit 50)
	- 위치: tools/quality_metrics/code_metrics.py:441
- [10] MockStressTestManager::runIoStressTest - 71 lines (limit 50), cyclomatic complexity 22 (limit 8)
	- 위치: test/mocks/MockStressTestManager.h:317
  --------------------------------
- [11] MockStressTestManager::runSystemStabilityTest - 55 lines (limit 50), cyclomatic complexity 16 (limit 8)
	- 위치: test/mocks/MockStressTestManager.h:521
- [12] analyze_trends - cyclomatic complexity 13 (limit 8)
	- 위치: tools/quality_metrics/trend_analyzer.py:36
- [13] MockSerialCommandHandler::processCommand - cyclomatic complexity 11 (limit 8)
	- 위치: test/mocks/MockSerialCommandHandler.h:27

