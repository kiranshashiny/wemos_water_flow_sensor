/* EEPROM Write
 *
 * Stores values and string to EEPROM.
 * These values and string will stay in the EEPROM when the board is
 * turned off and may be retrieved later by another sketch.
 */
 
#include <EEPROM.h>
 
// the current address in the EEPROM (i.e. which byte
// we're going to write to next)
uint addr = 0;
// fake data
struct { 
    uint litres = 30;
    uint milliliters = 350;
    char str[20] = "Wed Jul 31 10:30:20";
} data;

void read_write_eeprom() {
  EEPROM.get(addr,data);
  Serial.println ();
  Serial.println("Old water usage: "+String(data.litres)+" L,"+ String(data.milliliters) + " mL, " + String(data.str));

  // replace values in byte-array cache with modified data
  // no changes made to flash, all in local byte-array cache
  EEPROM.put(addr,data);

  // actually write the content of byte-array cache to
  // hardware flash.  flash write occurs if and only if one or more byte
  // in byte-array cache has been changed, but if so, ALL 512 bytes are 
  // written to flash
  EEPROM.commit();  

  // clear 'data' structure
  data.litres = data.litres + 10; 
  data.milliliters = data.milliliters +10 ;
  strncpy(data.str,"",20);

  // reload data for EEPROM, see the change
  //   OOPS, not actually reading flash, but reading byte-array cache (in RAM), 
  //   power cycle ESP8266 to really see the flash/"EEPROM" updated
  EEPROM.get(addr,data);
//  Serial.println("New values are: "+String(data.litres)+","+String(data.str));
  Serial.println("Total Water Usage: "+String(data.litres)+" Liters ,"+ String(data.milliliters) + " mL " + String(data.str));


  
}
void setup()
{
  Serial.begin(9600);
  // read bytes (i.e. sizeof(data) from "EEPROM"),
  // in reality, reads from byte-array cache
  // cast bytes into structure called data
  EEPROM.get(addr,data);
  Serial.println ();
  Serial.println("Old water usage: "+String(data.litres)+" Liters,"+ String(data.milliliters) + " mL, " + String(data.str));

  // replace values in byte-array cache with modified data
  // no changes made to flash, all in local byte-array cache
  EEPROM.put(addr,data);

  // actually write the content of byte-array cache to
  // hardware flash.  flash write occurs if and only if one or more byte
  // in byte-array cache has been changed, but if so, ALL 512 bytes are 
  // written to flash
  EEPROM.commit();  

  // clear 'data' structure
  data.litres = 800; 
  data.milliliters = 980;
  strncpy(data.str,"",20);

  // reload data for EEPROM, see the change
  //   OOPS, not actually reading flash, but reading byte-array cache (in RAM), 
  //   power cycle ESP8266 to really see the flash/"EEPROM" updated
  EEPROM.get(addr,data);
//  Serial.println("New values are: "+String(data.litres)+","+String(data.str));
  Serial.println("Total Water Usage: "+String(data.litres)+" Liters,"+ String(data.milliliters) + " mL " + String(data.str));
  
}
 
void loop()
{ read_write_eeprom();
  //We dont have anything in loop as EEPROM writing is done only once
  delay(1000);   
}
