sbit DS1302_CE = P1^7;
sbit DS1302_CK = P3^5;
sbit DS1302_IO = P3^4;

struct Time{
	uint32_t year;	//������
	uint8_t mon;	//������
	uint8_t date;	//������
	uint8_t hour;	//����Сʱ
	uint8_t min;	//���÷���
	uint8_t sec;	//������
	uint8_t day;	//��������
};

void DS1302_Byte_Write(uint8_t Data){
	uint8_t Mask;
	for(Mask = 0x01;Mask != 0;Mask << 1){	//��λ��ǰ�������
		if((Mask & Data) != 0)	//�������������
			DS1302_IO = 1;
		else
			DS1302_IO =0;
		DS1302_CK =1;	//����ʱ��
		DS1302_CK =0;	//����ʱ�ӣ����һλ����
	}
	DS1302_IO = 1;	//����ͷ�IO��
}

uint8_t DS1302_Byte_Read(){
	uint8_t Mask;
	uint8_t Data = 0;
	for(Mask = 0x01;Mask != 0;Mask << 1){	//��λ��ǰ�������
		if(DS1302_IO != 0)	//������������ݣ���������dataλ
			Data |= Mask;
		DS1302_CK =1;	//����ʱ��
		DS1302_CK =0;	//����ʱ�ӣ����һλ����
	}
	return Data;
}

//��Ĵ���дһ���ֽڵ�����
void DS1302_One_Byte_Write(uint8_t reg,uint8_t Data){
	DS1302_CE = 1;	//ѡƬʹ��
	DS1302_Byte_Write((reg << 1) | 0x80);	//���ͼĴ���ָ��
	DS1302_Byte_Write(Data);	//д��Ĵ���һ���ֽڵ�����
	DS1302_CE = 0;	//ѡƬ�ر�ʹ��
}

uint8_t DS1302_One_Byte_Read(uint8_t reg){
	uint8_t Data;
	DS1302_CE = 1;	//ѡƬʹ��
	DS1302_Byte_Write((reg << 1) | 0x81);	//���ͼĴ���ָ��
	Data = DS1302_Byte_Read();	//��ȡ�Ĵ���һ���ֽڵ�����
	DS1302_CE = 0;	//ѡƬ�ر�ʹ��
	return Data;
}

//ʹ��ͻ��ģʽ����д��8���Ĵ�������
void DS1302_Burst_Write(uint8_t *Data){
	uint8_t i;
	
	DS1302_CE = 1;
	DS1302_Byte_Write(0xbe);	//����ͻ��д�Ĵ���ָ��
	for(i = 0;i < 8;i++){	//����д��8���ֽڵ�����
		DS1302_Byte_Write(Data[i]);
	}
	DS1302_CE = 0;
}

//ʹ��ͻ��ģʽ������ȡ8���Ĵ�������
void DS1302_Burst_Read(uint8_t *Data){
	uint8_t i;
	
	DS1302_CE = 1;
	DS1302_Byte_Write(0xbe);	//����ͻ�����Ĵ���ָ��
	for(i = 0;i < 8;i++){	//������ȡ8���ֽڵ�����
		Data[i] = DS1302_Byte_Read();
	}
	DS1302_CE = 0;
}

//��ȡʵʱʱ��
void DS1302_Get_Time(struct Time *time){
	uint8_t buf[8];
	
	time -> year = buf[6] + 0x2000;
	time -> mon = buf[4];
	time -> date = buf[3];
	time -> hour = buf[2];
	time -> min = buf[1];
	time -> sec = buf[0];
	time -> day = buf[5];
}

//����ʵʱʱ��
void DS1302_Set_Real_Time(struct Time *time){
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

//DS1302��ʼ�����緢���������������ó�ʼʱ��
void DS1302_Init(){
	uint8_t Data;
	struct Time code Init_Time[] = {	//2021��5��25�� 12��30��00 ���ڶ�
		0x2021,0x05,0x25,0x12,0x30,0x00,0x02
	};
	
	DS1302_CE = 0;	//��ʼ��DS1302ͨ������
	DS1302_CK = 0;
	Data = DS1302_One_Byte_Read(0);	//��ȡ��Ĵ���
	if((Data & 0x80) != 0){	//����Ĵ������λCH��ֵ���ж�DS1302�Ƿ��Ѿ�ֹͣ
		DS1302_One_Byte_Write(7,0x00);	//����д��������д����
		DS1302_Set_Real_Time(&Init_Time);	//����DS1302ΪĬ��ʱ��
	}
}