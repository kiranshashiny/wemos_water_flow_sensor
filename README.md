# wemos_water_flow_sensor

## Blog that shows wemos Microcontroller being used for water flow sensor.

###  Bill of Materials

	Water flow sensor 

	Wemos D1 R1 module

	Wires to connect the water flow sensor to the Microcontroller

	Software :
	
	MyMQTT app to be downloaded onto a Smartphone, subscribe for data

### Prerequisites :
	
	Open an account ( basic is Cute Cat Model) in Cloudmqtt.com and keep the credentials handy, as you will need that to be included in the code when publishing data from the Microcontroller to the cloud.

### Objective:
	
	To measure the amount of water flow in liters that flows thru a tap.


Code is included in this repo.

### Connections :
	
	The water flow sensor has 3 pins, Vcc, Gnd, and Signal. Connect the signal pin to D1



Some snapshots 



Principles of the working of Hall Sensor ( courtesy of author listed below )
This is how the Water flow sensor detects the amount of water that flows thru it.

![Screen Shot 2019-07-29 at 3 23 04 PM](https://user-images.githubusercontent.com/14288989/62039203-da1e0000-b214-11e9-9e2b-e5fb3a3f1544.png)

My Actual water flow sensor connected to the tap.
This is a YF-S201 1/2 inch water flow pipe.

![20190729_150313](https://user-images.githubusercontent.com/14288989/62038790-08e7a680-b214-11e9-884d-3ee653a99783.jpg)

Wemos D1 R1 connected to the water flow sensor

Vcc, GND and the sensor/signal pin of the water flow ( yellow wire)  is connected to D1 of the wemos

![20190729_150330](https://user-images.githubusercontent.com/14288989/62038789-08e7a680-b214-11e9-8ab1-07c880a8fa64.jpg)


Output of the serial monitor showing the water flow .

![20190729_150336](https://user-images.githubusercontent.com/14288989/62038788-084f1000-b214-11e9-8af3-02fbad13aaa3.jpg)

Snapshot of the data being sent to cloudmqtt.com thru the wemos microcontroller

The topic in this case is "waterFlow"

This is an offshoot of the Arduino IDE-> PubSubClient -> esp8266_mqtt 

You would install the PubSubClient library to the Arduino IDE ( How to download and install is not covered in this article ) 
![20190729_150350](https://user-images.githubusercontent.com/14288989/62038786-084f1000-b214-11e9-9ddd-16c159dfaeab.jpg)


This is a snapshot of the MyMQTT ap installed on my SmartPhone.
First connect, go to Settings -> enter the credentials provided to you in the cloudmqtt.com, and then go to Subscribe -> enter "waterFlow" 

Go to Dashboard and watch for the data from the cloud.

This will only be active when there is water flow in the tap.

![Screenshot_20190729-150537_MyMQTT](https://user-images.githubusercontent.com/14288989/62038784-07b67980-b214-11e9-9115-9401c572e610.jpg)



Code to catch switch or pulse bounce and handle it

https://forum.arduino.cc/index.php?topic=417406.0
 
		void debounceISR()
		{
		  static unsigned long last_interrupt_time = 0;
		  unsigned long interrupt_time = millis();
		  if (interrupt_time - last_interrupt_time > debounceInterval) //declare the debounce/block out interval in setup
		  {
		    pulseCount++;
		  }
		  last_interrupt_time = interrupt_time;
		 
		}
