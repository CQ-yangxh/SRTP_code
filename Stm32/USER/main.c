#include "stm32f10x.h"  
#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "gps.h"	
#include "usart3.h" 
#include "usart.h"
#include "rs485.h"
#include "string.h"	
#include "vs10xx.h"
#include "w25qxx.h"      
#include "text.h"

void Delay(u32 count) 
 {    
	 u32 i=0;    
	 for(;i<count;i++);	 
 } 
 
u8 USART1_TX_BUF[USART3_MAX_RECV_LEN]; 					//串口1,发送缓存区
nmea_msg gpsx; 											//GPS信息
__align(4) u8 dtbuf[50];   								//打印缓存器
const u8*fixmode_tbl[4]={"Fail","Fail"," 2D "," 3D "};	//fix mode字符串 
	  
//显示GPS定位信息 
void Gps_Msg_Show(void)
{
 	float tp;		   
	POINT_COLOR=BLUE;  	 
	tp=gpsx.longitude;	   
	sprintf((char *)dtbuf,"Longitude:%.5f %1c   ",tp/=100000,gpsx.ewhemi);	//得到经度字符串
 	LCD_ShowString(30,140,200,16,16,dtbuf);	 	   
	tp=gpsx.latitude;	   
	sprintf((char *)dtbuf,"Latitude:%.5f %1c   ",tp/=100000,gpsx.nshemi);	//得到纬度字符串
 	LCD_ShowString(30,160,200,16,16,dtbuf);	 	 
	tp=gpsx.altitude;	   
 	sprintf((char *)dtbuf,"Altitude:%.1fm     ",tp/=10);	    			//得到高度字符串
 	LCD_ShowString(30,180,200,16,16,dtbuf);	 			   
	tp=gpsx.speed;	   
 	sprintf((char *)dtbuf,"Speed:%.3fkm/h     ",tp/=1000);		    		//得到速度字符串	 
 	LCD_ShowString(30,200,200,16,16,dtbuf);	 				    
	if(gpsx.fixmode<=3)														//定位状态
	{  
		sprintf((char *)dtbuf,"Fix Mode:%s",fixmode_tbl[gpsx.fixmode]);	
	  LCD_ShowString(30,220,200,16,16,dtbuf);			   
	}	 	   
	sprintf((char *)dtbuf,"GPS+BD Valid satellite:%02d",gpsx.posslnum);	 		//用于定位的GPS卫星数
 	LCD_ShowString(30,240,200,16,16,dtbuf);	    
	sprintf((char *)dtbuf,"GPS Visible satellite:%02d",gpsx.svnum%100);	 		//可见GPS卫星数
 	LCD_ShowString(30,260,200,16,16,dtbuf);
	
	sprintf((char *)dtbuf,"BD Visible satellite:%02d",gpsx.beidou_svnum%100);	 		//可见北斗卫星数
 	LCD_ShowString(30,280,200,16,16,dtbuf);
	
	sprintf((char *)dtbuf,"UTC Date:%04d/%02d/%02d   ",gpsx.utc.year,gpsx.utc.month,gpsx.utc.date);	//显示UTC日期
	LCD_ShowString(30,300,200,16,16,dtbuf);		    
	sprintf((char *)dtbuf,"UTC Time:%02d:%02d:%02d   ",gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec);	//显示UTC时间
  LCD_ShowString(30,320,200,16,16,dtbuf);		  	  
} 
//16进制转化为ASCII，在原子云服务器上显示
 void HexToAscii(unsigned char *pHex, unsigned char *pAscii, int nLen)
			{
				unsigned char Nibble[2];
				unsigned int i,j;
					for (i = 0; i < nLen; i++){
							Nibble[0] = (pHex[i] & 0xF0) >> 4;
							Nibble[1] = pHex[i] & 0x0F;
							for (j = 0; j < 2; j++){
									if (Nibble[j] < 10){ 			
											Nibble[j] += 0x30;
									}
									else{
											if (Nibble[j] < 16)
													Nibble[j] = Nibble[j] - 10 + 'A';
									}
									*pAscii++ = Nibble[j];
							}   			// for (int j = ...)
					}  		 	// for (int i = ...)
			}
 
 int main(void)  
{     
	u8 i=0,t=0;
	u8 cnt=0;
	u8 rs485buf[24]; 
	u16 len;	
	u16 k,rxlen;
	u8 key;  
	delay_init();   
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);     
	uart_init(9600); 
	LED_Init();		
	LCD_Init();
	uart2_Init(115200);	//初始化串口2
	usart3_init(38400);		//初始化串口3 
	W25QXX_Init();				//初始化W25Q128
	while(font_init()) 				//检查字库
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//清除显示	     
	}
	POINT_COLOR=RED;
  LCD_ShowString(30,30,210,24,24,"WarShip STM32 ^_^"); 
	Show_Str(30,60,300,24,"环境移动监测系统",24,0);
	Show_Str(30,85,200,24,"GPS位置信息:",24,0);	
	//传感器数据显示
	POINT_COLOR=RED;
	Show_Str(30,360,360,24,"传感器数据:",24,0);
	POINT_COLOR=BLUE;
	LCD_ShowString(30,390,200,16,16,"feature:");
	LCD_ShowString(30,410,200,16,16,"CO2    :");
	LCD_ShowString(30,430,200,16,16,"CH20   :");
	LCD_ShowString(30,450,200,16,16,"TVOC   :");
	LCD_ShowString(30,470,200,16,16,"PM2.5  :");
	LCD_ShowString(30,490,200,16,16,"PM10   :");
	LCD_ShowString(30,510,200,16,16,"Tem    :");
	LCD_ShowString(30,530,200,16,16,"Hum    :");
	LCD_ShowString(30,550,200,16,16,"sum    :");
	POINT_COLOR=BLUE;//设置字体为蓝色	
	//北斗-GPS定位显示的一个过程
	if(SkyTra_Cfg_Rate(5)!=0)	//设置定位信息更新速度为5Hz,顺便判断GPS模块是否在位. 
	{
   	LCD_ShowString(30,120,200,16,16,"SkyTraF8-BD Setting...");
		do
		{
			usart3_init(9600);			//初始化串口3波特率为9600
	  	SkyTra_Cfg_Prt(3);			//重新设置模块的波特率为38400
			usart3_init(38400);			//初始化串口3波特率为38400
      key=SkyTra_Cfg_Tp(100000);	//脉冲宽度为100ms
		}while(SkyTra_Cfg_Rate(5)!=0&&key!=0);//配置SkyTraF8-BD的更新速率为5Hz
	  LCD_ShowString(30,120,200,16,16,"SkyTraF8-BD Set Done!!");
		delay_ms(500);
		LCD_Fill(30,120,30+200,120+16,WHITE);//清除显示 
	}
	//数据的传送与显示
	u8 bitstatus;
	while(1) 
	{		
		//北斗-GPS定位显示的一个过程
		if(USART3_RX_STA&0X8000)		//接收到一次数据了
		{
			rxlen=USART3_RX_STA&0X7FFF;	//得到数据长度
			for(k=0;k<rxlen;k++)USART1_TX_BUF[k]=USART3_RX_BUF[k];	   //将数据从串口接收缓冲区中取出
 			USART3_RX_STA=0;		   	//启动下一次接收
			//USART1_TX_BUF[k]=0;			//自动添加结束符
			GPS_Analysis(&gpsx,(u8*)USART1_TX_BUF);//分析字符串
			Gps_Msg_Show();				//显示信息	
 		}
		 
		/*
		串口的状态可以通过状态寄存器 USART_SR 读取
		判断RXNE（读数据寄存器非空），当该位被置 1 的时候，就是提示已经有数据被接收到了，并且可以读出来了。
		这时候我们要做的就是尽快去读取 USART_DR，通过读 USART_DR 可以将 该位清零，也可以向该位写 0，直接清除
		
		*/
		USART_ReceiveData(USART1);
		//如我们要判断读寄存器是否非空(RXNE)，操作库函数的方法是： USART_GetFlagStatus(USART1, USART_FLAG_RXNE); 
		bitstatus=USART_GetFlagStatus(USART1, USART_RX_STA); 
		if(bitstatus)
		{	
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			USART_RX_STA=0;
			if(len==17)
			{
				for(t=0;t<len;t++)rs485buf[t]=USART_RX_BUF[t];	
			}
			else
			{
				for(t=len-17;t<len;t++)rs485buf[t+17-len]=USART_RX_BUF[t];
			}
			//传感器数据、定位信息位置的发送
			usart2_Send_Data(USART1_TX_BUF,69);
			u8 rs232[100]={};
			HexToAscii(rs485buf,rs232,17);//将接收的16位进制数转换为ASCII码
			usart2_Send_Data(rs232,34);
			//LCD屏传感器数据的展示
			for(i=0;i<len;i++)
				{
					//rs485buf[i]=cnt+rs485buf[i];//填充发送缓冲区
					if(i<2)
					{
						LCD_ShowxNum(60+(i+1)*33,390,rs485buf[i],2,16,0X80);	//显示数据
					}	
					else if(2<=i&i<4)
					{
						LCD_ShowxNum(60+(i-1)*33,410,rs485buf[i],2,16,0X80);
					}
					else if(4<=i&i<6)
					{
						LCD_ShowxNum(60+(i-3)*33,430,rs485buf[i],2,16,0X80);
					}
					else if(6<=i&i<8)
					{
						LCD_ShowxNum(60+(i-5)*33,450,rs485buf[i],2,16,0X80);
					}
					else if(8<=i&i<10)
					{
						LCD_ShowxNum(60+(i-7)*33,470,rs485buf[i],2,16,0X80);
					}
					else if(10<=i&i<12)
					{
						LCD_ShowxNum(60+(i-9)*33,490,rs485buf[i],2,16,0X80);
					}
					else if(12<=i&i<14)
					{
						LCD_ShowxNum(60+(i-11)*33,510,rs485buf[i],2,16,0X80);
					}
					else if(14<=i&i<16)
					{
						LCD_ShowxNum(60+(i-13)*33,530,rs485buf[i],2,16,0X80);
					}
					else if(i==16)
					{
						LCD_ShowxNum(60+(i-15)*33,550,rs485buf[i],3,16,0X80);
					}
				}	
				//LCD_ShowxNum(60+4*32,390,rs485buf[0]*16*16+rs485buf[1],5,16,0X80);
				LCD_ShowxNum(60+4*32,410,rs485buf[2]*16*16+rs485buf[3],5,16,0X80);
				LCD_ShowString(60+5*35,410,200,16,16,"ppm");
				LCD_ShowxNum(60+4*32,430,rs485buf[4]*16*16+rs485buf[5],5,16,0X80);
				LCD_ShowString(60+5*35,430,200,16,16,"ug/m*3");
				LCD_ShowxNum(60+4*32,450,rs485buf[6]*16*16+rs485buf[7],5,16,0X80);
				LCD_ShowString(60+5*35,450,200,16,16,"ug/m*3");
				LCD_ShowxNum(60+4*32,470,rs485buf[8]*16*16+rs485buf[9],5,16,0X80);
				LCD_ShowString(60+5*35,470,200,16,16,"ug/m*3");
				LCD_ShowxNum(60+4*32,490,rs485buf[10]*16*16+rs485buf[11],5,16,0X80);
				LCD_ShowString(60+5*35,490,200,16,16,"ug/m*3");
				LCD_ShowxNum(60+4*32,510,rs485buf[12],2,16,0X80);
				LCD_ShowxNum(60+5*30+2,510,rs485buf[13],2,16,0X80);
				LCD_ShowString(60+4*36,510,200,16,16,".");
				LCD_ShowString(60+5*35,510,200,16,16,"C");
				LCD_ShowxNum(60+4*32,530,rs485buf[14],2,16,0X80);
				LCD_ShowxNum(60+5*30+2,530,rs485buf[15],2,16,0X80);
				LCD_ShowString(60+4*36,530,200,16,16,".");
				LCD_ShowString(60+5*35,530,200,16,16,"RH");
				LCD_ShowxNum(60+4*32,550,rs485buf[16],4,16,0X80);
			if(t==1)
			{
				LED0=!LED0;//提示系统正在运行	
				t=0;
				cnt++;
				LCD_ShowxNum(30+48,150,cnt,3,16,0X80);	//显示数据
			}		   
		}
	}		
}

