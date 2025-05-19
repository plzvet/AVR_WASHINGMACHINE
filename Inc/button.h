/*
 * button.h
 *
 * Created: 2025-03-05 오후 2:30:04
 *  Author: microsoft
 */ 


#ifndef BUTTON_H_
#define BUTTON_H_
#define  F_CPU 16000000UL  // 16MHZ
#include <avr/io.h>  // PORTA PORTB등의 I/O 관련 register등이 있다. 
#include <util/delay.h>  // _delay_ms _delay_us

#define LED_DDR DDRA   // 이렇게 하는 이유: LED PORT가 변경 되면
                       // #define만 바꿔주면 compiler가 변경 
#define LED_PORT PORTA 
#define BUTTON_DDR DDRD 
#define BUTTON_PIN PIND    // PIND는 PORTD를 읽는 register 5v:1  0v:0 

#define BUTTON0PIN  4    // PORTD.3
#define BUTTON1PIN  5    // PORTD.4
#define BUTTON2PIN  6    // PORTD.5
#define BUTTON3PIN  7    // PORTD.6

#define BUTTON0   0   // PORTD.3의 가상의 index(S/W 번호)
#define BUTTON1   1   // PORTD.4의 가상의 index(S/W 번호)
#define BUTTON2   2   // PORTD.5의 가상의 index(S/W 번호)
#define BUTTON3   3   // PORTD.6의 가상의 index(S/W 번호)
#define BUTTON_NUMER 4   // button갯수 

#define BUTTON_PRESS  1   // 버튼을 누르면 high(active-high)
#define BUTTON_RELEASE 0  // 버튼을 떼면 low
#endif /* BUTTON_H_ */