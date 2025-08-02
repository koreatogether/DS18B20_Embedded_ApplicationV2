#pragma once
#include <vector>
#include <string>
#include "mocks/MockTemperatureSensor.h"

// 유틸리티 함수 선언
std::vector<MockTemperatureSensor::Entry> getMockData();
std::string makeTableLine(int idx, const MockTemperatureSensor::Entry &entry);
