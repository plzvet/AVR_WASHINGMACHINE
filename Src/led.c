/*
 * led.c
 *
 * Created: 2025-03-07 오후 2:23:10
 *  Author: microsoft
 */ 

#include "button.h"
#include "led.h"

int led_main(void); 
void led_all_on(void);
void led_all_off(void);
void state_transition(void);

extern void init_button(void);
extern int get_button(int button_num, int button_pin);

extern volatile uint32_t msec_count; // extern을 사용할 경우 초기값을 지정하면 중복 할당으로 오류 발생

int state=0;
#define delay_time 100
#define FUNC_NUM 2
int button_state[2]={0};
	
void (*fp[FUNC_NUM]) (void) =
{
	led_all_on,					//0
	led_all_off,				//1
};

int led_main(void)   // 정의 
{
	DDRA = 0xff;   // PORTA에 연결된 pin8개를 output mode로
	init_button();
	
	while(1)
	{
		if (get_button(BUTTON0, BUTTON0PIN))
		{
			button_state[0]=!button_state[0]; // button 0 toggle
		}
		if (get_button(BUTTON1, BUTTON1PIN))
		{
			button_state[1]++;
			button_state[1]%=FUNC_NUM;
		}
		fp[state]();
	} 
}

void led_all_on(void)
{
	PORTA = 0xff;
}

void led_all_off(void)
{
	PORTA = 0x00;
}

void state_transition(void)
{
	if(!button_state[0]) // 자동모드인 상태
	{
		state++;
		state%=FUNC_NUM;
	}
	else                 // 수동모드인 상태
	{
		state=button_state[1];
	}
}

