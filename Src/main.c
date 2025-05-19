/*
 * 01.LED_CONTROL.c
 *
 * Created: 2025-03-04 오후 4:25:34
 * Author : microsoft
 */ 
#include "Laundry.h"
#include "button.h"
#include "def.h"
#define F_CPU 16000000UL  // 16MHZ
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>  // _delay_ms _delay_us
#include <stdio.h>		 // printf scanf fgets puts gets 등이 들어있다.
// 타이머 함수선언
void init_timer0(void);
// 버튼 함수선언
extern void init_button(void);
extern int get_button(int button_num, int button_pin);
// 부저 함수선언
extern void Music_Player(int *tone, int *Beats);
extern void init_speaker(void);
extern void Beep(int repeat);
extern void Siren(int repeat);
extern void RRR(void);
extern void Laundry(int repeat);
// LED 함수선언
extern int led_main(void);
extern void led_wash(void);
extern void led_rinse(void);
extern void led_dry(void);   
// FND 함수선언
extern void init_fnd(void);
extern void fnd_display(void);
// UART 함수선언
extern void init_uart0(void);
extern void UART0_transmit(uint8_t data);
extern void init_uart1(void);
extern void bt_command_processing(void);
// 초음파센서 함수선언
extern void distance_ultrasonic(void);
extern void init_ultrasonic(void);
// PWM모터 함수선언
extern void make_pwm_led_control(void);
extern void pwm_fan_control_main(void);
extern void L298N_pwm_fan_control_main(void);
extern void init_L298N(void);
extern void init_timer3(void);
extern void motor_on(void);
extern void motor_off(void);
extern void motor_forward(void);
extern void motor_reverse(void);
// 세탁기 함수선언
void Laundry_main(int BUTTON_PRESSED);
void laundry_time(void);
void manual_mode(void);
void auto_mode(void);
void RUN(void);
void STOP(void);
extern void (*funcs[2])(void);
// 외부변수 extern 선언
extern const int Elise_Tune[];
extern const int Elise_Beats[];
extern volatile uint8_t rx_buff[COMMAND_NUMBER][COMMAND_LENGTH];	// uart0로부터 들어온 문자를 저장하는 버퍼(변수)
extern volatile uint8_t rx_msg_received;
// 전역변수 선언
volatile uint32_t msec_count=0;		// FND1을 위한 카운트변수
volatile uint8_t ultrasonic_check_timer=0;
volatile uint32_t sec_count=0;  // 초를 기록하는 count변수 unsigned int --> uint32_t
volatile uint8_t display_update=0;
volatile uint8_t current_mode = 2;    // 초기상태 init : 2 auto_mode_set : 0 manual_mode_set : 1 auto_mode_run : 3 manual_mode_run : 4
volatile uint8_t mode_select = 0;     // 자동모드 : 0 / 수동모드 : 1
volatile uint8_t Laundry_state = 0;   // 세탁 동작 : 1 / 중지 : 0
volatile uint8_t washtime_select = 0; // 세탁시간 : 0, 헹굼시간 : 1, 탈수시간 : 2
volatile uint32_t auto_total_washing_time = 0;
volatile uint32_t manual_total_washing_time = 0;

// 세탁 진행시 각 상황을 저장하는 전역변수
volatile uint32_t washtime = 0;
volatile uint32_t rinsetime = 0;
volatile uint32_t drytime = 0;
volatile uint32_t washtime_auto = 0;
volatile uint32_t rinsetime_auto = 0;
volatile uint32_t drytime_auto = 0;
volatile int stateselect = 0; // 초기동작은 멈춰있음.
// PE5 (OC1A) PWM 출력 사용.
// 16bit Timer/Counter
// OCR1A값이 같아지면 Low 출력.

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
WashingTime WashingTime_manual = { 0, 0, 0 };
WashingTime WashingTime_auto = { 10, 10, 10 };

ISR(TIMER0_OVF_vect)
{
	// 6~256 : 250(1ms) 그래서 TCNT0를 6으로 설정하는 것이다.
	TCNT0=6;
	msec_count++;	// 1ms마다 1씩 증가
	display_update++;
	//ultrasonic_check_timer++;
}

int main(void)
{
	init_button();
	init_fnd();
	init_timer0();
	init_timer3();
	init_uart0();
	init_uart1();
	init_ultrasonic();
	init_L298N();
	init_speaker();
	stdout = &OUTPUT;	// printf가 동작 될 수 있도록 stdout에 OUTPUT 파일 포인터 assign
	sei();	// 전역으로 interrupt 허용
	
	volatile static int BUTTON_PRESSED = 255; // 어떤 버튼도 눌리지 않은 초기상태
	//Beep(5);
	
	while(1) // 무한루프
	{
		if (msec_count >= 1000)   // 1000ms --> 1sec
		{
			msec_count=0;
			// 동작중일때 토탈 세탁시간 줄어들도록
			if(Laundry_state)
			{
				sec_count++;
				if(current_mode == auto_mode_run)
				{
					auto_total_washing_time--;
					if(!auto_total_washing_time)
					{
						Laundry(1);
						Laundry_state = 0;
						sec_count = 0;
					}
				}
				if(current_mode == manual_mode_run)
				{
					manual_total_washing_time--;	
					if(!manual_total_washing_time)
					{
						Laundry_state = 0;
						sec_count = 0;
						Laundry(1);
					}
				}
			}
		}
		
		if (get_button(BUTTON0, BUTTON0PIN)) BUTTON_PRESSED = BUTTON0;
		else if (get_button(BUTTON1, BUTTON1PIN)) BUTTON_PRESSED = BUTTON1;
		else if (get_button(BUTTON2, BUTTON2PIN)) BUTTON_PRESSED = BUTTON2;
		else if (get_button(BUTTON3, BUTTON3PIN)) BUTTON_PRESSED = BUTTON3;
		
		bt_command_processing();
		funcs[stateselect]();
		if(BUTTON_PRESSED != 255)
		{
			Laundry_main(BUTTON_PRESSED);				 // 버튼에 따른 상태천이가 결정이 되고
			BUTTON_PRESSED = 255;
		}
		// fnd Display 갱신
		if(display_update > 1)
		{
			display_update = 0;
			fnd_display();				 // 상태천이에 따른 FND display 출력되고
		}
		
		laundry_time();
		// 현재 모드에 따라 LED 제어와 모터 제어
		if (current_mode == manual_mode_run)
			manual_mode();
		if (current_mode == auto_mode_run)
			auto_mode();		
				
	}
	
	
	// fnd_main();
	/*
	init_speaker();	
		while(1)
	{

		OCR1A=1702;   // 삐~뽀 레 
		_delay_ms(1000);
		OCR1A=1431;   // 파  
		_delay_ms(1000);


		
		Beep(5);
		RRR();
 		_delay_ms(3000);
 		Siren(5);
 		_delay_ms(3000);
		Music_Player(Elise_Tune, Elise_Beats);		
	}
	*/
	// led_main();
	// make_pwm_led_control();
	// pwm_fan_control_main();
	// L298N_pwm_fan_control_main();
	// printf("init_uart0\n");
	/* while(1)
	{	
		//distance_ultrasonic();
	}
	*/

}



// timer0를 초기화 시키는 부분
// AVR에서 8bit timer 0/2번 중 0번을 초기화한다.
// 임베디드/FPGA 등에서 제일 중요한 것은 초기화를 정확히 해주는 것이다.
// 그래서 이부분을 특별히 신경을 써서 작성한다.
void init_timer0(void)
{
// 16MHz / 64 분주(down) 분주 : divider/prescale
// ------ 분주비 계산 ------
// (1) 16000000Hz / 64 ==> 250,000Hz	
// (2) T(주기) 1clock의 소요시간 : 1/f = 1/250,000 =  4us : 0.004ms
// (3) 8bit timer OV(OVflow) : 0.004ms * 256 = 0.001024sec = 1.024ms
// 1ms마다 정확하게 Interrupt를 띄우고 싶으면 0.004ms * 250번을 count
	TCNT0=6;	// TCNT : 0~256 1ms마다 TIMER0_OVF_vect로 진입한다.
				// TCNT0 = 6으로 설정을 한 이유 : 6-->256 : 250개의 펄스를 count하기 때문에
				// 정확히 1ms가 된다.
// (4) 분주비 설정 (250,000Hz --> 250KHz) P.296 표 13-1 참고
	TCCR0 |= 1 << CS02 | 0 << CS01 | 0 << CS00; // TCCR0 |= 0xf4; -> 가독성이 떨어짐.
// (5) Timer0 overflow Interrupt를 허용(enable)
	TIMSK |= 1 << TOIE0;
}

void laundry_time(void)
{
	washtime = WashingTime_manual.rinse_time+WashingTime_manual.dry_time;
	rinsetime = WashingTime_manual.wash_time+WashingTime_manual.dry_time;
	drytime = WashingTime_manual.dry_time;
	washtime_auto = WashingTime_auto.rinse_time+WashingTime_auto.dry_time;
	rinsetime_auto = WashingTime_auto.wash_time+WashingTime_auto.dry_time;
	drytime_auto = WashingTime_auto.dry_time;
}

void auto_mode(void)
{
	if (Laundry_state == 1)
	{  // 세탁 동작이 시작되었을 때
		if (auto_total_washing_time >= washtime_auto)
		{
			led_wash();  // 세탁 모드일 때 LED1 켜기
			motor_forward();//모터 정방향
		}
		else if (auto_total_washing_time <= washtime_auto && auto_total_washing_time >= drytime_auto)
		{
			led_rinse();  // 헹굼 모드일 때 LED2 켜기
			motor_reverse();//모터 역방향
		}
		else if (auto_total_washing_time <= drytime_auto)
		{
			led_dry();  // 탈수 모드일 때 LED3 켜기
			motor_forward();//모터 정방향
		}
	}
	else
	{
		PORTB = 0x00;  // 세탁이 끝나거나 중지되면 LED 모두 끄기
		motor_off();
	}
}

void manual_mode(void)
{
	if (Laundry_state == 1)
	{  // 세탁 동작이 시작되었을 때
		if (manual_total_washing_time >= washtime)
		{
			led_wash();  // 세탁 모드일 때 LED1 켜기
			motor_forward();//모터 정방향
		}
		else if (manual_total_washing_time <= washtime && manual_total_washing_time >= drytime)
		{
			led_rinse();  // 헹굼 모드일 때 LED2 켜기
			motor_reverse();//모터 역방향
		}
		else if (manual_total_washing_time <= drytime)
		{
			led_dry();  // 탈수 모드일 때 LED3 켜기
			motor_forward();//모터 정방향
		}
	}
	else
	{
		PORTB = 0x00;  // 세탁이 끝나거나 중지되면 LED 모두 끄기
		motor_off();
	}
}