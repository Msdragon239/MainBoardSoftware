#pragma once

#include "project_config.h"
#include <stdint.h>

namespace time_service
{
    extern volatile uint32_t systemTime;

    inline uint32_t getCurTime(void)
    {
        return systemTime;
    }

    void init();

    void stopTime(void);

    void startTime(void);

    // максимальное время задержки ограничено переполнением 32-х битного таймера, тикающего каждую 1 мс,
    // это примерно 49 дней.
    void delay_ms(uint32_t delta);

}// namespace
