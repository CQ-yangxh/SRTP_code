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
 
u8 USART1_TX_BUF[USART3_MAX_RECV_LEN]; 					//����1,���ͻ�����
nmea_msg gpsx; 											//GPS��Ϣ
__align(4) u8 dtbuf[50];   								//��ӡ������
const u8*fixmode_tbl[4]={"Fail","Fail"," 2D "," 3D "};	//fix mode�ַ��� 
	  
//��ʾGPS��λ��Ϣ 
void Gps_Msg_Show(void)
{
 	float tp;		   
	POINT_COLOR=BLUE;  	 
	tp=gpsx.longitude;	   
	sprintf((char *)dtbuf,"Longitude:%.5f %1c   ",tp/=100000,gpsx.ewhemi);	//�õ������ַ���
 	LCD_ShowString(30,140,200,16,16,dtbuf);	 	   
	tp=gpsx.latitude;	   
	sprintf((char *)dtbuf,"Latitude:%.5f %1c   ",tp/=100000,gpsx.nshemi);	//�õ�γ���ַ���
 	LCD_ShowString(30,160,200,16,16,dtbuf);	 	 
	tp=gpsx.altitude;	   
 	sprintf((char *)dtbuf,"Altitude:%.1fm     ",tp/=10);	    			//�õ��߶��ַ���
 	LCD_ShowString(30,180,200,16,16,dtbuf);	 			   
	tp=gpsx.speed;	   
 	sprintf((char *)dtbuf,"Speed:%.3fkm/h     ",tp/=1000);		    		//�õ��ٶ��ַ���	 
 	LCD_ShowString(30,200,200,16,16,dtbuf);	 				    
	if(gpsx.fixmode<=3)														//��λ״̬
	{  
		sprintf((char *)dtbuf,"Fix Mode:%s",fixmode_tbl[gpsx.fixmode]);	
	  LCD_ShowString(30,220,200,16,16,dtbuf);			   
	}	 	   
	sprintf((char *)dtbuf,"GPS+BD Valid satellite:%02d",gpsx.posslnum);	 		//���ڶ�λ��GPS������
 	LCD_ShowString(30,240,200,16,16,dtbuf);	    
	sprintf((char *)dtbuf,"GPS Visible satellite:%02d",gpsx.svnum%100);	 		//�ɼ�GPS������
 	LCD_ShowString(30,260,200,16,16,dtbuf);
	
	sprintf((char *)dtbuf,"BD Visible satellite:%02d",gpsx.beidou_svnum%100);	 		//�ɼ�����������
 	LCD_ShowString(30,280,200,16,16,dtbuf);
	
	sprintf((char *)dtbuf,"UTC Date:%04d/%02d/%02d   ",gpsx.utc.year,gpsx.utc.month,gpsx.utc.date);	//��ʾUTC����
	LCD_ShowString(30,300,200,16,16,dtbuf);		    
	sprintf((char *)dtbuf,"UTC Time:%02d:%02d:%02d   ",gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec);	//��ʾUTCʱ��
  LCD_ShowString(30,320,200,16,16,dtbuf);		  	  
} 
//16����ת��ΪASCII����ԭ���Ʒ���������ʾ
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
	uart2_Init(115200);	//��ʼ������2
	usart3_init(38400);		//��ʼ������3 
	W25QXX_Init();				//��ʼ��W25Q128
	while(font_init()) 				//����ֿ�
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//�����ʾ	     
	}
	POINT_COLOR=RED;
  LCD_ShowString(30,30,210,24,24,"WarShip STM32 ^_^"); 
	Show_Str(30,60,300,24,"�����ƶ����ϵͳ",24,0);
	Show_Str(30,85,200,24,"GPSλ����Ϣ:",24,0);	
	//������������ʾ
	POINT_COLOR=RED;
	Show_Str(30,360,360,24,"����������:",24,0);
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
	POINT_COLOR=BLUE;//��������Ϊ��ɫ	
	//����-GPS��λ��ʾ��һ������
	if(SkyTra_Cfg_Rate(5)!=0)	//���ö�λ��Ϣ�����ٶ�Ϊ5Hz,˳���ж�GPSģ���Ƿ���λ. 
	{
   	LCD_ShowString(30,120,200,16,16,"SkyTraF8-BD Setting...");
		do
		{
			usart3_init(9600);			//��ʼ������3������Ϊ9600
	  	SkyTra_Cfg_Prt(3);			//��������ģ��Ĳ�����Ϊ38400
			usart3_init(38400);			//��ʼ������3������Ϊ38400
      key=SkyTra_Cfg_Tp(100000);	//������Ϊ100ms
		}while(SkyTra_Cfg_Rate(5)!=0&&key!=0);//����SkyTraF8-BD�ĸ�������Ϊ5Hz
	  LCD_ShowString(30,120,200,16,16,"SkyTraF8-BD Set Done!!");
		delay_ms(500);
		LCD_Fill(30,120,30+200,120+16,WHITE);//�����ʾ 
	}
	//���ݵĴ�������ʾ
	u8 bitstatus;
	while(1) 
	{		
		//����-GPS��λ��ʾ��һ������
		if(USART3_RX_STA&0X8000)		//���յ�һ��������
		{
			rxlen=USART3_RX_STA&0X7FFF;	//�õ����ݳ���
			for(k=0;k<rxlen;k++)USART1_TX_BUF[k]=USART3_RX_BUF[k];	   //�����ݴӴ��ڽ��ջ�������ȡ��
 			USART3_RX_STA=0;		   	//������һ�ν���
			//USART1_TX_BUF[k]=0;			//�Զ���ӽ�����
			GPS_Analysis(&gpsx,(u8*)USART1_TX_BUF);//�����ַ���
			Gps_Msg_Show();				//��ʾ��Ϣ	
 		}
		 
		/*
		���ڵ�״̬����ͨ��״̬�Ĵ��� USART_SR ��ȡ
		�ж�RXNE�������ݼĴ����ǿգ�������λ���� 1 ��ʱ�򣬾�����ʾ�Ѿ������ݱ����յ��ˣ����ҿ��Զ������ˡ�
		��ʱ������Ҫ���ľ��Ǿ���ȥ��ȡ USART_DR��ͨ���� USART_DR ���Խ� ��λ���㣬Ҳ�������λд 0��ֱ�����
		
		*/
		USART_ReceiveData(USART1);
		//������Ҫ�ж϶��Ĵ����Ƿ�ǿ�(RXNE)�������⺯���ķ����ǣ� USART_GetFlagStatus(USART1, USART_FLAG_RXNE); 
		bitstatus=USART_GetFlagStatus(USART1, USART_RX_STA); 
		if(bitstatus)
		{	
			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			USART_RX_STA=0;
			if(len==17)
			{
				for(t=0;t<len;t++)rs485buf[t]=USART_RX_BUF[t];	
			}
			else
			{
				for(t=len-17;t<len;t++)rs485buf[t+17-len]=USART_RX_BUF[t];
			}
			//���������ݡ���λ��Ϣλ�õķ���
			usart2_Send_Data(USART1_TX_BUF,69);
			u8 rs232[100]={};
			HexToAscii(rs485buf,rs232,17);//�����յ�16λ������ת��ΪASCII��
			usart2_Send_Data(rs232,34);
			//LCD�����������ݵ�չʾ
			for(i=0;i<len;i++)
				{
					//rs485buf[i]=cnt+rs485buf[i];//��䷢�ͻ�����
					if(i<2)
					{
						LCD_ShowxNum(60+(i+1)*33,390,rs485buf[i],2,16,0X80);	//��ʾ����
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
				LED0=!LED0;//��ʾϵͳ��������	
				t=0;
				cnt++;
				LCD_ShowxNum(30+48,150,cnt,3,16,0X80);	//��ʾ����
			}		   
		}
	}		
}

