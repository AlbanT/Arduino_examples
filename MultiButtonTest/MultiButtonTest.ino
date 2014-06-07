/*
	Debounce multiple buttons (based on http://www.arduino.cc/en/Tutorial/Debounce)

	This example supports multiple buttons with a LED (I used: http://www.dx.com/p/diy-6-pin-push-button-switch-with-led-red-light-140572) using an array.
	If the button is pressed a LED is activated and starts blinking with a preset interval (use blinkInterval = 0 to stop the blinking) until the button is pressed again.
 
	Each time the input pin goes from LOW to HIGH (e.g. because of a push-button press), the output pin is toggled from LOW to HIGH or HIGH to LOW.  
	There's a minimum delay between toggles to debounce the circuit (i.e. to ignore noise).  
 
	The circuit:
	---Arduino---
	            |        __|__
	buttonPin o-|--------o   o-------o Gnd
	            |
	            |     //
	ledPin    o-|----|>----/\/\/\----o Gnd
	            |
	-------------
 

	This example code is in the public domain.
*/

// Variables will never change:
const byte buttonPin[] = {12};				// pins used by the buttons
const int NUMBUTTONS = sizeof(buttonPin);	// This handy macro lets us determine how big the array up above is, by checking the size
const byte ledPin[NUMBUTTONS] = {4};		// pins used by the status leds
const int debounceDelay = 50;				// the debounce time; increase if the output flickers


// Variables will change:
int ledState[NUMBUTTONS] = {LOW};			// the current state of the output pin
int buttonState[NUMBUTTONS] = {1};			// the current reading from the input pin
int lastButtonState[NUMBUTTONS] = {LOW};	// the previous reading from the input pin
long lastDebounceTime[NUMBUTTONS] = {0};	// the last time the output pin was toggled
long prevLedBlinkMillis[NUMBUTTONS] = {0};	// used to control the LED blink interval
int blinkState[NUMBUTTONS];					// the current state of the blinking
int blinkInterval = 250;					// determines the time (milliseconds) the LED is ON and OFF during blinking if set to 0 the LED stops blinking and -1 will disable the LED.

void setup() {
	Buttons_Begin();
}

void loop() {
	Buttons_Read();
}


void Buttons_Begin() {
	for (int i=0;i<NUMBUTTONS;i++) {
		// set the button pin as input
		pinMode(buttonPin[i], INPUT);

		// activate the internal pullup
		digitalWrite(buttonPin[i], HIGH);

		// set the ledpin as output
		pinMode(ledPin[i], OUTPUT);

		// set initial LED state
		digitalWrite(ledPin[i], ledState[i]);

		blinkState[i] = ledState[i];
	}
}

void Buttons_Read() {
	for (int i=0;i<NUMBUTTONS;i++) {
		// read the state of the switch into a local variable:
		int reading = digitalRead(buttonPin[i]);

		// check to see if you just pressed the button
		// (i.e. the input went from LOW to HIGH),  and you've waited
		// long enough since the last press to ignore any noise:  

		// If the switch changed, due to noise or pressing:
		if (reading != lastButtonState[i]) {
			// reset the debouncing timer
			lastDebounceTime[i] = millis();
		}
 
		if ((millis() - lastDebounceTime[i]) > debounceDelay) {
			// whatever the reading is at, it's been there for longer
			// than the debounce delay, so take it as the actual current state:

			// if the button state has changed:
			if (reading != buttonState[i]) {
				buttonState[i] = reading;

				// only toggle the LED if the new button state is HIGH
				if (buttonState[i] == HIGH) {
					ledState[i] = !ledState[i];
				}
			}
		}
 
		// set the LED:
		Buttons_SetLED(i, blinkInterval);

		// save the reading.  Next time through the loop,
		// it'll be the lastButtonState:
		lastButtonState[i] = reading;
	}
}

void Buttons_SetLED(int button, int interval) {	
	if (ledState[button] == HIGH) {
		// if the ledState is HIGH the LED starts blinking with the set interval

		if (interval < 0) {
			// if interval is set to -1, or any other negative value, the LED is completely disabled.
			digitalWrite(ledPin[button], LOW);
		}
		else {
			// get the current millis() to determine if the blink interval has passed
			unsigned long currentMillis = millis();
 
			// determine if the blink interval has passed
			if(currentMillis - prevLedBlinkMillis[button] > interval) {
				// save the last time you blinked the LED
				prevLedBlinkMillis[button]  = currentMillis;  

				// if the LED is off turn it on and vice-versa.
				// blinkState is used to preserve the ledState for other actions -> blinking is also ledState=HIGH...
				if (blinkState[button] == LOW) {
					blinkState[button] = HIGH;
				}
				else {
					blinkState[button] = LOW;
				}

				// set the LED with the ledState of the variable:
				digitalWrite(ledPin[button], blinkState[button]);
			}
		}
	}
	else {
		// if ledState is no longer HIGH turn off the LED. If omitted the led will remain ON or OFF depending on it's current blinkState.
		digitalWrite(ledPin[button], LOW);
		blinkState[button] = LOW;
	}
}

