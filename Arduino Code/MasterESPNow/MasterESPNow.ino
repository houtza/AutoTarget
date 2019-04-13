/**
   ESPNOW - Basic communication - Master
   Date: 26th September 2017
   Author: Arvind Ravulavaru <https://github.com/arvindr21>
   Purpose: ESPNow Communication between a Master ESP32 and multiple ESP32 Slaves
   Description: This sketch consists of the code for the Master module.
   Resources: (A bit outdated)
   a. https://espressif.com/sites/default/files/documentation/esp-now_user_guide_en.pdf
   b. http://www.esploradores.com/practica-6-conexion-esp-now/

   << This Device Master >>

   Flow: Master
   Step 1 : ESPNow Init on Master and set it in STA mode
   Step 2 : Start scanning for Slave ESP32 (we have added a prefix of `slave` to the SSID of slave for an easy setup)
   Step 3 : Once found, add Slave as peer
   Step 4 : Register for send callback
   Step 5 : Start Transmitting data from Master to Slave(s)

   Flow: Slave
   Step 1 : ESPNow Init on Slave
   Step 2 : Update the SSID of Slave with a prefix of `slave`
   Step 3 : Set Slave in AP mode
   Step 4 : Register for receive callback and wait for data
   Step 5 : Once data arrives, print it in the serial monitor

   Note: Master and Slave have been defined to easily understand the setup.
         Based on the ESPNOW API, there is no concept of Master and Slave.
         Any devices can act as master or salve.


  // Sample Serial log with 1 master & 2 slaves
      Found 12 devices 
      1: Slave:24:0A:C4:81:CF:A4 [24:0A:C4:81:CF:A5] (-44)
      3: Slave:30:AE:A4:02:6D:CC [30:AE:A4:02:6D:CD] (-55)
      2 Slave(s) found, processing..
      Processing: 24:A:C4:81:CF:A5 Status: Already Paired
      Processing: 30:AE:A4:2:6D:CD Status: Already Paired
      Sending: 9
      Send Status: Success
      Last Packet Sent to: 24:0a:c4:81:cf:a5
      Last Packet Send Status: Delivery Success
      Send Status: Success
      Last Packet Sent to: 30:ae:a4:02:6d:cd
      Last Packet Send Status: Delivery Success

*/

#include <esp_now.h>
#include <WiFi.h>
#include <HardwareSerial.h>

// Global copy of slave
#define NUMSLAVES 20
#define CHANNEL 1//was 3
#define PRINTSCANRESULTS 0

esp_now_peer_info_t slaves[NUMSLAVES] = {};
HardwareSerial MySerial(1);

unsigned long previousMillis = 0;
long interval =0;
int  x;

int waitForPhoneInitiation = 0;
int setUpTargets = 1;
//int waitForStartFromPhone = 2;
int beginGame = 2;
int gameState = 0;
int gameMode = 0;

int SlaveCnt = 0;
int LED = 2;
bool scanForSlaveFlag = true;
bool LEDToggle =false;
bool mailToSend = false;
bool tempFags = true;
byte rxbyte = 0;  // stores received byte from HC-06 bluetooth module
byte HCO6_tx_byte = 0;  // stores sent byte to HC-06 bluetooth module
byte sendToPhoneData =0;

char macAddressList[20][18];
// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    //Serial.println("ESPNow Init Success");
  }
  else {
    //Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

// Scan for slaves in AP mode
void ScanForSlave() {
  int8_t scanResults = WiFi.scanNetworks();
  //reset slaves
  memset(slaves, 0, sizeof(slaves));
  SlaveCnt = 0;
  //Serial.println("");
  if (scanResults == 0) {
    //Serial.println("No WiFi devices in AP Mode found");
  } else {
    //Serial.print("Found "); Serial.print(scanResults); Serial.println(" devices ");
    for (int i = 0; i < scanResults; ++i) {
      // Print SSID and RSSI for each device found
      String SSID = WiFi.SSID(i);
      int32_t RSSI = WiFi.RSSI(i);
      String BSSIDstr = WiFi.BSSIDstr(i);

      if (PRINTSCANRESULTS) {
        //Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(")"); Serial.println("");
      }
      delay(10);
      // Check if the current device starts with `Slave`
      if (SSID.indexOf("Slave") == 0) {
        // SSID of interest
        //Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(")"); Serial.println("");
        // Get BSSID => Mac Address of the Slave
        int mac[6];

        if ( 6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x%c",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5] ) ) {
          for (int ii = 0; ii < 6; ++ii ) {
            slaves[SlaveCnt].peer_addr[ii] = (uint8_t) mac[ii];
          }
        }
        slaves[SlaveCnt].channel = CHANNEL; // pick a channel
        slaves[SlaveCnt].encrypt = 0; // no encryption
        SlaveCnt++;
      }
    }
  }

  if (SlaveCnt > 0) {
    //Serial.print(SlaveCnt); Serial.println(" Slave(s) found, processing..");
  } else {
    //Serial.println("No Slave Found, trying again.");
  }

  // clean up ram
  WiFi.scanDelete();
}

// Check if the slave is already paired with the master.
// If not, pair the slave with master
void manageSlave() {
  if (SlaveCnt > 0) {
    for (int i = 0; i < SlaveCnt; i++) {
      const esp_now_peer_info_t *peer = &slaves[i];
      const uint8_t *peer_addr = slaves[i].peer_addr;
      //Serial.print("Processing: ");
      for (int ii = 0; ii < 6; ++ii ) {
        //Serial.print((uint8_t) slaves[i].peer_addr[ii], HEX);
        if (ii != 5);// Serial.print(":");
      }
      //Serial.print(" Status: ");
      // check if the peer exists
      bool exists = esp_now_is_peer_exist(peer_addr);
      if (exists) {
        // Slave already paired.
        //Serial.println("Already Paired");
      } else {
        // Slave not paired, attempt pair
        esp_err_t addStatus = esp_now_add_peer(peer);
        if (addStatus == ESP_OK) {
          // Pair success
          //Serial.println("Pair success");
        } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
          // How did we get so far!!
          //Serial.println("ESPNOW Not Init");
        } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
          //Serial.println("Add Peer - Invalid Argument");
        } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
         // Serial.println("Peer list full");
        } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
          //Serial.println("Out of memory");
        } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
          //Serial.println("Peer Exists");
        } else {
          //Serial.println("Not sure what happened");
        }
        delay(100);
      }
    }
  } else {
    // No slave found to process
   // Serial.println("No Slave found to process");
  }
}


//uint8_t data = 0;
// send data
void sendData(byte data) { //void sendData(long slaveNumber) {
  int slaveNumber = data >> 4;
  uint8_t dataToSend = (uint8_t)data; 
  //data++;
 
   const uint8_t *peer_addr = slaves[slaveNumber-1].peer_addr;
    
    esp_err_t result = esp_now_send(peer_addr, &dataToSend, sizeof(data));
    //Serial.print("Send Status: ");
    if (result == ESP_OK) {
      //Serial.println("Success");
    } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
      // How did we get so far!!
      //Serial.println("ESPNOW not Init.");
    } else if (result == ESP_ERR_ESPNOW_ARG) {
      //Serial.println("Invalid Argument");
    } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
      //Serial.println("Internal Error");
    } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
      //Serial.println("ESP_ERR_ESPNOW_NO_MEM");
    } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
      //Serial.println("Peer not found.");
    } else {
      //Serial.println("Not sure what happened");
    }
    delay(100);
  
}

// callback when data is sent from Master to Slave
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], (mac_addr[5]+1));
  //Serial.print("Last Packet Sent to: "); Serial.println(macStr);
  //Serial.print("Last Packet Send Status: "); Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}


void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  byte tmp = mac_addr[5]+1;
  byte tmpMacPlusOne = 0;
  byte hitData = *data;
  //snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           //mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], tmp);
  //Serial.print("Last Packet Recv from: "); 
  //Serial.println(macStr);
 // Serial.print("Last Packet Recv Data: "); Serial.println(*data);
  //Serial.println("");
  int i =-1;
  bool senderFound = false;
  //Serial.println(SlaveCnt);
  while(!senderFound and i<=SlaveCnt){
    i++;
    senderFound = true;
    
    //snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           //slaves[i].peer_addr[0], slaves[i].peer_addr[1], slaves[i].peer_addr[2], slaves[i].peer_addr[3], slaves[i].peer_addr[4], slaves[i].peer_addr[5]);
    //Serial.println(macStr);
   
    if(mac_addr[0] != slaves[i].peer_addr[0]){
      senderFound = false;      
    }
    
    if(mac_addr[1] != slaves[i].peer_addr[1]){
      senderFound = false;
    }
    
    if(mac_addr[2] != slaves[i].peer_addr[2]){
      senderFound = false;
    }
    
    if(mac_addr[3] != slaves[i].peer_addr[3]){
      senderFound = false;
    }
   
    if(mac_addr[4] != slaves[i].peer_addr[4]){
      senderFound = false;
    }
   
    tmpMacPlusOne = mac_addr[5]+1;
    
    if(tmpMacPlusOne != slaves[i].peer_addr[5]){
      senderFound = false;
    }
  }
  if(senderFound){
    //Serial.print(sendToPhoneData);
    sendToPhoneData = (i+1)<<4;
    sendToPhoneData =sendToPhoneData | (hitData << 1);///////////////////////
    Serial.println(sendToPhoneData);//////////////////////////////////
  }
  mailToSend = true;
  digitalWrite(LED,HIGH);
 
}


void setup() {
  pinMode(LED,OUTPUT);
  MySerial.begin(9600, SERIAL_8N1, 16, 17);
  Serial.begin(115200);
  //Serial.begin(115200);
  //uint8_t new_mac[8] = {0x30,0xAE,0xA4,0x11,0x11,0x11};//---------------------------------------------
  //esp_base_mac_addr_set(new_mac);
  //Set device in STA mode to begin with
  WiFi.mode(WIFI_STA);
  //Serial.println("ESPNow/Multi-Slave/Master Example");
  // This is the mac address of the Master in Station Mode
  //Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  rxbyte = 0b00000000;
  if (MySerial.available()) {  //Read serial sent from app
    //---------------------------------------------------
    rxbyte = MySerial.read();
    x = rxbyte; 
    Serial.print("f");
    Serial.println(x);
    if(gameMode == beginGame){  //If the game has started
      manageSlave();
      sendData(rxbyte);        //Send data to slave
    }
  }
  
  if(gameState == waitForPhoneInitiation && (rxbyte & 0b00001111) == 0b00001111){  //If 15 sent from app
    gameState = setUpTargets;
    gameMode = (rxbyte & 0b11110000) >> 4; //Set game mode, not important
  }

  
  if(scanForSlaveFlag && gameState == setUpTargets){  //Perform once before entering entering game state
    scanForSlaveFlag = false;
    ScanForSlave(); //In the loop we can scan at the beginning for slaves 1 time 
    //delay(10000);  
    manageSlave();
    gameMode = beginGame;

    //for(int slaveNum = 0;slaveNum<SlaveCnt;slaveNum++){ //Get slave mac addresses, and store all of them as a string in a 2D char array
       //uint8_t *mac_addr = slaves[slaveNum].peer_addr;      
    //}
       
    //sendData(random(SlaveCnt)); //Activate a randome slave (pop up)////
    
    byte slaveCount = (byte)SlaveCnt;  //Number of targets
       
    slaveCount = (slaveCount <<4) | 0b00001111;
    //Serial.write(slaveCount); 
    delay(2000);
    MySerial.write(slaveCount); //Send target count to app
    
    x = slaveCount; 
    Serial.print("e");
    Serial.println(x);   
    delay(2000);
    MySerial.write(0b11111111); //Tell app to enter game mode    
    delay(10);   
  }
  
  
  
 
  if (SlaveCnt > 0 && mailToSend) { // Update targets on app
    
    delay(10);
    
    MySerial.write(sendToPhoneData);

     x = sendToPhoneData; 
    Serial.print("e");
    Serial.println(x);
    //sendData(random(SlaveCnt));---------
    digitalWrite(LED,LOW);
    delay(10);
    mailToSend = false;
    
    
    
    
  } else {
    // No slave found to process
  }

  
  
}
