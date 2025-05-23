//
// Created by EiveLL on 25-5-23.
//

#include "tasks.h"

#include "counter.h"
uint8_t task_running = 1;
uint8_t task_index = 3;
int task2_flag = 0;
uint16_t dis_buf[10] = {0};
float angle_sample[SAMPLE_SIZE];
uint16_t task2_result = 0;
float task3_result = 0;
uint32_t last_time = 0;
uint32_t current_time = 0;

void angle_sample_push(float angle){
    for (int i = SAMPLE_SIZE - 1; i > 0; i--) {
        angle_sample[i] = angle_sample[i - 1];
    }
    angle_sample[0] = angle;
}

void task1() {
    uart_printf("System initialization successful!\r\n");
}

void task2() {
    angle_sample_push(mix_angle);
    detect_peaks_and_valleys();
}

void task3() {
    angle_sample_push(mix_angle);
    detect_peaks_and_valleys();
}

void task4() {

}

void task5() {

}

void task6() {

}


void detect_peaks_and_valleys() {
    float b0 = angle_sample[0];
    float b1 = angle_sample[4];
    float b2 = angle_sample[10];
    float b3 = angle_sample[16];
    float b4 = angle_sample[18];

    // 检测极大值（最低点）
    if ( b2 > 35 && b0 < b1 && b1 < b2 && b2 > b3 && b3 > b4) {
        if (task_index == 2) {
            tof_distance = readRangeSingleMillimeters(&distanceStr);
            task2_result = 720 - 115 - tof_distance;
            if (counter.led_ms == 0) {
                uart_printf("result:%d\r\n", task2_result);
            }
            counter.led_ms = 100;
        }
    }

    // 检测极小值（最高点）
    if (b2<10&&b0 > b1 && b1 > b2 && b2 < b3 && b3 < b4) {
        if (task_index == 3) {
            // 计算周期
            current_time = HAL_GetTick();
            float period = (current_time - last_time) / 1000.0f; // 转换为秒
            last_time = current_time;
            task3_result = 2 * period;
            if (counter.led_ms == 0 && task3_result > 0.5) {
                uart_printf("result:%.2f\r\n", task3_result);
            }
            if (task3_result > 0.5) {
                counter.led_ms = 100;
            }
        }
    }

}