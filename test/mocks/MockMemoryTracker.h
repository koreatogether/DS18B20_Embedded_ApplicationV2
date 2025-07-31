#ifndef MOCK_MEMORY_TRACKER_H
#define MOCK_MEMORY_TRACKER_H

#include "../../src/application/IMemoryTracker.h"
#include <vector>
#include <string>
#include <sstream>

class MockMemoryTracker : public IMemoryTracker
{
private:
    std::vector<MemorySnapshot> _snapshots;
    int _maxHistorySize;

public:
    MockMemoryTracker() : _maxHistorySize(0) {} // 0 = unlimited

    void recordSnapshot(int freeBytes, int usedBytes, const std::string &eventType, const std::string &description = "") override
    {
        unsigned long timestamp = _snapshots.size() * 1000; // Mock timestamp
        _snapshots.emplace_back(timestamp, freeBytes, usedBytes, eventType, description);

        // Apply max size limit if set
        if (_maxHistorySize > 0 && _snapshots.size() > static_cast<size_t>(_maxHistorySize))
        {
            _snapshots.erase(_snapshots.begin());
        }
    }

    void recordCurrentMemory(const std::string &eventType, const std::string &description = "") override
    {
        // Mock current memory state
        int freeBytes = 2048; // Mock free memory
        int usedBytes = 1024; // Mock used memory
        recordSnapshot(freeBytes, usedBytes, eventType, description);
    }

    std::vector<MemorySnapshot> getSnapshots() const override
    {
        return _snapshots;
    }

    MemoryReport generateReport() const override
    {
        MemoryReport report;
        report.totalSnapshots = static_cast<int>(_snapshots.size());

        if (!_snapshots.empty())
        {
            // Calculate min/max/avg
            int minFree = _snapshots[0].freeBytes;
            int maxFree = _snapshots[0].freeBytes;
            long totalFree = 0;

            for (const auto &snapshot : _snapshots)
            {
                if (snapshot.freeBytes < minFree)
                    minFree = snapshot.freeBytes;
                if (snapshot.freeBytes > maxFree)
                    maxFree = snapshot.freeBytes;
                totalFree += snapshot.freeBytes;
            }

            report.minFreeBytes = minFree;
            report.maxFreeBytes = maxFree;
            report.avgFreeBytes = static_cast<int>(totalFree / _snapshots.size());

            // Calculate trend (simplified)
            if (_snapshots.size() > 1)
            {
                int first = _snapshots.front().freeBytes;
                int last = _snapshots.back().freeBytes;
                if (last > first + 100)
                    report.memoryTrend = 1; // Increasing
                else if (last < first - 100)
                    report.memoryTrend = -1; // Decreasing
                else
                    report.memoryTrend = 0; // Stable
            }
            else
            {
                report.memoryTrend = 0;
            }

            // Calculate duration
            report.duration = _snapshots.back().timestamp - _snapshots.front().timestamp;

            // Generate summary
            report.summary = "Memory tracking report: " + std::to_string(report.totalSnapshots) + " snapshots recorded.";
        }
        else
        {
            report.minFreeBytes = 0;
            report.maxFreeBytes = 0;
            report.avgFreeBytes = 0;
            report.memoryTrend = 0;
            report.duration = 0;
            report.summary = "No memory snapshots recorded.";
        }

        return report;
    }

    std::string exportToCsv() const override
    {
        std::ostringstream oss;
        oss << "Timestamp,FreeBytes,UsedBytes,EventType,Description\n";

        for (const auto &snapshot : _snapshots)
        {
            oss << snapshot.timestamp << ","
                << snapshot.freeBytes << ","
                << snapshot.usedBytes << ","
                << snapshot.eventType << ","
                << snapshot.description << "\n";
        }

        return oss.str();
    }

    std::string exportReportToMarkdown() const override
    {
        MemoryReport report = generateReport();
        std::ostringstream oss;

        oss << "# Memory Tracking Report\n\n";
        oss << "- **Total Snapshots**: " << report.totalSnapshots << "\n";
        oss << "- **Min Free Memory**: " << report.minFreeBytes << " bytes\n";
        oss << "- **Max Free Memory**: " << report.maxFreeBytes << " bytes\n";
        oss << "- **Avg Free Memory**: " << report.avgFreeBytes << " bytes\n";
        oss << "- **Memory Trend**: ";
        switch (report.memoryTrend)
        {
        case -1:
            oss << "Decreasing\n";
            break;
        case 0:
            oss << "Stable\n";
            break;
        case 1:
            oss << "Increasing\n";
            break;
        }
        oss << "- **Duration**: " << report.duration << " ms\n\n";
        oss << "## Snapshots\n\n";
        oss << "| Timestamp | Free Bytes | Used Bytes | Event Type | Description |\n";
        oss << "|-----------|------------|------------|------------|-------------|\n";

        for (const auto &snapshot : _snapshots)
        {
            oss << "| " << snapshot.timestamp
                << " | " << snapshot.freeBytes
                << " | " << snapshot.usedBytes
                << " | " << snapshot.eventType
                << " | " << snapshot.description << " |\n";
        }

        return oss.str();
    }

    void clearHistory() override
    {
        _snapshots.clear();
    }

    void setMaxHistorySize(int maxSize) override
    {
        _maxHistorySize = maxSize;
    }

    // Test helper methods
    void addMockSnapshot(unsigned long timestamp, int freeBytes, int usedBytes, const std::string &eventType = "TEST")
    {
        _snapshots.emplace_back(timestamp, freeBytes, usedBytes, eventType, "Mock data");
    }

    size_t getSnapshotCount() const
    {
        return _snapshots.size();
    }
};

#endif // MOCK_MEMORY_TRACKER_H
