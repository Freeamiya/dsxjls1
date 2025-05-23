//
// Created by Free on 2025/5/23.
//
#include "UI_ble.h"

uint8_t task_running = 0;
uint8_t task_index = 6;
float speed = 0.0f;
int flag = 0;
VariableEntry variable_table[] = {
        {"index", &task_index, VAR_INT},
        {"speed", &speed, VAR_FLOAT},
        {"flag", &flag, VAR_INT}
};
const int variable_count = sizeof(variable_table) / sizeof(variable_table[0]);