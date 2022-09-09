//
// Created by 79162 on 25.09.2021.
//

#ifndef MICROSCOPE_A4_BOARD_BUTTON_HPP
#define MICROSCOPE_A4_BOARD_BUTTON_HPP


#include <Inc/main.h>

struct Button{
    explicit Button(BTN_TYPE incomeType): type(incomeType){}
    const BTN_TYPE type;
    inline constexpr BTN_TYPE getType(){return type;}
};

#endif //MICROSCOPE_A4_BOARD_BUTTON_HPP
