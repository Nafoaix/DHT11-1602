#include <reg51.h>
#include <intrins.h>
#include "1602.h"
#include "DHT11.h"
#define uchar unsigned char
#define uint unsigned int
/*------------按键引脚---------------*/
sbit ok_SW1 = P3^5; //确定-SW1
sbit add_SW2 = P3^4;//增加-SW2
sbit sub_SW3 = P3^3;//减少-SW3
sbit sw_SW4 = P3^2; //选择-SW4
/*------------蜂鸣器引脚---------------*/
sbit bell = P3^6;
/*------------变量声明---------------*/
uchar HTemp,LTemp,HRH,LRH,set;
/*------------函数声明---------------*/
unsigned char Key_Scan(); 
void Init_Alarm_Timer0();

int main ()
{	
	uchar i;   				//rec_dat[]数组显示用
	DHT11_delay_ms(1500);   //DHT11上电后要等待1S以越过不稳定状态在此期间不能发送任何指令
	DHT11_rec_40();
	Lcd_Init();				//Lcd初始化
	HTemp=30,LTemp=20;HRH=75,LRH=35;set=2;
	Init_Alarm_Timer0();
	while(1)
	{	
		switch(set)
		{
			case 1:
				DHT11_delay_ms(2000);
				DHT11_rec_40();
				Lcd_WriteCmd(0x80);
				Lcd_WriteStr("Press S1 to set ");
				Lcd_WriteCmd(0x80 + 0x40);//设置数据指针到第二行首
				for(i=0;i<16;i++){Lcd_WriteData(rec_dat[i]);}   //显示数据 
				if(Key_Scan() == 1) {set = 2;}				
			break;
			case 2:
				Lcd_WriteCmd(0x80);
				Lcd_WriteStr("Set the max RH  ");
				Lcd_WriteCmd(0x80 + 0x40);
				Lcd_WriteStr("Maximum RH: ");
				Lcd_WriteData('0'+(HRH/10));
				Lcd_WriteData('0'+(HRH%10));
				Lcd_WriteStr("%  ");
				switch(Key_Scan())
				{
					case 1:
						set = 3;
					break;
					case 2:
						HRH+=1;
					break;
					case 3:
						HRH-=1;
					break;
				}
			break;
			case 3:
				Lcd_WriteCmd(0x80);
				Lcd_WriteStr("Set the min RH  ");
				Lcd_WriteCmd(0x80 + 0x40);			
				Lcd_WriteStr("Minimum RH: ");
				Lcd_WriteData('0'+(LRH/10));
				Lcd_WriteData('0'+(LRH%10));
				Lcd_WriteStr("%  ");
				switch(Key_Scan())
					{
					case 1:
						set = 4;
					break;
					case 2:
						LRH+=1;
					break;
					case 3:
						LRH-=1;
					break;
				}			
			break;	
			case 4:
				Lcd_WriteCmd(0x80);
				Lcd_WriteStr("Set the max Temp ");
				Lcd_WriteCmd(0x80 + 0x40);
				Lcd_WriteStr("Max Temp:  ");
				Lcd_WriteData('0'+(HTemp/10));
				Lcd_WriteData('0'+(HTemp%10));
				Lcd_WriteData('0'+175); 
				Lcd_WriteStr("C   ");
				switch(Key_Scan())
					{
					case 1:
						set = 5;
					break;
					case 2:
						HTemp+=1;
					break;
					case 3:
						HTemp-=1;
					break;
				}			
			break;	
			case 5:
				Lcd_WriteCmd(0x80);
				Lcd_WriteStr("Set the min Temp ");
				Lcd_WriteCmd(0x80 + 0x40);			
				Lcd_WriteStr("Min Temp:  ");
				Lcd_WriteData('0'+(LTemp/10));
				Lcd_WriteData('0'+(LTemp%10));
				Lcd_WriteData('0'+175);
				Lcd_WriteStr("C   ");
				switch(Key_Scan())
					{
						case 1:
							set = 1;
						break;
						case 2:
							LTemp+=1;
						break;
						case 3:
							LTemp-=1;
						break;
					}			
			break;
			case 6: 
				Lcd_WriteCmd(0x80);
				Lcd_WriteStr("Over Temp alarm ");
				//bell = ~bell;
				if(Key_Scan()) { set = 4; }			
			break;
			case 7: 			
				Lcd_WriteCmd(0x80);
				Lcd_WriteStr("Over RH alarm   ");
				//bell = ~bell;
				if(Key_Scan()) { set = 2; }			
			break;				
			default :
			Lcd_WriteCmd(0x80);
			Lcd_WriteStr("ERROR 101       ");	
			Lcd_WriteCmd(0x80 + 0x40);
			Lcd_WriteStr("UNKNOW SET VALUE");
			if(Key_Scan()) { set = 1; }
			break;
		}	      
	}
	
	return 0;
}

void Init_Alarm_Timer0(void)
{
	TMOD |= 0x01;	  //使用工作模式1，16位定时器，使用"|"或等符号可以在使用多个定时器时不受影响		     
	TH0 = (65535-46082) / 256;	//给定初值50ms
	TL0 = (65535-46082) % 256;
	EA=1;             //打开总中断
	ET0=1;            //定时器中断打开 T0的溢出中断允许位
	TR0 = 1;		  //打开定时器开关
}

void Timer0_Alarm(void) interrupt 1
{	
	TH0 = (65535-46082) / 256;	//重设初值
	TL0 = (65535-46082) % 256;
		if(set == 1)
		{
			if(TH<LTemp||TH>HTemp)
			{
				set = 6;
			}else if(RH<LRH||RH>HRH)
			{
				set = 7;					
			}
		}
}

unsigned char Key_Scan()
{
    unsigned char keyValue = 0 , i; //保存键值
    //--检测按键SW1--//
    if (ok_SW1 != 1)//检测按键'确定-SW1'是否按下 按键被按下VCC上拉电阻接地，输入线线路电压0V
    {
	    DHT11_delay_ms(10);//消除抖动
        if (ok_SW1 != 1)//再次检测按键是否按下
        {
            keyValue = 1;
            i = 0;
            while ((i<50) && (ok_SW1 != 1)) //检测按键是否松开
            {
                DHT11_delay_ms(10);
                i++;
            }
        }
    }
    //--检测按键SW2--//
    if (add_SW2 != 1)//检测'增加-SW2'是否按下
    {
        DHT11_delay_ms(10);//消除抖动
        if (add_SW2!= 1)//再次检测按键是否按下
        {
            keyValue = 2;
            i = 0;
            while ((i<50) && (add_SW2 != 1)) //检测按键是否松开
            {
                DHT11_delay_ms(10);
                i++;
            }
        }
    }
    //--检测按键SW3--//
    if (sub_SW3 != 1)//检测'减少-SW3'是否按下
    {
        DHT11_delay_ms(10);//消除抖动
        if (sub_SW3 != 1)//再次检测按键是否按下
        {
            keyValue = 3;
            i = 0;
            while ((i<50) && (sub_SW3 != 1)) //检测按键是否松开
            {
                DHT11_delay_ms(10);
                i++;
            }
        }
    }
	 //--检测按键SW4--//
    if (sw_SW4 != 1)//检测'选择-SW4'是否按下
    {
        DHT11_delay_ms(10);//消除抖动
        if (sw_SW4!= 1)//再次检测按键是否按下
        {
            keyValue = 4;
            i = 0;
            while ((i<50) && (sw_SW4 != 1)) //检测按键是否松开
            {
                DHT11_delay_ms(10);
                i++;
            }
        }
    }
    return keyValue;   //将读取到键值的值返回
}