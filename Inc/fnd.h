/*
 * fnd.h
 *
 * Created: 2025-03-06 오후 12:23:18
 *  Author: microsoft
 */ 


#ifndef FND_H_
#define FND_H_
#define  F_CPU 16000000UL  // 16MHZ
#include <avr/io.h>
#include <util/delay.h>  // _delay_ms _delay_us

#define FND_DATA_PORT  PORTC
#define FND_DATA_DDR   DDRC

#define FND_DIGIT_PORT PORTA
#define FND_DIGIT_DDR  DDRA

#define FND2_DIGIT_D1  0
#define FND2_DIGIT_D2  1
#define FND2_DIGIT_D3  2
#define FND2_DIGIT_D4  3

#define FND_DIGIT_D1  4
#define FND_DIGIT_D2  5
#define FND_DIGIT_D3  6
#define FND_DIGIT_D4  7


#endif /* FND_H_ */