#ifndef __PID_H__
#define __PID_H__


#include <rtthread.h>


//积分分离+积分限幅

enum
{
    LLAST = 0,
    LAST,
    NOW,
};

typedef struct pid
{
    /* PID控制系数 */
    float kp;
    float ki;
    float kd;

    /* 给定量，传感器反馈量 */
    float ref;
    float fbk;
    
    /* 控制中间量 */
    float err[3];
    float pout;
    float iout;
    float dout;
    float out;

    float input_max_err;    //input max err;
    float output_deadband;  //output deadband; 
  
    rt_int16_t max_out;            /* 输出限幅 */
    rt_int16_t integral_limit;     /* 积分限幅 */
    rt_int16_t beta;               /* 积分开关系数 */

} pid_t;


float pid_calc(pid_t *pid, float fdb, float ref);

#endif










