#include <SPI.h>
#include <ESP8266WiFi.h>
#include <Servo.h>

#define servoPin D4 //defining servo motors
#define servo2Pin D5
Servo servo;
Servo servo2;
bool light1state=0; // mainīgie, kuros glabājas informācija par gaismas slēdžu stāvokli
bool light2state=0;
bool light1lvl=0; // mainīgie, kuros glabājas informācija par apgaismojuma līmeni pie katra sensora
bool light2lvl=0;
bool buzz=0; // Ja sis mainīgais ir 1, tad programma sāk klausīties skaņas
bool d1=1; // mainīgie priekš buzzer funkcijas
bool d2=1;
bool d11=1;
bool d22=1;

int MOVEMENT2 = D0;
int LIGHT2 = D1;
int SOUND2 = D2;
int MOVEMENT1 = D6;
int LIGHT1 = D7;
int SOUND1 = D8;

int i=0, t=0, t1=0, t2=0, tlght=millis(), closer, ts1=0, ts2=0;



String message="Hi client.\r";
byte ledPin = 2;
char ssid[] = "Smarthouse";               // SSID of your home WiFi
char pass[] = "Smarthouse";               // password of your home WiFi
WiFiServer server(80);                    
IPAddress ip(192, 168, 43, 47);            // IP address of the server
IPAddress gateway(192,168,43,1);           // gateway of your network
IPAddress subnet(255,255,255,0);          // subnet mask of your network

void onlight1(){
    servo.attach(servoPin);
    servo.write(15);
    delay(500);
    servo.detach();
    Serial.println("Light 1 turned on");
    light1state=1;
}
void onlight2(){
    servo2.attach(servo2Pin);
    servo2.write(126);
    delay(500);
    servo2.detach();
    Serial.println("Light 2 turned on");
    light2state=1;
}  
void offlight1(){
    servo.attach(servoPin);
    servo.write(105);
    delay(500);
    servo.detach();
    Serial.println("Light 1 turned off");
    light1state=0;
}
void offlight2(){
    servo2.attach(servo2Pin);
    servo2.write(36);
    delay(500);
    servo2.detach();
    Serial.println("Light 2 turned off");
    light2state=0;
}

void setup() {
  Serial.begin(115200);                   // only for debug
  pinMode(MOVEMENT1, INPUT);
  pinMode(LIGHT1, INPUT);
  pinMode(SOUND1, INPUT);
  pinMode(MOVEMENT2, INPUT);
  pinMode(LIGHT2, INPUT);
  pinMode(SOUND2, INPUT);
  pinMode(ledPin, OUTPUT);
  onlight1(); //calibration of light switches
  offlight1();
  onlight2();
  offlight2();
  WiFi.config(ip, gateway, subnet);       // forces to use the fix IP
  WiFi.begin(ssid, pass);                 // connects to the WiFi router
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  server.begin();                         // starts the server
  Serial.println("Connected to wifi");
  
}

void loop () {
  WiFiClient client = server.available();
  if (client) {
    if (client.connected()) {
      digitalWrite(ledPin, LOW);  // to show the communication only (inverted logic)
      Serial.println(".");
      String request = client.readStringUntil('\r');    // receives the message from the client
      Serial.print("From client: "); 
      Serial.println(request);
      // if recieved message is light1, then programm changes 1st light state
      if(request=="light1"){
        if(light1state==0) {
          onlight1();
          delay(10000);
        }
        else {
          offlight1(); 
          delay(10000);
        }
      }
      // if recieved message is light2, then programm changes 2nd light state
      if(request=="light2"){
        if(light2state==0) {
          onlight2();
          delay(10000);        
        }
        else {
          offlight2();
          delay(10000); 
        }
      }
      client.flush();
      client.println(message); // sends the answer to the client
      //buzzer funkcija
       if(buzz){
          message="Hi client.\r";
          buzz=0;
          delay(2000);
          for(int i=0; i<5000; i++){
            if((digitalRead(SOUND1)==1)&&d1){
              ts1=millis();
              d1=0;
            }
            if((digitalRead(SOUND2)==1)&&d2){
              ts2=millis();
              d2=0;
            }
            if((digitalRead(SOUND1)==0)&&d11&&(d1==0)){
              ts1=millis()-ts1;
              d11=0;
            }
            if((digitalRead(SOUND2)==0)&&d22&&(d2==0)){
              ts2=millis()-ts2;
              d22=0;
            }
            delay(1);  
          }
          if(ts1>10000) ts1=0;
          if(ts2>10000) ts2=0;
          if((ts1==0)&&(ts2==0)){
            Serial.println("Istabā neviena nav");
            offlight1();
            offlight2();
          }
          else {
            if(ts1>ts2) closer=1;
            else closer=2;
            Serial.print("Signāls bija tuvāk ");
            Serial.print(closer);
            Serial.println(". sensoram");
            if(closer==1) onlight1();
            else onlight2();
          }
          while((digitalRead(MOVEMENT1)==0)&&(digitalRead(MOVEMENT2)==0)){
            delay(10);//programma neturpinās kāmēr kustības sensors nereģistrēs kustību
          }  
          d1=1;
          d2=1;
          d11=1;
          d22=1;
          closer=0;
          ts1=0;
          ts2=0;
        }
      digitalWrite(ledPin, HIGH);
    }
    client.stop();                // tarminates the connection with the client
  }
  if(buzz==0){
  // Pārbauda apgaismojuma līmeni ar intervālu vismaz 5 minutes un tikai tad, kad visas gaismas ir izslēgtas
  if((light1state==0)&&(light2state==0)&&((millis()-tlght)>300000)){
    light1lvl=digitalRead(LIGHT1);
    light2lvl=digitalRead(LIGHT2);
    tlght=millis();
  }
  // Ja pamana kustību, tad tajā sensora reģiona gaisma degs 1 minūti kopš kustības reģistrēšanas brīza, taču otras gaismas degšanas laiku(ja tā deg) samazina līdz 30 sek
  if(digitalRead(MOVEMENT1)==1){
    Serial.println("Kustība 1. zonā");
    t1=millis()+30000;
    if(t2>(millis()+15000)) t2= millis()+15000;
  }
  if(digitalRead(MOVEMENT2)==1){
    Serial.println("Kustība 2. zonā");
    t2=millis()+30000;
    if(t1>(millis()+15000)) t1= millis()+15000;
  }
  
  // Ja apgaismojuma līmenis ir atbilstoss gaismas ieslēgšanai, taimeris gaismas degsanai vel nav baidzies un slēdzis ir stavoklī izslēgts, tad gaisma tiek ieslēgta
  if((light1lvl==0)&&(t1>millis())&&(light1state==0)) onlight1();
  if((light2lvl==0)&&(t2>millis())&&(light2state==0)) onlight2();
  
  // Ja apgaismojuma līmenis ir neatbilstoss gaismas ieslēgšanai vai taimeris gaismas degsanai jau ir beidzies un slēdzis ir stavoklī ieslēgts, tad gaisma tiek izslēgta
  if(((light1lvl==1)||(t1<millis()))&&(light1state==1))offlight1();
  if(((light2lvl==1)||(t2<millis()))&&(light2state==1))offlight2();

  // Ja taimeris abam gaismam beidzies vai nu iet uz beigām, programma sūta komandu uz aproci ieslēgt buzzeri un sāk klausīties
  if((t1<=(millis()+5000))&&(t2<=(millis()+5000))&&((light1lvl==0)||(light2lvl==0))){
    buzz=1; //aktivē buzzer funkciju
    message="buzzer";
    Serial.println("Aktivēta buzzer funkcija");
  }
  }
  delay(400);
  
}
