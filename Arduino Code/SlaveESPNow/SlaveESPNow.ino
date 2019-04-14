/**
   ESPNOW - Basic communication - Slave
   Date: 26th September 2017
   Author: Arvind Ravulavaru <https://github.com/arvindr21>
   Purpose: ESPNow Communication between a Master ESP32 and a Slave ESP32
   Description: This sketch consists of the code for the Slave module.
   Resources: (A bit outdated)
   a. https://espressif.com/sites/default/files/documentation/esp-now_user_guide_en.pdf
   b. http://www.esploradores.com/practica-6-conexion-esp-now/

   << This Device Slave >>

   Flow: Master
   Step 1 : ESPNow Init on Master and set it in STA mode
   Step 2 : Start scanning for Slave ESP32 (we have added a prefix of `slave` to the SSID of slave for an easy setup)
   Step 3 : Once found, add Slave as peer
   Step 4 : Register for send callback
   Step 5 : Start Transmitting data from Master to Slave

   Flow: Slave
   Step 1 : ESPNow Init on Slave
   Step 2 : Update the SSID of Slave with a prefix of `slave`
   Step 3 : Set Slave in AP mode
   Step 4 : Register for receive callback and wait for data
   Step 5 : Once data arrives, print it in the serial monitor

   Note: Master and Slave have been defined to easily understand the setup.
         Based on the ESPNOW API, there is no concept of Master and Slave.
         Any devices can act as master or salve.
*/

#include <esp_now.h>
#include <WiFi.h>
#include <Servo.h>

//Global copy of Master
esp_now_peer_info_t slave;
//uint8_t mac_master[8];
//uint8_t mac_master[] = {0x3c,0x71,0xbf,0x10,0x17,0xac};
//uint8_t mac_master[] = {0x30,0xae,0xa4,0x11,0x11,0x11};-----

Servo myservo;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position

#define CHANNEL 1
int LED = 2;
unsigned long previousMillis = 0;  // will store last time target poped up
unsigned long currentMillis = 0;
const long interval = 10000; //Interval at which to retract
bool LEDToggle =false;
bool firstCallFromMaster = true;
bool mailToSend = false;
bool targetSetFlag = false;
//int targetHitZone = 0;
uint8_t slaveNumberID = 0;
uint8_t targetHitData = 0;

//Interupt Stuff----------------------------------
const byte interruptPinBullsEye = 0;
volatile int interruptCounter = 0;
int numberOfInterrupts = 0;

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;


const byte interruptPinOuterRing = 15;

//-------------------------------------------------



//uint8_t data = 0;
// send data
void sendData(uint8_t data) {
  //data = targetHitZone;
  const uint8_t *peer_addr = slave.peer_addr;
  Serial.print("Sending: "); Serial.println(data);
  esp_err_t result = esp_now_send(peer_addr, &data, sizeof(data));
  Serial.print("Send Status: ");
  if (result == ESP_OK) {
    Serial.println("Success");
  } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
    // How did we get so far!!
    Serial.println("ESPNOW not Init.");
  } else if (result == ESP_ERR_ESPNOW_ARG) {
    Serial.println("Invalid Argument");
  } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
    Serial.println("Internal Error");
  } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
    Serial.println("ESP_ERR_ESPNOW_NO_MEM");
  } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
    Serial.println("Peer not found.");
  } else if (result == ESP_ERR_ESPNOW_IF ){
    Serial.println("current WiFi interface doesn’t match that of peer");
  } else {
    Serial.println("Not sure what happened");
  }
}



void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
   
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: "); Serial.println(macStr);
  Serial.print("Last Packet Send Status: "); Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }

  //memset(&slave, 0, sizeof(slave));
  //for (int i = 0; i < 6; ++i)
    //slave.peer_addr[i] = (uint8_t)mac_master[i];
  //slave.channel = CHANNEL; // pick a channel
  //slave.encrypt = 0; // no encryption
  //const esp_now_peer_info_t *peer = &slave;
  //const uint8_t *peer_addr = slave.peer_addr;
  //esp_err_t addStatus = esp_now_add_peer(peer);
}

// config AP SSID
void configDeviceAP() {
  char* SSID = "Slave_1";
  bool result = WiFi.softAP(SSID, "Slave_1_Password", CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}

void IRAM_ATTR handleInterruptBullsEye() {
  portENTER_CRITICAL_ISR(&mux);
  targetSetFlag = false;
  targetHitData = 0b0000010 | (slaveNumberID << 4);
  //targetHitZone = 1;
  mailToSend = true;
  portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR handleInterruptOuterRing() {
  portENTER_CRITICAL_ISR(&mux);
  targetSetFlag = false;
  targetHitData = 0b0000100 | (slaveNumberID << 4);
  //targetHitZone = 2;
  mailToSend = true;
  portEXIT_CRITICAL_ISR(&mux);
}

void setup() {
  myservo.attach(13);  // attaches the servo on pin 13 to the servo object

  //Interupt Stuff------------------
  pinMode(interruptPinBullsEye, INPUT_PULLUP);
  pinMode(interruptPinOuterRing, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPinBullsEye), handleInterruptBullsEye, RISING);
  attachInterrupt(digitalPinToInterrupt(interruptPinOuterRing), handleInterruptOuterRing, RISING);
  //--------------------------------
  pinMode(LED,OUTPUT);
  
  Serial.begin(115200);
  Serial.println("ESPNow/Basic/Slave Example");
  //Set device in AP mode to begin with
  WiFi.mode(WIFI_MODE_APSTA);//WiFi.mode(WIFI_AP);
  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);

  esp_now_register_send_cb(OnDataSent);
}

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  if(firstCallFromMaster){
    slaveNumberID = *data >>4;
    Serial.println("ha");
    memset(&slave, 0, sizeof(slave));
    for (int i = 0; i < 6; ++i)
      slave.peer_addr[i] = (uint8_t)mac_addr[i]; //(uint8_t)mac_master[i];
    slave.channel = CHANNEL; // pick a channel
    slave.encrypt = 0; // no encryption
    const esp_now_peer_info_t *peer = &slave;
    const uint8_t *peer_addr = slave.peer_addr;
    esp_err_t addStatus = esp_now_add_peer(peer);
    firstCallFromMaster = false;
  }
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Last Packet Recv Data: "); Serial.println(*data);
  Serial.println("");
  //LEDToggle = !LEDToggle;
  //if (LEDToggle == true){
  digitalWrite(LED,HIGH);
  myservo.write(10); // Raise target
  previousMillis = millis();
  targetSetFlag = true;
 
}

void loop() {

  if(targetSetFlag){
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      mailToSend = true;
      targetHitData = 0b0000001 | (slaveNumberID <<4 );
      targetSetFlag = false;
      //Serial.println(targeHitData);
    }
  }
  
  
  if(mailToSend){
    portENTER_CRITICAL(&mux); //May need to remove?????????????????
    digitalWrite(LED,LOW);
    myservo.write(100); // Lower target
    sendData(targetHitData);
    mailToSend = false;
    Serial.println("test");
    portEXIT_CRITICAL(&mux); //May need to remove??????????????
  }
  //sendData();
  //delay(1000);
  // Chill
}
