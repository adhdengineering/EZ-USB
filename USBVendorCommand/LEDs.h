#ifndef LEDS_H_
#define LEDS_H_

void SetupLEDs();
void SetLEDState(unsigned char index, char state);
char GetLEDState(unsigned char index);

#endif /* LEDS_H_ */
