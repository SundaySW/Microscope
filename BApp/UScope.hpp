//
// Created by 79162 on 25.09.2021.
//

#ifndef MICROSCOPE_A4_BOARD_HPP
#define MICROSCOPE_A4_BOARD_HPP

#include "../HW/IO/Button.hpp"
#include "../HW/IO/PIN.hpp"
#include "MovementController.hpp"
#include "StepperMotor/StepperMotor.hpp"
#include "base_device.hpp"
#include "main.h"
#include "tim.h"

static inline void delay(uint32_t delay){
    __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_UPDATE);
    HAL_TIM_Base_Start(&htim6);
    TIM6->CNT = 0;
    while(TIM6->CNT < delay){}
    HAL_TIM_Base_Stop(&htim6);
    __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
}

using namespace Protos;

class UScope : public BaseDevice{
    using MOTOR_PIN = PIN<MOTOR_OUTS, PinWriteable>;
public:
    const UScope& operator=(const UScope &) = delete;
    UScope& operator=(UScope &) = delete;
    UScope() = delete;

    UScope(DeviceUID::TYPE uidType, uint8_t family, uint8_t addr, FDCAN_HandleTypeDef* can)
           : BaseDevice(uidType, family, addr, can)
    {
    }

    void init(){
        //TODO add DIP check
        xStartPoint.setInverted();
        xMotorCFG.A = CONFIG1_ACCELERATION;
        xMotorCFG.Vmax = CONFIG1_SPEED;
        xMotorCFG.criticalNofSteps = XAxisSize;
        xMotorCFG.htim = &htim4;
        xMotorCFG.endPoint = &xStartPoint.currentState;
        xMotor.init(xMotorCFG, [this](StepperMotor* m){OnAxisEvent(m);});

        yMotorCFG.A = CONFIG1_ACCELERATION;
        yMotorCFG.Vmax = CONFIG1_SPEED;
        yMotorCFG.criticalNofSteps = YAxisSize;
        yMotorCFG.htim = &htim3;
        yMotorCFG.endPoint = &yStartPoint.currentState;
        yMotor.init(yMotorCFG, [this](StepperMotor* m){OnAxisEvent(m);});

        zMotorCFG.A = CONFIG1_ACCELERATION;
        zMotorCFG.Vmax = CONFIG1_SPEED;
        zMotorCFG.criticalNofSteps = ZAxisSize;
        zMotorCFG.htim = &htim2;
        zMotorCFG.endPoint = &zStartPoint.currentState;
        zMotor.init(zMotorCFG, [this](StepperMotor* m){OnAxisEvent(m);});
    }

    void update(){
        currentStatus = movementController.update();
        if(currentStatus == DEVICE_ERROR) errorHandler(currentError);
    }

    void OnTimer(int ms) override {
        if (!ms1000Counter--)
        {
            ms1000Counter += 1000;
            char data[8] = {0};
            uint16_t coords[3];
            movementController.fillCoordsToBuffer(coords);

            switch(currentStatus){
                case DEVICE_STANDBY:
                    data[0] = 0xC;
                    break;
                case DEVICE_MOTOR_MOVING:
                    data[0] = 0xD;
                    break;
                default:
                    data[0] = 0xF;
                    break;
            }

            data[1] = 0xfe;
            //TX 2,3 bytes
            int16_t TXpos = coords[0];
            data[2] = (TXpos >> 8);
            data[3] = TXpos;
            //TY 4,5 bytes
            int16_t TYpos = coords[1];
            data[4] = (TYpos >> 8);
            data[5] = TYpos;
            //TZ 6,7 bytes
            int16_t TZpos = coords[2];
            data[6] = (TZpos >> 8);
            data[7] = TZpos;
            SendProtosMsg (0xFF, MSGTYPE_EVENT, data ,8);
        }
    };

    void OnPoll() override{
        for(auto &pin: startPoints) pin->refresh();
    }

    void OnMotorTimer(TIM_HandleTypeDef *htim){
        if(htim->Instance == zMotorCFG.htim->Instance) zMotor.motor_OnTimer();
        if(htim->Instance == yMotorCFG.htim->Instance) yMotor.motor_OnTimer();
        if(htim->Instance == xMotorCFG.htim->Instance) xMotor.motor_OnTimer();
    }

    void OnAxisEvent(StepperMotor* motor)
    {
        MOTOR_EVENT event = motor->getEvent();
        char data[8] = {0};

        if (motor == &xMotor) data[0] = 0x32;
        else if(motor == &yMotor) data[0] = 0x33;
        else if(motor == &zMotor) data[0] = 0x34;

        if (event == EVENT_STOP) data[1] = 0x01;
        else if (event == EVENT_CSS) data[1] = 0x02;
        else if (event == EVENT_CSE) data[1] = 0x03;
        else return;

        SendProtosMsg (0xFF, MSGTYPE_EVENT, data ,2);
    }

    void ProcessMessage(const Protos::Msg& msg) override {
        if (msg.GetMSGType() == MSGTYPE_CMDSPEC) {
            if(currentStatus == DEVICE_STANDBY) movementController.processMsg(msg);
        }
    };

private:
    using PinRD = PIN<INPUT_TYPE, PinReadable>;
    PinRD xStartPoint {X_AXIS_ENDPOINT, X_endpoint_PA8_GPIO_Port, X_endpoint_PA8_Pin};
    PinRD yStartPoint {Y_AXIS_ENDPOINT, Y_endpoint_PA8_GPIO_Port, Y_endpoint_PA8_Pin};
    PinRD zStartPoint {Z_AXIS_ENDPOINT, Z_endpoint_PA8_GPIO_Port, Z_endpoint_PA8_Pin};
    PinRD* startPoints[3] = {&xStartPoint, &yStartPoint, &zStartPoint};

    StepperCfg xMotorCFG;
    StepperCfg yMotorCFG;
    StepperCfg zMotorCFG;

    MOTOR_PIN xSTEPpin = MOTOR_PIN(STEP_PIN, STEP_X_GPIO_Port, STEP_X_Pin);
    MOTOR_PIN xDIRpin = MOTOR_PIN(DIR_PIN, DIR_X_GPIO_Port, DIR_X_Pin);
    MOTOR_PIN xENpin = MOTOR_PIN(ENABLE_PIN, ENABLE_X_GPIO_Port, ENABLE_X_Pin);
    StepperMotor xMotor = StepperMotor(xSTEPpin, xDIRpin, xENpin);
    MOTOR_PIN ySTEPpin = MOTOR_PIN(STEP_PIN, STEP_Y_GPIO_Port, STEP_Y_Pin);
    MOTOR_PIN yDIRpin = MOTOR_PIN(DIR_PIN, DIR_Y_GPIO_Port, DIR_Y_Pin);
    MOTOR_PIN yENpin = MOTOR_PIN(ENABLE_PIN, ENABLE_Y_GPIO_Port, ENABLE_Y_Pin);
    StepperMotor yMotor = StepperMotor(ySTEPpin, yDIRpin, yENpin);
    MOTOR_PIN zSTEPpin = MOTOR_PIN(STEP_PIN, STEP_Z_GPIO_Port, STEP_Z_Pin);
    MOTOR_PIN zDIRpin = MOTOR_PIN(DIR_PIN, DIR_Z_GPIO_Port, DIR_Z_Pin);
    MOTOR_PIN zENpin = MOTOR_PIN(ENABLE_PIN, ENABLE_Z_GPIO_Port, ENABLE_Z_Pin);
    StepperMotor zMotor = StepperMotor(zSTEPpin, zDIRpin, zENpin);

    MovementController movementController {xMotor, yMotor, zMotor};

    uint16_t ms1000Counter = 0;
    BOARD_ERROR currentError = NO_ERROR;
    DEVICE_STATUS currentStatus = DEVICE_STANDBY;

    [[noreturn]] void errorHandler(BOARD_ERROR error){
        currentStatus = DEVICE_ERROR;
        switch (error) {
            case LIMIT_SWITCH_ERROR:
                break;
            case STANDBY_MOVEMENT_ERROR:
                break;
            default:
                break;
        }
        //TODO mb remove
        Error_Handler();
        while (true){
        };
    }
};

#endif //MICROSCOPE_A4_BOARD_HPP
