# 프로젝트에서 사용 중이지만 별도 매뉴얼이 없는 도구 목록 (2025-07-30)

이 문서는 tools/ 및 자동화 스크립트에서 사용되는 도구 중, docs/tools/ 이하에 별도 사용법/운영 매뉴얼이 없는 항목을 정리한 리스트입니다.

---

## 1. auto_commit.sh
- 설명: 전체 빌드, 테스트, 문서화, 시크릿 검사, git add까지 자동화하는 Bash 스크립트
- 매뉴얼 없음 (권장: docs/tools/auto_commit_manual.md 등으로 작성 필요)

## 2. trufflehog_gitscan.sh
- 설명: TruffleHog를 이용해 git 이력 및 파일시스템 전체를 시크릿 스캔하는 Bash 스크립트
- 매뉴얼 없음 (권장: docs/tools/trufflehog_gitscan_manual.md 등으로 작성 필요)

## 3. quality_metrics/code_metrics.py
- 설명: 코드 품질 메트릭(테스트, 빌드, 아키텍처 등) 자동 수집 Python 스크립트
- 매뉴얼: tools/quality_metrics/README.md에 간단 설명 있음 (상세 매뉴얼 별도 작성 가능)

## 4. quality_metrics/trend_analyzer.py
- 설명: 품질 메트릭 트렌드 분석 Python 스크립트
- 매뉴얼: tools/quality_metrics/README.md에 간단 설명 있음 (상세 매뉴얼 별도 작성 가능)

## 5. quality_metrics/monitor_quality.ps1, monitor_quality.sh
- 설명: 전체 품질 모니터링 자동화(빌드+테스트+분석) PowerShell/Bash 스크립트
- 매뉴얼: tools/quality_metrics/README.md에 간단 설명 있음 (상세 매뉴얼 별도 작성 가능)

## 6. trufflehog.exe
- 설명: TruffleHog 시크릿 탐지 Go 바이너리 (실행파일)
- 매뉴얼: docs/tools/truffleHog_manual.md (이미 존재)

---

### 요약
- auto_commit.sh, trufflehog_gitscan.sh는 별도 매뉴얼이 필요합니다.
- quality_metrics/ 하위 스크립트들은 README.md에 간단 설명이 있으나, 상세 매뉴얼이 필요하다면 별도 파일로 작성 권장합니다.
- trufflehog.exe는 이미 매뉴얼이 존재합니다.
