int servoPin = 9; //舵机

void setup()
{

  pinMode(servoPin, OUTPUT);
}

void servoPulse(int angle) //定义一个脉冲函数来控制舵机的转动
{
  int pulseWidth = (angle * 11) + 500; //将角度转化为500-2480的脉宽值，每多转1度，对应高电平多11us
  digitalWrite(servoPin, HIGH);        //将舵机接口电平至高
  delayMicroseconds(pulseWidth);       //延时脉宽值的微秒数
  digitalWrite(servoPin, LOW);         //将舵机接口电平至低
  delayMicroseconds(20000 - pulseWidth);
}

void loop()
{
  servoPulse(0);
  servoPulse(90);
  servoPulse(180);
}
