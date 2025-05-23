//
// Created by EiveLL on 25-5-23.
//

#ifndef COUNTER_H
#define COUNTER_H
#include "stm32f1xx_hal.h"

typedef struct{
    int16_t led_ms;  // ·äÃùÆ÷¼ÆÊı
} Counter;

typedef struct{
    int8_t task2_flag;
    int8_t task3_flag;
    int8_t task4_flag;
    int8_t task5_flag;
    int8_t task6_flag;
}Flag;

extern Flag flag;
extern Counter counter;

#endif //COUNTER_H
