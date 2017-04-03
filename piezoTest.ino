
bool peak = false;
int peaks = 0;
const int dropTime = 250;
const int inPin = 2;
unsigned long lastDropTime = 0;
const unsigned long maxTime = 5000;

void setup() {
 pinMode(inPin, INPUT);
 //pinMode(13, OUTPUT);
 //digitalWrite(13,LOW);
 Serial.begin(9600);
 lastDropTime = millis();
}

void loop() {
  if (digitalRead(inPin)==HIGH){
    peaks++;
    long startTime = millis();
    while ((millis()-startTime)<dropTime){
      if (digitalRead(inPin)==HIGH && !peak){
        peak=true;
        peaks++;
      }else if (digitalRead(inPin)==LOW && peak){
        peak=false;
      }
    }
    int dropInterval = millis()-lastDropTime-dropTime;
    lastDropTime = millis();
    Serial.print(dropInterval);
    Serial.print(" milliseconds since last drop. nr of peaks: ");
    Serial.println(peaks);
    peaks = 0;
  }
  if ((millis() - lastDropTime) == maxTime){
    lastDropTime = millis();
    Serial.print("no drops for ");
    Serial.print(maxTime);
    Serial.println(" milliseconds");
  }
}
