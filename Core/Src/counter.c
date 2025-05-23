//
// Created by EiveLL on 25-5-23.
//

#include "counter.h"

#include <stdbool.h>
Counter counter = {
    .led_ms = 0,
};

Flag flag = {
    .task2_flag = false,
    .task3_flag = false,
    .task4_flag = false,
    .task5_flag = false,
    .task6_flag = false,
};