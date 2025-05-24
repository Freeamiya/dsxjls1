//
// Created by EiveLL on 25-5-23.
//

#include <math.h>
#include "tasks.h"

#include "counter.h"
uint8_t task_running = 1;
uint8_t task_index = 4;
int task2_flag = 0;
uint16_t dis_buf[10] = {0};
float angle_sample[SAMPLE_SIZE];
uint16_t task2_result = 0;
float task3_result = 0;
uint32_t last_time = 0;
uint32_t current_time = 0;
uint16_t last_distance = 0;
SlopeState slope_state = STATE_WAIT_INITIAL;
MOVESlopeState move_state = STATE_WAIT;
uint16_t average1 = 0;
uint16_t average2 = 0;
int stable_count = 0;
float platform_length_mm = 645.0f;
uint16_t task4_count = 0;
uint16_t sum = 0;
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
    angle_sample_push(mix_angle);
    detect_peaks_and_valleys();
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

    float max_val = b0;
    float min_val = b0;

    float values[] = {b0, b1, b2, b3, b4};
    for (int i = 1; i < 5; i++) {
        if (values[i] > max_val) max_val = values[i];
        if (values[i] < min_val) min_val = values[i];
    }
    // 检测极大值（最低点）
    if ( b2 > 35 && b0 < b1 && b1 < b2 && b2 > b3 && b3 > b4) {
        if (task_index == 2) {
            tof_distance = readRangeSingleMillimeters(&distanceStr);
            task2_result = 720 - 115 - tof_distance;
            if (counter.led_ms == 0) {
                uart_printf("tof=%d,result:%d\r\n", tof_distance, task2_result);
            }
            counter.led_ms = 100;
        }
    }

    // 检测极小值（最高点）
    if (b2<10&&b0 > b1 && b1 > b2 && b2 < b3 && b3 < b4) {
        if (task_index == 3) {
            // 计算周期
            current_time = HAL_GetTick();
            float period = (float)(current_time - last_time) / 1000.0f; // 转换为秒
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

    // if(max_val - min_val < 10){
    //     if (task_index == 4) {
    //         tof_distance = readRangeSingleMillimeters(&distanceStr);
    //         uint16_t diff = tof_distance - last_distance > 0 ? tof_distance - last_distance : last_distance - tof_distance;
    //         last_distance = tof_distance;
    //
    //
    //         switch (slope_state) {
    //             case STATE_WAIT_INITIAL:
    //                 if (diff < DIFF_THRESHOLD) {
    //                     stable_count++;
    //                     if (stable_count > STABLE_COUNT_THRESHOLD) {
    //                         average1 = (tof_distance + last_distance) / 2;
    //                         uart_printf("average1=%d\r\n", average1);
    //                         slope_state = STATE_MONITOR_RISE;
    //                         stable_count = 0;
    //                     }
    //                 } else {
    //                     stable_count = 0;
    //                 }
    //                 break;
    //             case STATE_MONITOR_RISE:
    //                 if(tof_distance < average1 - 100 || tof_distance > average2 + 100){
    //                     uart_printf("slope detected!\r\n");
    //                     slope_state = STATE_WAIT_STABLE;
    //                     stable_count = 0;
    //                 }
    //                 break;
    //             case STATE_WAIT_STABLE:
    //                 if (diff < DIFF_THRESHOLD) {
    //                     stable_count++;
    //                     if (stable_count > STABLE_COUNT_THRESHOLD) {
    //                         average2 = (tof_distance + last_distance) / 2;
    //                         uart_printf("average2=%d\r\n", average2);
    //                         // 计算坡度角度
    //                         float height_diff = (float)(average1 - average2);  // 单位：mm
    //                         float slope_rad = atanf(height_diff / platform_length_mm);
    //                         float slope_deg = slope_rad * 180.0f / M_PI;
    //                         uart_printf("result=%.2f\r\n", slope_deg);
    //                         // 重置
    //                         slope_state = STATE_WAIT_INITIAL;
    //                         stable_count = 0;
    //                     }
    //                 } else {
    //                     stable_count = 0;
    //                 }
    //                 break;
    //         }
    //     }
    // }else {
    if (task_index == 4) {
        switch (move_state) {
            case STATE_WAIT:
                // 检测最高点 比较差值
                if (b2 < 10 && b0 > b1 && b1 > b2 && b2 < b3 && b3 < b4) {
                    tof_distance = readRangeSingleMillimeters(&distanceStr);
                    if (counter.led_ms == 0) {
                        uint16_t diff = tof_distance - last_distance > 0
                                            ? tof_distance - last_distance
                                            : last_distance - tof_distance;
                        last_distance = tof_distance;
                        if (diff < 40) {
                            task4_count++;
                        }
                        if (task4_count > 10) {
                            move_state = STATE_RECORD;
                            task4_count = 0;
                        }
                        uart_printf("task4_count=%d\r\n", task4_count);
                    }
                    counter.led_ms = 100;
                }
                break;
            case STATE_RECORD:
                // 检测最低点 记录
                if ( b2 > 35 && b0 < b1 && b1 < b2 && b2 > b3 && b3 > b4) {
                    tof_distance = readRangeSingleMillimeters(&distanceStr);
                    if (counter.led_ms == 0) {
                        uart_printf("%d\r\n",tof_distance);
                        uart_printf("move_state=%d\r\n", move_state);
                        uint16_t diff = tof_distance - last_distance > 0
                                        ? tof_distance - last_distance
                                        : last_distance - tof_distance;
                        last_distance = tof_distance;
                        if (diff < 50) {
                            sum += tof_distance;
                            task4_count++;
                        }
                        if (task4_count > 4) {
                            average1 = sum / 5;
                            task4_count = 0;
                            move_state = STATE_STABLE;
                        }
                    }
                    counter.led_ms = 100;
                }
                break;
            case STATE_STABLE:
                if ( b2 > 35 && b0 < b1 && b1 < b2 && b2 > b3 && b3 > b4) {
                    tof_distance = readRangeSingleMillimeters(&distanceStr);
                    if (counter.led_ms == 0) {
                        average2 = (tof_distance + last_distance)/2;
                        last_distance = tof_distance;
                        uart_printf("ave1=%d,ave2=%d\r\n", average1,average2);
                        // 计算坡度角度
                        float height_diff = (float)(average1 - average2);  // 单位：mm
                        float slope_rad = atanf(height_diff / platform_length_mm);
                        float slope_deg = slope_rad * 180.0f / M_PI;
                        uart_printf("result=%.2f\r\n", slope_deg);
                    }
                    counter.led_ms = 100;
                }
                break;
        }
    }
    // }
}