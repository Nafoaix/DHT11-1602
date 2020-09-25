#ifndef _DHT11_H_
#define _DHT11_H_

#include <reg51.h>
#include <intrins.h>

sbit DHT11_Data = P0^4;   //dht11数据端口

sbit NOresponse_led2 = P1^1;
sbit start_led3 = P1^2;
sbit ERRORREVISE_LED4 = P1^3;
sbit test_led5 = P1^4;
sbit ASK_LED6 = P1^5;
sbit rec_byte_led7 = P1^6;
sbit rec_40_led8 = P1^7;



/* DHT11 测量范围 20~90%RH(5%RH) 0~50C(2C) 工作电压3~ 5.5V*/
extern unsigned char rec_dat[16];	//全局变量 用于显示的接收数据数组             
extern unsigned char RH,TH;
//extern void DHT11_delay_us(unsigned char us); //11.0592大概每次循环延时4.3us
extern void DHT11_delay_ms(unsigned int ms);  //ms延时
extern void DHT11_rec_40(void); 			  //接收40位全部数据
#endif