/*
 * uart1.c
 *
 * Created: 2025-03-11 오후 4:35:40
 *  Author: microsoft
 */ 

#include "uart1.h"

void init_uart1(void);
void bt_command_processing(void);
extern volatile int stateselect;
extern char *func_names[];


ISR(USART1_RX_vect)
{
	volatile uint8_t rx_data;
	volatile static int i = 0;	// buffer의 index
	
	rx_data = UDR1; // uart1 하드웨어로 들어온 문자 읽어오기
					// 이렇게 assign연산자로 UDR1값을 읽어오는 순간, UDR1는 empty가됨 (자동)
	
	if(rx_data == '\n')
	{
		rx_buff1[rear1++][i] = 0; // 문장의 끝은 항상 0
		rear1 %= COMMAND_NUMBER; // 원형큐
		i = 0; // 새로오는건 버퍼에 새롭게 채워야함 (다른 메시지니까!)
		
		// 큐 full 여부 확인
	}
	else
	{
		// 그냥 버퍼에 넣기 (push)
		rx_buff1[rear1][i++] = rx_data;
		// command length 확인
	}
}


/*
[init에 고려해야할것]

전송속도 : 9600bps -> 1초에 960자를 받음
				1글자 송수신에 약 1ms가 걸린다 (1글자 == 10비트)

비동기 방식 : start, stop 비트의 활용

RX : interrupt (받는것)
TX : pooling (보내는것)

*/
void init_uart1(void)
{
	// 전송속도 설정
	UBRR1H = 0x00;
	UBRR1L = 207; // 2배속 모드임

	
	UCSR1A |= 1 << U2X1; // 2배속 통신
	
	UCSR1C |= 0x06; // 비동기, none 8bit, data 8 bit
	
	// 인터럽트, uart 기능 설정
	//UCSR0B |= 1 << RXEN0; // 수신 허용
	//UCSR0B |= 1 << TXEN0; // 송신 허용
	//UCSR0B |= 1 << RXCIE0; // 수신 인터럽트 허용
	UCSR1B |= 1 << RXEN1 | 1 << TXEN1 | 1 << RXCIE1;
	
	rear1 = front1 = 0;
}

// bt와 통신하여 프로그램 제어
void bt_command_processing(void)
{
	if(front1 != rear1)
	{
		printf("uart1 : %s\n", (char *)rx_buff1[front1]);
		
		for(int i = 0; i<2; i++){
			if(strncmp(rx_buff1[front1], func_names[i], strlen(func_names[i])) == NULL)
			{
				printf("uart1 find: %s\n", func_names[i]);
				stateselect = i;
				break;
			}
		}
		front1++;
		front1 %= COMMAND_NUMBER;
	}
}