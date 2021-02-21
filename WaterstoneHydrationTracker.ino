#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

//NEOPIXEL STUFF
#include <Adafruit_NeoPixel.h>
#define PIN 2
#define NUM_LEDS 20
#define BRIGHTNESS 100

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_RGBW + NEO_KHZ800);

byte neopix_gamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

////////////////////////////////////////////////////WIFI CONNECTION CREDENTIALS//////////////////////////////////////////////////
//char auth[] = "";
char auth[] = "";
char ssid[] = "";
char pass[] = "";
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////THESE ARE GLOBAL VARIABLES SUE ME/////////////////////////////////////////////
int waterLevel = 100; 
int emptyBottleWeight = 220;       //These values would get set in the Blynk App
int fullBottleWeight = 1100;       //User presses button to record both values, this determines water capacity in mL. 
int Status = 0; 
int lastWaterLevel; 
int consumedWater = 0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Declare MPU 
Adafruit_MPU6050 mpu;

//Declare Blynk Update Timers
BlynkTimer sendWaterLevel;
BlynkTimer sendAmbientTemp;
BlynkTimer updateLEDS;


void setup(){
  Serial.begin(115200);

/////////////////////////////////////////////////////////INITIALIZE MPU//////////////////////////////////////////////////////////

if (!mpu.begin()){
  Serial.println("MPU ERROR: FAILED TO FIND MPU6050");
  while(1){
    delay(10);
    }}
Serial.println("MPU Found!");

                       //////////////////////////////SET OPERATING RANGES///////////////////////////////
      mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
      Serial.println("MPU Accelerometer Range set to +-16G");

      mpu.setGyroRange(MPU6050_RANGE_500_DEG);
      Serial.println("MPU Gyro Range set to +- 500 deg/s");

      mpu.setFilterBandwidth(MPU6050_BAND_94_HZ);
      Serial.println("MPU LPDF set to 94 Hz");
      
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////INITIALIZE NEOPIXELS///////////////////////////////////////////////////////
strip.setBrightness(BRIGHTNESS);
strip.begin();
strip.show(); //Initialize neopixels to off
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  Blynk.begin(auth, ssid, pass);
  
  sendWaterLevel.setInterval(3000L, getWaterLevel);
  sendAmbientTemp.setInterval(5000L, getAmbientTemp);
  updateLEDS.setInterval(100L, setLEDS);
  
}


void loop(){
  Blynk.run();
  sendWaterLevel.run();
  updateLEDS.run();
  sendAmbientTemp.run();
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256 * 5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void displayCurrentWater(){
    //Initialize LEDS to blank
    strip.show();

  if(waterLevel > 80){
    strip.setPixelColor(0, strip.Color(0, 0, 0, 255));
    strip.setPixelColor(1, strip.Color(0, 0, 0, 255));
    strip.setPixelColor(2, strip.Color(0, 0, 0, 255));
    strip.setPixelColor(3, strip.Color(0, 0, 0, 255));
    strip.setPixelColor(4, strip.Color(0, 0, 0, 255));
    strip.show();
    }
  else if(waterLevel > 60){
    strip.setPixelColor(0, strip.Color(0, 0, 0, 255));
    strip.setPixelColor(1, strip.Color(0, 0, 0, 255));
    strip.setPixelColor(2, strip.Color(0, 0, 0, 255));
    strip.setPixelColor(3, strip.Color(0, 0, 0, 255));
    strip.setPixelColor(4, strip.Color(0, 255, 0, 0));
    strip.show();
    }
  else if(waterLevel > 40){
    strip.setPixelColor(0, strip.Color(0, 0, 0, 255));
    strip.setPixelColor(1, strip.Color(0, 0, 0, 255));
    strip.setPixelColor(2, strip.Color(0, 0, 0, 255));
    strip.setPixelColor(3, strip.Color(0, 255, 0, 0));
    strip.setPixelColor(4, strip.Color(0, 255, 0, 0));
    strip.show();
    }
  else if(waterLevel > 20){
    strip.setPixelColor(0, strip.Color(0, 0, 0, 255));
    strip.setPixelColor(1, strip.Color(0, 0, 0, 255));
    strip.setPixelColor(2, strip.Color(0, 255, 0, 0));
    strip.setPixelColor(3, strip.Color(0, 255, 0, 0));
    strip.setPixelColor(4, strip.Color(0, 255, 0, 0));
    strip.show();
    }
  else if(waterLevel > 10){
    strip.setPixelColor(0, strip.Color(0, 0, 0, 255));
    strip.setPixelColor(1, strip.Color(0, 255, 0, 0));
    strip.setPixelColor(2, strip.Color(0, 255, 0, 0));
    strip.setPixelColor(3, strip.Color(0, 255, 0, 0));
    strip.setPixelColor(4, strip.Color(0, 255, 0, 0));
    strip.show();
    }
  else{
    strip.setPixelColor(0, strip.Color(0, 255, 0, 0));
    strip.setPixelColor(1, strip.Color(0, 255, 0, 0));
    strip.setPixelColor(2, strip.Color(0, 255, 0, 0));
    strip.setPixelColor(3, strip.Color(0, 255, 0, 0));
    strip.setPixelColor(4, strip.Color(0, 255, 0, 0));
    strip.show();
    }  
  
  }

void getWaterLevel(){
  
  if(!checkIsUpright()){  //Check to see if the bottle is moving
    //Set stats = 0 for upright, bottle will display current water level
    Status = 1;  
    
    //Before measuring a new amount of water, we save the waterLevel value as a new value
    //If water level has changed since the last measurement, this difference is sent to the app
    //If no water has been drank, than lastWaterLevel - waterLevel evaluates to 0 and thus no water has been dronked
    //If lastWaterLevel - waterLevel = an integer value then that's the amount of water drank    
    //This is where the code for the weight sensor would go if I had one. 
    //currentWeight = weightSensor.getweight();
    //waterLevel = currentWeight - emptyBottleWeight;
    //For demo purposes water level will change whenever bottle is lifted and placed back down. 

    consumedWater = lastWaterLevel - waterLevel;
    
    if(consumedWater >= 0){ //We don't want to subtract refils from consumed water
    Blynk.virtualWrite(V1, consumedWater);
    delay(120);
    Blynk.virtualWrite(V1, consumedWater);
    delay(120);
    Blynk.virtualWrite(V1, consumedWater);
    delay(120);
    Blynk.virtualWrite(V1, consumedWater);
    delay(120);
    Blynk.virtualWrite(V1, consumedWater);
    delay(120);
    Blynk.virtualWrite(V1, consumedWater);
    delay(120);
    Serial.println("Sent consumedWater to BLYNK");
    }
  
    lastWaterLevel = waterLevel; 
    
    Serial.print("The amount of water consumed is: ");
    Serial.println(consumedWater);
    
    Blynk.virtualWrite(V0, waterLevel);
    
  }
  
  else{
    //Save the current water level to compare against last water level
    lastWaterLevel = waterLevel;
    
    //Since we don't have a weight sensor wer're going to slowly drain the bottle every time we "drink" from it. 
    //When the bottle is fully drained we reset the bottle to full
    waterLevel = waterLevel - random(21,43);

     if(waterLevel <= 0){
       waterLevel = 100;
     }    
    
    Serial.print("The new water level is: ");
    Serial.println(waterLevel);
    
    //Set status = 1 for Not upright/calculating current water level
    Status = 0;
  } 
}

void getAmbientTemp(){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  //This temperature sensor is intended for MPU calibration purposes but by modifying the C to F formula we 
  //can offset the temperature just enough to pass it as an ambient temp sensor. 
  int newtemp = ((((temp.temperature*1.8) + 26) + 0.05) * 10);
  newtemp = (newtemp / 10.0);
  
  Blynk.virtualWrite(V2,newtemp);
  Serial.println((temp.temperature*1.8)+26);
  }
  
bool checkIsUpright(){
  
sensors_event_t a, g, temp;
mpu.getEvent(&a, &g, &temp);

//Checks to see if the bottle has a minimum upright acceleration vector greater than 7m/s.
//Sensor is mounted upside down so a negative value = downward acceleration.
//For ideal measurment acceleration in Z direction would be = to gravitational constant but 8m/s is good enough. 
//This could be calibrated on a per bottle basis but like, I dont wanna do that right now bro. 

//It is important to know that these are instantaneous checks, a better solution would be to have a buffer of 
//accleration values and only allow measurements when the past X buffered values are in range. 

if(a.acceleration.z < -10.5){
  Serial.print(a.acceleration.z);
  Serial.println(" Bottle is upright!");
  return 0;  //no the bottle is not moving (the bottle is upright and safe)
  }
else{
  Serial.println(a.acceleration.z);
  Serial.print(" Bottle is NOT upright!");
  return 1; //The bottle got flipped lmao don't measure weight right now. 
  }  
}

void setLEDS(){
    //This will run every 100ms, at that time it will check the current value of Status and display the coresponding animation
    //The rainbow animation is a BLOCKING action and will take 10 seconds to complete one run through. 
    //This is FINE for now but ideally this would be handled in a non-blocking way, perhaps through an RTOS or cleaver coding
    //For now this gives the illusion of the sensor waiting for stable readings which we know it's not doing because it 
    //doesn't exist on the bottle. 
    //After 10 seconds the program should resume as normall and if the bottle is upright the quantity remaining mode should
    //show up instead. 

    //Oh also this method opens the option of adding different status animations. 

    switch (Status) {
       
       case 1:
          //set white LED's according to current water level
          displayCurrentWater();
          break;
       case 0:
         //Waiting to measure mode
         rainbowCycle(5);
         break;
      default:
      // statements
      break;
}
    
}




uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3,0);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3,0);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0,0);
}
uint8_t red(uint32_t c) {
  return (c >> 16);
}
uint8_t green(uint32_t c) {
  return (c >> 8);
}
uint8_t blue(uint32_t c) {
  return (c);
}
