
#### TruffleHog 기반 민감정보(시크릿) 자동 탐지 시스템

**설치 및 환경 구성:**
- https://github.com/trufflesecurity/trufflehog/releases 에서 Windows용 trufflehog.exe(Go 바이너리) 다운로드
- tools/ 폴더에 trufflehog.exe 배치

**자동화 스크립트:**
- tools/trufflehog_gitscan.sh: git 이력 + 파일시스템 전체를 모두 검사
  - git 모드: 커밋된 파일/이력 전체 탐지
  - filesystem 모드: 워킹 디렉토리 전체(커밋 안 된 파일, .venv 등 포함) 탐지
- tools/auto_commit.sh에서 빌드/테스트/문서화 후 자동 실행

**주요 옵션:**
```bash
trufflehog.exe --no-update git file://./ --json > [로그파일]   # git 이력 전체 검사
trufflehog.exe --no-update filesystem --json [프로젝트경로] > [로그파일]   # 파일시스템 전체 검사
```
- --no-update: 자동 업데이트 비활성화(Go 바이너리 권장)
- --json: 결과를 JSON 형식으로 저장
- file://./ : 현재 디렉토리 기준 git 저장소 전체
- [프로젝트경로]: 절대경로 사용 권장(Windows 호환)

**로그 저장 및 확인:**
- logs/detection/ 폴더에 git, filesystem 각각 별도 결과 파일 자동 저장
- 예시: trufflehog_git_scan_YYYYMMDD_HHMMSS.json, trufflehog_filesystem_scan_YYYYMMDD_HHMMSS.json
- 로그 파일 내 "SourceMetadata" 필드에 탐지 위치, "Raw" 필드에 시크릿 값(마스킹됨) 표시
- 실제 위험 데이터가 탐지되면 즉시 확인 및 조치

**운영 워크플로우:**
1. 코드 변경/커밋 전 auto_commit.sh 실행
2. 빌드/테스트/문서화 후 자동으로 TruffleHog 시크릿 검사
3. logs/detection/에서 탐지 결과 확인
4. 실제 시크릿/민감정보가 발견되면 즉시 삭제/교체 및 재검사

**확장:**
- Gitleaks, detect-secrets 등 추가 도구 병행 가능
- CI/CD 파이프라인에도 동일 방식 적용 가능
