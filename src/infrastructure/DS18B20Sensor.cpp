#include "DS18B20Sensor.h"

DS18B20Sensor::DS18B20Sensor(uint8_t pin)
    : pin(pin), oneWire(pin), sensors(&oneWire) {}

bool DS18B20Sensor::begin()
{
    sensors.begin();
    return true;
}

int DS18B20Sensor::getSensorCount()
{
    return sensors.getDeviceCount();
}

bool DS18B20Sensor::getAddress(uint8_t index, uint64_t &address)
{
    DeviceAddress addr;
    if (sensors.getAddress(addr, index))
    {
        address = 0;
        for (int i = 0; i < 8; ++i)
        {
            address |= ((uint64_t)addr[i] << (8 * i));
        }
        return true;
    }
    return false;
}

float DS18B20Sensor::getTemperature(uint8_t index)
{
    sensors.requestTemperatures();
    return sensors.getTempCByIndex(index);
}
