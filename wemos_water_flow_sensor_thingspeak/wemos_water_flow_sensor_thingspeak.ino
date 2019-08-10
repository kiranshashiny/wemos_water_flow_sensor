#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include <TM1637Display.h>
#include <EEPROM.h>
#include "ThingSpeak.h"

unsigned long myChannelNumber = 843620;
const char * myWriteAPIKey = "YOUR_API_WRITE_KEY";


// For LCD 16x2 - The pins are to be connected to SDA(D2) and SCL (D1) of Wemos
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// for LED TM1637 - 7 segment display
// Module connection pins (Digital Pins)
#define CLK D2 // ( of the wemos )
#define DIO D3 // ( ditto )

// for LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// The amount of time (in milliseconds) between tests
#define TEST_DELAY   2000

TM1637Display display(CLK, DIO);

byte statusLed    = 13;

byte sensorInterrupt = D5;  // 0 = digital pin 2

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 4.5;

volatile byte pulseCount;  

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;

// Update these with values suitable for your network.

const char* ssid = "JioFiber-ahGu7";
const char* password = "YOUR_ROUTER_PWD";
const char* mqtt_server = "m12.cloudmqtt.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


// the current address in the EEPROM (i.e. which byte
// we're going to write to next)
uint addr = 0;
// water flow data
struct { 
    uint litres = 0;
    uint milliliters = 0;
    char str[20] = "Wed Jul 31 10:30:20";
} data;

void read_write_eeprom() {
  
  float f = 123.456f;  //Variable to store in EEPROM.
  int eeAddress = 0;   //Location we want the data to be put.

  EEPROM.begin(512);
  Serial.print ("In Read data from EEPROM:, printing the float value : ");

  //Get the float data from the EEPROM at position 'eeAddress'
  EEPROM.get(eeAddress, f);
  Serial.println(f, 3);    //This may print 'ovf, nan' if the data inside the EEPROM is not a valid float.

  eeAddress += sizeof(float); //Move address to the next byte after float 'f'.

  EEPROM.get ( eeAddress, data );
  
  Serial.print ("Reading custom object from EEPROM:  Liters ");
  Serial.print (data.litres);
  Serial.print ( " L " );
  Serial.print (data.milliliters);
  Serial.print ( " mL" );
  Serial.print(data.str);
  Serial.println ();
  EEPROM.commit();
  EEPROM.end();

  // write data back.

  f = 123.456f;  //Variable to store in EEPROM.
  eeAddress = 0;   //Location we want the data to be put.

  // if there was data in the flash earlier then use it, and no new content - dont touch flash
  if  (  data.milliliters != 0 && totalMilliLitres == 0) {  //indicates that there was earlier data in flash,
    //set global val to value in flash
    totalMilliLitres = data.milliliters ;
    return;
  }
  // if there was NO data and NO water flow - dont update.
  if  (  data.milliliters == 0 && totalMilliLitres == 0)  //indicates not started 
     return;
  
  // else continue
  EEPROM.begin(512);
  //One simple call, with the address first and the object second.
  EEPROM.put(eeAddress, f);
  Serial.println("Written float data type!");
    
  //data.litres = totalMilliLitres/1000;
  data.milliliters = totalMilliLitres ;
  strncpy(data.str,"",20);  
  
  eeAddress += sizeof(float); //Move address to the next byte after float 'f'.

  EEPROM.put(eeAddress, data ); 

  EEPROM.commit();
  EEPROM.end();

}


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  lcd.clear();
  lcd.print ("Connecting ...");
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    Serial.println (clientId);
    if (client.connect(clientId.c_str(), "oxefqvkn", "uTM7RdarxTPA" )) {
//   if (client.connect("ESP8266Client", "oxefqvkn", "uTM7RdarxTPA" )) {

      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  
  // Initialize a serial connection for reporting values to the host
  Serial.begin(9600);
  ThingSpeak.begin(espClient);


  // LCD 
  lcd.begin();
  // Turn on the blacklight and print a message.
  lcd.backlight();

  
  setup_wifi();
  client.setServer(mqtt_server, 19757);
  client.setCallback(callback); 
  // Set up the status LED line as an output
  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, HIGH);  // We have an active-low LED attached
  
  //pinMode(sensorPin, INPUT);
  //digitalWrite(sensorPin, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;

  // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
  // Configured to trigger on a FALLING state change (transition from HIGH
  // state to LOW state)
  attachInterrupt(digitalPinToInterrupt(sensorInterrupt), pulseCounter, FALLING);

  // LED
  display.setBrightness(0x0f);
  display.clear();

}

/**
 * Main program loop
 */
void loop()
{
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if((millis() - oldTime) > 1000)    // Only process counters once per second
  { 
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(digitalPinToInterrupt(sensorInterrupt));
        
    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    
    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();
    
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;
    
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
      
    unsigned int frac;
    
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print("L/min");
    Serial.print("\t");       // Print tab space

    // Print the cumulative total of litres flowed since starting
    Serial.print("Output Liquid Quantity: ");        
    Serial.print(totalMilliLitres);
    Serial.print(" mL"); 
    //Serial.print("\t");       // Print tab space
    Serial.print (" ,");
    Serial.print(totalMilliLitres/1000);
    Serial.print("L");
    Serial.println();
    //snprintf (msg, 75, "hello world #%ld", value);
    snprintf (msg, 75, "totalMillilitres %d mL,  %d L", totalMilliLitres, totalMilliLitres/1000);

    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("waterFlow", msg);

    // display on LED 
    display.showNumberDec(totalMilliLitres, true); // Expect: 0301
    //ThingSpeak.writeField(myChannelNumber, 1, (float)totalMilliLitres, myWriteAPIKey);

    lcd.clear();
    // display on LCD
    lcd.setCursor ( 0,0);
    lcd.print("Usage ");
    //lcd.setCursor ( 7,0);
    lcd.print(  int( totalMilliLitres/1000)  );
    lcd.print ( " L " );

    //lcd.setCursor (11, 0);
    lcd.print ( int( totalMilliLitres) );
    //lcd.setCursor ( 14,0);
    lcd.print ( "mL");
    lcd.setCursor ( 0,1 );
    float  x = totalMilliLitres;
    x = x /1000 ;
    lcd.print ( x );
    lcd.print ("mL");
    // Update the water flow data back to EEProm
    
    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;
    //read_write_eeprom();
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(digitalPinToInterrupt(sensorInterrupt), pulseCounter, FALLING);
  }
}

/*
Insterrupt Service Routine
 */
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}
