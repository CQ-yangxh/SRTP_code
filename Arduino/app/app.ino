#define BLINKER_BLE
#include <Blinker.h>


BlinkerNumber CO2("num-co2");
BlinkerNumber PM25("num-pm25");
BlinkerNumber TEM("num-tem");
BlinkerNumber HUM("num-hum");
BlinkerNumber TVOC("num-tvoc");
BlinkerNumber CH2O("num-ch2o");
int tmp1=0;
int tmp2=0;
int arr[15];
bool flag=false;
int count=0;

int cal_co2()
{
  int h,l,f;
  h=arr[1];
  l=arr[2];
  f=h*256+l;
  return f;
}

int cal_pm25()
{
  int h,l,f;
  h=arr[7];
  l=arr[8];
  f=h*256+l;
  return f;
}

float cal_tem()
{
  int h,l;
  float f;
  h=arr[11];
  l=arr[12];
  f=h+0.1*l;
  return f;
}

float cal_hum() 
{
  int h,l;
  float f;
  h=arr[13];
  l=arr[14];
  f=h+0.1*l;
  return f;
}

float cal_tvoc()
{
  int h,l,f;
  h=arr[5];
  l=arr[6];
  f=h*256+l;
  return f;
}

float cal_ch2o()
{
  int h,l,f;
  h=arr[3];
  l=arr[4];
  f=h*256+l;
  return f;
}
void heartbeat()
{
    CO2.print(cal_co2());
    PM25.print(cal_pm25());
    TEM.print(cal_tem());
    HUM.print(cal_hum());
    TVOC.print(cal_tvoc());
    CH2O.print(5);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  BLINKER_DEBUG.stream(Serial);
  Blinker.begin();
  Blinker.attachHeartbeat(heartbeat);
}

void loop() {
  // put your main code here, to run repeatedly:
  Blinker.run();
  if(Serial.available())
  {
    tmp1=Serial.read();
      if(tmp1==2)
      {
        if(tmp2==60)
        {
          flag=true;
        }
      }
      if(flag==true)
      {
        if(count!=15)
        {
          arr[count]=tmp1;//count=0时存储的为帧头2
          count++;
        }
        else
        {
          for(int j=0;j<15;j++)
          {
            Serial.println(arr[j]);
          }
          
          count=0;
          flag=false;
        }
      }
      tmp2=tmp1;
  }
  delay(2000);
}
