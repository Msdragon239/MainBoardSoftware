#pragma once

#include "project_config.h"
#include "uart_settings.h"
#include "i_serial_port.h"
#include "circular_buffer/blocking_circular_buffer.h"

#include <algorithm>

namespace uart
{

    class UartHandleMock : public ISerialPort
    {
    public:

        UartHandleMock() :
            m_baudrate(0),
            m_wasReset(false),
            m_isReceiverEnabled(true),

            #ifdef UART_USE_FREERTOS

                m_queue(),
                m_ownerMutex(),
                m_transmitCompleteSem(),

            #else
                m_isNewByte(false),
                m_isLocked(false),
                m_rxBuffer(),

            #endif

            m_txStaticArray(0),
            m_txStaticArraySize(0),
            m_txLocalArray(),
            m_txLocalArrayUsedSize(0),
            m_isTransmitComplete(true),
            m_isReceiverOverrun(false),
            m_error(0)
        {
            #ifdef UART_USE_FREERTOS
                // нужно создать очередь из байтов
                m_queue = osQueueCreate( rx_buffer_size, 1 );

                UMBA_ASSERT(m_queue);

                // мьютекс на владение
                m_ownerMutex = osSemaphoreCreateMutex();

                UMBA_ASSERT(m_ownerMutex);

                // семафор на завершение передачи
                // семафор создается обнуленным
                m_transmitCompleteSem = osSemaphoreCreateBinary();

                UMBA_ASSERT(m_transmitCompleteSem);

            #endif
        }

        virtual void changeBaudrate( uint32_t baudrate )
        {
            m_baudrate = baudrate;
        }

        virtual void reset(void)
        {
            m_wasReset = true;
        }

        virtual void enableReceiver(void)
        {
            m_isReceiverEnabled = true;
        }
        virtual void disableReceiver(void)
        {
            m_isReceiverEnabled = false;
        }


        #ifdef UART_USE_FREERTOS

            virtual QueueHandle_t getQueue(void)
            {
                return m_queue;
            }

            virtual SemaphoreHandle_t getOwnerMutex(void)
            {
                return m_ownerMutex;
            }

            virtual SemaphoreHandle_t getTransmitCompleteSem(void)
            {
                return m_transmitCompleteSem;
            }

        #else

            virtual bool isNewByte(void)
            {
                return ! m_rxBuffer.isEmpty();
            }

            virtual uint8_t getByte(void)
            {
                return m_rxBuffer.readTail();
            }

            virtual bool isLocked(void)
            {
                return m_isLocked;
            }
            virtual void lock(void)
            {
                m_isLocked = true;
            }

            virtual void unLock(void)
            {
                m_isLocked = false;
            }

        #endif

        virtual bool sendStaticArray( const volatile uint8_t * staticArr, uint32_t size )
        {
            m_txStaticArray = staticArr;

            m_txStaticArraySize = size;

            return true;
        }

        virtual bool sendLocalArray( const uint8_t * localArr, uint32_t size )
        {
            UMBA_ASSERT(size <= tx_buffer_size);

            std::copy( localArr, localArr + size, m_txLocalArray );

            m_txLocalArrayUsedSize = size;

            return true;
        }

        virtual bool isTransmitComplete(void)
        {
            return m_isTransmitComplete;
        }

        virtual bool isReceiverOverrun(void)
        {
            return m_isReceiverOverrun;
        }

        virtual uint32_t getError(void)
        {
            return m_error;
        }

        // имитация принятия набора данных - массив копируется
        // или в очередь - или в отдельный массив
        void receiveData( const uint8_t * data, uint32_t size )
        {
            UMBA_ASSERT( size <= rx_buffer_size);

            #ifdef UART_USE_FREERTOS

                for(uint32_t i=0; i<size; i++)
                {
                    osQueueSendToBack(m_queue, (void *)(&data[i]), NULL);
                }

            #else

                m_rxBuffer.putData(data, size);

            #endif
        }

        // сравнение посланного массива с эталоном
        bool compareSentLocal( const uint8_t * data, uint32_t size )
        {
            if(size != m_txLocalArrayUsedSize)
                return false;

            for(uint32_t i=0; i<size; i++)
            {
                if( data[i] != m_txLocalArray[i] )
                    return false;
            }

            return true;
        }

        // сравнение посланного статического массива; сравнивает только указатель и размер
        bool compareSentStatic( const uint8_t * staticData, uint32_t size )
        {
            if( size != m_txStaticArraySize )
                return false;

            if( m_txStaticArray != staticData )
                return false;

            return true;
        }

        // сбросить все флаги и обнулить все массивы
        void flush(void)
        {
            m_baudrate = 0;
            m_wasReset = false;
            m_isReceiverEnabled = true;

            #ifdef UART_USE_FREERTOS

                osQueueReset(m_queue);
                osQueueReset(m_ownerMutex);
                osQueueReset(m_transmitCompleteSem);

            #else

                m_isLocked = false;
                m_rxBuffer.reset();

            #endif

            m_txStaticArray = 0;
            m_txStaticArraySize = 0;
            std::fill( m_txLocalArray, m_txLocalArray + tx_buffer_size, 0);

            m_txLocalArrayUsedSize = 0;
            m_isTransmitComplete = true;
            m_isReceiverOverrun = false;
            m_error = 0;
        }

        // публичные поддельные даные

        uint32_t m_baudrate;

        bool m_wasReset;

        bool m_isReceiverEnabled;

        #ifdef UART_USE_FREERTOS

            QueueHandle_t m_queue;

            SemaphoreHandle_t m_ownerMutex;

            SemaphoreHandle_t m_transmitCompleteSem;

        #else

            bool m_isNewByte;

            bool m_isLocked;

            CircularBuffer< uint8_t, rx_buffer_size > m_rxBuffer;

        #endif

        const volatile uint8_t * m_txStaticArray;
        uint32_t m_txStaticArraySize;

        uint8_t m_txLocalArray[ tx_buffer_size ];
        uint32_t m_txLocalArrayUsedSize;

        bool m_isTransmitComplete;
        bool m_isReceiverOverrun;

        uint32_t m_error;

    private:

        // чтобы варнинги заткнуть
        UartHandleMock( const UartHandleMock & );
        UartHandleMock & operator=( const UartHandleMock &);
    };

} /* namespace uart */
