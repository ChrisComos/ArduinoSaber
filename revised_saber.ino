#include <Adafruit_Soundboard.h>
#include <FastLED.h>
#include <avr/pgmspace.h>

#include <Wire.h>
#include <SoftwareSerial.h>
#include "Adafruit_Soundboard.h"
#define SFX_TX 5
#define SFX_RX 6
#define SFX_RST 4
SoftwareSerial ss = SoftwareSerial(SFX_TX, SFX_RX);
Adafruit_Soundboard sfx = Adafruit_Soundboard(&ss, NULL, SFX_RST);


#define MPU6050_ACCEL_XOUT_H       0x3B   // R  
#define MPU6050_ACCEL_XOUT_L       0x3C   // R  
#define MPU6050_ACCEL_YOUT_H       0x3D   // R  
#define MPU6050_ACCEL_YOUT_L       0x3E   // R  
#define MPU6050_ACCEL_ZOUT_H       0x3F   // R  
#define MPU6050_ACCEL_ZOUT_L       0x40   // R  
#define MPU6050_TEMP_OUT_H         0x41   // R  
#define MPU6050_TEMP_OUT_L         0x42   // R  
#define MPU6050_GYRO_XOUT_H        0x43   // R  
#define MPU6050_GYRO_XOUT_L        0x44   // R  
#define MPU6050_GYRO_YOUT_H        0x45   // R  
#define MPU6050_GYRO_YOUT_L        0x46   // R  
#define MPU6050_GYRO_ZOUT_H        0x47   // R  
#define MPU6050_GYRO_ZOUT_L        0x48   // R  
#define MPU6050_EXT_SENS_DATA_00   0x49   // R  
#define MPU6050_PWR_MGMT_1         0x6B   // R/W
#define MPU6050_WHO_AM_I           0x75   // R

 
#define MPU6050_I2C_ADDRESS 0x68

#define LED_TYPE NEOPIXEL
#define NUM_LEDS 144
#define DATA_PIN 6
CRGB leds[NUM_LEDS] ;
typedef union accel_t_gyro_union
{
  struct
  {
    uint8_t x_accel_h;
    uint8_t x_accel_l;
    uint8_t y_accel_h;
    uint8_t y_accel_l;
    uint8_t z_accel_h;
    uint8_t z_accel_l;
    uint8_t t_h;
    uint8_t t_l;
    uint8_t x_gyro_h;
    uint8_t x_gyro_l;
    uint8_t y_gyro_h;
    uint8_t y_gyro_l;
    uint8_t z_gyro_h;
    uint8_t z_gyro_l;
  } reg;
  struct
  {
    int16_t x_accel;
    int16_t y_accel;
    int16_t z_accel;
    int16_t temperature;
    int16_t x_gyro;
    int16_t y_gyro;
    int16_t z_gyro;
  } value;
};

void setup()
{
  
  int error;
  uint8_t c;
  String colorHold;

  Serial.begin(115200);
  ss.begin(9600);
//  if (!sfx.reset()) {
//    Serial.println("Not found");
//    while (1);
//  }
  Serial.println("SFX board found");

  // Initialize the 'Wire' class for the I2C-bus.
  Wire.begin();


  // default at power-up:
  //    Gyro at 250 degrees second
  //    Acceleration at 2g
  //    Clock source at internal 8MHz
  //    The device is in sleep mode.
  //

  error = MPU6050_read (MPU6050_WHO_AM_I, &c, 1);
  Serial.print(F("WHO_AM_I : "));
  Serial.print(c, HEX);
  Serial.print(F(", error = "));
  Serial.println(error, DEC);

  // According to the datasheet, the 'sleep' bit
  // should read a '1'.
  // That bit has to be cleared, since the sensor
  // is in sleep mode at power-up.
  error = MPU6050_read (MPU6050_PWR_MGMT_1, &c, 1);
  Serial.print(F("PWR_MGMT_1 : "));
  Serial.print(c, HEX);
  Serial.print(F(", error = "));
  Serial.println(error, DEC);

  FastLED.addLeds<NEOPIXEL,6>(leds, NUM_LEDS);
  

  // Clear the 'sleep' bit to start the sensor.
  MPU6050_write_reg (MPU6050_PWR_MGMT_1, 0);
}


void loop()
{
  static uint8_t hold = CRGB::Blue;
  static boolean on = false;
  int error;
  double dT;
  accel_t_gyro_union accel_t_gyro;
  static int button  =  3;                         // button pin
  static int button2 = 8;
 int buttonState = 0;                      // button state

  error = MPU6050_read(MPU6050_ACCEL_XOUT_H, (uint8_t *) &accel_t_gyro, sizeof(accel_t_gyro));
  uint8_t swap;
#define SWAP(x,y) swap = x; x = y; y = swap

  SWAP (accel_t_gyro.reg.x_accel_h, accel_t_gyro.reg.x_accel_l);
  SWAP (accel_t_gyro.reg.y_accel_h, accel_t_gyro.reg.y_accel_l);
  SWAP (accel_t_gyro.reg.z_accel_h, accel_t_gyro.reg.z_accel_l);
  SWAP (accel_t_gyro.reg.t_h, accel_t_gyro.reg.t_l);
  SWAP (accel_t_gyro.reg.x_gyro_h, accel_t_gyro.reg.x_gyro_l);
  SWAP (accel_t_gyro.reg.y_gyro_h, accel_t_gyro.reg.y_gyro_l);
  SWAP (accel_t_gyro.reg.z_gyro_h, accel_t_gyro.reg.z_gyro_l);
  static String color = "Blue";
  static int bright = 127;
  static int start;
  static int ends;
  Serial.print(digitalRead(button));
  if(digitalRead(button)){
    start = millis();
    while(digitalRead(button) == true){}
    ends = millis();
  }
//Serial.print("\t");
//Serial.print(start);
//Serial.print("\t");
//Serial.print(ends);
//Serial.print("\t");
  static boolean settings;
  double buttonTime = ends - start;
  static double lastPress;
  static boolean lastSwing;
  if( buttonTime != 0.0){
 Serial.println(buttonTime);
  }


 if (on && (accel_t_gyro.value.x_gyro < -3500 || accel_t_gyro.value.x_gyro > 3500 || accel_t_gyro.value.y_gyro < -3500 || accel_t_gyro.value.y_gyro > 3500 || accel_t_gyro.value.z_gyro < -3500 || accel_t_gyro.value.z_gyro > 3500)&&
              (accel_t_gyro.value.x_accel < -13500  || accel_t_gyro.value.x_accel > 13500|| accel_t_gyro.value.y_accel < -13500  || accel_t_gyro.value.y_accel > 13500|| accel_t_gyro.value.z_accel < -13500  || accel_t_gyro.value.z_accel > 13500)){
     
    Swing();

    idle();
  }
       
  else if (settings ){
    Serial.print(accel_t_gyro.value.x_accel);
      if ((accel_t_gyro.value.x_accel / 1000 > 0) && (accel_t_gyro.value.x_accel / 1000 <= 2)) {
//          color = " Blue ";
          hold = CRGB::Blue;
        }
        else if ((accel_t_gyro.value.x_accel / 1000 > 2) && (accel_t_gyro.value.x_accel / 1000 <= 4)) {
//          color = " Red ";
          hold = CRGB::Red;
        }
        else if ((accel_t_gyro.value.x_accel / 1000 > 4) && (accel_t_gyro.value.x_accel / 1000 <= 6)) {
//          color = " Green ";
          hold = CRGB::Green;
        }
        else if ((accel_t_gyro.value.x_accel / 1000 > 6) && (accel_t_gyro.value.x_accel / 1000 <= 8)) {
//          color = " Purple ";
          hold = CRGB::Purple;
        }
        else if ((accel_t_gyro.value.x_accel / 1000 > 8) && (accel_t_gyro.value.x_accel / 1000 <= 10)) {
//          color = " Yellow ";
          hold = CRGB::Yellow;
        }
        else if ((accel_t_gyro.value.x_accel / 1000 > 10) && (accel_t_gyro.value.x_accel / 1000 <= 12)) {
//         color = " White ";
         hold = CRGB::White;
       }
       else if ((accel_t_gyro.value.x_accel / 1000 > 12) && (accel_t_gyro.value.x_accel/ 1000 <= 14)) {
//         color = " Orange";
         hold = CRGB::Orange;
       }
       else if ((accel_t_gyro.value.x_accel / 1000 > 14) && (accel_t_gyro.value.x_accel / 1000 <= 16)) {
//         color = " Rainbow ";
       }
       else if ((accel_t_gyro.value.x_accel / 1000 > 16) && (accel_t_gyro.value.x_accel / 1000 <= 18)) {
//         color = " Custom ";
        }
        else {
//         color = color;
        }
        for(int i = 0; i < NUM_LEDS; i ++){
            leds[i] = hold;
          }
        FastLED.show();
          Serial.println(color);
      if ((accel_t_gyro.value.y_accel / 1000 > 4) && (accel_t_gyro.value.y_accel / 1000 < 40)) {
          Serial.print(" Brightness +");
          
          FastLED.setBrightness(FastLED.getBrightness() + 1);
        }
       if ((accel_t_gyro.value.y_accel / 1000 > -40) && (accel_t_gyro.value.y_accel / 1000 < -4)) {
          Serial.print (" Brightness -"); 
          FastLED.setBrightness(FastLED.getBrightness() + 1);
          }  
      if ((accel_t_gyro.value.z_gyro / 1000 > 10) && (accel_t_gyro.value.z_gyro / 1000 < 40)) {
        volup();}
       if ((accel_t_gyro.value.z_gyro / 1000 > -40) && (accel_t_gyro.value.z_gyro / 1000 < -10)) {
         voldown();}
      bright = FastLED.getBrightness();
      if (buttonTime > 0){
        settings = false; 
      }
  }
  
  else if (buttonTime <= 100 && buttonTime > 0){
    // do nothing /// rugged debouncer
  }
  else if(on == false && buttonTime > 100 && buttonTime < 3000){ // saber is off and button is pressed for less than 3 sec
    on = true;
    ignite(hold);
  }
  else if( on == false && buttonTime > 3000){ // saber is off and button is pressed for 3 sec or more
    Serial.print("\t Settings");
    settings = true;
      
  }



  else if(on == true && buttonTime > 1500 && buttonTime < 3000){ // saber is off and button is pressed for less than 3 sec
    on = false;
    stopTrack();
    powerOff();
  }

else if (on == true && buttonTime <= 0){ // on == true amd button not pressed
   idle();
   if (isClash(button, on, lastPress)){
    stopTrack();//14 9 
    clash();
    delay(100);
    stopTrack();
    Serial.print("Clash  ");
    while (digitalRead(button)){
      Serial.print("lockup");
     playNum(20); // change for clash number
    }
    stopTrack();
    idle();
   }
   //  Serial.print(F("accel x,y,z: "));
//  Serial.print(accel_t_gyro.value.x_accel/1000, DEC);
//  Serial.print(F(", "));
//  Serial.print(accel_t_gyro.value.y_accel/1000, DEC);
//  Serial.print(F(", "));
//  Serial.print(accel_t_gyro.value.z_accel/1000, DEC);
//  Serial.println(F("  "));
//   Serial.print(accel_t_gyro.value.x_gyro, DEC);
//      Serial.print(F(", \t"));
//      Serial.print(accel_t_gyro.value.y_gyro, DEC);
//      Serial.print(F(", \t"));
//      Serial.print(accel_t_gyro.value.z_gyro, DEC);
//      Serial.print(F(", \t"));
//      Serial.println(F(""));

  }
  
  lastPress = buttonTime;
//  Serial.print(lastPress);
//  Serial.println(" HERE");
  start = 0;
  ends = 0;
  
}




















boolean isClash(int button, boolean on, double lastPress){
  // if clash return true

  if(digitalRead(button) && on == true && lastPress < 500 && lastPress > 10){
    return true;
  }
  return false;
}
void clash(){ // double press when on?
  int track;
  track = random(6,18);
  Serial.print(track);
  playNum(track);// play clash sound
  Serial.println("Clash");
}
//boolean isSwing(){
//  //if swing return true
//  return false;
//}
void Swing(){
  stopTrack();
  int track = random(0,11);
  if(track > 5){
    track =track+ 18; 
  }
  Serial.println(track);
  playNum(track); // play swing sound
  delay(250);
  Serial.println("Swing");
  delay(500);
}

void ignite(uint8_t color){
  
  Serial.println("Power on");
  for( int i = 0; i < NUM_LEDS; i ++){
    leds[i] = color;
  }
  delay(1250);
  playNum(22);
  stopTrack();
}
void powerOff(){
  playNum(21);
  for(int i =NUM_LEDS; i > 0; i --){
    leds[i] = CRGB::Black; 
  }
  Serial.println("Power off");
  //LEDS = false
}
void idle(){
  playNum(29);
  Serial.println("HMMMM");
}
void lockup(){
  playNum(20);
  Serial.println("LOCK");
}


// --------------------------------------------------------
// MPU6050_read
//
// This is a common function to read multiple bytes
// from an I2C device.
//
// It uses the boolean parameter for Wire.endTransMission()
// to be able to hold or release the I2C-bus.
// This is implemented in Arduino 1.0.1.
//
// Only this function is used to read.
// There is no function for a single byte.
//
int MPU6050_read(int start, uint8_t *buffer, int size)
{
  int i, n, error;

  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  n = Wire.write(start);
  if (n != 1)
    return (-10);

  n = Wire.endTransmission(false);    // hold the I2C-bus
  if (n != 0)
    return (n);

  // Third parameter is true: relase I2C-bus after data is read.
  Wire.requestFrom(MPU6050_I2C_ADDRESS, size, true);
  i = 0;
  while (Wire.available() && i < size)
  {
    buffer[i++] = Wire.read();
  }
  if ( i != size)
    return (-11);

  return (0);  // return : no error
}


// --------------------------------------------------------
// MPU6050_write
//
// This is a common function to write multiple bytes to an I2C device.
//
// If only a single register is written,
// use the function MPU_6050_write_reg().
//
// Parameters:
//   start : Start address, use a define for the register
//   pData : A pointer to the data to write.
//   size  : The number of bytes to write.
//
// If only a single register is written, a pointer
// to the data has to be used, and the size is
// a single byte:
//   int data = 0;        // the data to write
//   MPU6050_write (MPU6050_PWR_MGMT_1, &c, 1);
//
int MPU6050_write(int start, const uint8_t *pData, int size)
{
  int n, error;

  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  n = Wire.write(start);        // write the start address
  if (n != 1)
    return (-20);

  n = Wire.write(pData, size);  // write data bytes
  if (n != size)
    return (-21);

  error = Wire.endTransmission(true); // release the I2C-bus
  if (error != 0)
    return (error);

  return (0);         // return : no error
}

// --------------------------------------------------------
// MPU6050_write_reg
//
// An extra function to write a single register.
// It is just a wrapper around the MPU_6050_write()
// function, and it is only a convenient function
// to make it easier to write a single register.
//
int MPU6050_write_reg(int reg, uint8_t data)
{
  int error;

  error = MPU6050_write(reg, &data, 1);

  return (error);
};


void volup() {
  uint16_t v;
  if (! (v = sfx.volDown()) ) {
    Serial.println("Failed to adjust");
  } else {
    Serial.print("Volume: ");
    Serial.println(v);

  }
};
void voldown() {
  uint16_t v;
  if (! (v = sfx.volUp()) ) {
    Serial.println("Failed to adjust");
  } else {
    Serial.print("Volume: "); Serial.println(v);
  }
};


void playName(char name[20]) {
  if (! sfx.playTrack(name) ) {
    Serial.println("Failed to play track?");
  }
};

void playNum (uint8_t numb) {
  uint8_t n = numb;
  if (! sfx.playTrack((uint8_t)n) ) {
    Serial.println("Failed to play track");
  }

};

void stopTrack(){
  if (! sfx.stop() ) Serial.println("Failed to stop");
};
