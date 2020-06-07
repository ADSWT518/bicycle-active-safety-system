#include <Servo.h>
#include <SoftwareSerial.h>
#include <math.h>

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

DT L_last, L_current;
float v;                //三次测量的距离和计算出的速度
int brakingTime = 2000; //这里估算刹车时间为2s
float brakingAcceleration = 6;
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
    delay(60);                                                                                              //保证这里得到的是一次探测的数据
    if (mySerial.readBytes(input, 4) > 0 && input[0] == 255 && (input[1] + input[2] - 1) % 256 == input[3]) //校验数据（有可能可以不需要校验？
    {
      DisTime.tim = millis();
      DisTime.dis = (input[1] << 8) + input[2]; //用位运算代替乘法
      //Serial.println(DisTime.dis);              //调试时使用
      return DisTime;
    }
  }
}

void loop()
{
  delay(2000);
  L_last = getDistance();
  L_current = getDistance();
  warningVelocity = 100000;
  v = abs(L_last.dis - L_current.dis) / (L_current.tim - L_last.tim);
  while (v < warningVelocity) //保存2个之前距离，测一个新距离，保存在L[i]中
  {
    L_current = getDistance();
    if (L_current.dis < warningDisdance && L_last.dis < warningDisdance)
    {
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
    }
    v = bs(L_last.dis - L_current.dis) / (L_current.tim - L_last.tim);
    L_last.dis = L_current.dis;
    L_last.tim = L_current.tim;
    //Serial.println(v);
    warningVelocity = L_current.dis / brakingTime;
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
