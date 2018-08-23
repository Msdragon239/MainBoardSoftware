#pragma once

#include "project_config.h"
#include "umba/umba_gpio_pin.h"

#include "sm32-api/platform/vl53l0x_platform.h"

class Vl53l0xDevice
{

public:

    static const uint8_t sensor_default_address = 0x52;

    Vl53l0xDevice( const umba::GpioPin & xshut, uint8_t address ) :
        m_xshutPin( xshut ),
        m_address( address )
    {
        m_dev.I2cDevAddr = sensor_default_address;
        m_dev.Present = 0;
        m_dev.Id = 0;
    }


    VL53L0X_Error init();

    uint16_t getDistance();

private:

    const umba::GpioPin & m_xshutPin;
    uint8_t m_address;

    VL53L0X_Dev_t m_dev;
};

