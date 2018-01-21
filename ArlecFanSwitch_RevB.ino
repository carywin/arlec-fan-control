/*
Serial to UHF remote converter

Initially for controlling Arlec ceiling fan/light combo

Cary Wintle - Jan 2018

*/

/* Version History
 *  2: Light state added
 *  RevB: Decoding protocol rather than comparing codes
 *  1: Initial release
*/

/* TO-DO:
 *  Implement timer command detection/sending
 */

// Software Configuration
#define HEARTBEAT_TIME 5000 // 5 seconds between heartbeats
#define HB_TIMEOUT 30000 // 30 seconds without heartbeat = host not valid

// Command Codes
#define C_TOGGLE_LIGHT 1586958336
#define C_FAN_OFF 1586972682
#define C_FAN_1 1586974976
#define C_FAN_2 1586962944
#define C_FAN_3 1586963200
#define C_FAN_4 1586963456
#define C_FAN_5 1586963712
#define C_FAN_6 1586963968


// ---- Global Variables ----
boolean hostValid = false; // Flag indicates that we're connected to a valid host
boolean lightState = false;

// Timing
uint32_t hbTime, lastHBReceived, toggleTime;

#include <RCSwitch.h>
RCSwitch uhf = RCSwitch();

void setup() {

  Serial.begin(115200);
  
  uhf.enableTransmit(5); // Transmitter is connected to pin #5
  uhf.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2

  uhf.setProtocol(2, 450);
  uhf.setRepeatTransmit(2);
}

void loop() {

  // Process received UHF commands
  if (uhf.available()) {
    uint32_t rx = uhf.getReceivedValue();
    uint32_t fanSpeed = rx & 0x700; // Mask fan speed bits
    fanSpeed >>= 8;
    switch (fanSpeed) {
      case 0:
        Serial.println("#F0");
      break;
      case 1:
        Serial.println("#F1");
      break;
      case 2:
        Serial.println("#F2");
      break;
      case 3:
        Serial.println("#F3");
      break;
      case 4:
        Serial.println("#F4");
      break;
      case 5:
        Serial.println("#F5");
      break;
      case 6:
        Serial.println("#F6");
      break;
      default:
      break;
    }
    uint32_t cmd = rx & 0x7000; // Mask command bits
    cmd >>= 12;
    switch (cmd) {
      case 1:
        if (millis() - toggleTime > 500) {
          lightState = !lightState;
          if (lightState) Serial.println("#L1");
          else Serial.println("#L0");
          toggleTime = millis();
        }
      break;
    }
    uhf.resetAvailable();
  }

  // Process received Serial commands
  if (Serial.available() >= 4) {
    char serialData[10];
    Serial.readBytesUntil('\n', serialData, 4);
    
    if (serialData[0] = '#') { // Check for start of packet symbol
      switch (serialData[1]) {
        case 'L': // Light command
          switch (serialData[2]) {
            case '0':
              if (lightState) {
                sendUHF(C_TOGGLE_LIGHT);
                lightState = false;
              }
            break;
            case '1':
              if (!lightState) {
                sendUHF(C_TOGGLE_LIGHT);
                lightState = true;
              }
            break;
            case 'T':
              lightState = !lightState;
            break;
            default:
            break;
          }
        break;
        case 'F': // Fan command
          switch (serialData[2]) {
            case '0':
              sendUHF(C_FAN_OFF);
            break;
            case '1':
              sendUHF(C_FAN_1);
            break;
            case '2':
              sendUHF(C_FAN_2);
            break;
            case '3':
              sendUHF(C_FAN_3);
            break;
            case '4':
              sendUHF(C_FAN_4);
            break;
            case '5':
              sendUHF(C_FAN_5);
            break;
            case '6':
              sendUHF(C_FAN_6);
            break;
            default:
            break;
          }
        break;
        case 'H':
          if (serialData[2] == 'B') {
            // Heartbeat signal
            lastHBReceived = millis();
            hostValid = true;
          }
        break;
        default:
        break;
      }
    }
  }

  if (millis() - hbTime > HEARTBEAT_TIME) {
    hbTime = millis();
    Serial.println("#HB");
  }

  if (hostValid && millis() - lastHBReceived > HB_TIMEOUT) {
    hostValid = false;
  }
  
} // end loop()

void sendUHF(uint32_t data) {
  if (hostValid) uhf.send(data, 32);
}

