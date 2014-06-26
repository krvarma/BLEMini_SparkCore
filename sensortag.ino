#define GAP_PROFILE_CENTRAL           0x08
#define KEYLEN                        16
#define B_ADDR_LEN                    6 
#define DEFAULT_DISCOVERY_WHITE_LIST  0x00
#define DEFAULT_LINK_HIGH_DUTY_CYCLE  0x00

#define BUILD_UINT16(loByte, hiByte) ((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#define DEVICE_INITIALIZED 0x600
#define DEVICE_DISCOVERY_DONE 0x601
#define DEVICE_LINK_ESTABLISHED 0x605
#define DEVICE_DISCONNECTED 0x606
#define DEVICE_NOTIFICATION_ENABLED 0x513
#define DEVICE_NOTIFICATION 0x51B

static uint8_t gapCentralRoleTaskId = 0;
static uint8_t  gapCentralRoleIRK[KEYLEN] = {0};
static uint8_t  gapCentralRoleSRK[KEYLEN] = {0};
static uint32_t gapCentralRoleSignCounter = 1;
static uint8_t  gapCentralRoleBdAddr[B_ADDR_LEN];
static uint8_t  gapCentralRoleMaxScanRes = 5;

uint8_t found_address[6];
uint8_t buf[64];

// publish the message
void publish(String message){
    Spark.publish("stag", message);
}

void setup(){Serial1.begin(57600);
    // Initialize hci
    hci_init();
}

void loop(){ 
    if(Serial1.available()){
        ble_event_process();
    }
}

// BLE Event Processing
byte ble_event_process(){
    uint8_t type, event_code, data_len, status1;
    uint16_t event;

    type = Serial1.read();
    delay(35);
    event_code = Serial1.read();
    data_len = Serial1.read();
  
    for (int i = 0; i < data_len; i++)
        buf[i] = Serial1.read();
    
    event = BUILD_UINT16(buf[0], buf[1]);
    status1 = buf[2];
  
    switch(event){
        case DEVICE_INITIALIZED:{
            hci_start_discovery();
            
            break;
        }
        case DEVICE_DISCOVERY_DONE:{
            uint8_t num_devs = buf[3];
      
            if (num_devs > 0){
                memcpy(found_address, &buf[6], 6);
          
                GAP_EstablishLinkReq();
            }
            else{
                publish("No device found.");
                
                delay(1000);
                
                hci_start_discovery();
            }
      
            break;
        }
        case DEVICE_LINK_ESTABLISHED:{
            GATT_WriteCharValue();
      
            break;
        }
        case DEVICE_DISCONNECTED:{
            publish("Device disconnected.");
            
            delay(1000);
            
            hci_start_discovery();
            
            break;
        }
        case DEVICE_NOTIFICATION_ENABLED:{
            publish("Notification enabled");
        
            break;
        }
        case DEVICE_NOTIFICATION:{
            uint8_t b = buf[6];
            uint8_t d = buf[8];
      
            if(0 == d){ // Buttons up
                publish("0");
            }
            else if(1 == d){ // Right Button down
                publish("1");
            }
            else if(2 == d){ // Left Button down
                publish("2");
            }
            
            break;
        }
    }
}

int hci_init(){
    return GAP_DeviceInit(gapCentralRoleTaskId, GAP_PROFILE_CENTRAL, gapCentralRoleMaxScanRes, gapCentralRoleIRK, gapCentralRoleSRK, &gapCentralRoleSignCounter);
}

int hci_start_discovery(){
    publish("Device discovery started...");
    
    return GAP_DeviceDiscoveryRequest();
}

int GAP_DeviceInit(uint8_t taskID, uint8_t profileRole, uint8_t maxScanResponses, uint8_t *pIRK, uint8_t *pSRK, uint32_t *pSignCounter){
    uint8_t len = 0;
    
    buf[len++] = 0x01;                  // -Type    : 0x01 (Command)
    buf[len++] = 0x00;                  // -Opcode  : 0xFE00 (GAP_DeviceInit)
    buf[len++] = 0xFE;
  
    buf[len++] = 0x26;                  // -Data Length
    buf[len++] = profileRole;           //  Profile Role
    buf[len++] = maxScanResponses;      //  MaxScanRsps
    memcpy(&buf[len], pIRK, 16);        //  IRK
    len += 16;
    memcpy(&buf[len], pSRK, 16);        //  SRK
    len += 16;
    memcpy(&buf[len], pSignCounter, 4); //  SignCounter
    len += 4;

    Serial1.write(buf, len);

    return 1;
}

int GAP_DeviceDiscoveryRequest(){
    uint8_t len = 0;
    
    buf[len++] = 0x01;                 // -Type    : 0x01 (Command)
    buf[len++] = 0x04;                 // -Opcode  : 0xFE04 (GAP_DeviceDiscoveryRequest)
    buf[len++] = 0xFE;
        
    buf[len++] = 0x03;                 // -Data Length
    buf[len++] = 0x03;                 //  Mode
    buf[len++] = 0x01;                 //  ActiveScan
    buf[len++] = 0x00;                 //  WhiteList
  
    Serial1.write(buf, len);
  
    return 1;
}

int GAP_EstablishLinkReq(){
    uint8_t len = 0;
    
    buf[len++] = 0x01;
    memcpy(&buf[len], "\x09\xFE", 2);
    len += 2;
  
    buf[len++] = 0x03 + B_ADDR_LEN;
  
    buf[len++] = 0x00;
    buf[len++] = 0x00;
    buf[len++] = 0x00;

    memcpy(&buf[len], found_address, B_ADDR_LEN);

    len+=B_ADDR_LEN;
 
    Serial1.write(buf, len); 
}

int GATT_WriteCharValue(){
    uint16_t connHandle = 0x0000;
    uint16_t handle = 0x006C;
    uint8_t value[2];
  
    uint8_t len = 0;
  
    value[0] = 0x01;
    value[1] = 0x00;  
  
    buf[len++] = 0x01;
    memcpy(&buf[len], "\x92\xFD", 2);
    len += 2;
  
    buf[len++] = 0x04 + 0x02;
  
    memcpy(&buf[len], &connHandle, 2);
    len += 2;
    memcpy(&buf[len], &handle, 2);
    len += 2;
    memcpy(&buf[len], value, 2);
    len += 2;
  
    Serial1.write(buf, len);

    return 1;
}