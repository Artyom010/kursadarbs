#include <SPI.h>
#include <ESP8266WiFi.h>

byte ledPin = 2;
char ssid[] = "Smarthouse";           // SSID of your home WiFi
char pass[] = "Smarthouse";            // password of your home WiFi
const int button1Pin=D0;              //Defining button pins
const int button2Pin=D2;
const int button3Pin=D6;
int button1State = 0;
int button2State = 0;
int button3State = 0;
const int buzzer = D8; //buzzer to arduino pin D8
String message="";
void ultrasonic(){
  tone(buzzer, 10000); // Send 1KHz sound signal
  delay(5000);        // for 1 sec
  noTone(buzzer);     // Stop sound
  delay(1000);        // delay for 1sec
  Serial.println("Buzzer is on\r");
}

unsigned long askTimer = 0;

IPAddress server(192, 168, 43, 47);       // the fix IP address of the server
WiFiClient client;

void setup() {
  Serial.begin(115200);               // only for debug
  WiFi.begin(ssid, pass);             // connects to the WiFi router
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  pinMode(ledPin, OUTPUT);
  pinMode(button1Pin, INPUT); // Set button pins as an input
  pinMode(button2Pin, INPUT);
  pinMode(button3Pin, INPUT);
  pinMode(buzzer, OUTPUT); // Set buzzer pin as an output
}

void loop () {
   // Checking buttons state
  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);
  button3State = digitalRead(button3Pin);
  // Changing message for server depends on button state
  if(button1State==HIGH) message="light1"; 
  else if(button2State==HIGH) message="light2";
  else message="Hello server! Are you sleeping?\r";
  if(button3State==HIGH) ultrasonic();
  Serial.println("Message: "+message+"\r"); // Serial print to check are buttons vorking properly
  
  client.connect(server, 80);   // Connection to the server
  digitalWrite(ledPin, LOW);    // to show the communication only (inverted logic)
  Serial.println(".");
  client.println(message);  // sends the message to the server
  String answer = client.readStringUntil('\r');   // receives the answer from the sever
  Serial.println("from server: " + answer);
  if(answer=="buzzer") ultrasonic();
  client.flush();
  digitalWrite(ledPin, HIGH);
  delay(2000);                  // client will trigger the communication after two seconds
}
