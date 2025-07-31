#include "MemoryTrackerService.h"
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iomanip>

MemoryTrackerService::MemoryTrackerService(std::shared_ptr<IHal> hal, int maxHistorySize)
    : _hal(hal), _maxHistorySize(maxHistorySize)
{
}

void MemoryTrackerService::recordSnapshot(int freeBytes, int usedBytes, const std::string &eventType, const std::string &description)
{
    unsigned long timestamp = _hal->millis();
    _snapshots.emplace_back(timestamp, freeBytes, usedBytes, eventType, description);

    enforceHistoryLimit();
}

void MemoryTrackerService::recordCurrentMemory(const std::string &eventType, const std::string &description)
{
    int freeBytes = _hal->getFreeMemoryBytes();
    // Arduino UNO R4 WiFi는 약 32KB RAM, 사용량은 추정값
    int totalMemory = 32768; // 32KB
    int usedBytes = totalMemory - freeBytes;

    recordSnapshot(freeBytes, usedBytes, eventType, description);
}

std::vector<MemorySnapshot> MemoryTrackerService::getSnapshots() const
{
    return std::vector<MemorySnapshot>(_snapshots.begin(), _snapshots.end());
}

MemoryReport MemoryTrackerService::generateReport() const
{
    MemoryReport report = {0, 0, 0, 0, 0, 0, ""};

    if (_snapshots.empty())
    {
        report.summary = "No memory data available";
        return report;
    }

    report.totalSnapshots = _snapshots.size();

    // 최소/최대/평균 계산
    auto minMax = std::minmax_element(_snapshots.begin(), _snapshots.end(),
                                      [](const MemorySnapshot &a, const MemorySnapshot &b)
                                      {
                                          return a.freeBytes < b.freeBytes;
                                      });

    report.minFreeBytes = minMax.first->freeBytes;
    report.maxFreeBytes = minMax.second->freeBytes;

    int totalFree = std::accumulate(_snapshots.begin(), _snapshots.end(), 0,
                                    [](int sum, const MemorySnapshot &snapshot)
                                    {
                                        return sum + snapshot.freeBytes;
                                    });
    report.avgFreeBytes = totalFree / report.totalSnapshots;

    // 추적 기간 계산
    if (_snapshots.size() > 1)
    {
        report.duration = _snapshots.back().timestamp - _snapshots.front().timestamp;
    }

    // 트렌드 계산
    report.memoryTrend = calculateTrend();

    // 요약 생성
    std::ostringstream summary;
    summary << "Tracked " << report.totalSnapshots << " snapshots over "
            << report.duration << "ms. ";
    summary << "Free memory: " << report.minFreeBytes << "-" << report.maxFreeBytes
            << " bytes (avg: " << report.avgFreeBytes << "). ";

    if (report.memoryTrend < 0)
    {
        summary << "Memory usage increasing (concerning).";
    }
    else if (report.memoryTrend > 0)
    {
        summary << "Memory usage decreasing (good).";
    }
    else
    {
        summary << "Memory usage stable.";
    }

    report.summary = summary.str();

    return report;
}

std::string MemoryTrackerService::exportToCsv() const
{
    std::ostringstream csv;

    // CSV 헤더
    csv << "timestamp,free_bytes,used_bytes,event_type,description\n";

    // 데이터 행
    for (const auto &snapshot : _snapshots)
    {
        csv << snapshot.timestamp << ","
            << snapshot.freeBytes << ","
            << snapshot.usedBytes << ","
            << snapshot.eventType << ","
            << "\"" << snapshot.description << "\"\n";
    }

    return csv.str();
}

std::string MemoryTrackerService::exportReportToMarkdown() const
{
    MemoryReport report = generateReport();
    std::ostringstream md;

    md << "# Memory Usage Report\n\n";
    md << "**Generated at:** " << _hal->millis() << "ms\n\n";

    md << "## Summary\n";
    md << report.summary << "\n\n";

    md << "## Statistics\n";
    md << "| Metric | Value |\n";
    md << "|--------|-------|\n";
    md << "| Total Snapshots | " << report.totalSnapshots << " |\n";
    md << "| Duration | " << report.duration << " ms |\n";
    md << "| Min Free Memory | " << report.minFreeBytes << " bytes |\n";
    md << "| Max Free Memory | " << report.maxFreeBytes << " bytes |\n";
    md << "| Avg Free Memory | " << report.avgFreeBytes << " bytes |\n";
    md << "| Memory Trend | ";

    if (report.memoryTrend < 0)
    {
        md << "Decreasing 📉";
    }
    else if (report.memoryTrend > 0)
    {
        md << "Increasing 📈";
    }
    else
    {
        md << "Stable ➡️";
    }
    md << " |\n\n";

    if (!_snapshots.empty())
    {
        md << "## Recent Snapshots\n";
        md << "| Timestamp | Free (bytes) | Used (bytes) | Event | Description |\n";
        md << "|-----------|--------------|--------------|-------|-------------|\n";

        // 최근 5개 스냅샷만 표시
        int start = std::max(0, static_cast<int>(_snapshots.size()) - 5);
        for (int i = start; i < static_cast<int>(_snapshots.size()); i++)
        {
            const auto &snapshot = _snapshots[i];
            md << "| " << snapshot.timestamp
               << " | " << snapshot.freeBytes
               << " | " << snapshot.usedBytes
               << " | " << snapshot.eventType
               << " | " << snapshot.description << " |\n";
        }
    }

    return md.str();
}

void MemoryTrackerService::clearHistory()
{
    _snapshots.clear();
}

void MemoryTrackerService::setMaxHistorySize(int maxSize)
{
    _maxHistorySize = maxSize;
    enforceHistoryLimit();
}

int MemoryTrackerService::getSnapshotCount() const
{
    return _snapshots.size();
}

int MemoryTrackerService::calculateTrend() const
{
    if (_snapshots.size() < 3)
    {
        return 0; // 데이터가 부족하면 안정으로 판단
    }

    // 최근 3개 데이터의 기울기 계산
    int recentCount = std::min(3, static_cast<int>(_snapshots.size()));
    double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;

    for (int i = 0; i < recentCount; i++)
    {
        int idx = _snapshots.size() - recentCount + i;
        double x = i;
        double y = _snapshots[idx].freeBytes;

        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }

    // 선형 회귀 기울기 계산
    double slope = (recentCount * sumXY - sumX * sumY) / (recentCount * sumX2 - sumX * sumX);

    if (slope > 50)
        return 1; // 메모리 증가 (사용량 감소)
    else if (slope < -50)
        return -1; // 메모리 감소 (사용량 증가)
    else
        return 0; // 안정
}

void MemoryTrackerService::enforceHistoryLimit()
{
    if (_maxHistorySize > 0 && static_cast<int>(_snapshots.size()) > _maxHistorySize)
    {
        _snapshots.pop_front();
    }
}
