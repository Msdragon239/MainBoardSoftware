#include "vl53l0x_device.h"
#include "time_service/time_service.h"

VL53L0X_Error Vl53l0xDevice::init()
{
    VL53L0X_Error status = VL53L0X_ERROR_NONE;

    uint32_t refSpadCount     = 0;
    uint8_t  isApertureSpads  = 0;
    uint8_t  VhvSettings      = 0;
    uint8_t  PhaseCal         = 0;

    // перезагружаем датчик
    m_xshutPin.reset();
    time_service::delay_ms(100);
    m_xshutPin.set();

    m_dev->I2cDevAddr = sensor_default_address;

    if (status == VL53L0X_ERROR_NONE)
    {
        status = VL53L0X_SetDeviceAddress( device, m_address );
        device->I2cDevAddr = m_address;
    }

    if (status == VL53L0X_ERROR_NONE)
    {
        status = VL53L0X_DataInit( device );
    }

    if (status == VL53L0X_ERROR_NONE)
    {
        status = VL53L0X_StaticInit( device );
    }

    if (status == VL53L0X_ERROR_NONE)
    {
        status = VL53L0X_PerformRefSpadManagement( device, &refSpadCount, &isApertureSpads);
    }

    if (status == VL53L0X_ERROR_NONE)
    {
        status = VL53L0X_PerformRefCalibration( device, &VhvSettings, &PhaseCal);
    }

    if (status == VL53L0X_ERROR_NONE)
    {
        status = VL53L0X_SetReferenceSpads( device, refSpadCount, isApertureSpads);
    }

    if (status == VL53L0X_ERROR_NONE)
    {
        status = VL53L0X_SetRefCalibration( device, VhvSettings, PhaseCal);
    }

    if (status == VL53L0X_ERROR_NONE)
    {
        status = VL53L0X_SetDeviceMode( device, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);
    }

    if (status == VL53L0X_ERROR_NONE)
    {
        status = VL53L0X_SetLimitCheckValue( device, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, (FixPoint1616_t)(0.25*65536) );
    }

    if (status == VL53L0X_ERROR_NONE)
    {
        status = VL53L0X_SetLimitCheckValue( device, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, (FixPoint1616_t)(32*65536) );
    }

    if (status == VL53L0X_ERROR_NONE)
    {
        status =VL53L0X_SetMeasurementTimingBudgetMicroSeconds( device, 20000 );
    }

    if (status == VL53L0X_ERROR_NONE)
    {
        status = VL53L0X_StartMeasurement( device );
    }

    return status;
}

uint16_t Vl53l0xDevice::getDistance()
{
    uint8_t data_ready;
    VL53L0X_RangingMeasurementData_t result;
    auto status = VL53L0X_GetMeasurementDataReady(&m_dev, &data_ready);

    if( status != VL53L0X_ERROR_NONE )
    {
        UMBA_ASSERT_FAIL();
    }

    status = VL53L0X_GetRangingMeasurementData(&m_dev, &result);

    if( status != VL53L0X_ERROR_NONE )
    {
        UMBA_ASSERT_FAIL();
    }

    status = VL53L0X_ClearInterruptMask(&m_dev, 0);

    if( status != VL53L0X_ERROR_NONE )
    {
        UMBA_ASSERT_FAIL();
    }

    return result.RangeMilliMeter;

}

