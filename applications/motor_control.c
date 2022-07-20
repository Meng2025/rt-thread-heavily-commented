#include "motor_control.h"

#include "data.h"



/*-------------------------- 力矩控制线程 --------------------------*/
static rt_thread_t torque_ctrl_thread = RT_NULL;

static void torque_ctrl_thread_entry(void *parameter)
{
    while(1)
    {
        
        
        
        rt_thread_mdelay(1);
    }
}

/*-------------------------- 转速控制线程 --------------------------*/
static rt_thread_t speed_ctrl_thread = RT_NULL;

static void speed_ctrl_thread_entry(void *parameter)
{

}

/*-------------------------- 位置控制线程 --------------------------*/
static rt_thread_t angle_ctrl_thread = RT_NULL;

static void angle_ctrl_thread_entry(void *parameter)
{

}



int control(int argc, char*argv[])
{
    torque_ctrl_thread = rt_thread_create("control", torque_ctrl_thread_entry, RT_NULL, 512, 10, 5);
    if (torque_ctrl_thread != RT_NULL)
    {
        rt_thread_startup(torque_ctrl_thread);
    }
    else
    {
        rt_kprintf("torque_ctrl_thread failed!\n");
    }    
    return 0;

}

MSH_CMD_EXPORT(control, 启动电机控制);

