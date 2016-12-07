#include <Wire.h>

#include "SPHMC8337L2.h"

#define SPHMC5883L_ARRAYSIZE(a) (sizeof(a) / sizeof((a)[0]))
#define SPHMC5883L_EEPROM_MAGIC (0xABCDEF01LU)

namespace {

enum
 {
  NO_TEST_BITMASK       = 0x00,
  POSITIVE_TEST_BITMASK = 0x01,
  NEGATIVE_TEST_BITMASK = 0x02
 };

enum
 {
  SELF_TEST_MIN_POSITIVE_BIAS = 243,
  SELF_TEST_MAX_POSITIVE_BIAS = 575,
  SELF_TEST_MIN_NEGATIVE_BIAS = -SELF_TEST_MAX_POSITIVE_BIAS,
  SELF_TEST_MAX_NEGATIVE_BIAS = -SELF_TEST_MIN_POSITIVE_BIAS
 };

enum
 {
  REG_CRA    = 0x00,
  REG_CRB    = 0x01,
  REG_MODE   = 0x02,
  REG_DATA   = 0x03, // actually there are 6 data regs, but we need only the first
  REG_STATUS = 0x09
 };

enum
 {
  AVG_SAMPLES_1_BITMASK = SPHMC5883L::SAMPLES_1 << 5,
  AVG_SAMPLES_2_BITMASK = SPHMC5883L::SAMPLES_2 << 5,
  AVG_SAMPLES_4_BITMASK = SPHMC5883L::SAMPLES_4 << 5,
  AVG_SAMPLES_8_BITMASK = SPHMC5883L::SAMPLES_8 << 5
 };

enum
 {
  DATA_RATE_0_75_BITMASK = SPHMC5883L::DATA_RATE_0_75 << 2,
  DATA_RATE_1_5_BITMASK  = SPHMC5883L::DATA_RATE_1_5 << 2,
  DATA_RATE_3_0_BITMASK  = SPHMC5883L::DATA_RATE_3_0 << 2,
  DATA_RATE_7_5_BITMASK  = SPHMC5883L::DATA_RATE_7_5 << 2,
  DATA_RATE_15_BITMASK   = SPHMC5883L::DATA_RATE_15 << 2,
  DATA_RATE_30_BITMASK   = SPHMC5883L::DATA_RATE_30 << 2,
  DATA_RATE_75_BITMASK   = SPHMC5883L::DATA_RATE_75 << 2
 };

enum
 {
  GAIN0_BITMASK = SPHMC5883L::GAIN0 << 5,
  GAIN1_BITMASK = SPHMC5883L::GAIN1 << 5,
  GAIN2_BITMASK = SPHMC5883L::GAIN2 << 5,
  GAIN3_BITMASK = SPHMC5883L::GAIN3 << 5,
  GAIN4_BITMASK = SPHMC5883L::GAIN4 << 5,
  GAIN5_BITMASK = SPHMC5883L::GAIN5 << 5,
  GAIN6_BITMASK = SPHMC5883L::GAIN6 << 5,
  GAIN7_BITMASK = SPHMC5883L::GAIN7 << 5
 };

enum
 {
  MODE_CONTINUOUS_BITMASK         = SPHMC5883L::MODE_CONTINUOUS,
  MODE_SINGLE_MEASUREMENT_BITMASK = SPHMC5883L::MODE_SINGLE,
  MODE_IDLE_BITMASK               = SPHMC5883L::MODE_IDLE
 };

static float DIGITAL_RESOLUTION[] = { 0.73f, 0.92f, 1.22f, 1.52f, 2.27f, 2.56f, 3.03f, 4.35f };

}

SPHMC5883L::SPHMC5883L                (byte                i2caddr)
 {
  this->i2caddr = i2caddr;
  offsetX       = 0.0f;
  offsetY       = 0.0f;
  offsetZ       = 0.0f;
  gainFactor    = getResolutionByGain(DEFAULT_GAIN);
  calibrationState.isCalibrating = false;
 }

bool
SPHMC5883L::selfTest                  ()
 {
  bool positive = selfTestPositive();
  bool negative = selfTestNegative();

  return positive && negative;
 }

void
SPHMC5883L::calibrate                 (unsigned int        durationInSeconds)
 {
  unsigned long endTime = durationInSeconds * 1000LU + millis();
  int16_t       minX,minY,minZ;
  int16_t       maxX,maxY,maxZ;

  minX = minY = minZ = 0x7FFF;
  maxX = maxY = maxZ = -0x7FFF - 1;

  writeReg(REG_CRA,AVG_SAMPLES_1_BITMASK | DATA_RATE_75_BITMASK);
  writeReg(REG_CRB,GAIN5_BITMASK);
  writeReg(REG_MODE,MODE_CONTINUOUS_BITMASK);

  int periodInMs = 1000 / 75 + 1;

  delay(periodInMs);

  while (millis() < endTime)
   {
    delay(periodInMs);

    doCalibrationStep(&minX,&minY,&minZ,&maxX,&maxY,&maxZ);
   }

  writeReg(REG_MODE,MODE_IDLE_BITMASK);

  float resolution = getResolutionByGain(5);

  offsetX = (minX + maxX) * resolution * 0.5f;
  offsetY = (minY + maxY) * resolution * 0.5f;
  offsetZ = (minZ + maxZ) * resolution * 0.5f;
  
  
 }

void
SPHMC5883L::doCalibrationStep         (int16_t            *minX,
                                       int16_t            *minY,
                                       int16_t            *minZ,
                                       int16_t            *maxX,
                                       int16_t            *maxY,
                                       int16_t            *maxZ)
 {
  int16_t x,y,z;

  readXYZRaw(&x,&y,&z);

  *minX = min(*minX,x);
  *maxX = max(*maxX,x);
  *minY = min(*minY,y);
  *maxY = max(*maxY,y);
  *minZ = min(*minZ,z);
  *maxZ = max(*maxZ,z);
 }

void
SPHMC5883L::getCalibration            (float              *offsetXInmG,
                                       float              *offsetYInmG,
                                       float              *offsetZInmG) const
 {
  *offsetXInmG = offsetX;
  *offsetYInmG = offsetY;
  *offsetZInmG = offsetZ;
 }

void
SPHMC5883L::getAutoCalibration            (float              *offsetXInmG,
                                           float              *offsetYInmG,
                                           float              *offsetZInmG) const
 {
 
  //float resolution = getResolutionByGain(5);

  *offsetXInmG = (calibrationState.minReadings.x + calibrationState.maxReadings.x) * gainFactor * 0.5f;
  *offsetYInmG = (calibrationState.minReadings.y + calibrationState.maxReadings.y) * gainFactor * 0.5f;
  *offsetZInmG = (calibrationState.minReadings.z + calibrationState.minReadings.z) * gainFactor * 0.5f;

 }

void
SPHMC5883L::setCalibration            (float               offsetXInmG,
                                       float               offsetYInmG,
                                       float               offsetZInmG)
 {
  offsetX = offsetXInmG;
  offsetY = offsetYInmG;
  offsetZ = offsetZInmG;
 }

bool
SPHMC5883L::selfTestPositive          ()
 {
  int16_t x,y,z;

  runSelfTest(&x,&y,&z,POSITIVE_TEST_BITMASK);

  return x >= SELF_TEST_MIN_POSITIVE_BIAS && x <= SELF_TEST_MAX_POSITIVE_BIAS &&
         y >= SELF_TEST_MIN_POSITIVE_BIAS && y <= SELF_TEST_MAX_POSITIVE_BIAS &&
         z >= SELF_TEST_MIN_POSITIVE_BIAS && z <= SELF_TEST_MAX_POSITIVE_BIAS;
 }

bool
SPHMC5883L::selfTestNegative          ()
 {
  int16_t x,y,z;

  runSelfTest(&x,&y,&z,NEGATIVE_TEST_BITMASK);

  return x >= SELF_TEST_MIN_NEGATIVE_BIAS && x <= SELF_TEST_MAX_NEGATIVE_BIAS &&
         y >= SELF_TEST_MIN_NEGATIVE_BIAS && y <= SELF_TEST_MAX_NEGATIVE_BIAS &&
         z >= SELF_TEST_MIN_NEGATIVE_BIAS && z <= SELF_TEST_MAX_NEGATIVE_BIAS;
 }

void
SPHMC5883L::runSelfTest               (int16_t            *x,
                                       int16_t            *y,
                                       int16_t            *z,
                                       byte                signBitMask)
 {
  writeReg(REG_CRA,AVG_SAMPLES_1_BITMASK | DATA_RATE_15_BITMASK | signBitMask);
  writeReg(REG_CRB,GAIN5_BITMASK);
  writeReg(REG_MODE,MODE_CONTINUOUS_BITMASK);

  // according to hmc5883l spec. first measurement set will be available
  // after a period of 2/fDO (fDO - frequency of data output) and
  // consequent measurements will be available at fDO frequency

  int periodInMs = 1000 / 15 + 1;

  delay(periodInMs * 2);

  readXYZRaw(x,y,z); // ignore this result since it can use old gain value

  delay(periodInMs);

  readXYZRaw(x,y,z);

  writeReg(REG_CRA,AVG_SAMPLES_1_BITMASK | DATA_RATE_15_BITMASK | NO_TEST_BITMASK);
  writeReg(REG_MODE,MODE_IDLE_BITMASK);
 }

void
SPHMC5883L::writeReg                  (byte                reg,
                                       byte                value)
 {
  Wire.beginTransmission(i2caddr);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
 }

byte
SPHMC5883L::readReg                   (byte                reg)
 {
  Wire.beginTransmission(i2caddr);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.requestFrom(i2caddr,(uint8_t)1);

  return Wire.read();
 }

void
SPHMC5883L::configure                 (byte                samplesPerMeasurementCode,
                                       byte                dataRateCode,
                                       byte                gainCode,
                                       byte                mode)
 {
  writeReg(REG_CRA,(samplesPerMeasurementCode << 5) | (dataRateCode << 2));
  writeReg(REG_CRB,gainCode << 5);
  writeReg(REG_MODE,mode);

  gainFactor = getResolutionByGain(gainCode);
 }


void
SPHMC5883L::readXYZRaw                (int16_t            *x,
                                       int16_t            *y,
                                       int16_t            *z)
 {
  Wire.beginTransmission(i2caddr);
  Wire.write(REG_DATA);
  Wire.endTransmission();

  Wire.requestFrom(i2caddr,sizeof(int16_t) * 3);

  uint16_t v;

  v  = Wire.read() << 8;
  v |= Wire.read();
  *x = (int16_t)v;

  v  = Wire.read() << 8;
  v |= Wire.read();
  *z = (int16_t)v;
// 00000000 10000000
// 10000000 00000000
// 10000000 11111111
  v  = Wire.read() << 8;
  v |= Wire.read();// v = v | Wire.read();
  *y = (int16_t)v;

  if (isAutoCalibrationEnable())
  {
    doAutoCalibrationStep (*x, *y, *z);
  }
 }
 
void
SPHMC5883L::startAutoCalibration           ()       
  {
     calibrationState.isCalibrating = true;
     calibrationState.minReadings.x = calibrationState.minReadings.y = calibrationState.minReadings.z = 0x7FFF;
     calibrationState.maxReadings.x = calibrationState.maxReadings.y = calibrationState.maxReadings.z = -0x7FFF - 1;
  
  }
  
void
SPHMC5883L::stopAutoCalibration            ()
  {
   calibrationState.isCalibrating = false;
  }
  
bool 
SPHMC5883L::isAutoCalibrationEnable        ()
  {
   return calibrationState.isCalibrating;
  }

/*void inc_a (int *a)
{
 (* a)++;
}

int b = 0;

inc_a(b);*/


      
 /*  
  *   void
SPHMC5883L::doCalibrationStep         (int16_t            *minX,
                                       int16_t            *minY,
                                       int16_t            *minZ,
                                       int16_t            *maxX,
                                       int16_t            *maxY,
                                       int16_t            *maxZ)
int16_t x,y,z;

  readXYZRaw(&x,&y,&z);

  *minX = min(*minX,x);
  *maxX = max(*maxX,x);
  *minY = min(*minY,y);
  *maxY = max(*maxY,y);
  *minZ = min(*minZ,z);
  *maxZ = max(*maxZ,z); */
  void
SPHMC5883L::doAutoCalibrationStep (int16_t x, int16_t y, int16_t z)
  {

  calibrationState.minReadings.x = min(calibrationState.minReadings.x, x);
  calibrationState.maxReadings.x = max(calibrationState.maxReadings.x, x);
  calibrationState.minReadings.y = min(calibrationState.minReadings.y, y);
  calibrationState.maxReadings.y = max(calibrationState.maxReadings.y, y);
  calibrationState.minReadings.z = min(calibrationState.minReadings.z, z);
  calibrationState.maxReadings.z = max(calibrationState.maxReadings.z, z);
 
  }

void
SPHMC5883L::readXYZ                   (float              *x,
                                       float              *y,
                                       float              *z)
 {
  int16_t rawX,rawY,rawZ;

  readXYZRaw(&rawX,&rawY,&rawZ);

  *x = rawX * gainFactor - offsetX;
  *y = rawY * gainFactor - offsetY;
  *z = rawZ * gainFactor - offsetZ;
 }

float
SPHMC5883L::getResolutionByGain           (byte                gainMode)
 {
  return DIGITAL_RESOLUTION[gainMode < SPHMC5883L_ARRAYSIZE(DIGITAL_RESOLUTION)
          ? gainMode
          : (SPHMC5883L_ARRAYSIZE(DIGITAL_RESOLUTION) - 1)];
 }

bool 
SPHMC5883L::hasSavedCalibrationData        (int eepromAddress) const
{
  unsigned long dataOfEeprom;
  float gainNow;
  bool isOk = false;
  
  EEPROM.get(eepromAddress, dataOfEeprom);
  eepromAddress += sizeof(uint32_t);
   
  if (dataOfEeprom == SPHMC5883L_EEPROM_MAGIC)
  {
    EEPROM.get(eepromAddress, gainNow);
   
    if  (gainFactor == gainNow)
    {
      isOk = true;
    }
  }
  return isOk;
}

void 
SPHMC5883L::saveCalibrationData            (int eepromAddress) const
{
  EEPROM.put(eepromAddress,(uint32_t)SPHMC5883L_EEPROM_MAGIC);
  eepromAddress += sizeof(uint32_t);
  EEPROM.put(eepromAddress, gainFactor); 
  eepromAddress += sizeof(float);
  EEPROM.put(eepromAddress, offsetX);
  eepromAddress += sizeof(float);
  EEPROM.put(eepromAddress, offsetY);
  eepromAddress += sizeof(float);
  EEPROM.put(eepromAddress, offsetZ);
}

bool
SPHMC5883L::loadCalibrationData            (int eepromAddress)
{
 bool ok = false;
 unsigned long eepromData;
 
 EEPROM.get(eepromAddress,eepromData);
 eepromAddress += sizeof(uint32_t);
 
 if (eepromData == SPHMC5883L_EEPROM_MAGIC)
  {
   float gainNow;  
   
   EEPROM.get(eepromAddress, gainNow);
  
   if (gainFactor == gainNow)
   {
    eepromAddress += sizeof(float);
    EEPROM.get(eepromAddress, offsetX);
    eepromAddress += sizeof(float);
    EEPROM.get(eepromAddress, offsetY);
    eepromAddress += sizeof(float);
    EEPROM.get(eepromAddress, offsetZ);

    Serial.print(offsetX);
    Serial.println();
    Serial.print(offsetY);
    Serial.println();
    Serial.print(offsetZ);
    Serial.println();
    
    ok = true;
   }
 }
 
 return ok;
}
          

 
