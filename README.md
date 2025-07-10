# 🧺 ATmega128 기반 스마트 세탁기 시스템

## 📖 프로젝트 개요
ATmega128 MCU와 FSM 설계를 활용하여 자동 및 수동 모드를 지원하는 스마트 세탁기 시스템을 구현했습니다. PWM 모터 제어, 부저음, LED/FND 디스플레이, 블루투스 원격제어까지 통합된 임베디드 시스템입니다.

## 🚀 기술 스택
- **언어:** C (AVR-GCC)
- **플랫폼:** ATmega128A, Atmel Studio
- **기술:** FSM, PWM, UART(Bluetooth), FND, 부저, 인터럽트, 디바운싱

## 🛠 주요 기능
| 기능 | 설명 |
|---|---|
| 🔄 자동/수동 모드 | 세탁 → 헹굼 → 탈수 순차 자동 진행 또는 단계별 수동 제어 |
| 🚨 부저음 & FND 표시 | 시작/종료 멜로디 재생 + 현재 모드/상태 4자리 FND 표시 |
| 📲 블루투스 원격 제어 | 스마트폰으로 세탁기 제어 및 상태 확인 (UART 기반) |
| 🕹️ 버튼 입력 | 물리 버튼 입력 → 디바운싱 → 안정적 입력 처리 |

## 🖥 시스템 구성도
![image](https://github.com/user-attachments/assets/543f38fa-2a5f-4e01-81f8-cd79c8536db4)
- 버튼 ↔ ATmega128 ↔ 모터(PWM) ↔ 부저 ↔ FND ↔ Bluetooth ↔ 스마트폰

## 📊 결과 및 성과
- 세탁기 전 과정 구현 → 자동/수동 모드 모두 정상 동작
- 부저 & 디스플레이 & 모터 동기화 성공
- 블루투스 제어 & 상태 피드백 정상 수행

## 📝 설치 및 실행 방법
- Atmel Studio로 hex 파일 빌드 후 ATmega128에 업로드
- 스마트폰 → 블루투스 → 명령어 전송으로 세탁기 제어

## 📁 프로젝트 구조
```
Washing_Machine_ATmega/
├── main.c
├── Laundry.c
├── Laundry_fnd.c
├── led.c
├── speaker.c
└── uart1.c
```

## ✉️ 개선 포인트
- delay() 제거 → 타이머 인터럽트로 개선
- 디바운싱 고도화 → 시스템 반응성 향상
- FND/부저 비동기 처리 → FreeRTOS 도입 고려
  
---
✅ 키워드: #AVR #FSM #세탁기 #ATmega128 #PWM #Bluetooth #임베디드시스템
