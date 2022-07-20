#include "motor_can.h"

#include "stdlib.h"



int voltage1 = 0;
int voltage2 = 0;
int angle1 = 0;
int angle2 = 0;

/*-------------------------- CAN接收线程 *--------------------------*/

static rt_sem_t rx_sem = RT_NULL;     /* 用于接收消息的信号量 */

static rt_device_t can_dev = RT_NULL;            /* CAN 设备句柄 */

static rt_thread_t can_rx_thread;

/* 接收数据回调函数 */
static rt_err_t can_rx_call(rt_device_t dev, rt_size_t size)
{
    /* CAN 接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(rx_sem);

    return RT_EOK;
}

static void can_rx_thread_entry(void *parameter)
{
    struct rt_can_msg rxmsg = {0};

    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(can_dev, can_rx_call);

    while (1)
    {
        /* hdr 值为 - 1，表示直接从 uselist 链表读取数据 */
        rxmsg.hdr = -1;
        /* 阻塞等待接收信号量 */
        rt_sem_take(rx_sem, RT_WAITING_FOREVER);
        /* 从 CAN 读取一帧数据 */
        rt_device_read(can_dev, 0, &rxmsg, sizeof(rxmsg));
        
        /* 电机1 */
        if (rxmsg.id == 0x205)
        {
            angle1 = ((rxmsg.data[0]<<8)|rxmsg.data[1])*20000/8191 - 10000;
        }
        
        /* 电机2 */
        if (rxmsg.id == 0x206)
        {
            angle2 = ((rxmsg.data[0]<<8)|rxmsg.data[1])*20000/8191 - 10000;
        }
//        rt_kprintf("%5d %5d\n",angle1, angle2);

    }
}

/*---------------------- CAN发送线程 ---------------------*/
static rt_thread_t can_tx_thread = RT_NULL;

static void can_tx_thread_entry(void *parameter)
{
    struct rt_can_msg msg = {0};
    rt_size_t   size;               /* 接收发送状态 */
      
    /* 数据转换为二进制数据的临时存储位 */
    rt_uint16_t temp;   
    
    /* 速度帧设置 */
    msg.ide = RT_CAN_STDID;         /* 标准格式 */
    msg.rtr = RT_CAN_DTR;           /* 数据帧 */
    msg.len = 8;                    /* 数据长度为 8 */
    
    msg.id  = 0x1ff;
    
    while(1)
    {
        voltage1 = (angle2 - angle1);
        voltage2 = (angle1 - angle2);
        
        /* 电机1 */
        msg.data[0] = (voltage1>>8)&0xff;       /* 电压给定高8位 */
        msg.data[1] = voltage1&0xff;            /* 电压给定低8位 */

        /* 电机2 */
        msg.data[2] = (voltage2>>8)&0xff;       /* 电压给定高8位 */
        msg.data[3] = voltage2&0xff;            /* 电压给定低8位 */

        size = rt_device_write(can_dev, 0, &msg, sizeof(msg));
        
        if (size == 0)
        {
            rt_kprintf("can dev write data failed in left motor!\n");
        }

        rt_thread_mdelay(5);
    }
}

/*-------------------------- 初始化 --------------------------*/

int can_init(void)
{
    rt_err_t res;

    /* 查找 CAN 设备 */
    can_dev = rt_device_find(CAN_DEV_NAME);
    if (!can_dev)
    {
        rt_kprintf("find %s failed!\n", CAN_DEV_NAME);
        return RT_ERROR;
    }
    /* 以中断接收及发送方式打开 CAN 设备 */
    res = rt_device_open(can_dev, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);

    RT_ASSERT(res == RT_EOK);
    
    rx_sem = rt_sem_create("rx_sem", 0, RT_IPC_FLAG_FIFO);

    /* 创建数据接收线程 */
    can_rx_thread = rt_thread_create("can_rx", can_rx_thread_entry, RT_NULL, CAN_RX_STACK_SIZE, CAN_RX_PRIORITY, CAN_RX_TIMESLICE);
    if (can_rx_thread != RT_NULL)
    {
        rt_thread_startup(can_rx_thread);
    }
    else
    {
        rt_kprintf("create can_rx thread failed!\n");
    }

    /* 创建数据发送线程 */
    can_tx_thread = rt_thread_create("can_tx", can_tx_thread_entry, RT_NULL, CAN_TX_STACK_SIZE, CAN_TX_PRIORITY, CAN_TX_TIMESLICE);
    if (can_rx_thread != RT_NULL)
    {
        rt_thread_startup(can_tx_thread);
    }
    else
    {
        rt_kprintf("create can_tx thread failed!\n");
    }
    
    return 0;
}

/* 导出到 msh 命令列表中 */
//MSH_CMD_EXPORT(can_init, can device sample);
INIT_APP_EXPORT(can_init);




