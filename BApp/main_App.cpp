//
// Created by outlaw on 31.03.2022.
//

#include "UScope.hpp"

extern FDCAN_HandleTypeDef hfdcan1;

UScope uScope(DeviceUID::TYPE_MICROCHIP, 0x01, 0x4D, &hfdcan1);

extern "C"
{
    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
    {
        if (htim->Instance == TIM1)
            uScope.update();
    }

    void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
    {
        uScope.OnMotorTimer(htim);
    }

    void OnCANRx(FDCAN_RxHeaderTypeDef header, uint8_t* data)
    {
        uScope.OnCanRX(header, data);
    }

    FDCAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8];
    void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
    {
        if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
        {
            /* Retrieve Rx messages from RX FIFO0 */
            if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
            {
                Error_Handler();
            }
            OnCANRx(RxHeader, RxData);
        }
    }

    void appInit()
    {
        uScope.init();
        if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK) Error_Handler();
        if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
            Error_Handler();
        HAL_TIM_Base_Start_IT(&htim1);
    }

    void while_in_main_App()
    {
        uScope.Poll();
    }

    void OnSysTickTimer()
    {
        uScope.OnTimerINT(1);
    }
}