sbit DS1302_CE = P1^7;
sbit DS1302_CK = P3^5;
sbit DS1302_IO = P3^4;

struct Time{
	uint32_t year;	//设置年
	uint8_t mon;	//设置月
	uint8_t date;	//设置日
	uint8_t hour;	//设置小时
	uint8_t min;	//设置分钟
	uint8_t sec;	//设置秒
	uint8_t day;	//设置星期
};

void DS1302_Byte_Write(uint8_t Data){
	uint8_t Mask;
	for(Mask = 0x01;Mask != 0;Mask <<= 1){	//低位在前，逐渐向后
		if((Mask & Data) != 0)	//首先输出该数据
			DS1302_IO = 1;
		else
			DS1302_IO =0;
		DS1302_CK =1;	//拉高时钟
		DS1302_CK =0;	//拉低时钟，完成一位操作
	}
	DS1302_IO = 1;	//最后释放IO口
}

uint8_t DS1302_Byte_Read(){
	uint8_t Mask;
	uint8_t Data = 0;
	for(Mask = 0x01;Mask != 0;Mask <<= 1){	//低位在前，逐渐向后
		if(DS1302_IO != 0)	//首先输出该数据，并设置在data位
			Data |= Mask;
		DS1302_CK =1;	//拉高时钟
		DS1302_CK =0;	//拉低时钟，完成一位操作
	}
	return Data;
}

//向寄存器写一个字节的数据
void DS1302_One_Byte_Write(uint8_t reg,uint8_t Data){
	DS1302_CE = 1;	//选片使能
	DS1302_Byte_Write((reg << 1) | 0x80);	//发送寄存器指令
	DS1302_Byte_Write(Data);	//写入寄存器一个字节的数据
	DS1302_CE = 0;	//选片关闭使能
}

uint8_t DS1302_One_Byte_Read(uint8_t reg){
	uint8_t Data;
	DS1302_CE = 1;	//选片使能
	DS1302_Byte_Write((reg << 1) | 0x81);	//发送寄存器指令
	Data = DS1302_Byte_Read();	//读取寄存器一个字节的数据
	DS1302_CE = 0;	//选片关闭使能
	return Data;
}

//使用突发模式连续写入8个寄存器数据
void DS1302_Burst_Write(uint8_t *Data){
	uint8_t i;
	
	DS1302_CE = 1;
	DS1302_Byte_Write(0xbe);	//发送突发写寄存器指令
	for(i = 0;i < 8;i++){	//连续写入8个字节的数据
		DS1302_Byte_Write(Data[i]);
	}
	DS1302_CE = 0;
}

//使用突发模式连续读取8个寄存器数据
void DS1302_Burst_Read(uint8_t *Data){
	uint8_t i;
	
	DS1302_CE = 1;
	DS1302_Byte_Write(0xbf);	//发送突发读寄存器指令
	for(i = 0;i < 8;i++){	//连续读取8个字节的数据
		Data[i] = DS1302_Byte_Read();
	}
	DS1302_CE = 0;
}

//获取实时时间
void DS1302_Get_Time(struct Time *time){
	uint8_t buf[8];
	
	DS1302_Burst_Read(buf);
	time -> year = buf[6] + 0x2000;
	time -> mon = buf[4];
	time -> date = buf[3];
	time -> hour = buf[2];
	time -> min = buf[1];
	time -> sec = buf[0];
	time -> day = buf[5];
}

//设置实时时间
void DS1302_Set_Time(struct Time *time){
	uint8_t buf[8];
	
	buf[7] = 0;
	buf[6] = time -> year;
	buf[5] = time -> day;
	buf[4] = time -> mon;
	buf[3] = time -> date;
	buf[2] = time -> hour;
	buf[1] = time -> min;
	buf[0] = time -> sec;
	
	DS1302_Burst_Write(buf);
}

//DS1302初始化，如发生掉电则重新设置初始时间
void DS1302_Init(){
	uint8_t Data;
	struct Time code Init_Time[] = {	//2021年5月25日 12：30：00 星期二
		0x2021,0x05,0x25,0x15,0x12,0x00,0x03
	};
	
	DS1302_CE = 0;	//初始化DS1302通信引脚
	DS1302_CK = 0;
	Data = DS1302_One_Byte_Read(0);	//读取秒寄存器
	if((Data & 0x80) != 0){	//由秒寄存器最高位CH的值，判断DS1302是否已经停止
		DS1302_One_Byte_Write(7,0x00);	//撤销写保护允许写操作
		DS1302_Set_Time(&Init_Time);	//设置DS1302为默认时间
	}
}
