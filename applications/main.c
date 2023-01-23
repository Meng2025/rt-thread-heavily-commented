/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>


int main(void)
{
    
    
    struct rt_device_pin_status pin_control;
    pin_control.pin = GET_PIN(F, 14);
    
    struct rt_device_pin_mode pin_mode;
    
    pin_mode.pin  = GET_PIN(F, 14);
    pin_mode.mode = PIN_MODE_OUTPUT;
    
    //rt_pin_mode(pin_control.pin, PIN_MODE_OUTPUT);
    
    
    
    rt_device_t pin_dev = rt_device_find("pin");
    
    rt_device_control(pin_dev,RT_NULL,&pin_mode);
    
    rt_device_open(pin_dev,RT_DEVICE_FLAG_RDWR);
    
    
    while(1)
    {
        pin_control.status = PIN_LOW;
        rt_device_write(pin_dev, 0, &pin_control, sizeof(struct rt_device_pin_status));
        
        rt_thread_mdelay(200);
        
        pin_control.status = PIN_HIGH;
        rt_device_write(pin_dev, 0, &pin_control, sizeof(struct rt_device_pin_status));  
        rt_thread_mdelay(800);
    
    }
}
