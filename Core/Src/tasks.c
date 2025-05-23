//
// Created by EiveLL on 25-5-23.
//

#include "tasks.h"
int task2_flag = 0;
uint16_t dis_buf[10] = {0};
void task1() {
    uart_printf("System initialization successful!\r\n");
}

void task2() {
    static int sum = 0;
    tof_distance = readRangeContinuousMillimeters(&distanceStr);
    for(int i = 0; i < 9; i++) {
        sum += dis_buf[i];
        dis_buf[i+1] = dis_buf[i];
    }
    dis_buf[0] = tof_distance;
    uart_printf("Distance: %d mm\r\n", dis_buf[0]);
//    if(dis_buf[0] > sum/9 && task2_flag == 0){
////        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
//        uart_printf("Distance: %d mm\r\n", dis_buf[0]);
//        task2_flag = 1;
//    }else if (dis_buf[0] < sum/9 && task2_flag == 1){
//        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
//        task2_flag = 0;
//    }
}

void task3() {

}

void task4() {

}

void task5() {

}

void task6() {

}
