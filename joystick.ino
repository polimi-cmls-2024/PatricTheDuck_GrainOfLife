#include <ArduinoOSCWiFi.h>  
#include <OSCMessage.h>  

int analogPin0 = A0; 
int analogPin1 = A1;   

int val0 = 0; 
int val1 = 0; 
int valButton = 0;
int count = 0;
boolean reset = false;

const char* ssid = "";
const char* password = "";
 
const char* serverIP = ""; 
const int serverPort = ;            

WiFiUDP udp;
OscMessage msg; 

//setup function, network initialization and udp socket construction
void setup() {
  pinMode(2, INPUT_PULLUP);
  Serial.begin(9600);

  //
  Serial.print("Connecting to WiFi network: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("Connected to the WiFi network with IP: ");
  Serial.println(WiFi.localIP());

  
  udp.begin(12000);  
}

//main loop:
// analog and digital read from the sensor
// count of cycles where the button is pressed
// value prints for debug 
// OSC messages declaration and initialization
// OSC messages send to the udp socket previously declared
void loop() {
  val0 = analogRead(analogPin0);
  val1 = analogRead(analogPin1);             
  if(!digitalRead(2) == HIGH){
    valButton = 1;
  }else{
    valButton = 0;
  }
  if(valButton==1){
    count++;
  }else{
    count=0;
  }
  if(count>=18){
    reset = true;
  }else{
    reset = false;
  }
  Serial.println(val0);    
  Serial.println(val1);
  Serial.println(valButton);
  Serial.println(reset);
  
  OSCMessage msg_coor("/coordinates");
  OSCMessage msg_but("/button");
  OSCMessage msg_res("/reset");
  msg_coor.add(val0);
  msg_coor.add(val1); 
  msg_but.add(valButton);   
  msg_res.add(reset);

  // coordinate state message
  if (udp.beginPacket(serverIP, serverPort)) {
    msg_coor.send(udp);
    udp.endPacket();
  }

  //button state message
  if (udp.beginPacket(serverIP, serverPort)) {
    msg_but.send(udp);
    udp.endPacket();
  }

  //reset message
  if (udp.beginPacket(serverIP, serverPort)) {
    if(reset==true){
      msg_res.send(udp);
    }
    udp.endPacket();
  }
  
  Serial.println("OSC message sent!");
}
