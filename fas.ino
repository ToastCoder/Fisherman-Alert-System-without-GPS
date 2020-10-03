
#include <Wire.h>
float t2=0,t1=0,a;
long accelX, accelY, accelZ;
float AcX,AcY,AcZ,T;
long gyroX, gyroY, gyroZ;
float angX,angY,angZ,u,v,x,r,y;
void setup() {
  Serial.begin(9600);
  Wire.begin();
  setupMPU();
}


void loop() {
  recordAccelRegisters();
  recordGyroRegisters();
  printData();
  if(a>=-0){
    tone(11,550);
  }
}

void setupMPU(){
  Wire.beginTransmission(0b1101000); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
  Wire.endTransmission();  
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1B); //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4) 
  Wire.write(0x00000000); //Setting the gyro to full scale +/- 250deg./s 
  Wire.endTransmission(); 
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1C); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
  Wire.write(0b00000000); //Setting the accel to +/- 2g
  Wire.endTransmission(); 
}

void recordAccelRegisters() {
  t1=millis();
  delay(10);
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x3B); //Starting register for Accel Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Accel Registers (3B - 40)
  while(Wire.available() < 6);
  accelX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  accelY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  accelZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
  t2=millis();
  processAccelData();
}

void processAccelData(){
  AcX = (accelX / 16384.0)*9.81;
  AcY = (accelY / 16384.0)*9.81; 
  AcZ = (accelZ / 16384.0)*9.81;
}

void recordGyroRegisters() {
  delay(10);
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x43); //Starting register for Gyro Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Gyro Registers (43 - 48)
  while(Wire.available() < 6);
  gyroX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  gyroY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  gyroZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
  processGyroData();
}

void processGyroData() {
  angX = gyroX / 131.0;
  angY = gyroY / 131.0; 
  angZ = gyroZ / 131.0;
}

void printData() {
  T=(t2-t1)/1000;
  v=u+(AcZ*T);
  r=((u*T)+(0.5*AcZ*T*T))*100;
  u=v;
  angZ=(angZ*71.0)/4068.0; 
  x=x+(r*(cos(angZ)));
  y=y+(r*(sin(angZ)));
  Serial.println("x-axis in cm");
  Serial.print(x);
  Serial.print("|y-axis in cm");
  Serial.print(y); 
  if(y>=3 && y<12){
    a=-(9*x)-(5*y)+150;
  }
  else{ 
    if(y<3 && y>=(-9)){
    a=-(7*x)+(4*y)+93;
    }
    else{
      a=8;
    }
  }
}
