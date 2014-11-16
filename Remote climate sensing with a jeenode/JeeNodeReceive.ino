#include <JeeLib.h>
#define myNodeID 1      // RF12 node ID in the range 1-30
#define network 212      // RF12 Network group
#define freq RF12_868MHZ // Frequency of RFM12B module
//########################################################################################################################
//Data Structure to be received
//########################################################################################################################
typedef struct {
    int supplyV;	// Supply voltage
    int temp;	// Temperature reading
    int temp2;	// Temperature 2 reading
    int temp3;	// Temperature 3 reading
    int humi;	// Humidity reading
} Payload;
Payload temptx;
 
const int LEDpin = 4;	// a LED is connected between DIO1 and GND on the JeeNode
void setup(){
   Serial.begin(57600);
   Serial.print("Start");
   rf12_initialize(myNodeID,freq,network);
   pinMode(LEDpin, OUTPUT);	// Set the LEDpin as an output
   digitalWrite(LEDpin, 1);	// turn LED on
   delay(500);
   digitalWrite(LEDpin, 0);	// turn LED off
 
}
void loop(){
   if (rf12_recvDone() && rf12_crc == 0){
digitalWrite(LEDpin, 1);	// turn LED on
 
int numSensors = rf12_len/2 - 1;
const Payload* p = (const Payload*) rf12_data;
 
Serial.print("Voltage: "); Serial.print(p->supplyV / 100.); Serial.print("V ");
Serial.print("temp1: "); Serial.print(p->temp / 100.); Serial.print(" C ");
Serial.print("temp2: "); Serial.print(p->temp2 / 100.);Serial.print(" C ");
Serial.print("temp3: "); Serial.print(p->temp3 / 100.);Serial.print(" C ");
Serial.print("RV: "); Serial.print(p->humi); Serial.println("%");
 
digitalWrite(LEDpin, 0);	// turn LED off
   }
}