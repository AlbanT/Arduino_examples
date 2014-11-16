#include <JeeLib.h>
#include <OneWire.h>
#include <DallasTemperature.h>
//const int TEMPERATURE_PRECISION = 9;
const int  ONE_WIRE_BUS = 5;	// DIO2 on je JeeNode = D5 on Arduino
const int  tempPower = A1;	// Power pin is connected to AIO2 on the JeeNode = A4 on Arduino
const int  minutes = 1;	// Duration of sleep between measurements, in minutes
const int debugging = 1;
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
 
 
#include <SHT1x.h>
 
// Specify data and clock connections and instantiate SHT1x object
// powerline for the SHT = tempPower!!!!
const int dataPin = 6;	// DIO3 on the JeeNode
const int clockPin = A2;	// AIO3 on the JeeNode
SHT1x sht1x(dataPin, clockPin);
float Temp_SHT;
float RV_SHT;
 
 
 
// interrupt handler for JeeLabs Sleepy power saving
ISR(WDT_vect) { Sleepy::watchdogEvent(); }	
//#define myNodeID 29 // RF12 node ID in the range 1-30
//#define network 210 // RF12 Network group
//#define freq RF12_868MHZ // Frequency of RFM12B module
byte myId;	// remember my own node ID
 
const int LEDpin = 4;	// a LED is connected between DIO1 and GND on the JeeNode
//########################################################################################################################
//Data Structure to be sent, it is variable in size and we only send 2+n*2 bytes where n is the number of DS18B20 sensors attached
//########################################################################################################################
typedef struct {
    int supplyV;	// Supply voltage
    int temp;	// Temperature reading
    int temp2;	// Temperature 2 reading
    int temp3;	// Temperature 3 reading
    int humi;	// Humidity reading
} Payload;
Payload temptx;
int numSensors;
//########################################################################################################################
void setup() {
// Initialize RFM12 with settings defined above
//rf12_initialize(myNodeID,freq,network);
myId = rf12_config();	// Initialize RFM12 with its internal settings
rf12_control(0xC000);	// Adjust low battery voltage to 2.2V
rf12_sleep(0);	// Put the RFM12 to sleep
PRR = bit(PRTIM1);	// only keep timer 0 going
ADCSRA &= ~ bit(ADEN);	// Disable the ADC
bitSet (PRR, PRADC);	// Power down ADC
bitClear (ACSR, ACIE);	// Disable comparator interrupts
bitClear (ACSR, ACD);	// Power down analogue comparator
pinMode(tempPower, OUTPUT);	// set power pin for DS18B20 to output
digitalWrite(tempPower, HIGH);	// turn sensor power on
Sleepy::loseSomeTime(50);	// Allow 50ms for the sensor to be ready
sensors.begin();	// Activate the one-wire connection to the DS18B20
numSensors=sensors.getDeviceCount() + 2;	// Get the number of parallel DS18B20 sensors in the network
 
pinMode(LEDpin, OUTPUT);	// Set the LEDpin as an output
FlashLED(50);	// Flash the LED to confirm that Setup has executed
}
void loop() {
digitalWrite(LEDpin, 1);	// turn the LED on to signal a measurement and transmission
pinMode(tempPower, OUTPUT);	// set power pin for DS18B20 to output
digitalWrite(tempPower, HIGH);	// turn DS18B20 sensor on
Sleepy::loseSomeTime(10);	// Allow 10ms for the sensor to be ready
 
 
sensors.requestTemperatures();	// Send the command to get temperatures
temptx.temp=(sensors.getTempCByIndex(0)*100);	// read sensor 1
// read second sensor.. you may have multiple and count them upon startup but I only need two
if (numSensors>1) {	
temptx.temp2=(sensors.getTempCByIndex(1)*100);
}
 
//SHT11 readings:
temptx.temp3 = (sht1x.readTemperatureC() * 100);
temptx.humi = sht1x.readHumidity();
digitalWrite(tempPower, LOW);	// turn DS18B20 sensor off
pinMode(tempPower, INPUT);	// set power pin for DS18B20 to input before sleeping, saves power
vccRead();	// Read current supply voltage
rfwrite();	// Send data via RF
digitalWrite(LEDpin, 0);	// Turn the LED off after transmission
 
for(byte j = 0; j < minutes; j++) {	// Sleep for x minutes
if (debugging == 1) {
Sleepy::loseSomeTime(10000);	//JeeLabs power save function: enter low power mode for 60 seconds (valid range 16-65000 ms)
}
else {
Sleepy::loseSomeTime(60000);	//JeeLabs power save function: enter low power mode for 60 seconds (valid range 16-65000 ms)
}
}
}
//--------------------------------------------------------------------------------------------------
// Send payload data via RF
//--------------------------------------------------------------------------------------------------
static void rfwrite(){
rf12_sleep(-1);	//wake up RF module
while (!rf12_canSend())
rf12_recvDone();
rf12_sendStart(0, &temptx, numSensors*2 + 2);	// two bytes for the battery reading, then 2*numSensors for the number of DS18B20s attached to Funky
rf12_sendWait(2);	//wait for RF to finish sending while in standby mode
rf12_sleep(0);	//put RF module to sleep
}
//--------------------------------------------------------------------------------------------------
// Reads current voltage
//--------------------------------------------------------------------------------------------------
void vccRead(){
  bitClear(PRR, PRADC);	// power up the ADC
  ADCSRA |= bit(ADEN);	// enable the ADC
  Sleepy::loseSomeTime(10);	// wait for 10ms
  temptx.supplyV = map(analogRead(6), 0, 1023, 0, 660);	// read the voltage
  ADCSRA &= ~ bit(ADEN);	// disable the ADC
  bitSet(PRR, PRADC);	// power down the ADC
}
 
//--------------------------------------------------------------------------------------------------
// Flash the LED for an amount of ms
//--------------------------------------------------------------------------------------------------
void FlashLED(int ms){
    digitalWrite(LEDpin, 1);	// turn LED on
Sleepy::loseSomeTime(ms);	// wait for x ms
digitalWrite(LEDpin, 0);	// turn LED off
}