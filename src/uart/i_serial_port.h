#pragma once

#include "project_config.h"

class ISerialPort
{
public:

    virtual ~ISerialPort() {}

    virtual void changeBaudrate( uint32_t baudrate ) = 0;

    virtual void reset(void) = 0;

    virtual void enableReceiver(void) = 0;
    virtual void disableReceiver(void) = 0;


    #ifdef UART_USE_FREERTOS

        // прием через очередь
        virtual QueueHandle_t getQueue(void) = 0;

        // мьютекс на работу с этим хендлом вообще
        virtual SemaphoreHandle_t getOwnerMutex(void) = 0;

        // семафор - "передача окончена"
        virtual SemaphoreHandle_t getTransmitCompleteSem(void) = 0;

    #else

        // методы для приема без использования freeRTOS
        virtual bool  isNewByte(void) = 0;

        virtual uint8_t getByte(void) = 0;

        // "мьютекс" на владение
        virtual bool isLocked(void) = 0;
        virtual void lock(void) = 0;
        virtual void unLock(void) = 0;

    #endif

    virtual bool sendStaticArray( const volatile uint8_t * staticArr, uint32_t size ) = 0;
    virtual bool sendLocalArray( const uint8_t * localArr, uint32_t size ) = 0;

    virtual bool sendInt32( uint32_t data )
    {
        return sendLocalArray( ( const uint8_t *)(&data), sizeof(data) );
    }

    virtual bool sendByte( uint8_t byte )
    {
        return sendLocalArray( (const uint8_t *)(&byte), sizeof(byte) );
    }

    virtual bool isTransmitComplete(void) = 0;

    // произошло ли переполнение приемного буфера
    virtual bool isReceiverOverrun(void) = 0;

    // получить код ошибки - возвращает cpu-specific набор флагов ошибок
    virtual uint32_t getError(void) = 0;

};


