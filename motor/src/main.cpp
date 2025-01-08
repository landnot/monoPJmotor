#include <Arduino.h>
#include <M5Atom.h>
#define MOTOR_PIN 25 //モーター制御ピン
#define MOTOR_PIN2 21 //モーター制御ピン
#define MIXER_LED 23 //撹拌機LED出力
#define WATER_LED 33 //水投入LED出力
#define SENSOR_LOW 19 //水位センサ下限の入力
#define SENSOR_HIGH 22 //水位センサ上限の入力
#define MOTOR_SPEED 255 //モータースピード(デューティー比) 0~255
#define MOVESECONDS_MOTOR 3*1000 //モーター稼働時間
#define MOVESECONDS_MIXER 3*1000 //撹拌機稼働時間
#define WAITINGTIME 1*1000 //水位センサチェック時間
// put function declarations here:
bool motorMove();
void motorStop();
void waterReset();//水位センサ下限リセット
void lowCheck();//水位センサ下限チェック
void highCheck();//水位センサ上限チェック

bool start = false; //スタートフラグ
//bool waterCountStart = false;
bool intoWater = false; //水投入フラグ
bool complete = false; //完了フラグ
unsigned long water1=0, water2=0,motor1=0,motor2=0,inWater1=0,inWater2=0; //時間計る用

void setup()
{
  M5.begin(true, true, true);
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(MOTOR_PIN2, OUTPUT);
  pinMode(MIXER_LED, OUTPUT);
  pinMode(WATER_LED, OUTPUT);
  pinMode(SENSOR_LOW, INPUT_PULLUP);
  pinMode(SENSOR_HIGH, INPUT_PULLUP);


  digitalWrite(MOTOR_PIN, LOW);
  digitalWrite(MIXER_LED, LOW);
  digitalWrite(WATER_LED, LOW);
  digitalWrite(MOTOR_PIN2, LOW);
}

void loop()
{
  if(!start && !intoWater){
    Serial.println("lowCheck");
    lowCheck();
  }
  /*
  if (digitalRead(SENSOR_LOW) == HIGH) {
    if(!waterCountStart && !start && !intoWater){
      water1 = millis();
      waterCountStart = true;
    }else if(waterCountStart && !start && !intoWater){
      water2 = millis();
      if(water2 - water1 > WAITINGTIME){
        start = true;
      }
    }
  }
  */
  if(start){
    waterReset();
    if(motor1 == 0){
      motor1 = millis();
    }
    if(!motorMove()){

        motorStop();
        Serial.println("motorStop");
        digitalWrite(MIXER_LED, HIGH);
        Serial.println("mixerStart");
        delay(MOVESECONDS_MIXER);
        digitalWrite(MIXER_LED, LOW);
        Serial.println("mixerStop");
        delay(10);
        motor1 = 0;
        start = false;
        intoWater = true;

      
    }
 

  }
  if(intoWater){
      Serial.println("intoWater");
      digitalWrite(WATER_LED, HIGH);
      delay(1000);
      highCheck();
    }
   



}

// put function definitions here:
bool motorMove()
{
  Serial.println("motorMove");
  digitalWrite(MOTOR_PIN, HIGH);
  //analogWrite(MOTOR_PIN, MOTOR_SPEED);
  motor2 = millis();
  if(motor2 - motor1 > MOVESECONDS_MOTOR)return false;
  return true;
}

void motorStop()
{
  digitalWrite(MOTOR_PIN, LOW);
}

void waterReset()
{
  water1=0;water2=0;
}

void lowCheck()
{
  if (digitalRead(SENSOR_LOW) == LOW)
  {
    water1 = millis();
    while (digitalRead(SENSOR_LOW) == LOW)
    {
      water2 = millis();
      if (water2 - water1 > WAITINGTIME)
      {
        start = true;
        break;
      }
    }
  }
}

void highCheck()
{
  Serial.println("highCheck");
  if (digitalRead(SENSOR_HIGH) == HIGH)
  {
    inWater1 = millis();
    while (digitalRead(SENSOR_HIGH) == HIGH)
    {
      inWater2 = millis();
      if (inWater2 - inWater1 > WAITINGTIME)
      {
        intoWater = false;
        digitalWrite(WATER_LED, LOW);
        complete = true;
        break;
      }
    }
  }
}