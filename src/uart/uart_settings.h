#pragma once

#include "project_config.h"

/***************************************************************************************************
  Это настроечный файл для UART
***************************************************************************************************/

namespace uart
{

    // размер приемного буфера или приемной очереди (freertos) в байтах
    const uint32_t rx_buffer_size = 255;

    // размер отправного буфера, который используется для передачи с копированием
    const uint32_t tx_buffer_size = 255;

}



// использование UART'ов

//#define UART_N1_ENABLE

#define UART_N2_ENABLE

/*#define UART_N3_ENABLE

#define UART_N4_ENABLE

#define UART_N5_ENABLE

#define UART_N6_ENABLE*/

// для использования FreeRTOS, объявите на весь проект дефайн UART_USE_FREERTOS
