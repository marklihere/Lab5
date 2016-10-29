# Lab5
AD &amp; DA


4a
What is the input voltage range for the DAC?
3-5 Volts
2.7V to 5.5V  (MCP4725 Datasheet, page 19)

4b
What is the function of the A0 pin of the DAC?
External Address Bit Selection Pin

Select a value for A0.
We choose 0, nothing attached to A0.


A0 = 0 means 0x62 address (7-bit address)
A1 = 1 means 0x63 address

4c
Draw a wiring diagram connecting your microcontroller to the needed components

Analog Test Board Potentiometer
Output: ADC1
   |
Input: PD3? (ADC1)
Micontroller
Output: PB2 (I2C0SCL), PB3 (I2C0SDA)
   |
Inputs:  SCL (i2c serial clock input) SDA (i2c serial data input)
  MCP4725 DAC   VCC = 5V
Output:  Vout (analog output voltage)
   |
Inputs: Audio_IN (Vout)
Analog Test Board   VCC = 3.3V, and 5V for LM386M-1
Outputs: None

TM4C ADC 12-bit straight binary, 0 to 3.3V range


// Assume we want 100 Hz to 1000 Hz
// What range of timer C Reload value does this equate to?


// Calc frequencies associated with 
0, 1023, 2047, 3071, and 4095 codes (12-bit) in the Freq Data array
say 0 is 100 Hz in our design
4095 is 1000 Hz

What do set sysClk to in order to get this range of frequencies for the output
f_sine = f_interrupt / n,  n is 40 in our case
       = ceil(1./(40*12e-6*((2^8-1) + 1 - timers)))   // timers means what we set the Timer B to
	   F_interrupt = 1 / wavelength of interrupt = 1 / (40 elements in array * (CC / array_element) * seconds / CC))
	     1 / ((40 / X MHz) *((2^16-1) + 1 - timer_reset value?))
	   = 
100 Hz * 40 = 4 kHz = f_interrupt
4kHz = 1 / (X*((2^8-1) + 1 - timers) ; X = SysClk, where Timers [2^16-1:0.2*2^16-1]


let's just disregard the lowest 20% of the timer reset range, because during this portion, the difference in frequencies corresponding to  adjacent timer set values is very large.

We want to choose a smooth output range of frequencies based on our discrete timer values that we can set, so we focus on the 2^16-1 to 0.2*2^16-1 range
Also, we can't choose a value of 1 for the timer because we would be interrupting our ISR
So now we solve for 100Hz at the highest 2^16-1 timer value, and solve for

4kHz = 1 / (X)
so X = 4 kHz

1kHz at 0.2*2^16-1

4kHz = 1 / (40*X*((2^16-1) + 1 - 0.2*(2^16-1))
X = 


// Finish this later

