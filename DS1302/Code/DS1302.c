#include <units.h>
#include <delay.h>
#include <DS1302.h>
#include <LCD.h>

void Config_Timer0(uint32_t ms);
void Refresh_Time();
void Show_BCD_One_Byte(uint8_t bcd);

struct Time code Set_Time[] = {
		0x2021,0x05,0x26,0x15,0x18,0x00,0x03
	};
struct Time Buf_Time;		//日期时间缓冲区
uint8_t T0RH = 0;	//T0重载值高字节
uint8_t T0RL = 0;	//T0重载值低字节

void main(){
	uint8_t Preserve_Sec =0xaa;
	
	Config_Timer0(20);	//时钟频率设置为20ms
	DS1302_Init();	//初始化DS1302，当系统掉电后会进行检查，如果掉电则设置初始时间
	DS1302_Set_Time(&Set_Time);	//设置时间
	LCD_Init();
	
	LCD_Set_Cursor(0,3);
	LCD_Print_Str("20  -  -");
	LCD_Set_Cursor(1,6);
	LCD_Print_Str(":  :");
	DS1302_Get_Time(&Buf_Time);
	Refresh_Time();
	while(1);
}

void Config_Timer0(uint32_t ms){
	uint64_t temp;
	EA = 1;
	
	temp = 11059200/12;
	temp = (temp * ms) / 1000;
	temp = 65536 - temp;
	temp = temp + 28;
	T0RH = (uint8_t)(temp >> 8);
	T0RL = (uint8_t)temp;
	TMOD &= 0xf0;	//清零T0控制位
	TMOD |=  0x01;	//配置T0为模式1
	TH0 = T0RH;	//加载T0重载值
	TL0 = T0RL;	
	ET0 = 1;	//使能T0中断
	TR0 = 1;	//启动T0
}

void Show_BCD_One_Byte(uint8_t bcd){
	uint8_t str[3];
	
	str[0] = (bcd >> 4) + '0';
	str[1] = (bcd & 0x0f) + '0';
	str[2] = '\0';
	LCD_Print_Str(str);
}

void Refresh_Time(){
	LCD_Set_Cursor(0,5);
	Show_BCD_One_Byte(Buf_Time.year);
	LCD_Set_Cursor(0,8);
	Show_BCD_One_Byte(Buf_Time.mon);
	LCD_Set_Cursor(0,11);
	Show_BCD_One_Byte(Buf_Time.date);
	
	LCD_Set_Cursor(1,4);
	Show_BCD_One_Byte(Buf_Time.hour);
	LCD_Set_Cursor(1,7);
	Show_BCD_One_Byte(Buf_Time.min);
	LCD_Set_Cursor(1,10);
	Show_BCD_One_Byte(Buf_Time.sec);
}

void Interrupt0() interrupt 1{
	static uint8_t Preserved_Sec = 0;
	static uint8_t count = 0;
	
	TH0 = T0RH;
	TL0 = T0RL;
	count++;
	if(count>=10){	//20*10=200，每秒5次对秒数进行检查
		count = 0;
		DS1302_Get_Time(&Buf_Time);
		if(Preserved_Sec != Buf_Time.sec){
			Refresh_Time();
			Preserved_Sec = Buf_Time.sec;
		}
	}
}
