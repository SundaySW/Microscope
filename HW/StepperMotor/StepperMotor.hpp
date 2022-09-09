//
// Created by 79162 on 25.09.2021.
//

#ifndef MICROSCOPE_A4_BOARD_STEPPERMOTOR_HPP
#define MICROSCOPE_A4_BOARD_STEPPERMOTOR_HPP

#include <cstdlib>
#include <functional>
#include <utility>
#include "main.h"

struct StepperCfg
{
    float A;
    float Vmin = START_SPEED;
    float Vmax;
    int criticalNofSteps;
    bool directionInverted = false;
    TIM_HandleTypeDef *htim;
    uint32_t timChannel = TIM_CHANNEL_2;
    LOGIC_LEVEL* endPoint;
};

class StepperMotor{
    using MOTOR_IOS = PIN<MOTOR_OUTS, PinWriteable>;
    using Collable = std::function<void (StepperMotor* motor)>;
public:
    enum MODE
    {
        IDLE,
        ACCEL,
        CONST,
        DECCEL,
        in_ERROR
    };
    enum MOTOR_DIRECTION{
        BACKWARDS = 0,
        FORWARD = 1
    };

    Collable callBackOnEvent;
    Collable callBackOnStep;

    StepperMotor() = delete;
    StepperMotor(MOTOR_IOS& stepPin, MOTOR_IOS& dirPin, MOTOR_IOS& enablePin):
            step(stepPin), direction(dirPin), enable(enablePin)
    {
        enable.setInverted();
    }

    void init(StepperCfg &incomeCfg, Collable incomeFunc){
        A = incomeCfg.A;
        Vmin = incomeCfg.Vmin;
        Vmax = incomeCfg.Vmax;
        htim = incomeCfg.htim;
        timChannel = incomeCfg.timChannel;
        criticalNofSteps = incomeCfg.criticalNofSteps;
        directionInverted = incomeCfg.directionInverted;
        endPoint = incomeCfg.endPoint;

        callBackOnEvent = std::move(incomeFunc);
        timerDividend = SystemCoreClock/(htim->Instance->PSC);
    }
    inline void motor_OnTimer(){
        if(mode == in_ERROR) return;
        if(currentStep >= stepsToGo || (*endPoint && !currentDirection)) {
            stopMotor();
            return;
        }
        if(currentStep >= criticalNofSteps) changeDirection();
        if(accelerationMode) {
            reCalcSpeed();
            regValueCalc();
        }
        else currentStepPP();
    }

    inline void get_position(MOTOR_DIRECTION dir, int numOfSteps, float speed = 0){
        if ((int)speed) Vmax = speed;
        stepsToGo = numOfSteps;
        accelerationMode = true;
        V = Vmin;
        setDirection(dir);
        startMotor();
    }

    inline void stopMotor(){
        if(motorMoving){
            HAL_TIM_PWM_Stop_IT(htim, timChannel);
            enable.setValue(HIGH);
            motorMoving = false;
            mode = MODE::IDLE;
            event = EVENT_STOP;
            callBackOnEvent(this);
        }
    }

    inline void changeDirection(){
        setDirection(currentDirection ? BACKWARDS : FORWARD);
        //currentStep = 0;
        accel_step = 0;
        mode = MODE::ACCEL;
        if(dirChanged++ > 1){
            stopMotor();
            mode = in_ERROR;
        }
    }

    [[nodiscard]] inline bool isMotorMoving() const {
        return motorMoving;
    }

    [[nodiscard]] inline MODE getMode() const {
        return mode;
    }

    [[nodiscard]] inline MOTOR_EVENT getEvent() const {
        return event;
    }

    [[nodiscard]] inline MOTOR_DIRECTION getDirection() const {
        return currentDirection;
    }

private:
    MOTOR_IOS step;
    MOTOR_IOS direction;
    MOTOR_IOS enable;
    LOGIC_LEVEL* endPoint;
    TIM_HandleTypeDef *htim;
    uint32_t timChannel;
    uint32_t timerDividend;

    int criticalNofSteps;
    int currentStep = 0;
    int accel_step = 0;
    int stepsToGo = 0;

    float A;
    float Vmin;
    float Vmax;

    float V = 0.0f;

    MOTOR_DIRECTION currentDirection = FORWARD;
    MODE mode = IDLE;
    MOTOR_EVENT event = EVENT_STOP;
    int dirChanged = 0;
    bool motorMoving = false;
    bool accelerationMode = false;
    bool directionInverted = false;

    inline void startMotor(){
        accel_step = 0;
        currentStep = 0;
        dirChanged = 0;
        mode = MODE::ACCEL;
        motorMoving = true;
        enable.setValue(LOW);
        regValueCalc();
        HAL_TIM_PWM_Start_IT(htim, timChannel);
    }

    inline void regValueCalc(){
        if(V > 0){
            int buf = timerDividend / V;
            if(buf > 0 && buf < UINT16_MAX){
                __HAL_TIM_SET_AUTORELOAD(htim, buf);
                __HAL_TIM_SET_COMPARE(htim, timChannel, buf/2);
            }
        }
    }

    inline void setDirection(MOTOR_DIRECTION newDirection){
        currentDirection = newDirection;
        if(directionInverted) direction.setValue(LOGIC_LEVEL((currentDirection ? BACKWARDS : FORWARD)));
        else direction.setValue(LOGIC_LEVEL(currentDirection));
    }

    inline void currentStepPP(){
        currentStep++;
        callBackOnStep(this);
    }

    inline void reCalcSpeed(){
        if (mode == IDLE) return;
        switch (mode)
        {
            case MODE::ACCEL:
            {
                if (V >= Vmax)
                {
                    V = Vmax;
                    event = EVENT_CSS;
                    mode = MODE::CONST;
                }else
                    V += A;
                if (accel_step >= stepsToGo / 2)
                {
                    mode = MODE::DECCEL;
                    break;
                }
                accel_step++;
            }
            break;

            case MODE::CONST:
            {
                if (currentStep + accel_step >= stepsToGo) {
                    mode = MODE::DECCEL;
                }
            }
            break;

            case MODE::DECCEL:
            {
                if (accel_step <= 0)
                {
                    stopMotor();
                    mode = MODE::IDLE;
                    event = EVENT_STOP;
                    break;
                }else{
                    V -= A;
                    if (V < Vmin) V = Vmin;
                    accel_step--;
                }
            }
            break;

            default:
                break;
        }

        if (mode == ACCEL || mode == CONST || mode == DECCEL)
            currentStepPP();
    }
};

#endif //MICROSCOPE_A4_BOARD_STEPPERMOTOR_HPP