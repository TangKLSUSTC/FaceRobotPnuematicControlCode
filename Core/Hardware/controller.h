#pragma once
#include "sys.h"
#include "rtthread.h"
#include <stdio.h>
#include <stdlib.h>

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
				/*Extern */
extern rt_sem_t adc_sig;


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
				/*Functions prototypes*/
void ctrl_thread_init();
void Muscles(void* param);

void Muscle_channel_init();
static uint8_t get_msta(uint8_t k);
static void write_msta(uint8_t k, uint8_t bit);
static char BBcontrol(int dest_val,int threshold,int val_now);
static rt_bool_t chn_assert(uint8_t chn);
static uint8_t get_use(uint8_t k);
static void write_use(uint8_t k,uint8_t bit);


static uint16_t pressure_read(uint8_t channel);
static void pressure_set(uint8_t channel,uint16_t value);
static void threshold_adj(uint8_t channel,uint16_t value);
static void prec_help();

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/