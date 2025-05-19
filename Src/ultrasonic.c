/*
 * ultrasonic.c
 *
 * Created: 2025-03-12 오후 2:49:38
 *  Author: microsoft
 */ 

#include "ultrasonic.h"

extern volatile int ultrasonic_check_timer;

void init_ultrasonic(void);
void trigger_ultrasonic(void);
void distance_ultrasonic(void);

volatile int led_state=0;
volatile int ultrasonic_dis=0;
volatile char scm[50];
// P278 표 12-3
// PE4 : 외부 INT4 초음파 센서의 상승,하강 에지 둘다 INT가 ISR(INT4_vect)로 들어온다
// 결국 2번(상승 : 1 하강 : 1) 들어온다.
ISR(INT4_vect)
{
	// 1 상승에지
	if (ECHO_PIN & 1 << ECHO)
	{ 
		TCNT1=0;	// Timer 1번 H/W counter를 clear
	}
	else	// 2 하강에지
	{
		// ECHO핀에 들어온 펄스 개수를 us로 환산
		ultrasonic_dis = 1000000.0 * TCNT1 * 1024 / F_CPU;
		// 예 TCINT에 10이 들어있다고 가정하자.
		// 15.625KHz의 1주기 : 64us이다.
		// 0.000064sec(64us) * 10 ==> 0.00064sec(640us)
		// 640us / 58us(1cm 이동하는 소요 시간) ==> 11cm
		// --- 1cm : 58us
		sprintf(scm, "dis : %dcm\n",ultrasonic_dis / 58); // cm환산
		led_state = ultrasonic_dis / 58;
	}
}

void init_ultrasonic(void)
{
	TRIG_DDR |= 1 << TRIG;
	ECHO_DDR &= ~(1 << TRIG); // input mode ECHO_DDR &= 0b11110111;
	// P289 표 12-6 
	// 0 1 : 상승에지(riging edge)와 하강에지(falling edge) 둘다 INT를 띄우도록 요청
	EICRB |= 0 << ISC41 | 1 << ISC40;
	// 16 bit timer 1번을 설정을 해서 사용 65534(max) : 0xffff
	// 16Mhz를 1024로 분주 16000000Hz / 1024 --> 15625Hz --> 15.625KHz
	// 1주기 T(주기) = 1/f = 1/15625 ==> 0.000064sec ==> 64us
	TCCR1B |= 1 << CS12 | 1 << CS10; // 1024로 분주
	// P287 표 14-1
	EIMSK |= 1 << INT4;		// EXTERNAL int 4 (ECHO핀)
}

void trigger_ultrasonic(void)
{
	TRIG_PORT &= ~(1 << TRIG);	// low
	_delay_ms(1);
	TRIG_PORT |= 1 << TRIG;		// high
	_delay_ms(15);				// 규격에는 10us인데 여유를 둬서 15us
	TRIG_PORT &= ~(1 << TRIG);	// low
}

void distance_ultrasonic(void)
{
	if (ultrasonic_check_timer >= 1000)
	{
		ultrasonic_check_timer = 0;
		printf("%s",scm);
		trigger_ultrasonic();
	}
}

// 1 2 3 4 5 6
// /3 ->  