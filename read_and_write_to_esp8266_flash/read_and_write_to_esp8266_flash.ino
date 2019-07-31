/***
 * This example is modified for ESP8266 specifically.
 * Important link :
 * https://arduino.stackexchange.com/questions/33501/eeprom-put-and-get-not-working-on-esp8266-nodemcu
 * 
 * ESP8266 does not have EEPROM, instead it has flash.
 * we read and write to flash
 * It is important to use the EEPROM.begin(512);  and EEPROM.end() at the end. Or else it wont work.
 * EEPROM.commit ( ) moves the data from RAM to Flash. Or else it wont work.
 * 
    eeprom_put example.

    This shows how to use the EEPROM.put() method.
    Also, this sketch will pre-set the EEPROM data for the
    example sketch eeprom_get.

    Note, unlike the single byte version EEPROM.write(),
    the put method will use update semantics. As in a byte
    will only be written to the EEPROM if the data is actually
    different.

    Written by Christopher Andrews 2015
    Modified for ESP8266 and introduced a data structure to hold water flow in liters and milliliters.
    
    Released under MIT licence.
***/

#include <EEPROM.h>

/*struct MyObject {
  float field1;
  byte field2;
  char name[10];
};*/

struct { 
    uint litres = 0;
    uint milliliters = 0;
    char str[20] = "Wed Jul 31 10:30:20";
  } data;

void read_from_flash(){

  float f = 123.456f;  //Variable to store in EEPROM.
  int eeAddress = 0;   //Location we want the data to be put.

  EEPROM.begin(512);
  //Get the float data from the EEPROM at position 'eeAddress'
  EEPROM.get(eeAddress, f);
  Serial.println(f, 3);    //This may print 'ovf, nan' if the data inside the EEPROM is not a valid float.

  eeAddress += sizeof(float); //Move address to the next byte after float 'f'.

  EEPROM.get ( eeAddress, data );
  
  Serial.println("Read custom object from EEPROM: ");
  Serial.println(data.litres);
  Serial.println(data.milliliters);
  Serial.println(data.str);
  EEPROM.commit();
  EEPROM.end();
}

void write_to_flash(){

  float f = 123.456f;  //Variable to store in EEPROM.
  int eeAddress = 0;   //Location we want the data to be put.

  
  EEPROM.begin(512);
  //One simple call, with the address first and the object second.
  EEPROM.put(eeAddress, f);
  Serial.println("Written float data type!");

  data.litres = data.litres + 10; 
  data.milliliters = data.milliliters +10 ;
  strncpy(data.str,"",20);  

  eeAddress += sizeof(float); //Move address to the next byte after float 'f'.

  //EEPROM.put(eeAddress, customVar);
  EEPROM.put(eeAddress, data ); 
  Serial.print("Written custom data type! \n\nView the example sketch eeprom_get to see how you can retrieve the values!");

  EEPROM.commit();
  EEPROM.end();

}

void setup() {

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
}

void loop() {
  
  // read the last values in Flash first.
  read_from_flash();
  data.litres  += 10; 
  data.milliliters  += 10 ;
  strncpy(data.str,"",20);  
  write_to_flash();  
  delay(2000);
}
