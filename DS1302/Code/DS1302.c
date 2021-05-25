#include <units.h>
#include <delay.h>
#include <DS1302.h>
#include <LCD.h>

void Config_Timer0(uint32_t ms);
void Refresh_Time();
void Show_BCD_One_Byte(uint8_t bcd);

bit flag200ms = 1;	//200ms��ʱ��־
struct Time Buf_Time;		//����ʱ�仺����
uint8_t Set_Index = 0;	//ʱ����������
uint8_t T0RH = 0;	//T0����ֵ���ֽ�
uint8_t T0RL = 0;	//T0����ֵ���ֽ�

void main(){
	uint8_t Preserve_Sec =0xaa;
	
	EA = 1;
	Config_Timer0(1);
	DS1302_Init();
	LCD_Init();
	
	LCD_Set_Cursor(0,3);
	LCD_Print_Str("20 - - ");
	LCD_Set_Cursor(1,4);
	LCD_Print_Str(" : : ");
	
	while(1){
		if(flag200ms && (Set_Index == 0)){
			flag200ms = 0;
			DS1302_Get_Time(&Buf_Time);
			if(Preserve_Sec != Buf_Time.sec){
				Refresh_Time();
				Preserve_Sec = Buf_Time.sec;
			}
		}
	}
}

void Config_Timer0(uint32_t ms){
	uint64_t temp;
	
	temp = 11059200/12;
	temp = (temp * ms) / 1000;
	temp = 65536 - temp;
	temp = temp + 28;
	T0RH = (uint8_t)(temp >> 8);
	T0RL = (uint8_t)temp;
	TMOD &= 0xf0;	//����T0����λ
	TMOD |=  0x01;	//����T0Ϊģʽ1
	TH0 = T0RH;	//����T0����ֵ
	TL0 = T0RL;	
	ET0 = 1;	//ʹ��T0�ж�
	TR0 = 1;	//����T0
}

void Show_BCD_One_Byte(uint8_t bcd){
	uint8_t str[4];
	
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