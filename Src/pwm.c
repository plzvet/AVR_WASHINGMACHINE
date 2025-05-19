/*
 * pwm.c
 *
 * Created: 2025-03-13 오후 12:49:01
 *  Author: microsoft
 */ 

#include "pwm.h"

// 버튼관련 함수
extern void init_button(void);
extern int get_button(int button_num, int button_pin);
// PWM관련
void init_timer3(void);
void init_L298N(void);
void L298N_pwm_fan_control_main(void);
void pwm_fan_control_main(void);
void msec_count_dir(void);
// 외부변수선언
extern volatile uint32_t msec_count;
extern volatile uint32_t sec_count;  // 초를 재는 count변수 unsigned int --> uint32_t (정상상태의 초 세는 카운트)
volatile uint8_t forward = 1;	// forward <--> backward 초기값 : forward
volatile uint32_t fnd_speed_sec = 1000; // FND 출력 속도
/*
	16bit timer 3번 활용
	PWM 출력 신호(3EA)
	=================
	PE3  : OC3A
	PE4  : OC3B / INT4 ---> 사용중(초음파 echo 펄스 external INT용)
	PE5  : OC3C ---> 모터 연결
	BTN0 : start / stop
	BTN1 : speed-up   (OC3C : 20씩 증가 MAX 250)
	BTN2 : speed-down (OC3C : 20씩 감소 MIN 60)
*/
void init_timer3(void)
{
	DDRE |= 1 << 3 | 1 << 5;	// 현재 4번핀은 초음파 echo
	// 16 bit timer 3번의 모드 5 : 8비트 고속 PWM (P348 표 15-6)
	TCCR3A |= 1 << WGM30;
	TCCR3A |= 1 << WGM32;
	// 비반전 모드
	// 사용자가 설정한 PWM 값에 일치시 OC3C 의 출련핀이 LOW로 바뀌고 BOTTOM 에서 HIGH 로 바뀐다.
	// P350 표 15-7
	TCCR3A |= 1 << COM3C1;
	// 분주비 : 64 16000000 / 64 ---> 250000Hz(250Khz)
	// T=1/f = 1/250000Hz ==> 0.000004sec (4us)
	// 250000Hz 에서 256개의 펄스를 count하면 소요시간 1.02ms
	//				 127개							 0.5ms
	// P318 표 14-1
	TCCR3B |= 1 << CS31 | 1 << CS30;	// 분주비 : 64
	OCR3C = 0;	// OCR(Output Compare Register) : PWM 값
}

void pwm_fan_control_main(void)
{
	uint8_t start_button = 0;
	
	init_button();
	init_timer3();
	
	while(1)
	{
		if (get_button(BUTTON0, BUTTON0PIN))	// start/stop
		{
			start_button = !start_button; // toggle
			OCR3C = (start_button) ? 250 : 0; // 250 : 모터 회전의 기 본 값 0 : stop
		}	
		else if (get_button(BUTTON1, BUTTON1PIN))	  // speed-up
		{
			OCR3C = (OCR3C >= 250) ? 250 : OCR3C + 20;
		}
		else if (get_button(BUTTON2, BUTTON2PIN))
		{
			OCR3C = (OCR3C <= 70) ? 60 : OCR3C - 20;
		}		
	}
}

// PF6 : IN1 (L298N)
// PF7 : IN2 (L298N)
// PF6(IN1) PF7(IN2)
//	  0			0    : 역회전
//	  1			0    : 정회전
//	  1			1    : stop
void init_L298N(void)
{
	DDRF |= 1 << 6 | 1 << 7; // 출력 모드
	PORTF &= ~(1 << 6 | 1 << 7);
	PORTF |= 1 << 6; // 정회전
}
#if 0
// PE5 모터연결
void L298N_pwm_fan_control_main(void)
{
	uint8_t start_button = 0;
	
	init_button();
	init_timer3();
	init_L298N();
	
	while(1)
	{
		if (get_button(BUTTON0, BUTTON0PIN))	// start/stop
		{
			start_button = !start_button; // toggle
			OCR3C = (start_button) ? 250 : 0; // 250 : 모터 회전의 기 본 값 0 : stop
			fnd_speed_sec = 1000; // 가장 빠른속도로 세팅되어있음.
			vel_count = (start_button) ? 0 : 13;
		}
		else if (get_button(BUTTON1, BUTTON1PIN))	  // speed-up
		{
			OCR3C = (OCR3C >= 250) ? 250 : OCR3C + 50;
			fnd_speed_sec = (OCR3C >= 250) ? 1000 : 2000-OCR3C*4;
			// 기본속도 250 max_th : 250 min_th : 50
			// 250 200 150 100 50
			
		}
		else if (get_button(BUTTON2, BUTTON2PIN))	  // speed-down
		{
			OCR3C = (OCR3C <= 50) ? 50 : OCR3C - 50;
			fnd_speed_sec = (OCR3C <= 50) ? 1800 : 2000-OCR3C*4;
		}
		else if (get_button(BUTTON3, BUTTON3PIN))     // 방향 설정
		{
			forward = !forward;
			PORTF &= ~(1 << 6 | 1 << 7);		// IN1 IN2 reset
			PORTF = (forward) ? (PORTF | 1 << 6) : (PORTF | 1 << 7);
								//   정회전			역회전
		}
		msec_count_dir();
		_delay_ms(1);
		fnd1_display();
		_delay_ms(1);
		fnd2_display();
		_delay_ms(1);
	}
}
#endif
#if 0
void msec_count_dir(void)
{		
	if(vel_count != 13)
	{
		if (msec_count_vel >= fnd_speed_sec)
		{
			msec_count_vel = 0;
			if(forward) // 정방향일때
			{
				vel_count += 1;
				vel_count %= 13;
			}
			else       // 역방향일때
			{
				if(vel_count==0)
				vel_count = 12;
				else
				{
					vel_count -= 1;
					vel_count %= 13;
				}
			}
		}
	}
		
		// 기본속도 250 max_th : 250 min_th : 50
		// 250 200 150 100 50
	
		if (msec_count >= 1000)   // 1000ms --> 1sec, 속도 상관없이 분초시계 출력하는 카운트에 대한 조건문
		{
			msec_count=0;
			sec_count++;
		}

}
#endif