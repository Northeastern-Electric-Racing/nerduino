#include "amc6821.h"
#include <Arduino.h>


AMC6821::AMC6821()
{
    
    if(verifyFunctionality())
    {
        Serial.println("AMC6821 PWM Chip connected");
        getCharacteristics();
        enablePWM(1);
        writeConfig(1, 0x08);
        writeConfig(2, 0x03);
        writeConfig(3, 0x02);
        writeConfig(4, 0x88); 
        return;
    }
    Serial.println("~~~~~~~~~~~~~~~~WARNING: Unable to verify functionality of AMC6821~~~~~~~~~~~~~~~~~~~~~~~");
    return;
    
}


AMC6821::~AMC6821(){}


void AMC6821::AMC6821write(uint8_t *cmd, uint8_t numBytes)
{
    Wire.beginTransmission(AMC6821_I2C_ADDR);
    for(uint8_t i=0; i<numBytes;i++)
    {
        Wire.write(cmd[i]);
    }
    Wire.endTransmission();
}


bool AMC6821::AMC6821read(uint8_t *msg,uint8_t numBytes)
{
    Wire.requestFrom(AMC6821_I2C_ADDR, (int)numBytes);

    if (Wire.available())
    {
        uint8_t i2cByte=0;
        while(Wire.available())
        {
            msg[i2cByte] = Wire.read();
            i2cByte++;
        }
        return true;
    }

    return false;
}


bool AMC6821::verifyFunctionality()
{
    uint8_t msg[1];
    uint8_t cmd[1]= {AMC6821_DEVID_REG};
    AMC6821write(cmd,1);
    if(AMC6821read(msg,1))
    {
        if(msg[0] == AMC6821_DEVID)
        {
            return true;
        }
        return false;
    }
    return false;
}


void AMC6821::enablePWM(bool pwm_toggle)
{
    uint8_t config2 = getConfig2Reg();
    if (config2 == NULL)
    {
        Serial.println("PWM Toggle not successful");
        return;
    }
    uint8_t newconfig2;
    if (pwm_toggle == true){
        newconfig2 = config2 | 0x01;
    }
    else{
        newconfig2 = config2 & 0xFE;
    }
    setConfig2Reg(newconfig2);
}

void AMC6821::enableFanSpinup(bool fanspinup_toggle)
{
    characteristicsmsg.bitfieldmsg.fanspinup_enable = fanspinup_toggle;
    setCharacteristics();
}


void AMC6821::setFanSpinUpTime(fanspinuptime_t fanspinuptime)
{
    characteristicsmsg.bitfieldmsg.fanspinuptime = fanspinuptime;
    setCharacteristics();
}


void AMC6821::setPWMFreq(pwmfreq_t pwmfreq)
{
    characteristicsmsg.bitfieldmsg.pwmfreq = pwmfreq;
    setCharacteristics();
}


void AMC6821::setDutyCycle(uint8_t dutycycle)
{
    uint8_t cmd[2] = {AMC6821_DUTYCYCLE_REG, dutycycle};
    AMC6821write(cmd, 2);
    AMC6821write(cmd, 1);
}


void AMC6821::setCharacteristics()
{
    AMC6821write(characteristicsmsg.msg,1);
}


void AMC6821::getCharacteristics()
{
    uint8_t cmd[1] = {AMC6821_CHARACTERISTICS_REG};
    AMC6821write(cmd,1);
    AMC6821read(characteristicsmsg.msg,1);
    return;
}

uint8_t AMC6821::getConfig2Reg()
{
    uint8_t cmd[1] = {AMC6821_CONFIG2_REG};
    uint8_t msg[1];
    AMC6821write(cmd,1);
    if(AMC6821read(msg,1))
    {
        return msg[0];
    }
    Serial.println("ERROR: Unable to read from AMC6821 Configuration 2 Register");
    return NULL;
}


void AMC6821::setConfig2Reg(uint8_t config)
{
    uint8_t cmd[2] = {AMC6821_CONFIG2_REG, config};
    AMC6821write(cmd,2);
    return;
}

void AMC6821::resetChip()
{
  uint8_t cmd[2] = {AMC6821_CONFIG2_REG, 0x80}; //Writes a 1 in the reset bit in Config 2
  AMC6821write(cmd,2);
}

void AMC6821::writeConfig(uint8_t configNum, uint8_t config)
{
  uint8_t cmd[2];
  cmd[1] = uint8_t(config);

  switch(configNum)
  {
    case 1:
      cmd[0] =  AMC6821_CONFIG1_REG;
      break;
    case 2:
      cmd[0] =  AMC6821_CONFIG2_REG;
      break;
    case 3:
      cmd[0] =  AMC6821_CONFIG3_REG;
      break;
    case 4:
      cmd[0] =  AMC6821_CONFIG4_REG;
      break;
    default:
      Serial.println("Unidentified Config #!");
      break;
  }
  
  AMC6821write(cmd,2);
}