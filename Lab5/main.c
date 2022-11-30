#include <msp430.h>

#define CALADC_15V_30C *((unsigned int *)0x1A1A) // Temperature Sensor Calibration-30 C
// See device datasheet for TLV table memory mapping
#define CALADC_15V_85C *((unsigned int *)0x1A1C) // Temperature Sensor Calibration-High Temperature (85 for Industrial, 105 for Extended)

void ConfigureAdc_temp();
float calculateTemp(float val);

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
    _delay_cycles(5);         // Wait for ADC Ref to settle
    ConfigureAdc_temp();
    float tempDegF = 0;
    float reading12 = 0;
    float reading5 = 0;
    while (1)
    {
        int channel = ADCMCTL0 & 0b1111; // Bitmasking to obtain the current ADC channel that's being used
        ADCCTL0 |= ADCENC | ADCSC;       // Sampling and conversion start
        while (!(ADCIFG & ADCIFG0))
            ; // Wait for sample to be sampled and converted
        switch (channel)
        {
        case 12:                                 // If channel 12/Internal temperature sensor,
            reading12 = ADCMEM0;                 // read the converted data into a variable
            tempDegF = calculateTemp(reading12); // Covnert reading to fahrenheit
            break;
        case 5:                 // If channel 5/external thermistor
            reading5 = ADCMEM0; // read the converted data into a variable
            break;
        default:
            break;
        }
        __no_operation(); // Only for debug
    }
}

// Configure ADC Temperature
void ConfigureAdc_temp()
{
    P1SEL0 |= BIT5;
    P1SEL1 |= BIT5;
    PM5CTL0 &= ~LOCKLPM5;

    ADCCTL0 |= ADCSHT_8 | ADCON;     // ADCON, S&H=16 ADC clks
    ADCCTL1 |= ADCSHP | ADCCONSEQ_1; // sampling timer
    /*
    ADCCONSEQ_1 sets the sampling mode to start at the specified channel and
    decrement after each conversion until channel 0, then restart at the specified input channel
    */
    ADCCTL2 &= ~ADCRES;                 // clear ADCRES in ADCCTL
    ADCCTL2 |= ADCRES_2;                // 12-bit conversion results
    ADCMCTL0 |= ADCSREF_1 | ADCINCH_12; // A1 ADC input select; Vref=AVCC
    ADCIE |= ADCIE0;                    // Enable ADC conv complete interrupt

    // Configure reference
    PMMCTL0_H = PMMPW_H;             // Unlock the PMM registers
    PMMCTL2 |= INTREFEN | TSENSOREN; // Enable internal reference and temperature sensor
}

float calculateTemp(float temp)
{
    volatile float IntDegF;
    volatile float IntDegC;
    IntDegC = (temp - CALADC_15V_30C) * (85 - 30) / (CALADC_15V_85C - CALADC_15V_30C) + 30;
    // Temperature in Fahrenheit
    // Tf = (9/5)*Tc | 32
    IntDegF = 9 * IntDegC / 5 + 32;
    return IntDegF;
}
