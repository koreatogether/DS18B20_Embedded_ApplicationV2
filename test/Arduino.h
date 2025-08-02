#pragma once

#include <string>

using String = std::string;

class HardwareSerial
{
public:
    void println(const char *) {}
    void println(const std::string &) {}
    void print(const std::string &) {}
};

extern HardwareSerial Serial;
