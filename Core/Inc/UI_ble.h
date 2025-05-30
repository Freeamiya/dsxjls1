//
// Created by Free on 2025/5/23.
//

#ifndef XJLS_UI_BLE_H
#define XJLS_UI_BLE_H
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "tasks.h"
typedef enum {
    VAR_INT,
    VAR_FLOAT
} VarType;

typedef struct {
    const char *name;
    void *ptr;
    VarType type;
} VariableEntry;

// 声明实际变量
extern float speed;
extern VariableEntry variable_table[];
extern const int variable_count;

#endif //XJLS_UI_BLE_H
