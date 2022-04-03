#include "fix_fft.h"

char im[128], data[128]; 
int i = 0, n = 0, val,newval, freq, answ, avgpress = 0, lightning, pressure;
unsigned long myTime;
int PIRin= PD2;
int MOVEMENT = 6;
int LIGHT = 8;
int SOUND = 10;
int lightpin = A0;


int freqdetect(){
  for (i=0; i <128; i++)     //take 128 samples
  {
    val = analogRead(A3);
    data[i] = val/4 - 128;
    im[i] = 0;

    }
  fix_fft(data, im, 7, 0);    //FFT on data
  int dat = sqrt(data[49] * data[49] + im[49] * im[49]);
  return dat; 
}
int soundpress(){
  myTime=millis();
  while((millis()-myTime)<1000){
    avgpress+=freqdetect();
    n++;
  }
  answ=avgpress/n;
  avgpress=0;
  n=0;
  return answ;
}

void setup()
  {
    Serial.begin(115200);
    analogReference(DEFAULT);
    pinMode(PIRin, INPUT);
    pinMode(MOVEMENT, OUTPUT);
    pinMode(LIGHT, OUTPUT);
    pinMode(SOUND, OUTPUT);
  }

void loop()
  {
  // Processing data from IR sensor
  newval= digitalRead(PIRin);
  Serial.println(newval);
  if(1==newval) digitalWrite(MOVEMENT, HIGH);
  else digitalWrite(MOVEMENT, LOW);
  
  // Processing data from sound sensor
  freq= freqdetect();
  if(freq>6) {
    pressure = soundpress();
    Serial.println(pressure); 
    digitalWrite(SOUND, HIGH);
    delay(pressure*100);
    digitalWrite(SOUND, LOW);
  }
  
  // Processing data from lightning sensor
  lightning=analogRead(lightpin);
  Serial.println(analogRead(lightning));
  if(lightning>250) digitalWrite(LIGHT, HIGH);
  else digitalWrite(LIGHT, LOW);
  delay(50);


}
