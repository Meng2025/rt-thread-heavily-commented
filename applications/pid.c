#include "pid.h"


void abs_limit(float *a, float ABS_MAX)
{
  if (*a > ABS_MAX)
    *a = ABS_MAX;
  if (*a < -ABS_MAX)
    *a = -ABS_MAX;
}

static void pid_param_set(
    pid_t*      pid,
    rt_int16_t  maxout,
    rt_int16_t  intergral_limit,
    rt_int16_t  
)
{

  pid->integral_limit = intergral_limit;
  pid->max_out        = maxout;
}

/**
  * @brief     modify pid parameter when code running
  * @param[in] pid: control pid struct
  * @param[in] p/i/d: pid parameter
  * @retval    none
  */
static void pid_set(pid_t *pid, float p, float i, float d)
{
  pid->kp = p;
  pid->ki = i;
  pid->kd = d;
  
  pid->pout = 0;
  pid->iout = 0;
  pid->dout = 0;
  pid->out  = 0;
  
}


float pid_control(pid_t *pid, float get, float set)
{
  pid->fbk = get;
  pid->ref = set;
  pid->err[NOW] = set - get;

  if ((pid->input_max_err != 0) && (fabs(pid->err[NOW]) > pid->input_max_err))
      return 0;

  if (pid->pid_mode == POSITION_PID) //position PID
  {
      pid->pout = pid->kp * pid->err[NOW];
      pid->iout += pid->ki * pid->err[NOW];
      pid->dout = pid->kd * (pid->err[NOW] - pid->err[LAST]);
    
      abs_limit(&(pid->iout), pid->integral_limit);
      pid->out = pid->pout + pid->iout + pid->dout;
      abs_limit(&(pid->out), pid->max_out);
  }
  else if (pid->pid_mode == DELTA_PID) //delta PID
  {
      pid->pout = pid->kp * (pid->err[NOW] - pid->err[LAST]);
      pid->iout = pid->ki * pid->err[NOW];
      pid->dout = pid->kd * (pid->err[NOW] - 2 * pid->err[LAST] + pid->err[LLAST]);

      pid->out += pid->pout + pid->iout + pid->dout;
      abs_limit(&(pid->out), pid->max_out);
  }

  pid->err[LLAST] = pid->err[LAST];
  pid->err[LAST]  = pid->err[NOW];
  
  
  if ((pid->output_deadband != 0) && (fabs(pid->out) < pid->output_deadband))
    return 0;
  else
    return pid->out;

}
/**
  * @brief     initialize pid parameter
  * @retval    none
  */
void PID_struct_init(
    pid_t*   pid,
    rt_uint32_t mode,
    rt_uint32_t maxout,
    rt_uint32_t intergral_limit,

    float kp,
    float ki,
    float kd)
{
  pid->param_init = pid_param_init;//将此函数指针赋给结构体中的函数指针
  pid->f_pid_reset  = pid_reset;

  pid->param_init(pid, mode, maxout, intergral_limit, kp, ki, kd);
  pid->f_pid_reset(pid, kp, ki, kd);
}




