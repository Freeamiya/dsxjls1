//
// Created by Free on 2025/5/23.
//
#include "UI_ble.h"

uint8_t task_running = 1;
uint8_t task_index = 2;
float speed = 0.0f;
VariableEntry variable_table[] = {
        {"index", &task_index, VAR_INT},
};
const int variable_count = sizeof(variable_table) / sizeof(variable_table[0]);