#pragma once

#include "project_config.h"
#include "uart_settings.h"
#include "i_serial_port.h"

#ifndef UART_USE_FREERTOS
   #include "circular_buffer/blocking_circular_buffer.h"
#endif

namespace uart
{
    const uint32_t error_none    = 0;
    const uint32_t error_parity  = 1<<0;
    const uint32_t error_framing = 1<<1;
    const uint32_t error_noise   = 1<<2;
    const uint32_t error_overrun = 1<<3;
    
    STRONG_ENUM(RxPins, UART1_PA10,
                        UART1_PB7,

                        UART2_PA3,
                        UART2_PD6,

                        UART3_PB11,
                        UART3_PC11,
                        UART3_PD9,

                        UART4_PA1,
                        UART4_PC11,

                        UART5_PD2,

                        UART6_PC7,
                        UART6_PG9);


    STRONG_ENUM(TxPins, UART1_PA9,
                        UART1_PB6,

                        UART2_PA2,
                        UART2_PD5,

                        UART3_PB10,
                        UART3_PC10,
                        UART3_PD8,

                        UART4_PA0,
                        UART4_PC10,

                        UART5_PC12,

                        UART6_PC6,
                        UART6_PG14);
    
}

namespace uart
{

    class Handle : public ISerialPort
    {
    public:

        Handle();

        void init( RxPins rxPin, TxPins txPin, uint32_t baudrate);

        void init( RxPins rxPin, TxPins txPin, uint32_t baudrate, uint8_t irqPrio);

        void init(RxPins rxPin,
                  TxPins txPin,
                  uint32_t baudrate,
                  GPIO_TypeDef * rs485Port,
                  uint16_t rs485Pin,
                  uint8_t irqPrio = IRQ_PRIORITY_LEVEL_NORMAL);

        void initExtended( RxPins rxPin,
                           TxPins txPin,
                           USART_InitTypeDef & initStruct,
                           GPIO_TypeDef * rs485Port = 0,
                           uint16_t rs485Pin = 0,
                           uint8_t irqPrio = IRQ_PRIORITY_LEVEL_NORMAL);

        virtual void changeBaudrate( uint32_t baudrate );

        virtual void reset(void);

        virtual void enableReceiver(void);
        virtual void disableReceiver(void);


        #ifdef UART_USE_FREERTOS

            // прием через очередь
            virtual QueueHandle_t getQueue(void);

            // мьютекс на работу с этим хендлом вообще
            virtual SemaphoreHandle_t getOwnerMutex(void);

            // семафор - "передача окончена"
            virtual SemaphoreHandle_t getTransmitCompleteSem(void);

        #else

            // здесь к базовому интерфейсу добавляются методы для приема
            // без использования freeRTOS

            virtual bool isNewByte(void);

            virtual uint8_t getByte(void);

            // "мьютекс" на владение
            virtual bool isLocked(void);
            virtual void lock(void);
            virtual void unLock(void);

        #endif

        virtual bool sendStaticArray( volatile const uint8_t * staticArr, uint32_t size );
        virtual bool sendLocalArray( const uint8_t * localArr, uint32_t size );

        virtual bool isTransmitComplete(void);

        // произошло ли переполнение приемного буфера
        virtual bool isReceiverOverrun(void);

        // получить статус ошибки
        virtual uint32_t getError(void);

        // к сожалению, его не сделать приватным
        void irqHandler(void);

    private:

        struct UartSettings;

        // копировать хендл запрещено
        Handle(const Handle & that);
        
        // присваивать хендл запрещено
        Handle & operator=( Handle & that);

        void initPrivate(RxPins rxPin, TxPins txPin, USART_InitTypeDef & initStruct, uint8_t irqPrio);

        void uartInitHelper( UartSettings & settings );

        // сброс внутренних буферов и т.п.
        void teardown();

        void enableTransmit485(void);

        void disableTransmit485(void);

        // счетчик, не позволяющий создавать экземпляры этого класса
        // помимо тех, которые уже созданы для работы с уартами
        static uint8_t handlesCounter;

        bool m_isInited;

        USART_TypeDef * m_uartRegs;

        USART_InitTypeDef m_curSettings;

        uint32_t m_error;

        // передача
        const volatile uint8_t * m_pTxBuf;
        volatile uint32_t m_txCount;
        volatile uint32_t m_txMsgSize;
        volatile bool m_isTransmitComplete;
        volatile uint8_t m_txBuffer[ tx_buffer_size ];

        GPIO_TypeDef * m_rs485Port;
        uint16_t m_rs485Pin;

        // прием

        volatile bool m_isRxOverrun;

        #ifdef UART_USE_FREERTOS

            QueueHandle_t m_rxQueue;
            SemaphoreHandle_t m_ownerMutex;
            SemaphoreHandle_t m_transmitCompleteSem;

        #else

            BlockingCircularBuffer< uint8_t, rx_buffer_size > rxBuffer;

            bool m_isLocked;

        #endif

    };

    #ifdef UART_N1_ENABLE
        extern Handle uart1;
    #endif

    #ifdef UART_N2_ENABLE
        extern Handle uart2;
    #endif

    #ifdef UART_N3_ENABLE
        extern Handle uart3;
    #endif

    #ifdef UART_N4_ENABLE
        extern Handle uart4;
    #endif

    #ifdef UART_N5_ENABLE
        extern Handle uart5;
    #endif

    #ifdef UART_N6_ENABLE
        extern Handle uart6;
    #endif

} // namespace uart
