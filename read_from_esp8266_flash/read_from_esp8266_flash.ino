/***
    eeprom_get example.

    This shows how to use the EEPROM.get() method.

    To pre-set the EEPROM data, run the example sketch eeprom_put.
    This sketch will run without it, however, the values shown
    will be shown from what ever is already on the EEPROM.

    This may cause the serial object to print out a large string
    of garbage if there is no null character inside one of the strings
    loaded.

    Written by Christopher Andrews 2015
    Released under MIT licence.
***/

#include <EEPROM.h>

void setup() {

  float f = 0.00f;   //Variable to store data read from EEPROM.
  int eeAddress = 0; //EEPROM address to start reading from

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println ();
  Serial.print("Read float from EEPROM: ");
  EEPROM.begin(512);
  //Get the float data from the EEPROM at position 'eeAddress'
  EEPROM.get(eeAddress, f);
  Serial.println(f, 3);    //This may print 'ovf, nan' if the data inside the EEPROM is not a valid float.
  
  struct { 
    uint litres = 30;
    uint milliliters = 350;
    char str[20] = "Wed Jul 31 10:30:20";
  } data;

  eeAddress += sizeof(float); //Move address to the next byte after float 'f'.

  EEPROM.get ( eeAddress, data );
  
  Serial.println("Read custom object from EEPROM: ");
  Serial.println(data.litres);
  Serial.println(data.milliliters);
  Serial.println(data.str);
  
  EEPROM.end();
  
}

void loop() {
  /* Empty loop */
}
