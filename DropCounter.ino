/*
  DropCounter
    1 This program continuously reads the voltage on Pin A3.
    2 When the voltage exceed a set threshold, a drop is assumed to be detected. 
    3 The time interval in miliseconds between drops are calculated and reported.
    4 After a drop has been detected, Pin 6 and Pin 8 ar eset HIGH to shorten the the piezo and capacitor
    
    See https://github.com/nvandegiesen/Intervalometer/wiki/Intervalometer for the circuit and further explanations.
  
*/

const int analogPin = A3;    // pin that the sensor is attached to
const int threshold = 10;    // threshold level to determine if drop has hit surface
const int under = 1;         // Value used to reset for drift
int resetDrum = 8;           // Pin to short out piezo drum
int resetCap = 6;            // Pin to short out capacitor
int sensorValue = 0;         // Variable to store reading from analogPin
unsigned long time1;         // Time last drop fell (ms)
unsigned long time2;         // Time new drop fell (ms)
unsigned long time3;         // Auxilliary debug variable (ms)
unsigned long interval;      // Time interval between drop (ms) 

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(resetDrum, OUTPUT);   // Pin connected to piezo
  pinMode(resetCap, OUTPUT);    // Pin connected to capacitor
  time1 = millis();             // Start time

}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin:
  sensorValue = analogRead(analogPin);   // Analog reading, which goes from 0 - 1023 (voltage 0V - 5V):

  if (sensorValue > threshold) {
    time2 = millis();                 // Sets time drop fell
    
    digitalWrite(resetDrum, HIGH);    // Shorten piezo drum to stop it from vibrating
    digitalWrite(resetCap, HIGH);     // Discharge capacitor

    interval = time2 - time1;         // Interval between drops
    time1 = time2;                    // Reset old time

    Serial.println(interval);
    delay(1);
    digitalWrite(resetDrum, LOW);    // Stop shortening piezo drum 
    digitalWrite(resetCap, LOW);     // Stop discharging capacitor


/*
// Not sure if this is needed.
   } else if (sensorValue > under) {
    digitalWrite(resetDrum, HIGH);    // Shorten piezo drum to stop it from vibrating
    digitalWrite(resetCap, HIGH);     // Keep capacitor charge from drifting upwards (not sure if necessary)
    delay(1);
    digitalWrite(resetDrum, LOW);    // Stop shortening piezo drum 
    digitalWrite(resetCap, LOW);     // Stop discharging capacitor  
*/
    
  }

}

