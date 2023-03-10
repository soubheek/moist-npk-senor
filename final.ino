#include <SoftwareSerial.h>
#include <EEPROM.h>
 
#define RE A2 // RE pin of MAX485
#define DE A3 // DE pin for MAX485
 
const byte moist[] = {0x01, 0x03, 0x00, 0x02, 0x00, 0x02, 0x65, 0xcb}; // enquiry code for moisture
const byte nitro[] = {0x01, 0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c}; // enquiry code for nitrogen
const byte phos[] = {0x01, 0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc}; // enquiry code for phosphorous
const byte pota[] = {0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0}; // enquiry code for potassium
 
byte values[11];
SoftwareSerial npk(A0,A1); // RX, TX #software serial for npk sensor (connected RO, DI of MAX485)
SoftwareSerial most(A5,A4); // RX, TX #software serial for moisture sensor (connected RO, DI of MAX485) 



void setup() {
  Serial.begin(9600);  
  most.begin(9600); // SoftwareSerial for moisture sensor
  npk.begin(9600); // SoftwareSerial for NPK sensor

  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);

  EEPROM.update(0, 1); // Memmory location zero is for keeping record the last written values in the EEPROM location
}
 
void loop() {
  byte mos, nitro, phos, pota;
  
  most.listen();
  delay(50);
  mos = moisture(); // variable for moisture
  delay(250);

  mos = moisture(); // variable for moisture 
  delay(250);
  most.flush();

  npk.listen(); 
  phos = phosphorous(); // variable for phosphorous
  delay(250);

  phos = phosphorous(); // variable for phosphorous
  delay(250);
  npk.flush();

  nitro = nitrogen(); // variable for nitrogen
  delay(250);
  npk.flush();
  
  pota = potassium(); // variable for potassium
  delay(250);
  npk.flush();
  
  Serial.print("Moisture: ");
  Serial.print(((float(mos)/256)*100)); // MAx value is 255 and converted into percentage
  Serial.println(" %");

  Serial.print("Nitrogen: ");
  Serial.print(nitro);
  Serial.println(" mg/kg"); // Max value is 255

  Serial.print("Phosphorous: ");
  Serial.print(phos);
  Serial.println(" mg/kg"); // Max value is 255

  Serial.print("Potassium: ");
  Serial.print(pota);
  Serial.println(" mg/kg"); // Max value is 255

  delay(500);

  if(Serial.available() > 0) {
    switch (Serial.read()) {
      case 'W': // W = memory write
        mem_write();
        break;
      case 'R': // R = memory read
        mem_read();
        break;
      case 'D': // D = memory reset
        mem_reset();
        break;
    }
  }
}

// Function for the moisture value

byte moisture(){  
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(most.write(moist,sizeof(moist))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    values[i] = most.read();
    }
  }
  return values[4];
}

// Function for nitrogen value
 
byte nitrogen() {
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(npk.write(nitro,sizeof(nitro))==8) {
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    values[i] = npk.read();
    }
  }
  return values[4];
}

// Function for phosphorus value
 
byte phosphorous() {
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  //npk.listen();
  if(npk.write(phos,sizeof(phos))==8) {
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    values[i] = npk.read();
    }
  }
  return values[4];
}

// Function for potassium value
 
byte potassium() {
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  //npk.listen();
  if(npk.write(pota,sizeof(pota))==8) {
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    values[i] = npk.read();
    }
  }
  return values[4];
}

// Function for EEPROM memory write
 
byte mem_write() {

  byte addr = EEPROM.read(0);

  if(addr == 401) { // allocation 400 numbers of memory locations
    EEPROM.update(0, 1);
    addr = EEPROM.read(0);
  }

  byte mos, nitro, phos, pota;

  most.listen();
  mos = moisture();
  delay(250);
  mos = moisture(); // variable for moisture
  EEPROM.update(addr, ((float(mos)/256)*100)); // memory write for moisture value
  addr = addr + 1;
  delay(250);
  most.flush();

  npk.listen();
  nitro = nitrogen();
  delay(250);
  nitro = nitrogen(); // variable for nitrogen
  EEPROM.update(addr, nitro); //memory write for nitrogen value
  addr = addr + 1;
  delay(250);
  npk.flush();

  phos = phosphorous(); // variable for phosphorous
  EEPROM.update(addr, phos); //memory write for phosphorus value
  addr = addr + 1;
  delay(250);
  npk.flush();

  pota = potassium(); // variable for potassium
  EEPROM.update(addr, pota); // memory write for potassium value
  addr = addr + 1;
  delay(250);
  npk.flush();

  EEPROM.update(0, addr); // updating the last memory location written

  Serial.println("Data saved in the memory!");

  delay(3000);
}

// Function for EEPROM memory reading

byte mem_read() {
  int addr = 1;

  Serial.print("\n");
  
  for (addr = 1; addr <= 400; addr++) {

    if (EEPROM.read(addr) == 0) {
      if (addr == 1) {
        Serial.println("No data found!");
      }
      break;
    }
    
    Serial.print("Data Set Number: ");
    Serial.println((addr / 4) + 1);

    Serial.print("Moisture: ");
    Serial.print(EEPROM.read(addr));
    Serial.println(" %");  
    addr = addr + 1;

    Serial.print("Nitrogen: ");
    Serial.print(EEPROM.read(addr));
    Serial.println(" mg/kg");
    addr = addr + 1;

    Serial.print("Phosphorus: ");
    Serial.print(EEPROM.read(addr));
    Serial.println(" mg/kg");
    addr = addr + 1;

    Serial.print("Potassium: ");
    Serial.print(EEPROM.read(addr));
    Serial.println(" mg/kg");

    Serial.print("\n");
 }

 if (addr == 1) {
   delay(500);
 }
 else {
   delay(10000); //the programme will wait for 20 seconds to capture the data
 }  
}

// Function for EEPROM memory deletion

byte mem_reset() {
  EEPROM.update(0, 1); // address Zero is to remember the last reading location
  // four simultaneous memory locations for four 
  // different parameters, total 100 sets of data can be saved
  for (int i = 1; i <= 400; i++) { 
    EEPROM.update(i, 0);
  }
  Serial.println("Memory loation, data reset successfully!");
  delay(2000);
}