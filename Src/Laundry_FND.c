/*
 * Laundry_FND.c
 *
 * Created: 2025-03-14 오후 2:25:21
 *  Author: microsoft
 */
#include "fnd.h"
#include "Laundry.h"
void init_fnd(void);
void fnd_display(void);

										// 0     1     2     3
							            // W     A     S     H
volatile uint8_t fnd_font_laundry[] =	 {0xd5, 0x88, 0x92, 0x89,
										// 4     5     6     7
							   			// R     N     E     D
										  0x8f, 0xab, 0x86, 0xa1,
									    // 8     9     10    11    
										// R     Y     O     T    
										  0x8f, 0x99, 0xc0, 0x87, 
										//  12   13    14    15    16
										//  C    U     P     I     L
										  0x92, 0xc1, 0x8c, 0xcf, 0xc7};
										  
								// 0   1    2    3     4    5   6     7   8    9    .
volatile uint8_t fnd_font[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xd8,0x80,0x90,0x7f};									  
//                               0    1    2    3    4    5    6    7    8    9    10   11   12  
volatile uint8_t fnd_font4[] = {0xff,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xf6,0xf2,0xf0}; // D4 0~12단계로 진행도출력
volatile uint8_t fnd_font3[] = {0xff,0xff,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xf6,0xf6,0xf6,0xf6}; // D3 0~12단계로 진행도출력
volatile uint8_t fnd_font2[] = {0xff,0xff,0xff,0xfe,0xfe,0xfe,0xfe,0xfe,0xf6,0xf6,0xf6,0xf6,0xf6}; // D2 0~12단계로 진행도출력
volatile uint8_t fnd_font1[] = {0xff,0xff,0xff,0xff,0xfe,0xde,0xce,0xc6,0xc6,0xc6,0xc6,0xc6,0xc6}; // D1 0~12단계로 진행도출력										  
					
extern volatile uint8_t current_mode;    // 초기상태 init : 2 auto_mode_set : 0 manual_mode_set : 1 auto_mode_run : 3 manual_mode_run : 4
extern volatile uint8_t mode_select;	 // 자동모드 : 0 / 수동모드 : 1
extern volatile uint8_t Laundry_state;   // 세탁 동작 : 1 / 중지 : 0
extern volatile uint8_t washtime_select; // 세탁시간 : 0, 헹굼시간 : 1, 탈수시간 : 2
extern volatile uint32_t sec_count;
extern volatile uint32_t auto_total_washing_time;
extern volatile uint32_t manual_total_washing_time;
extern WashingTime WashingTime_manual;
extern WashingTime WashingTime_auto;

void fnd_display(void)
{
	static uint8_t digit_select=0;  // static를 쓰면 전역 변수 처럼 함수가 빠져 나갔다가 다시 들어 오더라도 값을 유지
	
	switch(digit_select) // FND2 출력용 Switch - case 함수
	{
		case 0: // common anode
		FND_DIGIT_PORT = 0x80;
		if(current_mode == init_state) // 초기화면
			FND_DATA_PORT = fnd_font_laundry[11];   // FND1 D4 T(INIT)
		else if(current_mode == manual_mode_set)
		{
			if(washtime_select == wash_sel)
				FND_DATA_PORT = fnd_font[WashingTime_manual.wash_time % 10];   // FND1 D4 Wash Time 0-9sec
			else if(washtime_select == rinse_sel)
				FND_DATA_PORT = fnd_font[WashingTime_manual.rinse_time % 10];   // FND1 D4 Rinse Time 0-9sec
			else if(washtime_select == dry_sel)
				FND_DATA_PORT = fnd_font[WashingTime_manual.dry_time % 10];   // FND1 D4 Dry Time 0-9sec	
		}
		else if(current_mode == manual_mode_run)
		{
			FND_DATA_PORT = fnd_font[manual_total_washing_time % 10];   // FND1 D4 Total Time 0-9sec
		}
		else if(current_mode == auto_mode_set)
		{
			FND_DATA_PORT = fnd_font_laundry[16];   // FND1 D4 L(IDEL)
		}
		else if(current_mode == auto_mode_run)
		{
			FND_DATA_PORT = fnd_font[auto_total_washing_time % 10];   // FND1 D4 Total Time 0-9sec
		}
		break;
				
		case 1: // common anode
		FND_DIGIT_PORT = 0x40;
		if(current_mode == init_state) // 초기화면
			FND_DATA_PORT = fnd_font_laundry[15];   // FND1 D3 I(INIT)
		else if(current_mode == manual_mode_set)
		{
			if(washtime_select == wash_sel)
				FND_DATA_PORT = fnd_font[WashingTime_manual.wash_time / 10 % 6];   // FND1 D3 Wash Time 10초단위출력
			else if(washtime_select == rinse_sel)
				FND_DATA_PORT = fnd_font[WashingTime_manual.rinse_time / 10 % 6];   // FND1 D3 Rinse Time 10초단위출력
			else if(washtime_select == dry_sel)
				FND_DATA_PORT = fnd_font[WashingTime_manual.dry_time / 10 % 6];   // FND1 D3 Dry Time 10초단위출력
		}
		else if(current_mode == manual_mode_run)
		{
			FND_DATA_PORT = fnd_font[manual_total_washing_time / 10 % 6];   // FND1 D3 Total Time 10초단위출력
		}
		else if(current_mode == auto_mode_set)
		{
			FND_DATA_PORT = fnd_font_laundry[6];   // FND1 D3 E(IDEL)
		}
		else if(current_mode == auto_mode_run)
		{
			FND_DATA_PORT = fnd_font[auto_total_washing_time / 10 % 6];   // FND1 D3 Total Time 10초단위출력
		}
		break;
				
		case 2: // common anode
		FND_DIGIT_PORT =  0x20;
		if(current_mode == init_state) // 초기화면
			FND_DATA_PORT = fnd_font_laundry[5];   // FND1 D2 N(INIT)
		else if(current_mode == manual_mode_set)
		{
			if(washtime_select == wash_sel)
				FND_DATA_PORT = fnd_font[WashingTime_manual.wash_time / 60 % 10];   // FND1 D2 Wash Time 0-9분단위출력
			else if(washtime_select == rinse_sel)
				FND_DATA_PORT = fnd_font[WashingTime_manual.rinse_time / 60 % 10];   // FND1 D2 Rinse Time 0-9분단위출력
			else if(washtime_select == dry_sel)
				FND_DATA_PORT = fnd_font[WashingTime_manual.dry_time / 60 % 10];   // FND1 D2 Dry Time 0-9분단위출력
		}
		else if(current_mode == manual_mode_run)
		{
			FND_DATA_PORT = fnd_font[manual_total_washing_time / 60 % 10];   // FND1 D2 Total Time 0-9분단위출력
		}
		else if(current_mode == auto_mode_set)
		{
			FND_DATA_PORT = fnd_font_laundry[7];   // FND1 D2 D(IDEL)
		}
		else if(current_mode == auto_mode_run)
		{
			FND_DATA_PORT = fnd_font[auto_total_washing_time / 60 % 10];   // FND1 D2 Total Time 0-9분단위출력
		}
		break;
				
		case 3: // common anode
		FND_DIGIT_PORT = 0x10;
		if(current_mode == init_state) // 초기화면
			FND_DATA_PORT = fnd_font_laundry[15];   // FND1 D1 I(INIT)
		else if(current_mode == manual_mode_set)
		{
			if(washtime_select == wash_sel)
				FND_DATA_PORT = fnd_font[WashingTime_manual.wash_time / 600 % 6];   // FND1 D1 Wash Time 10분단위출력
			else if(washtime_select == rinse_sel)
				FND_DATA_PORT = fnd_font[WashingTime_manual.rinse_time / 600 % 6];   // FND1 D1 Rinse Time 10분단위출력
			else if(washtime_select == dry_sel)
				FND_DATA_PORT = fnd_font[WashingTime_manual.dry_time / 600 % 6];   // FND1 D1 Dry Time 10분단위출력
		}
		else if(current_mode == manual_mode_run)
		{
			FND_DATA_PORT = fnd_font[manual_total_washing_time / 600 % 6];   // FND1 D1 Total Time 10분단위출력
		}
		else if(current_mode == auto_mode_set)
		{
			FND_DATA_PORT = fnd_font_laundry[15];   // FND1 D1 I(IDEL)
		}
		else if(current_mode == auto_mode_run)
		{
			FND_DATA_PORT = fnd_font[auto_total_washing_time / 600 % 6];   // FND1 D1 Total Time 10분단위출력
		}
		break;
		
		case 4: // common anode
		FND_DIGIT_PORT = 0x08;
		if(current_mode == init_state) // 초기화면
		{
			if(!mode_select) // 0 자동모드 출력
				FND_DATA_PORT = fnd_font_laundry[10];    // FND2 D4 O(AUTO)
			else             // 1 수동모드 출력
				FND_DATA_PORT = fnd_font_laundry[7];     // FND2 D4 D(HAND)
		}
		else if(current_mode == manual_mode_set) // 수동모드 설정
		{
			if(washtime_select == wash_sel) // 세탁시간 설정
				FND_DATA_PORT = fnd_font_laundry[3];    // FND2 D4 H(WASH)
			else if(washtime_select == rinse_sel) // 헹굼시간 설정
				FND_DATA_PORT = fnd_font_laundry[6];    // FND2 D4 E(RNSE)
			else if(washtime_select == dry_sel) // 탈수시간 설정
				FND_DATA_PORT = fnd_font_laundry[9];    // FND2 D4 Y(_DRY)
		}
		else if(current_mode == manual_mode_run) // 수동모드 동작중
		{
			if(Laundry_state) // 세탁모드에서 동작중이면 Circle 출력
				FND_DATA_PORT = fnd_font4[sec_count % 13];    // FND2 D4 Circle
			else // 세탁모드에서 중지상태면 STOP 출력
				FND_DATA_PORT = fnd_font_laundry[14];    // FND2 D4 P(STOP)
		}
		else if(current_mode == auto_mode_set) // 자동모드 설정
		{
				FND_DATA_PORT = fnd_font_laundry[10];    // FND2 D4 O(AUTO)
		}
		else if(current_mode == auto_mode_run) // 자동모드 동작중
		{
			if(Laundry_state) // 세탁모드에서 동작중이면 Circle 출력
				FND_DATA_PORT = fnd_font4[sec_count % 13];    // FND2 D4 Circle
			else // 세탁모드에서 중지상태면 STOP 출력
				FND_DATA_PORT = fnd_font_laundry[14];    // FND2 D4 P(STOP)
		}			
		break;
		
		case 5: // common anode
		FND_DIGIT_PORT = 0x04;
		if(current_mode == init_state) // 초기화면
		{
			if(!mode_select) // 0 자동모드 출력
				FND_DATA_PORT = fnd_font_laundry[11];    // FND2 D3 T(AUTO)
			else             // 1 수동모드 출력
				FND_DATA_PORT = fnd_font_laundry[5];    // FND2 D3 N(HAND)
		}
		else if(current_mode == manual_mode_set) // 수동모드 설정
		{
			if(washtime_select == wash_sel) // 세탁시간 설정
				FND_DATA_PORT = fnd_font_laundry[2];    // FND2 D3 S(WASH)
			else if(washtime_select == rinse_sel) // 헹굼시간 설정
				FND_DATA_PORT = fnd_font_laundry[2];    // FND2 D3 S(RNSE)
			else if(washtime_select == dry_sel) // 탈수시간 설정
				FND_DATA_PORT = fnd_font_laundry[8];    // FND2 D3 R(_DRY)
		}
		else if(current_mode == manual_mode_run) // 수동모드 동작중
		{
			if(Laundry_state) // 세탁모드에서 동작중이면 Circle 출력
				FND_DATA_PORT = fnd_font3[sec_count % 13];    // FND2 D3 Circle
			else // 세탁모드에서 중지상태면 STOP 출력
				FND_DATA_PORT = fnd_font_laundry[10];    // FND2 D3 O(STOP)
		}
		else if(current_mode == auto_mode_set) // 자동모드 설정
		{
				FND_DATA_PORT = fnd_font_laundry[11];    // FND2 D3 T(AUTO)
		}
		else if(current_mode == auto_mode_run) // 자동모드 동작중
		{
			if(Laundry_state) // 세탁모드에서 동작중이면 Circle 출력
				FND_DATA_PORT = fnd_font3[sec_count % 13];    // FND2 D3 Circle
			else // 세탁모드에서 중지상태면 STOP 출력
				FND_DATA_PORT = fnd_font_laundry[10];    // FND2 D3 O(STOP)
		}
		break;
		
		case 6: // common anode
		FND_DIGIT_PORT =  0x02;
		if(current_mode == init_state) // 초기화면
		{
			if(!mode_select) // 0 자동모드 출력
				FND_DATA_PORT = fnd_font_laundry[13];    // FND2 D2 U(AUTO)
			else             // 1 수동모드 출력
				FND_DATA_PORT = fnd_font_laundry[1];    // FND2 D2 A(HAND)
		}
		else if(current_mode == manual_mode_set) // 수동모드 설정
		{
			if(washtime_select == wash_sel) // 세탁시간 설정
				FND_DATA_PORT = fnd_font_laundry[1];    // FND2 D2 A(WASH)
			else if(washtime_select == rinse_sel) // 헹굼시간 설정
				FND_DATA_PORT = fnd_font_laundry[5];    // FND2 D2 N(RNSE)
			else if(washtime_select == dry_sel) // 탈수시간 설정
				FND_DATA_PORT = fnd_font_laundry[7];    // FND2 D2 D(_DRY)
		}
		else if(current_mode == manual_mode_run) // 수동모드 동작중
		{
			if(Laundry_state) // 세탁모드에서 동작중이면 Circle 출력
				FND_DATA_PORT = fnd_font2[sec_count % 13];    // FND2 D2 Circle
			else // 세탁모드에서 중지상태면 STOP 출력
				FND_DATA_PORT = fnd_font_laundry[11];    // FND2 D2 T(STOP)
		}
		else if(current_mode == auto_mode_set) // 자동모드 설정
		{
				FND_DATA_PORT = fnd_font_laundry[13];    // FND2 D2 U(AUTO)
		}
		else if(current_mode == auto_mode_run) // 자동모드 동작중
		{
			if(Laundry_state) // 세탁모드에서 동작중이면 Circle 출력
				FND_DATA_PORT = fnd_font2[sec_count % 13];    // FND2 D2 Circle
			else // 세탁모드에서 중지상태면 STOP 출력
				FND_DATA_PORT = fnd_font_laundry[11];    // FND2 D2 T(STOP)
		}
		break;
		
		case 7: // common anode
		FND_DIGIT_PORT = 0x01;
		if(current_mode == init_state) // 초기화면
		{
			if(!mode_select) // 0 자동모드 출력
				FND_DATA_PORT = fnd_font_laundry[1];    // FND2 D1 A(AUTO)
			else             // 1 수동모드 출력
				FND_DATA_PORT = fnd_font_laundry[3];    // FND2 D1 H(HAND)
		}
		else if(current_mode == manual_mode_set) // 수동모드 설정
		{
			if(washtime_select == wash_sel) // 세탁시간 설정
				FND_DATA_PORT = fnd_font_laundry[0];    // FND2 D1 W(WASH)
			else if(washtime_select == rinse_sel) // 헹굼시간 설정
				FND_DATA_PORT = fnd_font_laundry[4];    // FND2 D1 R(RNSE)
			else if(washtime_select == dry_sel) // 탈수시간 설정
				FND_DATA_PORT = 0xff;    // FND2 D4 off(_DRY)
		}
		else if(current_mode == manual_mode_run) // 수동모드 동작중
		{
			if(Laundry_state) // 세탁모드에서 동작중이면 Circle 출력
				FND_DATA_PORT = fnd_font1[sec_count % 13];    // FND2 D1 Circle
			else // 세탁모드에서 중지상태면 STOP 출력
				FND_DATA_PORT = fnd_font_laundry[2];    // FND2 D1 S(STOP)
		}
		else if(current_mode == auto_mode_set) // 자동모드 설정
		{
			FND_DATA_PORT = fnd_font_laundry[1];    // FND2 D1 A(AUTO)
		}
		else if(current_mode == auto_mode_run) // 자동모드 동작중
		{
			if(Laundry_state) // 세탁모드에서 동작중이면 Circle 출력
				FND_DATA_PORT = fnd_font1[sec_count % 13];    // FND2 D1 Circle
			else // 세탁모드에서 중지상태면 STOP 출력
				FND_DATA_PORT = fnd_font_laundry[2];    // FND2 D1 S(STOP)
		}
		break;
	}
	digit_select++;
	digit_select %= 8;   // 다음 표시할 자리수 선택
}

void init_fnd(void)
{
	FND_DATA_DDR = 0xff;  // 출력 모드로 설정
	// FND_DIGIT_DDR |= 0xf0;   // 자릿수 선택 7654
	FND_DIGIT_DDR |= 1 << FND_DIGIT_D1 | 1 << FND_DIGIT_D2 | 1 << FND_DIGIT_D3
	| 1 << FND_DIGIT_D4;
	// FND2_DIGIT_DDR |= 0xff; // 자릿수 선택 4321
	FND_DIGIT_DDR |= 1 << FND2_DIGIT_D1 | 1 << FND2_DIGIT_D2 | 1 << FND2_DIGIT_D3
	| 1 << FND2_DIGIT_D4;
	// fnd를 all off
	FND_DATA_PORT = ~0x00;   // fnd를 all off  0xff;
}							  