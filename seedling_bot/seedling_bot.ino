#include "BluetoothSerial.h"
#include <ESP32Servo.h>
#define m1 33
#define m2 25
#define m3 26
#define m4 27
#define enb1 32
#define enb2 14
#define led 2
#define front1 18
#define front2 4
#define back1 5
#define back2 19
#define sandled 21
int motorspeed = 70;
Servo seed_servo ;
Servo sand_servo ;
int seed_servo_pin = 13;
int sand_servo_pin = 12;
int relay = 15;
BluetoothSerial SerialBT;

void setup() 
{
  pinMode(m1,OUTPUT);
  pinMode(m2,OUTPUT);
  pinMode(m3,OUTPUT);
  pinMode(m4,OUTPUT);
  pinMode(enb1,OUTPUT);
  pinMode(enb2,OUTPUT);
  pinMode(front1,OUTPUT);
  pinMode(front2,OUTPUT);
  pinMode(back1,OUTPUT);
  pinMode(back2,OUTPUT);
  pinMode(sandled,OUTPUT);
  Serial.begin(115200);
  pinMode(led,OUTPUT);
  pinMode(relay,OUTPUT);
  digitalWrite(relay,HIGH);
  SerialBT.begin("AUTOMATIC PLANTING ROBOT");
  motorstops();
  seed_servo.attach(seed_servo_pin);
  sand_servo.attach(sand_servo_pin);
  seed_servo.write(25);
  sand_servo.write(0);
}
  void motorstops()
  {
    digitalWrite(m1,0);
    digitalWrite(m2,0);
    digitalWrite(m3,0);
    digitalWrite(m4,0);

  }
  void moveforward()
  {
    digitalWrite(front1,HIGH);
    digitalWrite(front2,HIGH);
    digitalWrite(m1,0);
    digitalWrite(m2,1);
    digitalWrite(m3,0);
    digitalWrite(m4,1);

  }
  void movereverse()
  {
    digitalWrite(m1,1);
    digitalWrite(m2,0);
    digitalWrite(m3,1);
    digitalWrite(m4,0);
  }
  void turnleft()
  {
    digitalWrite(m1,0);
    digitalWrite(m2,1);
    digitalWrite(m3,0);
    digitalWrite(m4,0);
  }
  void turnright()
  {

    digitalWrite(m1,0);
    digitalWrite(m2,0);
    digitalWrite(m3,0);
    digitalWrite(m4,1);
  }
int value;
String direction="";
void loop() {
  analogWrite(enb1,motorspeed);
  analogWrite(enb2,motorspeed);
  if(SerialBT.hasClient()){
    digitalWrite(led,HIGH); 
  }
  else{
    digitalWrite(led,LOW);
  }
  if(SerialBT.available() > 0){    
    value = SerialBT.read();     
  }
  Serial.println(value);
    if (value == 5){
      Serial.println("stop");
      direction = "stop";
      motorstops();
    }
    else if (value == 1){
      direction = "forward";
      Serial.println("forward");
      moveforward();
    }
    else if (value == 4){
      direction = "right";
      Serial.println("right");
      turnright();
    }
    else if (value == 2){
      direction = "reverse";
      Serial.println("reverse");
      movereverse();
    }
    else if(value==3){
      direction = "left";
      turnleft();
      Serial.println("left");
    }
    else if(value == 6){
      seed_servo.write(0);
      delay(300);
      seed_servo.write(25);
      delay(300);
      value =5 ;
    }
    else if(value == 7){
      digitalWrite(sandled,LOW);
      sand_servo.write(0);
      value = 5;
    }
    else if(value == 8){
      digitalWrite(sandled,HIGH);
      sand_servo.write(30);
      value = 5;
    }
    else if(value == 9){
      digitalWrite(relay,LOW);
      value = 5;
    }
    else if(value == 10){
      digitalWrite(relay,HIGH);
      value = 5;
    }
    else if (value > 10){
      int seeds = value - 10;
      seed_servo.write(25);
      sand_servo.write(30);
      digitalWrite(sandled,HIGH);
      digitalWrite(relay,LOW);
      for(int i=1;i<=seeds;i++){
        SerialBT.print(i);
        Serial.println(i);
        moveforward();
        delay(1000);
        seed_servo.write(0);
        delay(300);
        seed_servo.write(25);
        delay(300);
      }
      digitalWrite(relay,HIGH);
      value = 5;
      sand_servo.write(0);
      digitalWrite(sandled,LOW);
      seed_servo.write(25);
    }
    if(direction == "forward"){
      digitalWrite(front1,HIGH);
      digitalWrite(front2,HIGH);
    }
    else{
      digitalWrite(front1,LOW);
      digitalWrite(front2,LOW);
    }
    if(direction == "reverse"){
      digitalWrite(back1,HIGH);
      digitalWrite(back2,HIGH);
    }
    else{
      digitalWrite(back1,LOW);
      digitalWrite(back2,LOW);
    }
  }