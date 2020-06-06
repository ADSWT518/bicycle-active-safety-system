#include <Servo.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX
int LED1 = 6;                    //车尾的灯
int LED2 = 7;                    //车把上的灯
int Buzzer = 8;                  //蜂鸣器
Servo myservo;                   //舵机

float L1, L2, L3, v;        //三次测量的距离和计算出的速度
int brakingTime = 2;        //这里估算刹车时间为2s
float warningVelocity;      //速度大于这个就自动刹车
int warningDisdance = 1500; //距离小于这个就提醒（1.5m
int interval;               //灯和蜂鸣器的响应的间隔

void setup()
{
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  myservo.attach(9); //舵机连9号口
  Serial.begin(9600);
  mySerial.begin(9600);
}

int getDistance()
{
  mySerial.write(85); //向RX端发送0X55
  byte serialData[5];
  int dis;
  while (mySerial.available())
  {
    byte b = mySerial.read();
  }
  mySerial.setTimeout(95);
  if (mySerial.readBytes(serialData, 4) > 0)
  {
    dis = ((serialData[1] * 256) + serialData[2]);
  }
  return dis; //单位mm
}

void loop()
{
  L1 = getDistance(); //从传感器获取距离，函数在上面
  delay(15);
  L2 = getDistance();
  delay(15);
  L3 = getDistance();
  v = ((L1 - L2) / 15 + (L2 - L3) / 15) / 2;
  warningVelocity = 2 * L3 / brakingTime;

  while (L3 < warningDisdance && v > 0) //距离小于安全距离且不断接近中
  {
    interval = (warningDisdance - L3) / warningDisdance * 100; //距离越小，闪烁间隔越小
    digitalWrite(LED2, HIGH);
    digitalWrite(Buzzer, HIGH);
    delay(interval / v);
    digitalWrite(LED2, LOW);
    digitalWrite(Buzzer, LOW);
    delay(interval / v);

    L1 = getDistance(); //从传感器获取距离，函数在上面
    delay(15);
    L2 = getDistance();
    delay(15);
    L3 = getDistance();
    v = ((L1 - L2) / 15 + (L2 - L3) / 15) / 2;
    warningVelocity = 2 * L3 / brakingTime;

    if (v > warningVelocity) //相对速度大于危险速度
    {                        //舵机刹车，车尾预警灯亮起
      digitalWrite(LED1, HIGH);
      for (int i = 0; i < 180; i += 5) //最大角度需实测
      {
        myservo.write(i); //这里想实现慢慢刹车（模拟手动刹车的情况）但是不知道效果如何
        delay(15);
      }
    }
    digitalWrite(LED1, LOW); //刹车后恢复原位
    myservo.write(0);
  }
}
