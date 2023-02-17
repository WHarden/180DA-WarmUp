/****************************************************************
 * Example1_Basics.ino
 * ICM 20948 Arduino Library Demo
 * Use the default configuration to stream 9-axis IMU data
 * Owen Lyke @ SparkFun Electronics
 * Original Creation Date: April 17 2019
 *
 * Please see License.md for the license information.
 *
 * Distributed as-is; no warranty is given.
 ***************************************************************/
#include "ICM_20948.h" // Click here to get the library: http://librarymanager/All#SparkFun_ICM_20948_IMU

//#define USE_SPI       // Uncomment this to use SPI

#define SERIAL_PORT Serial

#define SPI_PORT SPI // Your desired SPI port.       Used only when "USE_SPI" is defined
#define CS_PIN 2     // Which pin you connect CS to. Used only when "USE_SPI" is defined

#define WIRE_PORT Wire // Your desired Wire port.      Used when "USE_SPI" is not defined
// The value of the last bit of the I2C address.
// On the SparkFun 9DoF IMU breakout the default is 1, and when the ADR jumper is closed the value becomes 0
#define AD0_VAL 1

#ifdef USE_SPI
ICM_20948_SPI myICM; // If using SPI create an ICM_20948_SPI object
#else
ICM_20948_I2C myICM; // Otherwise create an ICM_20948_I2C object
#endif

void setup()
{

  SERIAL_PORT.begin(115200);
  while (!SERIAL_PORT)
  {
  };

#ifdef USE_SPI
  SPI_PORT.begin();
#else
  WIRE_PORT.begin();
  WIRE_PORT.setClock(400000);
#endif

  //myICM.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  bool initialized = false;
  while (!initialized)
  {

#ifdef USE_SPI
    myICM.begin(CS_PIN, SPI_PORT);
#else
    myICM.begin(WIRE_PORT, AD0_VAL);
#endif

    SERIAL_PORT.print(F("Initialization of the sensor returned: "));
    SERIAL_PORT.println(myICM.statusString());
    if (myICM.status != ICM_20948_Stat_Ok)
    {
      SERIAL_PORT.println("Trying again...");
      delay(500);
    }
    else
    {
      initialized = true;
    }
  }
}

void loop()
{

  if (myICM.dataReady())
  {
    myICM.getAGMT();         // The values are only updated when you call 'getAGMT'
    //printRawAGMT( myICM.agmt );     // Uncomment this to see the raw values, taken directly from the agmt structure
    //printScaledAGMT(&myICM); // This function takes into account the scale settings from when the measurement was made to calculate the values with units
    //printsimpleraw(&myICM);
    //classfier(&myICM);
    classifier3(&myICM);
    delay(800);
  }
  else
  {
    SERIAL_PORT.println("Waiting for data");
    delay(500);
  }
}

// Below here are some helper functions to print the data nicely!

void printPaddedInt16b(int16_t val)
{
  if (val > 0)
  {
    SERIAL_PORT.print("+");
    if (val < 10000)
    {
      SERIAL_PORT.print("0");
    }
    if (val < 1000)
    {
      SERIAL_PORT.print("0");
    }
    if (val < 100)
    {
      SERIAL_PORT.print("0");
    }
    if (val < 10)
    {
      SERIAL_PORT.print("0");
    }
  }
  else
  {
    SERIAL_PORT.print("-");
    if (abs(val) < 10000)
    {
      SERIAL_PORT.print("0");
    }
    if (abs(val) < 1000)
    {
      SERIAL_PORT.print("0");
    }
    if (abs(val) < 100)
    {
      SERIAL_PORT.print("0");
    }
    if (abs(val) < 10)
    {
      SERIAL_PORT.print("0");
    }
  }
  SERIAL_PORT.print(abs(val));
}

void printRawAGMT(ICM_20948_AGMT_t agmt)
{
  SERIAL_PORT.print("RAW. Acc [ ");
  printPaddedInt16b(agmt.acc.axes.x);
  SERIAL_PORT.print(", ");
  printPaddedInt16b(agmt.acc.axes.y);
  SERIAL_PORT.print(", ");
  printPaddedInt16b(agmt.acc.axes.z);
  SERIAL_PORT.print(" ], Gyr [ ");
  printPaddedInt16b(agmt.gyr.axes.x);
  SERIAL_PORT.print(", ");
  printPaddedInt16b(agmt.gyr.axes.y);
  SERIAL_PORT.print(", ");
  printPaddedInt16b(agmt.gyr.axes.z);
  SERIAL_PORT.print(" ], Mag [ ");
  printPaddedInt16b(agmt.mag.axes.x);
  SERIAL_PORT.print(", ");
  printPaddedInt16b(agmt.mag.axes.y);
  SERIAL_PORT.print(", ");
  printPaddedInt16b(agmt.mag.axes.z);
  SERIAL_PORT.print(" ], Tmp [ ");
  printPaddedInt16b(agmt.tmp.val);
  SERIAL_PORT.print(" ]");
  SERIAL_PORT.println();
}

void printFormattedFloat(float val, uint8_t leading, uint8_t decimals)
{
  float aval = abs(val);
  if (val < 0)
  {
    SERIAL_PORT.print("-");
  }
  else
  {
    SERIAL_PORT.print(" ");
  }
  for (uint8_t indi = 0; indi < leading; indi++)
  {
    uint32_t tenpow = 0;
    if (indi < (leading - 1))
    {
      tenpow = 1;
    }
    for (uint8_t c = 0; c < (leading - 1 - indi); c++)
    {
      tenpow *= 10;
    }
    if (aval < tenpow)
    {
      SERIAL_PORT.print("0");
    }
    else
    {
      break;
    }
  }
  if (val < 0)
  {
    SERIAL_PORT.print(-val, decimals);
  }
  else
  {
    SERIAL_PORT.print(val, decimals);
  }
}

#ifdef USE_SPI
void printScaledAGMT(ICM_20948_SPI *sensor)
{
#else
void printScaledAGMT(ICM_20948_I2C *sensor)
{
#endif
  SERIAL_PORT.print("Scaled. Acc (mg) [ ");
  printFormattedFloat(sensor->accX(), 5, 2);
  //test
  SERIAL_PORT.print("RAW ACCX: ");
  SERIAL_PORT.print(sensor->accX());
  //test end
  SERIAL_PORT.print(", ");
  printFormattedFloat(sensor->accY(), 5, 2);
  SERIAL_PORT.print(", ");
  printFormattedFloat(sensor->accZ(), 5, 2);
  SERIAL_PORT.print(" ], Gyr (DPS) [ ");
  printFormattedFloat(sensor->gyrX(), 5, 2);
  SERIAL_PORT.print(", ");
  printFormattedFloat(sensor->gyrY(), 5, 2);
  SERIAL_PORT.print(", ");
  printFormattedFloat(sensor->gyrZ(), 5, 2);
  SERIAL_PORT.print(" ], Mag (uT) [ ");
  printFormattedFloat(sensor->magX(), 5, 2);
  SERIAL_PORT.print(", ");
  printFormattedFloat(sensor->magY(), 5, 2);
  SERIAL_PORT.print(", ");
  printFormattedFloat(sensor->magZ(), 5, 2);
  SERIAL_PORT.print(" ], Tmp (C) [ ");
  printFormattedFloat(sensor->temp(), 5, 2);
  SERIAL_PORT.print(" ]");
  SERIAL_PORT.println();
    
}
void classifier3(ICM_20948_I2C *sensor)
{
  //if ((result1 == "Left turn")&&(sensor->accZ()<=980)&&(sensor->accY()>0)){SERIAL_PORT.println("circular turn");}
  //if ((result2 == "right turn")&&(sensor->accZ()<=980)&&(sensor->accY()<0)){SERIAL_PORT.println("circular turn");}
  //char result1[9] = "";
  //char result2[10] = "";


 if(sensor->accZ()>980){
   
   SERIAL_PORT.println("IDLE");
 }
 if((sensor->accZ()<980)&&(sensor->accZ()>0) ){
   if(sensor->accY()<0){
     //result1="Left turn";
      SERIAL_PORT.println("Left turn");}
   if(sensor->accY()>0){
     //result2="right turn";
      SERIAL_PORT.println("right turn");}
   
 }
 if(sensor->accZ()<0) {
   SERIAL_PORT.println("circular turn");
 }
}



void printsimpleraw(ICM_20948_I2C *sensor)
{
  SERIAL_PORT.print("ACC: ");
  SERIAL_PORT.print(sensor->accX());
  SERIAL_PORT.print(", ");
  SERIAL_PORT.print(sensor->accY());
  SERIAL_PORT.print(", ");
  SERIAL_PORT.print(sensor->accZ());
  SERIAL_PORT.print(", ");/*
  SERIAL_PORT.print("GYR: ");
  SERIAL_PORT.print(sensor->gyrX());
  SERIAL_PORT.print(", ");
  SERIAL_PORT.print(sensor->gyrY());
  SERIAL_PORT.print(", ");
  SERIAL_PORT.print(sensor->gyrZ());
  SERIAL_PORT.print(", ");
  SERIAL_PORT.print("MAG: ");
  SERIAL_PORT.print(sensor->magX());
  SERIAL_PORT.print(", ");
  SERIAL_PORT.print(sensor->magY());
  SERIAL_PORT.print(", ");
  SERIAL_PORT.print(sensor->magZ());*/
  SERIAL_PORT.print(".");
  SERIAL_PORT.println();
}
 
 // This classfier can recognize five motions
 // 1.idle  2. slight left turn 3. heavy left turn 4. slight right turn 5. heavy right turn
void classfier(ICM_20948_I2C *sensor)
{
  if( 900<sensor->accX()<1100)
  {
    SERIAL_PORT.print("IDLE");
  }

  else if( 500<sensor->accX()<800)
  { if(sensor->accY()>0)
    {
      SERIAL_PORT.print("Slight right turn");
    }
    if(sensor->accY()<0)
    {
      SERIAL_PORT.print("Slight left turn");
    }
  }
  else if( -50<sensor->accX()<500)
  {
    if(850<sensor->accY()<1150)
    {
      SERIAL_PORT.print("Heavy right turn");      
    }
    if(-1150<sensor->accY()<-850)
    {
      SERIAL_PORT.print("Heavy left turn");
    }
    
  }  
  else 
  {
    SERIAL_PORT.print("Not recgonized");
  }
  SERIAL_PORT.println();

}
