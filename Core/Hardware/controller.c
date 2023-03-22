#include "controller.h"
#include "sys.h"
#include <main.h>
#include <stdlib.h>
#include "IO_def.h"

#define ctrl_Priority 5
#define ctrl_Stack_Size 1500
#define ctrl_tick 20


rt_sem_t adc_sig = RT_NULL;


static rt_thread_t ctrl_tcb = RT_NULL;
char mcv_sta=0;
static uint16_t use=0;
static uint16_t mussta=0;
uint32_t air_initial[12]={2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048};
uint8_t air_threshold[12]={200,200,200,200,200,200,200,200,200,200,200,200};
//static char use_of_muscle[12]={0,0,0,0,0,0,0,0,0,0,0,0};
/**/
#if defined(STM32F767xx)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
		/*IO port define*/

const io_base muscle_in[12]=\
	{
		{PC,Pin7},
		{PC,Pin8},
		{PH,Pin2},
		{PH,Pin3},
		{PH,Pin4},
		{PH,Pin5},
		{PH,Pin14},
		{PH,Pin15},
		{PH,Pin8},
		{PH,Pin9},
		{PH,Pin10},
		{PH,Pin11}
	};

const io_base muscle_out[12]=\
	{
		{PE,Pin5},
		{PE,Pin6},
		{PE,Pin3},
		{PE,Pin4},
		{PI,Pin7},
		{PE,Pin2},
		{PI,Pin5},
		{PI,Pin6},
		{PB,Pin9},
		{PI,Pin4},
		{PB,Pin5},
		{PB,Pin7}
	};
#endif

//Initialize control thread
void ctrl_thread_init()
{
	ctrl_tcb = rt_thread_create("Muscle_Control",
												Muscles,
												RT_NULL,
												ctrl_Stack_Size,
												ctrl_Priority,
												ctrl_tick);
	if(ctrl_tcb != RT_NULL)
		{
//			rt_kprintf("Control thread is created\n");
			io_group_init(muscle_in,12);
			io_group_init(muscle_out,12);
			rt_sem_init(adc_sig,"adc_ready",0,RT_IPC_FLAG_FIFO);
			return;
		}
}

//Start muscles channel control thread
void muscles_start()
{
	rt_uint8_t stat;
	rt_err_t uwret;
	stat=ctrl_tcb->stat&RT_THREAD_STAT_MASK;
	if((stat==RT_THREAD_READY)||(stat==RT_THREAD_RUNNING)||(stat==RT_THREAD_SUSPEND))
	{
		rt_kprintf("Muscle Control thread is Created!\n");
	}
	else if(stat==RT_THREAD_INIT)
	{
		uwret=rt_thread_startup(ctrl_tcb);
		if(uwret!=RT_EOK)rt_kprintf("Muscle Control thread error\n");
	}
	
}
MSH_CMD_EXPORT_ALIAS(muscles_start,muscles_start,muscle_start);

void muscles_suspend()
{
	rt_uint8_t stat;
	rt_err_t uwret;
	stat=ctrl_tcb->stat&RT_THREAD_STAT_MASK;
	if(stat!=RT_THREAD_SUSPEND)
	{
		uwret=rt_thread_suspend(ctrl_tcb);
		if(uwret!=RT_EOK)rt_kprintf("Muscle Control suspend error\n");
	}
	else if(stat==RT_THREAD_INIT)
	{
		rt_kprintf("Muscle Control thread is not started\n");
	}
}
MSH_CMD_EXPORT_ALIAS(muscles_suspend,muscles_suspend,suspend muscle control);

void muscles_resume()
{
	rt_uint8_t stat;
	rt_err_t uwret;
	stat=ctrl_tcb->stat&RT_THREAD_STAT_MASK;
	if(stat!=RT_THREAD_SUSPEND)
	{
		uwret=rt_thread_suspend(ctrl_tcb);
		if(uwret!=RT_EOK)rt_kprintf("Muscle Control resume error\n");
	}
	else if(stat==RT_THREAD_INIT)
	{
		rt_kprintf("The thread is not started\n");
	}
}
MSH_CMD_EXPORT_ALIAS(muscles_resume,muscles_resume,resume muscle control);

void Muscles(void* param)
{
	//肌肉控制源码

	char mcv[12]={0,0,0,0,0,0,0,0,0,0,0,0};
	uint32_t tmp=0;
	rt_kprintf("Muscle control thread start\n");
	while(1)
	{
		if(rt_sem_take(adc_sig,RT_WAITING_FOREVER)!=RT_EOK)
		{
			rt_thread_yield();
		}
		BOARD_LEDEX;
		for(uint8_t i=0;i<12;i++)
		{
			if(get_use(i)==1)
				{
					switch(BBcontrol(air_initial[i],air_threshold[i],adc_buffer[i]))
					{
						case 0:
										{
											mcv[i]++;
											io_control(&muscle_in[i],0);
											io_control(&muscle_out[i],0);
											if(mcv[i]>=100)
											{
												/*	Muscle is stable		*/
												write_msta(i,1);
												mcv[i]=0;
											}
											break;
										}		//No action
						case 1:
										{
											io_control(&muscle_in[i],1);
											io_control(&muscle_out[i],0);
											write_msta(i,0);
											mcv[i]=0;
											break;
										}				//Air in
						case 2:
										{
											io_control(&muscle_in[i],0);
											io_control(&muscle_out[i],1);
											write_msta(i,0);
											mcv[i]=0;
											break;
										}				//Air out
						default:{break;}
					}
				}
//				else if(get_use(i)==0)
//				{
//					io_control(&muscle_in[i],0);
//					io_control(&muscle_out[i],1);
//				}
		}
		if((mussta&0x0fff)==use&&use!=0)
		{
			rt_kprintf("MOK\n");
			mussta&=0x0000;
		}
		rt_thread_mdelay(50);

	}
}


//void Muscle_channel_init()
//{
//	io_group_init(muscle_in,12);
//	io_group_init(muscle_out,12);
//	for(char i=0;i<12;i++)
//	{
//		io_control(&muscle_in[i],0);
//		io_control(&muscle_out[i],0);
//	}
//	
//}
//INIT_BOARD_EXPORT(Muscle_channel_init);

static inline char BBcontrol(int dest_val,int threshold,int val_now)
{
		int diff=dest_val-threshold,sum=dest_val+threshold;
		if((diff<0)||(sum>4095))
		{
			return 0;
		}
		if((diff>0)&&(sum<4095))
		{
			if((val_now>=diff)&&(val_now<=sum))return 0;									//位于比较区间内
			if(val_now<diff)return 1;																			//低于下界
			if(val_now>sum)return 2;																			//高于上界
		}
}

static inline uint8_t get_msta(uint8_t k)
{
	return (mussta>>k)&0x1;
}
static inline void write_msta(uint8_t k, uint8_t bit)
{
	switch(bit)
	{
		case 0:{mussta&=0<<k;break;return;}
		case 1:{mussta|=1<<k;break;return;}
		default:{return;}
	}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	/*Setchn cmd functions*/

static rt_bool_t chn_assert(uint8_t chn)
{
	if((chn>=0)&&(chn<=11))return 1;
	else return 0;
}

//Set channel number to use
static void set_channel(int argc, char** argv)
{
	if(!rt_strcmp(argv[1],"all"))
	{
		if(!rt_strcmp(argv[2],"on"))
		{
			use=0xfff;
			rt_kprintf("ok\n");
			return;
		}
		if(!rt_strcmp(argv[2],"off"))
		{
			use=0;
			rt_kprintf("ok\n");
			return;
		}
		else
		{
			rt_kprintf("Parameter error\n");return;
		}
	}
	else if(!rt_strcmp(argv[1],"ra"))
	{
		for(char i=0;i<12;i++)
			{
//				rt_kprintf("Channel %d = %d\n",i,use_of_muscle[i]);
//				rt_kprintf("Channel %d = %d\n",i,(use>>i)&0x01);
				rt_kprintf("Channel %d = %d\n",i,get_use(i));
			}
		return;
	}
	else if(rt_strstr("01234567891011",argv[1]))
	{
		for(char i=1;i<argc;i++)
			{
//				use_of_muscle[atoi(argv[i])]=1;
//				use|=1<<atoi(argv[i]);
				write_use(atoi(argv[i]),1);
			}
		rt_kprintf("ok\n");
		return;
	}
	else
	{
		rt_kprintf("Option error\n");
	}
}
MSH_CMD_EXPORT_ALIAS(set_channel,setchn,setchn <[num] | all <on|off> | ra>);

//read kth bit in use
static inline uint8_t get_use(uint8_t k)
{
	return (use>>k)&0x1;
}

static inline void write_use(uint8_t k,uint8_t bit)
{
	switch(bit)
	{
		case 0:{use&=0<<k;break;return;}
		case 1:{use|=1<<k;break;return;}
		default:{return;}
	}
}



/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	/*Prec cmd static functions*/


//读取通道气压值
static inline uint16_t pressure_read(uint8_t channel)
{
	if((channel>=0)&&(channel<12))
	{
		return (uint16_t)adc_buffer[channel];
	}
	else
	{
		rt_kprintf("Channel no. error\n");
	}
}

//写入通道气压中心值
static inline void pressure_set(uint8_t channel,uint16_t value)
{
	if((channel>=0)&&(channel<12))
	{
		if(value<4096)
		{
			air_initial[channel]=(uint32_t)value;
			rt_kprintf("OK\n");
		}
		else
		{
			rt_kprintf("Pressure value error\n");
		}
	}
	else
	{
		rt_kprintf("Channel no. error\n");
	}
}


//设置通道气压控制门限
static void threshold_adj(uint8_t channel,uint16_t value)
{
	if((channel>=0)&&(channel<=12))
	{
		if(value<2048)
		{
			air_threshold[channel]=(uint32_t)value;
			rt_kprintf("OK\n");
		}
		else
		{
			rt_kprintf("Threshold value error\n");
		}
	}
	else
	{
		rt_kprintf("Channel no. error\n");
	}
}


static void pressure_control(int argc, char** argv)
{
	uint8_t channel;
	uint16_t value;
	uint8_t pass_flag=1;		//参数检查标志位
	if(argc<2)
	{
		rt_kprintf("Please input correct option or parameter\n");
		return;
	}
	if(!rt_strcmp(argv[1],"sw"))			//单通道写入		prec sw [chn] [val]
	{
		if(argc==4)
		{
			channel=(uint8_t)atoi(argv[2]);
			value=(uint16_t)atoi(argv[3]);
			pressure_set(channel,value);
			rt_kprintf("Chn %d, pres %d\n",channel,air_initial[channel]);
			rt_kprintf("OK\n");
		}
		else
		{
			rt_kprintf("Need parameters\n");return;
		}
	}
	if(!rt_strcmp(argv[1],"r"))				//单通道读取		prec r [chn]
	{
		if(argc==3)
		{
			channel=(uint8_t)atoi(argv[2]);
			value=pressure_read(channel);
			rt_kprintf("%d\n",value);
		}
		else
		{
			rt_kprintf("Parameter nubmer error\n");return;
		}
	}
	if(!rt_strcmp(argv[1],"adj"))			//调整门限值		prec adj [chn] [val]
	{
		if(argc==4)
		{
			channel=(uint8_t)atoi(argv[2]);
			value=(uint16_t)atoi(argv[3]);
			threshold_adj(channel,value);
			rt_kprintf("Chn %d, threshold %d\n",channel,air_threshold[channel]);
			rt_kprintf("OK\n");
		}
		else
		{
			rt_kprintf("Need parameters\n");return;
		}
	}
	if(!rt_strcmp(argv[1],"w"))			//写入气压中心值			prec w [chn] [value] [chn] [value] ....
	{
		if(argc > 4)			//大于一个通道
		{
			if(argc%2==0)			//偶数个参数
			{
				for(uint8_t i=1;i<(argc/2);i++)
				{
					channel=atoi(argv[2*i]);
					value=atoi(argv[2*i+1]);
					if( (  (value==0)  &&  rt_strlen(argv[2*i+1])>1)   \
						&& ( (channel==0)  &&    rt_strlen(argv[2*i])>1) \
						&&	(channel>=0&&channel<12)\
						&&	 (value<0&&value>4095))  				//检查输入错误
					{
						rt_kprintf("Parameter error\n");pass_flag=0;
					}
				}
				if(pass_flag==1)
				{
					for(uint8_t i=1;i<(argc/2);i++)
					{
						channel=atoi(argv[2*i]);
						value=atoi(argv[2*i+1]);
						pressure_set(channel,value);
					}
					rt_kprintf("OK\n");return;
				}
			}
			else
			{
				rt_kprintf("Please check number of parameters\n");return;
			}
		}
		else if(argc==4)
		{
			channel=(uint8_t)atoi(argv[2]);
			value=(uint16_t)atoi(argv[3]);
			pressure_set(channel,value);
			rt_kprintf("Chn %d, pres %d\n",channel,air_initial[channel]);
			rt_kprintf("OK\n");
		}
		return;
	}
	if(!rt_strcmp(argv[1],"ra"))				//读取所有通道
	{
		if(argc!=2)
		{
			if(argc==3)					//判断读取设置值还是ADC采样值
			{
				if(!rt_strcmp(argv[2],"set"))				//设置值
				{
					rt_kprintf("set value:\n");
					for(uint8_t i=0;i<12;i++)
					{
						rt_kprintf("ch%d=%d\n",i,air_initial[i]);
					}
				}
				else if(!rt_strcmp(argv[2],"adc"))
				{
					rt_kprintf("adc value:\n");
					for(uint8_t i=0;i<12;i++)
					{
						rt_kprintf("ch%d=%d\n",i,adc_buffer[i]);
					}
				}
				else if(!rt_strcmp(argv[2],"adj"))
				{
					rt_kprintf("threshold value:\n");
					for(uint8_t i=0;i<12;i++)
					{
						rt_kprintf("ch%d=%d\n",i,air_threshold[i]);
					}
				}
				else
				{
					rt_kprintf("Please input 'prec ra <set|adc>'\n");return;
				}
			}
		}
		else					//缺省输入：读取设置值
		{
			rt_kprintf("set value:\n");
			for(uint8_t i=0;i<12;i++)
			{
				rt_kprintf("ch%d=%d\n",i,air_initial[i]);
			}
			return;
		}
	}
	if(!rt_strcmp(argv[1],"?"))					//帮助
	{
		prec_help();return;
	}
	
	
	
}
MSH_CMD_EXPORT_ALIAS(pressure_control,prec,prec <w [value]>);


void prec_help(void)
{
	rt_kprintf("pressure control command:\n");
	rt_kprintf("Usage:\n");
	rt_kprintf("[sw channel value]				Write pressure value to single channel\n");
	rt_kprintf("[w channel value channel value...]  	Write pressure value to multiple channel\n");
	rt_kprintf("[r channel]			Read pressure value of single channel\n");
	rt_kprintf("[adj channel value]			Adjust central control threshold of a channel\n");
	rt_kprintf("[ra <set|adc|adj>]				Read all channel, return set value as default\n");
}


/*		BSP function api		*/
void __attribute__((weak)) soft_reset(void)
{
	rt_kprintf("BSP soft reset is undefined\n");
}
MSH_CMD_EXPORT_ALIAS(soft_reset,reset,software reset the system);

/*-------------------------------------------------------------------*/
const char NAME[]="Air Muscles Controller Slave Board";
const char main_version=1;
const char sub_version=41;
const char branch[]="Release";
void rt_show_version(void)
{
	rt_kprintf("%s\n",NAME);
	rt_kprintf("Version: %s %d.%d\n",branch,main_version,sub_version);
	rt_kprintf("Build: %s\n",__DATE__);
}






