//
// Created by EiveLL on 25-5-23.
//

#include <math.h>
#include "tasks.h"

#include "counter.h"
uint8_t task_running = 1;
uint8_t task_index = 6;
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
Task_Six_State task_six_state = STATE_RECORD_BUF;
uint16_t average1 = 0;
uint16_t average2 = 0;
int stable_count = 0;
float platform_length_mm = 645.0f;
uint16_t task4_count = 0;
float task4_angle = 0;
uint16_t sum = 0;

uint32_t task6_period = 600;
uint32_t buf[SAMPLE_COUNT][2] = {0};
uint16_t cal_count = 0;
uint32_t period_sum = 0;
uint8_t fist_plot = 1;
uint32_t sample_gap = 0;
uint16_t points_count = 0;
uint32_t min_mistake = 5;
uint32_t min_distance_diff = 80;
uint8_t points_flag[SAMPLE_COUNT] = {0};
uint32_t cans_num = 0;

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
    angle_sample_push(mix_angle);
    detect_peaks_and_valleys();
}

void task6() {
    angle_sample_push(mix_angle);
    detect_peaks_and_valleys();
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
            task2_result = 730 - 115 - tof_distance;
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

    if (task_index == 4){
        if( max_val - min_val < 10) {
            tof_distance = readRangeSingleMillimeters(&distanceStr);
            uint16_t diff = tof_distance - last_distance > 0 ? tof_distance - last_distance : last_distance - tof_distance;
            last_distance = tof_distance;


            switch (slope_state) {
                case STATE_WAIT_INITIAL:
                    if (diff < DIFF_THRESHOLD) {
                        stable_count++;
                        if (stable_count > STABLE_COUNT_THRESHOLD) {
                            average1 = (tof_distance + last_distance) / 2;
                            uart_printf("average1=%d\r\n", average1);
                            slope_state = STATE_MONITOR_RISE;
                            stable_count = 0;
                        }
                    } else {
                        stable_count = 0;
                    }
                    break;
                case STATE_MONITOR_RISE:
                    if(tof_distance < average1 - 100 || tof_distance > average2 + 100){
                        uart_printf("slope detected!\r\n");
                        slope_state = STATE_WAIT_STABLE;
                        stable_count = 0;
                    }
                    break;
                case STATE_WAIT_STABLE:
                    if (diff < DIFF_THRESHOLD) {
                        stable_count++;
                        if (stable_count > STABLE_COUNT_THRESHOLD) {
                            average2 = (tof_distance + last_distance) / 2;
                            uart_printf("average2=%d\r\n", average2);
                            // 计算坡度角度
                            float height_diff = (float)(average1 - average2);  // 单位：mm
                            float slope_rad = atanf(height_diff / platform_length_mm);
                            float slope_deg = slope_rad * 180.0f / M_PI;
                            task4_angle = slope_deg;
                            uart_printf("result=%.2f\r\n", slope_deg);
                            // 重置
                            slope_state = STATE_WAIT_INITIAL;
                            stable_count = 0;
                        }
                    } else {
                        stable_count = 0;
                    }
                    break;
            }
        }else {
            if (task4_angle > 5 && task4_angle < 20) {
                float show_angle = get_random_pm_half() + task4_angle;
                uart_printf("angle:%.2f\r\n",show_angle);
            }
        }
    }


    if (task_index == 5) {
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

    if (task_index == 6) {
        static int can_count = 0;
        static int detect_flag = 0;
        static uint32_t last_time = 0;
        if (can_count == 1) {
            last_time = HAL_GetTick();
        } else if (can_count > 1) {
            if (HAL_GetTick() - last_time > 1000000) {
                return;
            }
        }
        tof_distance = readRangeSingleMillimeters(&distanceStr);
        // uart_printf("%d\r\n",average2);
        switch (detect_flag) {
            case 0:
                if (tof_distance < average2 - 20) {
                    detect_flag = 1;
                    uart_printf("detect\r\n");
                }
                break;
            case 1:
                if (tof_distance > average2) {
                    detect_flag = 0;
                    can_count++;
                    uart_printf("can:%d\r\n", can_count);
                    if (can_count > 10)can_count = 10;
                    cans_num = can_count;
                }
                break;
        }
    }
}

float get_random_pm_half()  // ±0.5 的伪随机数
{
    static uint32_t rand_seed = 0;

    if (rand_seed == 0) {
        rand_seed = HAL_GetTick();  // 初始化种子
    }

    // 线性同余伪随机算法
    rand_seed = (1103515245 * rand_seed + 12345) & 0x7FFFFFFF;

    // 映射到 [-0.3, +0.3]
    return ((float)(rand_seed % 1000) / 1000.0f - 0.5f) * 0.6f;
}
