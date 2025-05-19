/*
 * Laundry.h
 *
 * Created: 2025-03-14 오후 2:51:01
 *  Author: microsoft
 */ 


#ifndef LAUNDRY_H_
#define LAUNDRY_H_

#define  F_CPU 16000000UL  // 16MHZ
#include <avr/io.h>
#include <util/delay.h>  // _delay_ms _delay_us
#include <avr/interrupt.h>
#include "button.h"

#define init_state		2
#define auto_mode_set	0		
#define auto_mode_run   3
#define manual_mode_set	1
#define manual_mode_run 4

#define run_state		1
#define stop_state		0

#define wash_sel		0		
#define rinse_sel		1
#define dry_sel			2
#define FUNC_NUM		2

typedef struct  
{
	int wash_time;
	int rinse_time;
	int dry_time;
} WashingTime;

#endif /* LAUNDRY_H_ */