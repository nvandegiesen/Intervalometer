#include <Wire.h>
#include "RTClib.h"
#include <SD.h>
#include <SPI.h>
#include <Filters.h>

/* -----------------------------------------*
 *            DECLARATIONS
 * -----------------------------------------*/

const char DeviceID[]    = "22";    // Unique board ID for the intervalometer (alphanumeric values, total unique IDs can be 1296 i.e. 36^2)

// INPUT/OUTPUT pins declaration
const int csPin          = 10;      // pin tO communicate with SD card
const int inputPin       = A5;      // pin that the sensor is attached to
const int indicatorPin   = 13;      // LED blinks if drop is detected
const int resetDrumPin   = 6;       // Pin to short out piezo drum

// Variables for file Name
char fileName[15];                  // Declare fileName
File intervalFile;                  // Declare intervalFile for Intervals

// Filter constants for Piezo data
const float HighFilterFrequency = 10.0;    // filters out changes slower that 10 Hz.
const float LowFilterFrequency  = 2000.0;  // filters out changes faster that 2 kHz.
FilterOnePole highpassFilter(HIGHPASS, HighFilterFrequency); // create a one pole (RC) highpass filter
FilterOnePole lowpassFilter(LOWPASS, LowFilterFrequency);    // create a one pole (RC) lowpass filter
const int threshold       = 8;             // threshold level to determine if drop has hit surface
const int under           = 1;             // Value used to reset for drift
int sensorValue           = 0;             // Variable to store reading from inputPin

// Variables for RTC
RTC_PCF8523 rtc;        // Define the RTC
DateTime now;

// Variables for timing
unsigned long currentTime      = 0;       // Time last drop fell (ms)
unsigned long lastCheckTime    = 0;       // Time since last alive text
const long aliveCheckInterval  = 1800000; // Time to check if the board is still alive and write voltage and time to SD card

/* -----------------------------------------*
 *            SETUP SECTION
 * -----------------------------------------*/
void setup()
{
  delay(5000);            // wait 5 sec before starting the code
  pinMode(resetDrumPin, OUTPUT); // Setting pin for short circuit as OUTPUT
  pinMode(indicatorPin, OUTPUT); // Pin to indicate error and drop detected.
  digitalWrite(indicatorPin, LOW);
  
  // SD card Section
  if (!SD.begin(csPin)) {
    digitalWrite(indicatorPin, HIGH);
    while(1);            // Wait here forever, if SD card have failed to initialise
  }
  
  // RTC Section
  if (!rtc.begin()) {
    digitalWrite(indicatorPin, HIGH);
    while(1);            // Wait here forever, if RTC have failed to initialise
  }

  now = rtc.now();  
  String timeString = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()) + " , " 
              + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
  
  // Making file name with unique ID (filename can be of maximum 8characters)
  // Name follow order (2characters of each): UniqueID, current day, current hour, current minute
  // eg. 00081144.txt
  sprintf(fileName, "%s%02d%02d%02d.txt",DeviceID,now.day(),now.hour(),now.minute());
  writeSDCard(String(now.unixtime()));
}

/* -----------------------------------------*
 *            LOOP SECTION
 * -----------------------------------------*/
void loop()
{
  // read the input on analog pin: which goes from 0 - 1023 (voltage 0V - 5V):
  sensorValue = lowpassFilter.input(highpassFilter.input(analogRead(inputPin)));
  currentTime = millis();              // Time stamp for current drop
  
   // If sensor Value is grater than threshold, a drop is detected
  if (sensorValue > threshold) {
    digitalWrite(indicatorPin, HIGH); // Turning LED on to show a drop is detected
    digitalWrite(resetDrumPin, HIGH); // Shorting piezo drum to stop it from vibrating and discharge energy
    
    writeSDCard(String(currentTime)); // Writing current time to SD card
    
    delay(20);                        // Wait of 20ms
    digitalWrite(resetDrumPin, LOW);  // Stop shorting piezo drum 
    digitalWrite(indicatorPin, LOW);  // Turning LED off to indicate process is complete and device is ready for another drop
  }
  
  // check if the meter is still alive every aliveCheckInterval and write to SD 
  if ((currentTime - lastCheckTime > aliveCheckInterval) && (currentTime >= lastCheckTime)) {
    now = rtc.now();
    String timeString = "t,"+String(now.year()) + "," 
                + String(now.month()) + "," + String(now.day()) + "," 
                + String(now.hour()) + "," + String(now.minute()) + "," 
                + String(now.second()) + "," + String(currentTime);
    
    writeSDCard(timeString);          // Writing constructed string to SD card
    lastCheckTime = millis();         // Updating the timer
  }
}

// Function to write to SD Card with provided value
void writeSDCard(String value)
{
  intervalFile = SD.open(fileName, FILE_WRITE);  // Opening File to write
  if(!intervalFile) {                            // Checking wheather file opened or not
    return;
  }
  intervalFile.println(value);                   // Writing value to SD card when file opened successfully
  intervalFile.close();                          // Closing file after writing value
}
