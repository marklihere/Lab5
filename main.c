#include "tm4c123gh6pm.h"

#define I2C_START 0x00000002;
#define I2C_STOP 0x00000004;
#define I2C_RUN 0x00000001;

// Sine Data array of 40 entries (12-bit for the DAC), 0x0 is min, 0xFFF is max
unsigned short sine_array[] = {0x800,0x940,0xa78,0xba1,0xcb3,0xda7,0xe78,0xf20,
0xf9b,0xfe6,0xfff,0xfe6,0xf9b,0xf20,0xe78,0xda7,
0xcb3,0xba1,0xa78,0x940,0x800,0x6bf,0x587,0x45e,
0x34c,0x258,0x187,0xdf,0x64,0x19,0x0,0x19,
0x64,0xdf,0x187,0x258,0x34c,0x45e,0x587,0x6bf};



// Global variables 
unsigned int adcsamples = 0;  // Running total of last 250 samples of ADC
int i = 0;                    // index into sine_array

// Initialize PLL to 16 MHz
void INIT_PLL(void) {
  SYSCTL->RCC2 |= 0x80000000;  // Use RCC2
	SYSCTL->RCC2 |= 0x00000800;  // bypass PLL while initializing
	SYSCTL->RCC = (SYSCTL->RCC &~0x000007C0) + 0x00000540;  // 16 MHz
	SYSCTL->RCC2 &= ~0x00000070;  // Cfg for main OSC
  SYSCTL->RCC2 &= ~0x00002000;  // activate PLL by clearing PWRDN
	SYSCTL->RCC2 |= 0x40000000;  // use 400 MHz PLL
	SYSCTL->RCC2 = (SYSCTL->RCC2 & ~0x1FC00000) + (4<<22);  // 80 MHz
	while((SYSCTL->RIS & 0x00000040)==0){};  // Wait for PLL to lock
  SYSCTL->RCC2 &= ~0x00000800;  // enable PLL by clearing bypass
}


// Initialize ADC
// Periodic Timer A  2ms
void INIT_ADC(void){
	SYSCTL->RCGCADC |= 0x01;      // ADC Clk
	SYSCTL->RCGCGPIO |= 0x08;     // Port D
	SYSCTL->RCGCTIMER |= 0x01;    // Timer 0 = Timer A
	
	// GPIO D3 for ADC0
	GPIOD->DIR &= ~0x08;
	GPIOD->AFSEL |= 0x08;
	GPIOD->DEN &= ~0x08;          // 
	GPIOD->AMSEL |= 0x08;         // enable analog funtionality
	
	// for (i = 0; i < 10; i++){};   
	
	
	ADC0->PC = 0x1;               // Config for 125k samples/sec
	ADC0->SSPRI = 0x0;            // Priority of sequencers (we don't care we just use one)
	TIMER0->CTL = 0x00000000;     // disable this while enabling
	TIMER0->CTL |= 0x00000020;    // enables Timer0 to trigger ADC
	TIMER0->CFG = 0;              // enable 32-bit mode
	TIMER0->TAMR = 0x00000002;    // periodic mode
	TIMER0->TAPR = 0;             // no prescale
	TIMER0->TAILR = 159999;       // 160000 cycles is 2ms
	TIMER0->IMR = 0x00000000;     // disable all interrupts
	TIMER0->CTL |= 0x00000001;    // enable timer0A, 32-bit, periodic
	
	ADC0->ACTSS &= ~0x08;         // disable sample sequencer
	ADC0->EMUX = (ADC0->EMUX & 0xFFFF0FFF) + 0x5000;  // timer trigger	 
	ADC0->SSMUX3 = 4;             // PD3 is analog channel 4
	ADC0->SSCTL3 = 0x06;          // set flag and end after first sample
	ADC0->IM |= 0x08;             // enable SS3 interrupts
	ADC0->ACTSS |= 0x08;          // enable sample sequencer 3
  // config NVIC interrupts
  NVIC->IP[17] = 0x40;            // interrupt priority 2
  NVIC->ISER[0] = 1 << 17;              // Enable interrupt 17
}

// Interrupt to trigger ADC conversion (every 2ms)
// Interrupt when ADC finishes getting input

void ADC0SS3_Handler (void) {
  // clear interrupt flag
	ADC0->ISC = 0x08;
	// add this ADC0 output to rolling total
	adcsamples = ADC0->SSFIFO3 + adcsamples;
}

// Periodic Timer B  500 ms
void INIT_TIMERB (void) {
	SYSCTL->RCGCTIMER |= 0x02;    // Timer 1 = Timer B
  TIMER1->CTL = 0x00000000;     // disable this while configuring
	TIMER1->CFG = 0;              // enable 32-bit mode
	TIMER1->TAMR = 0x00000002;    // periodic mode
	TIMER1->TAPR = 0;             // no prescale
	TIMER1->TAILR = 40000000;     // 40,000,000 cycles is 500ms
	TIMER1->IMR = 0x00000001;     // enable interrupt
	TIMER1->CTL |= 0x00000001;    // enable timer1A, 32-bit, periodic
	
	// config NVIC interrupts
  NVIC->IP[21] = 0x40;            // interrupt priority 2
  NVIC->ISER[0] = 1 << 21;              // Enable interrupt 21
	
}

void INIT_TIMERC (void) {
	SYSCTL->RCGCTIMER |= 0x04;    // Timer 2 = Timer C
  TIMER2->CTL = 0x00000000;     // disable this while configuring
	TIMER2->CFG = 0;              // enable 32-bit mode
	TIMER2->TAMR = 0x00000002;    // periodic mode
	TIMER2->TAPR = 0;             // no prescale
	TIMER2->TAILR = 40000000;     // 40,000,000 cycles is 500ms  arbitrary start, will be reset by TimerB_handler
	TIMER2->IMR = 0x00000000;     // disable interrupt for now, TimerB_Handler will set once freq calc
	TIMER2->CTL |= 0x00000001;    // enable timer1A, 32-bit, periodic

		// config NVIC interrupts
  NVIC->IP[23] = 0x40;            // interrupt priority 2
  NVIC->ISER[0] = 1 << 23;        // Enable interrupt 23
	
	
}

void TIMER1A_Handler(void) {
  int temp;
  // clear interrupt
	TIMER1->ICR = 0x00000001;
// Interrupt to Average Frequency Data and 

	// @80Mhz, 80000 cycles = 1ms
	// we want 0 to be slowest frequency 100 Hz
	// we want 4095 to be fastest frequency 1 kHz
	//for 100 Hz,
  //T = 1 / f = 1 / (100 Hz) = 10 ms
  //for 1 kHz, 
  //T = 1 / (1 kHz) = 1 ms
  // therefore (10ms to 1ms range) / 4096  = incremental timer value (in microsec)
  // 2.19726563 microseconds
	// so our desired timeout value = incremental timer value * adcsampleaverage

	temp = (adcsamples / 250.0) * 4.39560439560439;

	adcsamples = 0;  // reset our running total for next 250 samples

// Update Timer C
	TIMER2->CTL = 0x00000000; // disable while configuring
	TIMER2->TAILR = temp + 2000;  // 80000 cycles = 1ms at 80 Mhz
	TIMER2->IMR = 0x00000001; // enable interrupt
	TIMER2->CTL = 0x00000001; // enable while configuring
}

// Timer C timeout

// Send to I2C to update the output voltage of the DAC
void TIMER2A_Handler(void) {
	// clear timer interrupt
	TIMER2->ICR = 0x00000001;
  // Get next value of Sine array
	
  // send master code for high speed at fast mode speed
	//I2C0->MSA = 0x09;
	
  // Get next value of Sine array
	
	// address of DAC is 0x62 = 01100010
	I2C0->MSA = 0x62 << 1;     // LSB = 0 means Master writes
	I2C0->MDR = (sine_array[i%40] >> 8)&0x0F;
	I2C0->MCS = 0x00000003;    // Start and Run 	
  while(I2C0->MCS_I2C0_ALT & 0x00000001) {};  //wait
	I2C0->MDR = sine_array[i%40] & 0xFF;
	I2C0->MCS = 5;             // Stop 
	while(I2C0->MCS_I2C0_ALT & 0x00000001) {};  //wait
	
	i++;
/*	while(I2C0->MCS_I2C0_ALT & 0x00000001) {};  //wait	
	//I2C0->MTPR = 0x03;  // set timer period for high-speed mode  ??

	// address of DAC is 0x62 = 01100010
	I2C0->MSA = 0x09;     // LSB = 0 means Master writes
	I2C0->MCS = 0x00000013;    // Start and Run 	and start high-speed mode

  while(I2C0->MCS_I2C0_ALT & 0x00000001) {};  //wait
  I2C0->MSA = 0x62 << 1;     // LSB = 0 means Master writes
	I2C0->MDR = (sine_array[i%40] >> 8)&0xFF;
  I2C0->MCS = 0x00000003;    // Start and Run 

  while(I2C0->MCS_I2C0_ALT & 0x00000001) {};  //wait
	I2C0->MDR = sine_array[i%40] & 0xFF;
	I2C0->MCS = 5;             // Stop 
	while(I2C0->MCS_I2C0_ALT & 0x00000001) {};  //wait
	
	i++;*/
}
	
// Initialize I2C
void INIT_I2C(void){
// We want I2C Frequencey of 400 kHz (fast mode), so 400 kbps
// TPR = (System Clock/(2*(SCL_LP + SCL_HP)*SCL_CLK))-1;
// TPR = (80 MHz / (2*(6 + 4) * 1200000)) - 1 = 9
	#define TPR 9
	SYSCTL->RCGCI2C |= 0x0001;   // activate I2C0
	SYSCTL->RCGCGPIO |= 0x0002;  // activate port B
	while((SYSCTL->PRGPIO&0x0002) == 0){};  // ready?
	GPIOB->AFSEL |= 0x0C;       // enable alt func on PB2, 3 
	GPIOB->DEN |= 0x0C; 		    // enable digital I/O on PB2,3
		GPIOB->ODR |= 0x08;          // enable open drain PB3, i2c0 SDA
		//GPIOB->DIR |= 0x0C;          // outputs for PB2, 3
		//GPIOB->PUR |= 0x04;          // Pull-up on PB2, i2c0 SCL
		GPIOB->PCTL = (GPIOB->PCTL&0xFFFF00FF) + 0x00003300;  // I2C
		       
		I2C0->MCR = 0x00000010;    // master function enable
		I2C0->MTPR = TPR;          // cfg for 3.3 Mbps fast clock
		
}



int main(void)
{
	//your code here.

	INIT_PLL();
  INIT_ADC();
	INIT_I2C();
	INIT_TIMERC();
	INIT_TIMERB();
	
	
	
	while(1);
}
