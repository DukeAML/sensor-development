//// Complete Instructions to Get and Change ESP MAC Address: https://RandomNerdTutorials.com/get-change-esp32-esp8266-mac-address-arduino/

//Libs for espnow and wifi
#include <esp_now.h>
#include <WiFi.h>
// User Buttons
#define USER_BT1      27

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
int x = 10;
/*
 * Interrupt Definitions
 */
volatile uint8_t bt1Pressed = 0;
long debouncing_time = 1000; //Debouncing Time in Milliseconds
volatile unsigned long last_micros;


void setup() {
  Serial.begin(115200);  
  Serial.println("Sentry Active");
  pinMode(USER_BT1, INPUT_PULLUP);

  Serial.begin(115200);
 
  WiFi.mode(WIFI_STA);

  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_peer_info_t peerInfo;
   
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  
  // Some Delay for things to calm down
  delay(30);

  // Add Interrupts
  attachInterrupt(digitalPinToInterrupt(USER_BT1), debouncingInterrupt, CHANGE);
}

void loop() {
  // Do Nothing
}

void debouncingInterrupt() {
  if((long)(micros() - last_micros) >= debouncing_time * 1000) {
    Interrupt();
    last_micros = micros();
  }
}

void Interrupt() {
  Serial.println("Registered MFing Interrupt");
  x = digitalRead(USER_BT1);
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &x, sizeof(int));
     
  if (result == ESP_OK) {
      Serial.println("Sent with success");
  }
  else {
      Serial.println("Error sending the data");
  }
}
