#ifndef __DATA_H__
#define __DATA_H__

#include <rtthread.h>

/* 大疆电机数据类型 */

#include "pid.h"

typedef struct {
    rt_int16_t  angle;
    rt_int16_t  speed;
    rt_int16_t  torque;
    rt_int16_t  voltage;
    pid_t       pid;
} motor_data_t;

extern motor_data_t motor1, motor2, motor3, motor4;








#endif



