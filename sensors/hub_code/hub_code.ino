#include <esp_now.h>
#include <WiFi.h>
#include "ThingSpeak.h"
#include "secrets.h"

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
WiFiClient  client;
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
String macId = "";
int dir = -1;
bool newData = false;
// Initialize our values

void onReceiveData(const uint8_t *mac, const uint8_t *data, int len) {
  String id = "";
  for (int i = 0; i < 6; i++) {
 
    Serial.printf("%02X", mac[i]);
    if (i < 5)Serial.print(":");
    id+=String(mac[i], HEX);
    if (i < 5)id+=":";
  }

  int * messagePointer = (int*)data;
   
  Serial.println();

  Serial.println("New Data Recieved");
  newData = true;
  macId = id;
  dir = *messagePointer;
}

void setup() {
  
  Serial.begin(115200);
 
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
// Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
  esp_now_register_recv_cb(onReceiveData);
  Serial.println("\nReady to Go");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
  if (newData) {
     Serial.println("Got new data");
     newData = false;
     Send_Data(); 
  }
}

void Send_Data() {
  Serial.println(macId);
  Serial.println(dir);
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  // set the fields with the values
  ThingSpeak.setField(1, macId);
  ThingSpeak.setField(2, dir);
    
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  macId = "";
  dir = -1;
}
