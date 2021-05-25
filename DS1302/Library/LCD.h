#define LCD_Port P0

uint8_t DISPLAYTYPE = 0x38;
uint8_t DISPLAYCOMMAND = 0x0c;
uint8_t DATAADDRESS = 0x06;
uint8_t DISPLAYCLEAR = 0x01;

sbit  LCD_E = P1^5;
sbit  LCD_RW = P1^1;
sbit  LCD_RS = P1^0;

void LCD_Ready(){
	uint8_t status;
	//读命令
	LCD_Port = 0xff;
	LCD_RS = 0;
	LCD_RW = 1;
	do{
		LCD_E = 1;
		status = LCD_Port;
		LCD_E = 0;
	}while(status & 0x80);	//当最高位为1时表示正在忙
}

void LCD_Write_CMD(uint8_t cmd){
	LCD_Ready();
	//进入写命令操作
	LCD_RS=0;
	LCD_RW=0;
	LCD_Port=cmd;	//指令
	LCD_E=1;	//允许命令写操作
	LCD_E=0;	//关闭命令写操作
}

void LCD_Write_Data(uint8_t Data){
	//进入写数据操作
	LCD_Ready();
	LCD_RS=1;
	LCD_RW=0;
	LCD_Port=Data;	//数据
	LCD_E=1;	//允许数据写操作
	LCD_E=0;	//关闭数据写操作
}

void LCD_Init(){
	LCD_Write_CMD(DISPLAYTYPE);	//16*2显示，5*7点阵，8位数据接口
	LCD_Write_CMD(DISPLAYCOMMAND);	//显示器开，光标关闭
	LCD_Write_CMD(DATAADDRESS);	//文字不动，地址加自动+1
	LCD_Write_CMD(DISPLAYCLEAR);	//清屏
}

void LCD_Clear(){
	LCD_Write_CMD(DISPLAYCLEAR);	//清屏
}

void LCD_Cursor(){
	DISPLAYCOMMAND = DISPLAYCOMMAND|0x02;	//按位或运算进行取真
	LCD_Write_CMD(DISPLAYCOMMAND);	//打开光标
}

void LCD_No_Cursor(){
	DISPLAYCOMMAND = DISPLAYCOMMAND & 0x02;	//按位且运算进行取假
	LCD_Write_CMD(DISPLAYCOMMAND);	//关闭光标
}
void LCD_Blink(){
	DISPLAYCOMMAND = DISPLAYCOMMAND|0x01;	//按位或运算进行取真
	LCD_Write_CMD(DISPLAYCOMMAND);	//光标闪烁
}
void LCD_No_Blink(){
	DISPLAYCOMMAND = DISPLAYCOMMAND & 0x01;	//按位且运算进行取假
	LCD_Write_CMD(DISPLAYCOMMAND);	//光不闪烁
}
	
void LCD_Set_Cursor(uint8_t y,uint8_t x){
	uint8_t address;
	if(y==0)address = 0x00 + x;	//第一行字符地址从0x00开始
	else if(y == 1)address = 0x40 + x;	//第二行字符地址从0x40开始
	LCD_Write_CMD(address | 0x80);	//设置RAM地址
}

void LCD_Print_Char(uint8_t Data){
  LCD_Write_Data(Data);
}

void LCD_Print_Str(uint8_t *str){
	while(*str != '\0'){
		LCD_Write_Data(*str++);
	}
}

void LCD_Custom_Char(uint8_t Num,uint8_t Data[]){
	uint8_t i;
  LCD_Write_CMD(0x40+8*(Num-1));   // CGRAM地址设为起始位置处
  for(i=0;i<8;i++) {
     LCD_Write_Data(Data[i]);	// 按数组写入数据
  }
}

void LCD_Print_Custom_Char(uint8_t y,uint8_t x,uint8_t Num){
	LCD_Set_Cursor(y,x);
  LCD_Write_Data(0x00+Num-1);
}