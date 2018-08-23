#pragma once

#include "project_config.h"

namespace soft_i2c
{
    void init( void );
    bool start( void );
    void stop( void );
    void ack( void );
    void nack( void );
    bool waitAck( void );
    void sendByte( uint8_t byte );
    uint8_t receiveByte( void );
}
