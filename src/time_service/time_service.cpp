#include "time_service.h"

extern "C"
{
    void SysTick_Handler( void )
    {
        time_service::systemTime++;
    }
}

namespace time_service
{
    volatile uint32_t systemTime = 0;

    void stopTime(void)
    {
        SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
    }

    void startTime(void)
    {
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    }

    void delay_ms(uint32_t delta)
    {
        uint32_t start = getCurTime();
        while( getCurTime() - start <= delta);

    }

    void init()
    {
        if( SysTick_Config(SystemCoreClock / 1000) )
        {
            UMBA_ASSERT_FAIL();
        }
    
    }
}



