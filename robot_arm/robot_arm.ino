#include<Wire.h>                //I2C Wire Library 

#include<ESP32Servo.h>               //Servo Motor Library
#define button 23

Servo servo_1;      

Servo servo_2;

Servo servo_3;

Servo servo_4;


const int MPU_addr=0x68;        //MPU6050 I2C Address

int16_t axis_X,axis_Y,axis_Z;

int minVal=265;

int maxVal=402;


double x;

double y;

double z;

 

void setup()

{

  Serial.begin(9600);

  pinMode(button,INPUT_PULLUP);
  

  Wire.begin();                      //Initilize I2C Communication

  Wire.beginTransmission(MPU_addr);  //Start communication with MPU6050

  Wire.write(0x6B);                  //Writes to Register 6B

  Wire.write(0);                     //Writes 0 into 6B Register to Reset

  Wire.endTransmission(true);        //Ends I2C transmission

  

  servo_1.attach(18);   // Forward/Reverse_Motor

  servo_2.attach(5);   // Up/Down_Motor

  servo_3.attach(4);   // Gripper_Motor

  servo_4.attach(19);   // Left/Right_Motor
  pinMode(button,INPUT_PULLUP);

  

}


void loop()

{

  Wire.beginTransmission(MPU_addr);    

  Wire.write(0x3B);                  //Start with regsiter 0x3B 

  Wire.endTransmission(false);     

  Wire.requestFrom(MPU_addr,14,true);  //Read 14 Registers 

  

  axis_X=Wire.read()<<8|Wire.read();                //Reads the MPU6050 X,Y,Z AXIS Value

  axis_Y=Wire.read()<<8|Wire.read();

  axis_Z=Wire.read()<<8|Wire.read();

  

  int xAng = map(axis_X,minVal,maxVal,-90,90);     // Maps axis values in terms of -90 to +90  

  int yAng = map(axis_Y,minVal,maxVal,-90,90);

  int zAng = map(axis_Z,minVal,maxVal,-90,90);

       

  x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);       //Formula to convert into degree

  y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);

  z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);

  Serial.print("X : ");
  Serial.print(x);
  Serial.print(" Y : ");
  Serial.print(y);
  Serial.print(" Z : ");
  Serial.println(z);
  

  if(x >=0 && x <= 60) 

  {
    int mov2 = map(x,0,60,130,180);

     servo_2.write(mov2);

  } 

 

  else if(x >=300 && x <= 360) 

  {

    int mov1 = map(x,300,360,0,50);

     servo_1.write(mov1);

  } 




 

  if(y >=300 && y <= 360) 

  {
      int mov3 = map(y,360,300,90,180);

     servo_4.write(mov3);


  } 
  else if(y >=0 && y <= 60) 

  {

    int mov3 = map(y,0,60,90,0);

     servo_4.write(mov3);
     

  } 

  int a = digitalRead(button);
  if(a == 0){
    servo_3.write(30);
  }
  else{
    servo_3.write(0);
  }
  delay(10);
}