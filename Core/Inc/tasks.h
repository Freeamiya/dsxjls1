//
// Created by EiveLL on 25-5-23.
//

#ifndef TASKS_H
#define TASKS_H

#include "stm32f1xx_hal.h"
#include "usart.h"
#include "VL53L0X.h"
#define SAMPLE_SIZE 20
void angle_sample_push(float angle);

void task1();

void task2();

void task3();

void task4();

void task5();

void task6();

void detect_peaks_and_valleys();

extern float mix_angle;
extern uint16_t dis_buf[10];
extern uint8_t task_running;
extern uint8_t task_index;
extern uint16_t task2_result;
#endif //TASKS_H
