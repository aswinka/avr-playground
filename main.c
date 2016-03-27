/*
    _   ___   ___   ___     _   _ 
   /_\ |   \ / __| | _ \___| |_(_)
  / _ \| |) | (__  |  _/ _ \  _| |
 /_/ \_\___/ \___| |_| \___/\__|_|
 (http://patorjk.com/software/taag/#p=display&f=Small&t=ADC%20Poti)

 This program uses ADC with Auto Trigger to turn on/off an LED 
 by using the analog signal of an rotary potentiometer. If the
 signal is half od the reference the LED goes off and on 
 otherwise.

*/
#include<avr/io.h>
#include <avr/interrupt.h>

void initLED ();
void initADC ();
void turnLEDOn ();
void turnLEDOff ();

/**
 * This is the boundary at which the LED turns off or on. If the 
 * poti values goes below, the LED turns off. In this program
 * we only use the 8-bit value, instead of the possible 10-bit
 * resolution. Therefore, the boundary is set to the half of the 
 * possible max value of 255. 
 */
#define LIMIT 127

int main (void)
{
    initLED();
    initADC();

    while (1) {
	// On this pin, there's a second LED, which I've used to debug.
	// Below, the LED will be turned off and
	// turned on by the ISR of the ADC interrupt. THis is causing
	// the LED to shine much darker which was an indication that
	// the ISR is working.
	PORTB &= ~(1<<PB0);
    }

    return 0;
}

// Interrupt Service Routine called by the ADC Interrupt
// To make use of it, the ADIE flag must be set.
ISR (ADC_vect)
{
    PORTB ^= 0x01;
    if (ADCH >= LIMIT) {
	turnLEDOn();
    } else {
	turnLEDOff();
    }
}

/**
 * Turn LED on 
 * Reset pin 1 on port B.
 * Note: This is the LED we want to control using the signal of the Poti.
 */
void turnLEDOff ()
{
    PORTB &= ~0x02;
}

/**
 * Turn LED on.
 * Set pin 1 on port B.
 */
void turnLEDOn ()
{
    PORTB |= 0x02;
}

/**
 * Initiate LED.
 */
void initLED ()
{
    // LED to control on pin 1 port B.
    DDRB |= 0x02;
    // Debug LED on pin 0 port B.
    DDRB |= 0x01;
}

/**
 * Initiate ADC (Analog Digital Conversion)
 */
void initADC ()
{
    // We use "AVcc with external capacitor at Aref pin"
    // Note: I didn't use a capacitor here, which was working fine, though.
    // Note: Seems like the AVcc works with +5V
    ADMUX = (1<<REFS0);
    // We need the low byte only, see page 250 and ADMUX bit 5 on page 248
    ADMUX |= (1<<ADLAR);
    // We use a prescaler with factor 128
    ADCSRA = (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
    // Enable Auto Trigger "ADATE" which is required when using ADTS2:0 (ADC Auto Trigger Source),
    // in this case, we use Free Running Mode, which is configured by default
    ADCSRA |= (1<<ADATE);
    // Enable ADC Interrupt
    ADCSRA |= (1<<ADIE);
    // Enable ADC
    ADCSRA |= (1<<ADEN);
    // Start first conversion, which takes longer.
    ADCSRA |= (1<<ADSC);
    // Enable global interrupts.
    sei();
}
