//#include "J1708.h"
/****************************************************************/
/*                         Pin Defintions                       */
const int greenLEDpin       = 2;
const int redLEDpin         = 5;
const int ignitionCtlPin    = 53;
const int buttonPin         = 24;

//Set up a timer to run after each byte is received.
elapsedMicros J1708RXtimer; 

boolean J1708FrameComplete = 0;
uint8_t J1708Frame[21];

boolean greenLEDstate = false;
boolean redLEDstate = true;
boolean LEDstate = true;
boolean ignitionCtlState = false;
int J1708ByteCount;

uint8_t J1708RXbuffer[256];
uint8_t J1708FrameLength = 0;


char hexDisp[10];

void setup() {
  // put your setup code here, to run once:
  pinMode(greenLEDpin,       OUTPUT);
  pinMode(redLEDpin,         OUTPUT);
  pinMode(ignitionCtlPin,    OUTPUT);
  digitalWrite(ignitionCtlPin,HIGH);

  Serial.begin(9600);
  //The J1708 tranceiver is connected to UART 1
  Serial3.begin(9600);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial3.available()){
    J1708RXtimer = 0;
    J1708ByteCount++;
    redLEDstate = !redLEDstate;
    digitalWrite(redLEDpin,redLEDstate);
    if (J1708ByteCount < sizeof(J1708RXbuffer)){
       J1708RXbuffer[J1708ByteCount] = Serial3.read();
    }
    else{
      Serial.println("J1708 Buffer Overflow");
      Serial3.clear();
      J1708ByteCount = 0;
    
    }
    
  }
  
  
  if (J1708RXtimer > 2190 && J1708ByteCount > 0){ //21 bit times is 2.19 ms
    J1708FrameComplete = true;
    J1708FrameLength = J1708ByteCount;
    J1708ByteCount = 0;
  }
  
  if (J1708FrameComplete) {
    uint8_t J1708Checksum = 0;
    for (int i = 0; i < J1708FrameLength; i++){
       J1708Checksum += J1708RXbuffer[i];
       sprintf(hexDisp,"%02X ",J1708RXbuffer[i]);
       Serial.print(hexDisp);
       
    }
    sprintf(hexDisp,"%02X ",J1708RXbuffer[J1708FrameLength]);
    Serial.print(hexDisp);
    Serial.print("Checksum: ");
    J1708Checksum = (~J1708Checksum + 1) & 0xFF;
    
    sprintf(hexDisp,"%02X",J1708Checksum );
    Serial.print(hexDisp);

    boolean J1708ChecksumOK = J1708Checksum == J1708RXbuffer[J1708FrameLength];
    if (J1708ChecksumOK) Serial.println ();
    else Serial.println (" Checksum failed, Message has errors.");
    
    
    J1708FrameComplete = false;
    
  }
  
}
