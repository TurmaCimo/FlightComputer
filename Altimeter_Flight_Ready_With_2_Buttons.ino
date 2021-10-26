/* 
 * Opens File on SD Card and Writes Altimeter Data.
 * Uses 2 status indicator LEDs (one for sensor, and one for SD card). 
 * Yellow is standby, red is error/problem, green is good. 
 * Currently coded for 2 Botton Operation
   * button_WRITE_SD_card begins writing Altimeter Data to SD Card
   * button_WRITE_SD_card card safely by ending file. 

 **** DO NOT power off module while SD card is writing ****
      This will cause loss of data. 

      * Must hit EJECT Button to end file and safely remove card. 
      * Wait for yellow standby on status LED. Green means it is 
      * actively writing data (may change to continuous blinking green)   

 * Can change Sting Variable "fileName" below 

 * Will open file by same name if file by that name already exists
   and write new data at end of previous data. 

* Module can be wired for power switch or not (No change to code)

*/


#include <SPI.h>  // Serial Peripheral Interface Library
#include <SD.h>  // SD Card Library

#include <Wire.h> // Allows I2C connection with sensor board below

// Libraries for 3 sensors on Pololu AltiMU-10 v5 sensor board

#include <LPS.h> // Baromteric Altimeter Library
#include <LIS3MDL.h> // 3-axis Magnetometer Library
#include <LSM6.h> // 3D Accelerometer and Gyro Library

// Initialize file and sensor variables

File myFile;
String fileName = "flight.txt"; // Can change name here if desired
LPS pressureSensor;

// define pins for LEDs

#define sensorGREEN 3
#define sensorRED 5
#define cardRED 6
#define cardGREEN 9

// Initialize Button ON/OFF Switches

int button_WRITE_SD_card = 2;
int button_EJECT_SD_card = 4;

// Blink Function Prototype. Defined Below Loop

void blinkFunction(String LED, int redValue, int greenValue, int amountBlinks);

void setup() { // Setup Function For Sensor and SD Card

 // LED setup
 // Make defined pins as output
  
  pinMode(sensorRED, OUTPUT);
  pinMode(sensorGREEN, OUTPUT);
  pinMode(cardRED, OUTPUT);
  pinMode(cardGREEN, OUTPUT);

// Button setup

  pinMode(button_WRITE_SD_card, INPUT_PULLUP);  
  pinMode(button_EJECT_SD_card, INPUT_PULLUP); 

// define variables to control color and brightness on 2 RGBs
// and set initial values

// Sensor status LED set to yellow solid to start
  
int redSensorValue = 255; // Red Full Brightness
int greenSensorValue = 30; // Low Green Value (Green is stronger in these so need less of it for yellow)

// Card Reader status LED set to yellow solid to start
  
int redCardReaderValue = 255; 
int greenCardReaderValue = 30;

// Analog write Initial Color to both LEDs
// (Actually Sends the Color to LEDs)
 
analogWrite(sensorRED, redSensorValue);
analogWrite(sensorGREEN, greenSensorValue);
analogWrite(cardRED, redCardReaderValue);
analogWrite(cardGREEN, greenCardReaderValue);

// Determine Sensor Status

  if (!pressureSensor.init()){
    Serial.println("Failed to autodetect pressure sensor!");
    blinkFunction("sensor", 255, 0, 3); // 3 blinks to solid RED to show sensor detect failure
    while (1);
  }
  else{
    blinkFunction("sensor", 0, 150, 3); // 3 blinks to solid GREEN while sensor works
  }

// Turn on the pressure sensor in a default configuration 

pressureSensor.enableDefault();
 
  
}


void loop() { 

  if (digitalRead(button_WRITE_SD_card) == LOW)
  {
   // Open serial communications and wait for port to open:

    Serial.begin(9600);
    
    Wire.begin(); // join i2c bus (Sensor can communicate to Arduino)
    
    Serial.print("Initializing SD card...");

    if (!SD.begin(10)) {
      Serial.println("initialization failed!");
      blinkFunction("card", 255, 0, 3); // 3 blinks to solid red to show initialization failure
      while (1);
      }

    Serial.println("initialization done.");
    blinkFunction("card", 0, 150, 3); // 3 blinks of green to confirm initialization
    
    // create file on card to write to.
    // file name is string in quotes
    // if file name already exists - will open
    // and new data appended at end
    
    myFile = SD.open(fileName, FILE_WRITE);
  
    // if the file opened -> write to it:
    
    if (myFile) {
      blinkFunction("card", 0, 150, 1); // blink to solid green while writing data 
    }
    else {
      // if the file didn't open, print an error and blink red on status LED:
      Serial.println("error opening fileName");
      blinkFunction("card", 255, 0, 5); // 5 blinks to solid red 
      }
    
      // Print Welcome Message
  
    Serial.print("Flight Altitude Data:\n");
    myFile.print("Flight Altitude Data:\n");
  
    // Define Sensor Variables
    
    float pressure = pressureSensor.readPressureInchesHg();
    float altitude = pressureSensor.pressureToAltitudeFeet(pressure);
    float temperature = pressureSensor.readTemperatureF();
  
    // Print Sensor Data (Pressure, Altitude, Temperature)
    // Serial Print Unnecessary but will allow troubleshooting
    // while connected to computer and using Serial Viewer.
    // myfile.print prints to SD card.

    while(digitalRead(button_EJECT_SD_card) != LOW){
  
    Serial.print("p: ");
    myFile.print("p: ");
    Serial.print(pressure);
    myFile.print(pressure);
    Serial.print(" inHg\ta: ");
    myFile.print(" inHg\ta: ");
    Serial.print(altitude);
    myFile.print(altitude);
    Serial.print(" ft\tt: ");
    myFile.print(" ft\tt: ");
    Serial.print(temperature);
    myFile.print(temperature);
    Serial.println(" deg F");
    myFile.println(" deg F");
  
    delay(100);
    }
  }
  if (digitalRead(button_EJECT_SD_card) == LOW)
  {
    // close the file:
    
    myFile.close();
    Serial.println("done.");
    blinkFunction("card", 255, 30, 1); // Blink and hold to solid Yellow
  }

}

// Blink Pattern Function: Can specify which LED,
// Choose color with two int values
// set number of blinks

void blinkFunction(String LED, int redValue, int greenValue, int amountBlinks){ 

  if(LED == "sensor" || LED == "s" || LED == "Sensor" || LED == "S"){

      for (int i = 0; i < amountBlinks; i++){

          analogWrite(sensorRED, 0);
          analogWrite(sensorGREEN, 0);
        
          delay(200);
        
          analogWrite(sensorRED, redValue);
          analogWrite(sensorGREEN, greenValue);

          delay(200);
      }
      }
   else if(LED == "card" || LED == "c" || LED == "Card" || LED == "C" || LED == "sd" || LED == "SD"){


       for (int i = 0; i < amountBlinks; i++){
            
          
          analogWrite(cardRED, 0);
          analogWrite(cardGREEN, 0);
        
          delay(200);
        
          analogWrite(cardRED, redValue);
          analogWrite(cardGREEN, greenValue);

          delay(200);
       }
      }
      else{
        
          redValue = 255;
          greenValue =30;

          analogWrite(sensorRED, 0);
          analogWrite(sensorGREEN, 0);
        
          delay(200);
        
          analogWrite(sensorRED, redValue);
          analogWrite(sensorGREEN, greenValue);

          delay(200);
      }
        
    }
