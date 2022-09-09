//
// Created by outlaw on 08.04.2022.
//

#ifndef MICROSCOPE_A4_BOARD_MOVEMENTCONTROLLER_HPP
#define MICROSCOPE_A4_BOARD_MOVEMENTCONTROLLER_HPP

#include <StepperMotor/StepperMotor.hpp>
#include <protos_msg.h>

namespace GCmds{

#define LOWSPEED 1800;
#define MEDIUMSPEED 5003;
#define HIGHSPEED 7129;

    enum Commands{
        GoToCoord = 0x10,
        FinishLine = 0x20,
        GoToStart = 0x30,
        GoToEnd = 0x40
    };
    enum SPEED{
        LOW = 0x1,
        MEDIUM = 0x2,
        HIGH = 0x3,
    };
}
class MovementController{
public:
    MovementController() = delete;

    MovementController(StepperMotor& x, StepperMotor& y, StepperMotor& z):
    xMotor(&x), yMotor(&y), zMotor(&z)
    {
        xMotor->callBackOnStep = [this](StepperMotor* motor){updateCurrentPosition(motor);};
        yMotor->callBackOnStep = [this](StepperMotor* motor){updateCurrentPosition(motor);};
        zMotor->callBackOnStep = [this](StepperMotor* motor){updateCurrentPosition(motor);};
    }

    void fillCoordsToBuffer(uint16_t* buffer) const {
        buffer[0] = {currentXCoord};
        buffer[1] = {currentYCoord};
        buffer[2] = {currentZCoord};
    }

    DEVICE_STATUS update(){
        if(!checkMovement(xMotor) && !checkMovement(yMotor) && !checkMovement(zMotor))
            currentStatus = DEVICE_STANDBY;
        if(checkError(xMotor) || checkError(yMotor) || checkError(zMotor))
            currentStatus = DEVICE_ERROR;
        return currentStatus;
    }

    void processMsg(const Protos::Msg& msg){
        switch (msg.GetCMDId()) {
            case GCmds::Commands::GoToStart:
                goTo0Position(msg.Data[1]);
                break;
            case GCmds::Commands::GoToEnd:
                goToEndPosition(msg.Data[1]);
                break;
            case GCmds::Commands::GoToCoord:
                goToCoord(msg);
            default:
                break;
        }
    }

    void checkMotorMove(){
        currentStatus = DEVICE_MOTOR_MOVING;
        yMotor->get_position(StepperMotor::FORWARD, 1000, 8000);
        xMotor->get_position(StepperMotor::FORWARD, 1000, 8000);
    }

    void stopMotor(StepperMotor* motor){
        motor->stopMotor();
    }

protected:
    inline void goTo0Position(const unsigned char cmd) {
        currentStatus = DEVICE_RETURN_TO_INITIAL_STATE;
        xMotor->get_position(StepperMotor::BACKWARDS, XAxisSize+1, getSpeedFromCmd(cmd));
        yMotor->get_position(StepperMotor::BACKWARDS, YAxisSize+1, getSpeedFromCmd(cmd));
        zMotor->get_position(StepperMotor::BACKWARDS, ZAxisSize+1, getSpeedFromCmd(cmd));
    }

    inline void goToEndPosition(const unsigned char cmd){
        currentStatus = DEVICE_MOTOR_MOVING;
        calcStepsAndGo(currentXCoord, xMotor, XAxisSize, getSpeedFromCmd(cmd));
        calcStepsAndGo(currentXCoord, yMotor, YAxisSize, getSpeedFromCmd(cmd));
        calcStepsAndGo(currentXCoord, zMotor, ZAxisSize, getSpeedFromCmd(cmd));
    }

    void goToCoord(const Protos::Msg& msg){
        float speed = getSpeedFromCmd(msg.Data[1]);
        uint16_t x = (msg.Data[2]<<8) + msg.Data[3];
        uint16_t y = (msg.Data[4]<<8) + msg.Data[5];
        uint16_t z = (msg.Data[6]<<8) + msg.Data[7];

        if(x <= XAxisSize && x > 0) calcStepsAndGo(currentXCoord, xMotor, x, speed);
        if(y <= YAxisSize && y > 0) calcStepsAndGo(currentYCoord, yMotor, y, speed);
        if(z <= ZAxisSize && z > 0) calcStepsAndGo(currentZCoord, zMotor, z, speed);
    }

    static float getSpeedFromCmd(const unsigned char cmd){
        float speed = 0;
        switch (cmd) {
            case GCmds::SPEED::LOW:
                speed = LOWSPEED;
                break;
            case GCmds::SPEED::MEDIUM:
                speed = MEDIUMSPEED;
                break;
            case GCmds::SPEED::HIGH:
                speed = HIGHSPEED;
                break;
            default:
                break;
        }
        return speed;
    }

    inline void calcStepsAndGo(uint16_t currentCoord, StepperMotor* motor, int newCoord, float speed){
        if(currentCoord == newCoord) return;
        currentStatus = DEVICE_MOTOR_MOVING;
        if(currentCoord > newCoord) motor->get_position(StepperMotor::BACKWARDS, currentCoord - newCoord, speed);
        else motor->get_position(StepperMotor::FORWARD, newCoord - currentCoord, speed);
    }

    static inline bool checkError(StepperMotor* motor){
        return motor->getMode() == StepperMotor::in_ERROR;
    }

    static inline bool checkMovement(StepperMotor* motor){
        return motor->isMotorMoving();
    }

    inline void updateCurrentPosition(StepperMotor* motor){
        if(motor == xMotor) motor->getDirection() == StepperMotor::FORWARD ? currentXCoord++ : currentXCoord--;
        if(motor == yMotor) motor->getDirection() == StepperMotor::FORWARD ? currentYCoord++ : currentYCoord--;
        if(motor == zMotor) motor->getDirection() == StepperMotor::FORWARD ? currentZCoord++ : currentZCoord--;
    }

private:
    StepperMotor *xMotor;
    StepperMotor *yMotor;
    StepperMotor *zMotor;
    uint16_t currentXCoord = 0;
    uint16_t currentYCoord = 0;
    uint16_t currentZCoord = 0;

    BOARD_ERROR currentError = NO_ERROR;
    volatile DEVICE_STATUS currentStatus = DEVICE_STANDBY;
};
#endif //MICROSCOPE_A4_BOARD_MOVEMENTCONTROLLER_HPP