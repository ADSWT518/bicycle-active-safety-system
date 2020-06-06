#include <Servo.h>
#include <SoftwareSerial.h>

struct DT
{
  float dis;
  float tim;
};

Servo myservo;
SoftwareSerial mySerial(10, 11); // RX, TX
int LED1 = 6;                    //车尾的灯
int LED2 = 7;                    //车把上的灯
int Buzzer = 8;                  //蜂鸣器
int i;

DT L[3];
float v;              //三次测量的距离和计算出的速度
int brakingTime = 20; //这里估算刹车时间为2s
float brakingAcceleration = 6;
float currentDistance;
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

DT getDistance()
{
  while (1) //测距不成功则再次测距，直至成功
  {
    while (mySerial.available()) //清空缓冲区
    {
      mySerial.read();
    }
    mySerial.write(85); //发送测距指令
    byte input[4];
    DT DisTime;
    mySerial.setTimeout(95);
    if (mySerial.readBytes(input, 4) > 0 && input[0] == 255 && (input[1] + input[2] - 1) % 256 == input[3]) //校验数据
    {
      DisTime.tim = millis();
      DisTime.dis = ((input[1] * 256) + input[2]);
      Serial.println(DisTime.dis);
      return DisTime;
    }
  }
}

void loop()
{
  delay(2000);
  L[0] = getDistance();
  L[1] = getDistance();
  warningVelocity = 100000;
  v = ((L[0]).dis - (L[1]).dis) / ((L[1]).tim - (L[0]).tim);
  for (i = 2; v < warningVelocity; ++i) //保存2个之前距离，测一个新距离，保存在L[i]中
  {
    i = i % 3;
    L[i] = getDistance();
    switch (i)
    {
    case 0:
      v = ((L[1]).dis - (L[0]).dis) / ((L[0]).tim - (L[1]).tim);
      break;
    case 1:
      v = ((L[2]).dis - (L[1]).dis) / ((L[1]).tim - (L[2]).tim);
      break;
    case 2:
      v = ((L[0]).dis - (L[2]).dis) / ((L[2]).tim - (L[0]).tim);
      break;
    }
    Serial.println(v);
    warningVelocity = (L[i]).dis / brakingTime;
  } //跳出循环，进入刹车模式
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(Buzzer, HIGH); //预警
  delay(20);
  myservo.write(120); //刹车
  delay(2000);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(Buzzer, LOW);
  myservo.write(30);
}
