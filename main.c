#include "tm4c123gh6pm.h"

// Interrupt when ADC finishes getting input
// Will copy voltage from ADC to our Frequency Data structure
// some sort of ADC handler ISR

// Periodic Timer B  500 ms
// Interrupt to Average Frequency Data and 
// Update Timer C

// Timer C
// Get next value of Sine array
// Send to I2C to update the output voltage of the DAC
int i;
// Initialize PLL to 16 MHz
void INIT_PLL(void) {
  SYSCTL->RCC2 |= 0x80000000;  // Use RCC2
	SYSCTL->RCC2 |= 0x00000800;  // bypass PLL while initializing
	SYSCTL->RCC = (SYSCTL->RCC &~0x000007C0) + 0x00000540;  // 16 MHz
	SYSCTL->RCC2 &= ~0x00000070;  // Cfg for main OSC
  SYSCTL->RCC2 &= ~0x00002000;  // activate PLL by clearing PWRDN
	SYSCTL->RCC2 |= 0x40000000;  // use 400 MHz PLL
	SYSCTL->RCC2 = (SYSCTL->RCC2 & ~0x1FC00000) + (4<<2);  // 80 MHz
	while((SYSCTL->RIS & 0x00000040)==0){};  // Wait for PLL to lock
  SYSCTL->RCC2 &= ~0x00000800;  // enable PLL by clearing bypass
}
// Initialize GPIOs
// PB2, PB3 [I2C]
// PD3 [ADC]
void INIT_GPIO(void){
	
}

// Initialize ADC
// Periodic Timer A  2ms
void INIT_ADC(void){
	SYSCTL->RCGCADC |= 0x01;      // ADC Clk
	SYSCTL->RCGCGPIO |= 0x08;     // Port D
	SYSCTL->RCGCTIMER |= 0x01;    // Timer 0 = Timer A
	// initialize structure??
	
	// GPIO D3 for ADC0
	GPIOD->DIR &= ~0x08;
	GPIOD->AFSEL |= 0x08;
	GPIOD->DEN &= ~0x08;          // 
	GPIOD->AMSEL |= 0x08;         // enable analog funtionality
	
	for (i = 0; i < 10; i++){};
	
	
	ADC0->PC = 0x1;              // Config for 125k samples/sec
	ADC0->SSPRI = 0x3210;         // Priority of sequencers (we don't care we just use one)
	TIMER0->CTL = 0x00000000;     // disable this while enabling
	TIMER0->CTL |= 0x00000020;    // enables Timer0 to trigger ADC
	TIMER0->CFG = 0;              // enable 32-bit mode
	TIMER0->TAMR = 0x00000002;    // periodic mode
	TIMER0->TAPR = 0;             // no prescale
	TIMER0->TAILR = 159999;       // 160000 cycles is 2ms
	TIMER0->IMR = 0x00000000;     // disable all interrupts
	TIMER0->CTL |= 0x00000001;    // enable timer0A, 32-bit, periodic
	
	ADC0->ACTSS &= ~0x08;         // disable sample sequencer
	ADC0->EMUX = (ADC0->EMUX & 0xFFFFF0FF) + 0x0500;  // timer trigger	 
	ADC0->SSMUX3 = 4;             // PD3 is analog channel 4
	ADC0->SSCTL3 = 0x06;          // set flag and end after first sample
	ADC0->IM |= 0x08;             // enable SS3 interrupts
	ADC0->ACTSS |= 0x08;          // enable sample sequencer 3
  // config NVIC interrupts
  NVIC->IP[17] = 0x40;            // interrupt priority 2
  NVIC->ISER[2] = 1;              // Enable interrupt 17
}

// Interrupt to trigger ADC conversion (every 2ms)
void ADC0SS3_Handler (void) {
  // clear interrupt flag?
	ADC0->ISC = 0x08;
	
	
}

// Initialize I2C
void INIT_I2C(void){
// We want I2C Frequencey of 400 kHz (fast mode), so 400 kbps
// TPR = (System Clock/(2*(SCL_LP + SCL_HP)*SCL_CLK))-1;
// TPR = (80 MHz / (2*(6 + 4) * 400000)) - 1 = 9!
	#define TPR 9
	SYSCTL->RCGCI2C |= 0x0001;   // activate I2C0
	SYSCTL->RCGCGPIO |= 0x0002;  // activate port B
	while((SYSCTL->PRGPIO&0x0002) == 0){};  // ready?
	GPIOB->AFSEL |= 0x0C;          // enable alt func on PB2, 3
		GPIOB->ODR |= 0x08;          // enable open drain
		GPIOB->PCTL = (GPIOB->PCTL&0xFFFF00FF) + 0x00003300;  // I2C
		GPIOB->DEN |= 0x0C;        // enable digital I/O on PB2,3
		I2C0->MCR = 0x00000010;    // master function enable
		I2C0->MTPR = TPR;          // cfg for 400 kbps fast clock

}


// Frequency Data structure to hold 250 entries of 12-bits for ADC


// Sine Data array of 40 entries (12-bit for the DAC), 0x0 is min, 0xFFF is max
unsigned short sine_array[] = {0x800,0x940,0xa78,0xba1,0xcb3,0xda7,0xe78,0xf20,
0xf9b,0xfe6,0xfff,0xfe6,0xf9b,0xf20,0xe78,0xda7,
0xcb3,0xba1,0xa78,0x940,0x800,0x6bf,0x587,0x45e,
0x34c,0x258,0x187,0xdf,0x64,0x19,0x0,0x19,
0x64,0xdf,0x187,0x258,0x34c,0x45e,0x587,0x6bf};


int main(void)
{
	//your code here.

	//INIT_PLL();
  INIT_ADC();
	
	while(1);
}
