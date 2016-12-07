#ifndef __SPHMC5883L_H__
#define __SPHMC5883L_H__

#include <Arduino.h>
#include <EEPROM.h>

class SPHMC5883L
 {
  public           :

  enum { SAMPLES_1, SAMPLES_2, SAMPLES_4, SAMPLES_8 };
  enum { DATA_RATE_0_75, DATA_RATE_1_5, DATA_RATE_3_0, DATA_RATE_7_5,
         DATA_RATE_15, DATA_RATE_30,DATA_RATE_75 };
  enum { GAIN0, GAIN1, GAIN2, GAIN3, GAIN4, GAIN5, GAIN6, GAIN7, DEFAULT_GAIN = GAIN1 };
  enum { MODE_CONTINUOUS, MODE_SINGLE, MODE_IDLE };



                   SPHMC5883L         (byte                i2caddr);

  bool             selfTest           ();
  void             calibrate          (unsigned int        durationInSeconds);

  void             getCalibration     (float              *offsetXInmG,
                                       float              *offsetYInmG,
                                       float              *offsetZInmG) const;

  void             setCalibration     (float               offsetXInmG,
                                       float               offsetYInmG,
                                       float               offsetZInmG);

  void             configure          (byte                samplesPerMeasurement,
                                       byte                dataRate,
                                       byte                gain,
                                       byte                mode);

  void             readXYZ            (float              *x,
                                       float              *y,
                                       float              *z);

  void             readXYZRaw         (int16_t            *x,
                                       int16_t            *y,
                                       int16_t            *z);
  bool isAutoCalibrationEnable        ();
  void startAutoCalibration           ();
  void stopAutoCalibration            ();
  void doAutoCalibrationStep          (int16_t            x,
                                       int16_t            y,
                                       int16_t            z);
  void getAutoCalibration             (float              *offsetXInmG,
                                       float              *offsetYInmG,
                                       float              *offsetZInmG) const;

  bool hasSavedCalibrationData        (int eepromAddress) const;
  void saveCalibrationData            (int eepromAddress) const;
  bool loadCalibrationData            (int eepromAddress);

  private          :

  bool             selfTestPositive   ();
  bool             selfTestNegative   ();
  void             runSelfTest        (int16_t            *x,
                                       int16_t            *y,
                                       int16_t            *z,
                                       byte                signBitMask);
  void             doCalibrationStep  (int16_t            *minX,
                                       int16_t            *minY,
                                       int16_t            *minZ,
                                       int16_t            *maxX,
                                       int16_t            *maxY,
                                       int16_t            *maxZ);


  

  void             writeReg           (byte                reg,
                                       byte                value);
  byte             readReg            (byte                reg);

  static float     getResolutionByGain(byte                gainMode); // gainMode in range [0..7]

  byte             i2caddr;
  float            offsetX;
  float            offsetY;
  float            offsetZ;
  float            gainFactor;

  

  struct Vector
  {
    int16_t x;
    int16_t y;
    int16_t z;
  }; 
  
  struct CalibrationState
   {
    Vector minReadings;
    Vector maxReadings;
 
    bool   isCalibrating;
   };

   CalibrationState calibrationState;

 };

#endif
 
