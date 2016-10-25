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
