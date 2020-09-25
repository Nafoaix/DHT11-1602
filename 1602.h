#ifndef _1602_H_
#define _1602_H_

#include <reg51.h>
#include <intrins.h>
#define LCD_Bus P2 //LCD1602数据总线
sbit RS = P0^7;	// LCD数据/命令选择端
sbit RW = P0^6;	// LCD读写选择端
sbit EN = P0^5;	// LCD使能端，高脉冲有效

sbit busy_led1 = P1^0; //LCD忙指示灯

extern void Lcd_Init(void);                    //1602初始化函数
extern void Lcd_WriteData(unsigned char Data); //写数据
extern void Lcd_WriteCmd(unsigned char Cmd);	 //写指令
extern void Lcd_WriteStr(unsigned char *Str); //写一串字符串

#endif