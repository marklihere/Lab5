// Periodic Timer A  2ms
// Interrupt to trigger ADC conversion

// Interrupt when ADC finishes getting input
// Will copy voltage from ADC to our Frequency Data structure

// Periodic Timer B  500 ms
// Interrupt to Average Frequency Data and 
// Update Timer C

// Timer C
// Get next value of Sine array
// Send to I2C to update the output voltage of the DAC

// Initialize PLL to 16 MHz
// Initialize GPIOs
// Initialize ADC
// Initialize I2C

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
	while(1);
}
