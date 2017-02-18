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
uint8_t J1708Checksum = 0;

char hexDisp[10];


uint8_t J1708RXmessage(uint8_t (&J1708RXFrame)[256]) {

  if (Serial3.available()){
    J1708RXtimer = 0; //Reset the RX message timer for J1708 message framing
    J1708ByteCount++; // Increment the recieved byte counts
    if (J1708ByteCount < sizeof(J1708RXbuffer)){ //Ensure the RX buffer can handle the new messages
       J1708RXbuffer[J1708ByteCount] = Serial3.read();
       J1708Checksum += J1708RXbuffer[J1708ByteCount];
    }
    else{
      //This is what we do if we don't have room in the RX buffer.
      Serial.println("J1708 Buffer Overflow");
      Serial3.clear();
      J1708ByteCount = 0;
    }
  }

  //Check to see if a message has been framed?
  if (J1708RXtimer > 2190 && J1708ByteCount > 0){ //21 bit times is 2.19 ms
    J1708FrameLength = J1708ByteCount;
    J1708ByteCount = 0; //reset the counter
    
    J1708Checksum -= J1708RXbuffer[J1708FrameLength]; //remove the received Checksum byte (last one)
    J1708Checksum = (~J1708Checksum + 1) & 0xFF; // finish calculating the checksum
    boolean J1708ChecksumOK = J1708Checksum == J1708RXbuffer[J1708FrameLength];
      
    if (J1708ChecksumOK) {
      return J1708FrameLength;
    }
    else {
      Serial.println ("Checksum failed, Message has errors.");
      return 0; //data would not be valid, so pretend it didn't come
    }
  }
  else
    return 0; //A message isn't ready yet.
}

void setup() {
  // put your setup code here, to run once:
 
  //This is needed for testing with an ECM
  pinMode(ignitionCtlPin,    OUTPUT);
  digitalWrite(ignitionCtlPin,HIGH);

  //The J1708 tranceiver is connected to UART3
  Serial3.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if ( J1708RXmessage(J1708RXbuffer) > 0 ){ //Execute this if the number of recieved bytes is more than zero.
    Serial.print(micros());
    Serial.print(" ");
    for (int i = 0; i < J1708FrameLength; i++){
       J1708Checksum += J1708RXbuffer[i];
       sprintf(hexDisp,"%02X ",J1708RXbuffer[i]);
       Serial.print(hexDisp);
    }
    Serial.println();
  }
}



