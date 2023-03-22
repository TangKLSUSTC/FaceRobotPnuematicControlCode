#include "myconsole.h"
/*This file is based on usart initialization function outside 
	from STM32 Cube MX or ATK init file
	Default port is USART1 
*/
/** 
 * get the size of data in rb 
 */
rt_size_t rt_ringbuffer_data_len(struct rt_ringbuffer *rb)
{
    switch (rt_ringbuffer_status(rb))
    {
    case RT_RINGBUFFER_EMPTY:
        return 0;
    case RT_RINGBUFFER_FULL:
        return rb->buffer_size;
    case RT_RINGBUFFER_HALFFULL:
    default:
        if (rb->write_index > rb->read_index)
            return rb->write_index - rb->read_index;
        else
            return rb->buffer_size - (rb->read_index - rb->write_index);
    };
}

void rt_ringbuffer_init(struct rt_ringbuffer *rb,
                        rt_uint8_t           *pool,
                        rt_int16_t            size)
{
    RT_ASSERT(rb != RT_NULL);
    RT_ASSERT(size > 0);

    /* initialize read and write index */
    rb->read_mirror = rb->read_index = 0;
    rb->write_mirror = rb->write_index = 0;

    /* set buffer pool and size */
    rb->buffer_ptr = pool;
    rb->buffer_size = RT_ALIGN_DOWN(size, RT_ALIGN_SIZE);
}

/**
 * put a character into ring buffer
 */
rt_size_t rt_ringbuffer_putchar(struct rt_ringbuffer *rb, const rt_uint8_t ch)
{
    RT_ASSERT(rb != RT_NULL);

    /* whether has enough space */
    if (!rt_ringbuffer_space_len(rb))
        return 0;

    rb->buffer_ptr[rb->write_index] = ch;

    /* flip mirror */
    if (rb->write_index == rb->buffer_size-1)
    {
        rb->write_mirror = ~rb->write_mirror;
        rb->write_index = 0;
    }
    else
    {
        rb->write_index++;
    }

    return 1;
}
/**
 * get a character from a ringbuffer
 */
rt_size_t rt_ringbuffer_getchar(struct rt_ringbuffer *rb, rt_uint8_t *ch)
{
    RT_ASSERT(rb != RT_NULL);

    /* ringbuffer is empty */
    if (!rt_ringbuffer_data_len(rb))
        return 0;

    /* put character */
    *ch = rb->buffer_ptr[rb->read_index];

    if (rb->read_index == rb->buffer_size-1)
    {
        rb->read_mirror = ~rb->read_mirror;
        rb->read_index = 0;
    }
    else
    {
        rb->read_index++;
    }

    return 1;
}


#define UART_RX_BUF_LEN 200
rt_uint8_t uart_rx_buf[UART_RX_BUF_LEN] = {0};
struct rt_ringbuffer  uart_rxcb;         /* ����һ�� ringbuffer cb */
static struct rt_semaphore shell_rx_sem; /* ����һ����̬�ź��� */
UART_HandleTypeDef UART1_Handler;
#define RXBUFFERSIZE   1
uint8_t aRxBuffer[RXBUFFERSIZE];

void my_console_init(void)
{
	rt_ringbuffer_init(&uart_rxcb, uart_rx_buf, UART_RX_BUF_LEN);
	rt_sem_init(&(shell_rx_sem), "shell_rx", 0, 0);
//	__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
//	HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE);

}
INIT_BOARD_EXPORT(my_console_init);

void rt_hw_console_output(const char *str)
{
	rt_size_t i=0,size=0;
	__HAL_UNLOCK(&UART1_Handler);
	size=rt_strlen(str);
	for(i=0;i<size;i++)
	{
		while((USART1->ISR&0X40)==0);
		if(*(str+i)=='\n')
		{
			USART1->TDR='\r';
			while((USART1->ISR&0X40)==0);
		}
		USART1->TDR=*(str+i);
	}
}
/* ��ֲ FinSH��ʵ�������н���, ��Ҫ��� FinSH Դ�룬Ȼ���ٶԽ� rt_hw_console_getchar */
/* �жϷ�ʽ */
char rt_hw_console_getchar(void)
{
    char ch = 0;

    /* �� ringbuffer ���ó����� */
    while (rt_ringbuffer_getchar(&uart_rxcb, (rt_uint8_t *)&ch) != 1)
    {
        rt_sem_take(&shell_rx_sem, RT_WAITING_FOREVER);
    } 
    return ch;   
}


//void usart_init(uint32_t bound)
//{	
//	//UART ��ʼ������

//	UART1_Handler.Instance=USART1;					    //USART1
//	UART1_Handler.Init.BaudRate=bound;				    //������
//	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
//	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
//	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
//	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
//	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
//	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()��ʹ��UART1
//	
//	HAL_UART_Receive_IT(&UART1_Handler, (uint8_t *)aRxBuffer, RXBUFFERSIZE);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
//  
//}







void USART1_IRQHandler(void)
{
    int ch = -1;
    rt_base_t level;
    /* enter interrupt */
    rt_interrupt_enter();          //���ж���һ��Ҫ������Ժ����������ж�

    if ((__HAL_UART_GET_FLAG(&(UART1_Handler), UART_FLAG_RXNE) != RESET) &&
        (__HAL_UART_GET_IT_SOURCE(&(UART1_Handler), UART_IT_RXNE) != RESET))
    {
        while (1)
        {
            ch = -1;
            if (__HAL_UART_GET_FLAG(&(UART1_Handler), UART_FLAG_RXNE) != RESET)
            {
                ch =  USART1->RDR & 0xff;
            }
            if (ch == -1)
            {
                break;
            }  
            /* ��ȡ�����ݣ������ݴ��� ringbuffer */
            rt_ringbuffer_putchar(&uart_rxcb, ch);
        }        
        rt_sem_release(&shell_rx_sem);
    }

    /* leave interrupt */
    rt_interrupt_leave();    //���ж���һ��Ҫ������Ժ������뿪�ж�
}
