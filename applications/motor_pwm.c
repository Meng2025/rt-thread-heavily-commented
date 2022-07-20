#include <stdlib.h>
#include "motor_pwm.h"
#include "dr16.h"



/*----------------------   pwm周期占空比初始化配置   --------------------------*/
int motor_pwm_init(void)
{
    rt_err_t result = RT_EOK;

    struct rt_device_pwm *pwm_dev;
    
    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
    
    result = rt_pwm_set(pwm_dev, PWM_CH_L, 20000000, 1500000);
    
    if (result != RT_EOK)
        rt_kprintf("%s channel %d config error.\n", PWM_DEV_NAME, PWM_CH_L);
    
    result = rt_pwm_set(pwm_dev, PWM_CH_R, 20000000, 1500000);
  
    if (result != RT_EOK)
        rt_kprintf("%s channel %d config error.\n", PWM_DEV_NAME, PWM_CH_R);
    
    result = rt_pwm_enable(pwm_dev, PWM_CH_L);
    result = rt_pwm_enable(pwm_dev, PWM_CH_R);
    
    if (result != RT_EOK)
        rt_kprintf("%s channel enable error.\n", PWM_DEV_NAME);
    return 0;
}

INIT_APP_EXPORT(motor_pwm_init);


/*----------------------   pwm电机控制线程   --------------------------*/
/* pwm电机控制线程 */
#define MOTOR_PWM_PRIORITY         25
#define MOTOR_PWM_STACK_SIZE       1024
#define MOTOR_PWM_TIMESLICE        5

static rt_thread_t motor_pwm_thread = RT_NULL;


int pwm_duty = 1500;


/* 线程 1 的入口函数 */
static void motor_pwm_thread_entry(void *parameter)
{
    int ch2;
    rt_int16_t dr16_right_key = 1;
	
    struct rt_device_pwm *pwm_dev;      /* PWM设备句柄 */
    
    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
    
    while (1)
    {
        /* 读公共内存，写的话用互斥量保护起来 */
        ch2 = dr16.rc.ch2;
				dr16_right_key  = dr16.rc.s2;
			
				if (dr16_right_key == 1)
				{
					/* 速度控制模式 */
					ch2 = (1024 - ch2) * 3 /660; /* 摇杆量转占空比 */
					pwm_duty = (pwm_duty + ch2);
					rt_pwm_set(pwm_dev, 4, 20000000, pwm_duty*1000);
				}
				else
				{
					/* 位置指向模式 */
					ch2 = (1024 - ch2) * 1000 /660; /* 摇杆量转占空比 */
					ch2 = (1500 + ch2)*1000;
					rt_pwm_set(pwm_dev, 4, 20000000, ch2);   			
				}

        rt_thread_mdelay(20);
    }
}

int motor_thread_init(void)
{
    /* 创建线程 1，名称是 thread1，入口是 thread1_entry*/
    motor_pwm_thread = rt_thread_create("m_pwm",
                            motor_pwm_thread_entry, 
                            RT_NULL,
                            MOTOR_PWM_STACK_SIZE,
                            MOTOR_PWM_PRIORITY, 
                            MOTOR_PWM_TIMESLICE);

    /* 如果获得线程控制块，启动这个线程 */
    if (motor_pwm_thread != RT_NULL)
        rt_thread_startup(motor_pwm_thread);

    return 0;
}

/* 导出到 msh 命令列表中 */
//MSH_CMD_EXPORT(motor_thread_init, motor_thread_init);
INIT_APP_EXPORT(motor_thread_init);





