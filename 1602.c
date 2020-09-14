#include "1602.h"

unsigned char Test_Busy(void);  //读写检测，检测液晶是否空闲，空闲才能写命令或者数据。
/**
 * @method Test_Busy
 * @return {unsigned char} 忙为1，空闲为0
 */
unsigned char Test_Busy(void)
{
	unsigned char Busy,times = 0;
	LCD_Bus = 0xff;  //1111 1111
	do{
		busy_led1 = 0;
		RS = 0;
		RW = 1;
		EN = 1;
		_nop_();
		Busy = LCD_Bus&0x80; //按位与1000 0000 状态字SAT7为1禁止 为0允许
		EN = 0;
		times++;
	}while(Busy&&(times <= 10));
	busy_led1 = 1;
	return Busy;     //用完就拉低使能端，释放总线
}
/****************************************/
void Lcd_WriteData(unsigned char Data)
{
	while(Test_Busy());

	RS = 1;
	RW = 0;
	EN = 0;   //拉低使能端准备产生使能高脉冲信号
	_nop_();
	_nop_();
	LCD_Bus = Data;
	EN = 1;   //产生高脉冲使能信号
	_nop_();
	_nop_();
	EN = 0;
	_nop_();

}
/****************************************/
void Lcd_WriteCmd(unsigned char Cmd)
{
	while(Test_Busy());

	RS = 0;
	RW = 0;
	EN = 0;	
	_nop_();
	_nop_();
	LCD_Bus = Cmd;
	EN = 1;   //产生高脉冲使能信号
	_nop_();
	_nop_();
	EN = 0;
	_nop_();

}
/****************************************/
void Lcd_Init(void)
{
	Lcd_WriteCmd(0x38);//初始化
	Lcd_WriteCmd(0x38);//初始化
	Lcd_WriteCmd(0x38);//初始化
	
	_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
	
	Lcd_WriteCmd(0x38);//设置16*2显示，5*7点阵，8位数据接口
  
	Lcd_WriteCmd(0x0c);//1100 开显示 (无光标不闪烁)
	//0x0f 1111 开显示 显示光标闪烁
  
	
	Lcd_WriteCmd(0x06);//0110 设定输入方式 指针光标加一右移 整屏不移动
	//	0x04(0100)指针光标减1左移 整屏不移动
	//	0x07(0111) 整屏左移
	// 	0x05(0101) 整屏右移
	
	Lcd_WriteCmd(0x01);//显示清屏(数据指针清零显示清零)
	//0x02 显示回车 数据指针清零
	

}
/****************************************/
void Lcd_WriteStr(unsigned char *Str)
{	
	while(*Str != '\0')
	{
		Lcd_WriteData(*Str);
		Str++;
	}
}