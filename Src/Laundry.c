/*
 * Laundry.c
 *
 * Created: 2025-03-14 오후 2:59:29
 *  Author: microsoft
 */ 

#include "Laundry.h"
#include <stdio.h>
#include <string.h>    // strcpy 함수가 선언된 헤더 파일
#include <stdlib.h>    // malloc, free 함수가 선언된 헤더 파일

void Laundry_main(int BUTTON_PRESSED);
void led_wash(void);
void led_rinse(void);
void led_dry(void);
void RUN(void);
void STOP(void);

// 초기상태 init : 2 auto_mode_set : 0 manual_mode_set : 1 auto_mode_run : 3 manual_mode_run : 4
extern volatile uint8_t current_mode;    
extern volatile uint8_t mode_select;     // 자동모드 : 0 / 수동모드 : 1
extern volatile uint8_t Laundry_state;   // 세탁 동작 : 1 / 중지 : 0
extern volatile uint8_t washtime_select; // 세탁시간 : 0, 헹굼시간 : 1, 탈수시간 : 2
extern volatile uint32_t auto_total_washing_time;
extern volatile uint32_t manual_total_washing_time;

extern WashingTime WashingTime_manual;
extern WashingTime WashingTime_auto;
extern int get_button(int button_num, int button_pin);
extern void Beep(int repeat);
extern void Laundry(int repeat);
void Laundry_main(int BUTTON_PRESSED)
{

		switch(BUTTON_PRESSED)
		{
			case BUTTON0: Beep(1);
				if (current_mode == init_state)       // 초기 화면: Auto/Manual 모드 토글
					mode_select = !mode_select;
				else if (current_mode == auto_mode_set) // 자동 화면: 세탁 시작/중지
				{
					auto_total_washing_time = WashingTime_auto.wash_time + WashingTime_auto.rinse_time + WashingTime_auto.dry_time;
					Laundry_state = !Laundry_state;
					current_mode = auto_mode_run;
					if(Laundry_state == 1) //세탁 시작 시 부저 음악이 한 번 재생
					{
						Laundry(1);
					}
				}
				else if (current_mode == auto_mode_run) // 자동 화면: 세탁 시작/중지
				{
					Laundry_state = !Laundry_state;
					if(Laundry_state == 0) //세탁 중지 시 부저 음악이 한 번 재생
					{
						Laundry(1);
					}
				}
				else if (current_mode == manual_mode_set) // 수동 화면: 세탁 시작/중지
				{
					manual_total_washing_time = WashingTime_manual.wash_time + WashingTime_manual.rinse_time + WashingTime_manual.dry_time;
					Laundry_state = !Laundry_state;
					current_mode = manual_mode_run;
					if(Laundry_state == 1) //세탁 시작 시 부저 음악이 한 번 재생
					{
						Laundry(1);
					}
				}
				else if (current_mode == manual_mode_run) // 수동 화면: 세탁 시작/중지
				{
					Laundry_state = !Laundry_state;
					if(Laundry_state == 0) //세탁 중지 시 부저 음악이 한 번 재생
					{
						Laundry(1);
					}
				}
					
			break;

			case BUTTON1: Beep(1);
				if (current_mode == init_state)  // 초기 화면: 선택된 모드로 전환
					current_mode = mode_select;
				else if (current_mode == manual_mode_set)  // 수동 화면: 세탁/헹굼/탈수 모드 변경
				{
					washtime_select = (washtime_select + 1) % 3;
				}
			break;

			case BUTTON2: Beep(1);
				if (current_mode == manual_mode_set)  // 수동 화면: 세탁/헹굼/탈수 시간 증가
				{
					if (washtime_select == wash_sel)
						WashingTime_manual.wash_time++;
					else if (washtime_select == rinse_sel)
						WashingTime_manual.rinse_time++;
					else if (washtime_select == dry_sel)
						WashingTime_manual.dry_time++;
				}	
			break;

			case BUTTON3: Beep(1);
				if ((current_mode == auto_mode_run || current_mode == manual_mode_run) && !Laundry_state)
				{
					// 세탁 중지 시 초기 화면으로
					current_mode = init_state;
					// 저장되었던 설정 값도 초기화
					WashingTime_manual.wash_time = 0;
					WashingTime_manual.rinse_time = 0;
					WashingTime_manual.dry_time = 0;
				}
				else if (current_mode == manual_mode_set)
				{
					// 세탁/헹굼/탈수 시간 초기화
					if (washtime_select == wash_sel)
						WashingTime_manual.wash_time = 0;
					else if (washtime_select == rinse_sel)
						WashingTime_manual.rinse_time = 0;
					else if (washtime_select == dry_sel)
						WashingTime_manual.dry_time = 0;
				}		
			break;

			default:
			
			break;
		}
}

void led_wash(void)//세탁모드일때 led 1번째를 킨다
{
	PORTB = 0x01;
}

void led_rinse(void)//헹굼모드일때 led 2번째를 킨다
{
	PORTB = 0x04;
}

void led_dry(void)//탈수모드일 때 led 3번째를 킨다
{
	PORTB = 0x08;
}

void motor_off(void)//모터를 정지시키는 코드
{
	OCR3C = 0;
}

void motor_forward(void)//정방향 회전
{
	OCR3C = 250;
	PORTF &= ~(1 << 6 | 1 << 7);		// IN1 IN2 reset
	PORTF = (1 << PF6); //역방향 회전 포트 6번핀을 high
}

void motor_reverse(void)//역방향 회전
{
	OCR3C = 250;
	PORTF &= ~(1 << 6 | 1 << 7);		// IN1 IN2 reset
	PORTF = (1 << PF7);
}

void (*funcs[FUNC_NUM])(void) = { RUN, STOP };
char *func_names[FUNC_NUM] = { "RUN", "STOP"};

void RUN(void)
{
	Laundry_state = 1;
}

void STOP(void)
{
	Laundry_state = 0;
}