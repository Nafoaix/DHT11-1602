#include "DHT11.h"

unsigned char rec_dat[16];
unsigned char RH,TH;
unsigned int timeout;

void DHT11_delay_ms(unsigned int ms)
{
   unsigned int i,j; 
   for(i=ms;i>0;i--)
      for(j=113;j>0;j--);
}

void DHT11_start()
{
	start_led3 = 0;
    DHT11_Data = 1;			// 主机把总线拉高准备发送开始信号
	DHT11_Data = 0;            
	DHT11_delay_ms(20);		// 主机把总线拉低必须大于18ms保证DHT11能检测到起始信号 
	DHT11_Data=1;			//发送开始信号结束后拉高电平延时20-40us 等待DHT的响应信号
	start_led3 = 1;
}

unsigned char ask()
{
    
    timeout=0;
	while(DHT11_Data)
	{
		if(++timeout>300)
			return 0;
	}
	ASK_LED6 = 0;
	if(DHT11_Data == 0)     //DHT11是否低电平响应
	{
		timeout=0;
		while(!DHT11_Data) //低电平结束
		{
			if(++timeout>167)
				return 0;
		}
		timeout=0;
		while(DHT11_Data)    //拉高延时准备输出
		{
			if(++timeout>50)
				return 0;
		}
	}
	else
	{
		return 0;
	}
    ASK_LED6 = 1;
	return 1;
}
    
unsigned char DHT11_rec_byte()      //接收一个字节
{
	unsigned char i,retry,dat=0;
	rec_byte_led7 = 0;
	for(i=0;i<8;i++)    //从高到低依次接收8位数据
	{          
		while(!DHT11_Data);   //等待50us低电平过去
		{retry++;if(retry>100)return -1;}
		retry = 0;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_(); //26个机器周期延时28.21us
		dat <<= 1;           //数据为0时直接移位
		if(DHT11_Data == 1)    //数据为1时，使dat加1来接收数据1
		dat += 1;
		while(DHT11_Data);  //等待数据线拉低  
		{retry++;if(retry>100)return -1;}
		retry = 0;
	} 
	rec_byte_led7 = 1;
	return dat;
}


void  DHT11_rec_40()      //接收40位的数据
{

    unsigned char R_H,R_L,T_H,T_L,RL,TL,revise; //Relative Humidity(相对湿度)在45%～65%之间合适。temperature
	rec_40_led8 = 0;
    DHT11_start();
	
    if( ask() )   //判断是否有DHT11发送低电平响应信号
    {	
		test_led5 = 0;
		NOresponse_led2 = 1;	 //DHT11响应LED2关闭	  
        R_H=DHT11_rec_byte();    //接收湿度高八位
        R_L=DHT11_rec_byte();    //接收湿度低八位  
        T_H=DHT11_rec_byte();    //接收温度高八位  
        T_L=DHT11_rec_byte();    //接收温度低八位
        revise=DHT11_rec_byte(); //接收校正位八位
        
	
        while(!DHT11_Data)    //DATA 引脚输出40位数据后继续输出低电平 50 微秒后转为输入状态，由于上拉电阻随之变为高电平。DHT11 内部重测环境温湿度数据，并记录数据，等待外部信号的到来。

        if((R_H+R_L+T_H+T_L) == revise)      //校正
        {
            RH = R_H;
            RL = R_L;
            TH = T_H;
            TL = T_L;
            ERRORREVISE_LED4 = 1; //校验成功LED4熄灭
        }else{
            ERRORREVISE_LED4 = 0; //校验失败LED4点亮
        } 		
            /*数据处理，方便显示*/
				rec_dat[0] = 'R';
				rec_dat[1] = 'H';
				rec_dat[2] = ':';
				rec_dat[3] = '0'+(RH/10);
				rec_dat[4] = '0'+(RH%10);
				rec_dat[5] = '%';
				rec_dat[6] = ' ';
				rec_dat[7] = ' ';
				rec_dat[8] = ' ';
				rec_dat[9] = ' ';
				rec_dat[10] = 'T';
				rec_dat[11] = ':';
				rec_dat[12] = '0'+(TH/10);
				rec_dat[13] = '0'+(TH%10);
				rec_dat[14] = '0'+175; 
				rec_dat[15] = 'C';
 
				rec_40_led8 = 1;
				test_led5 = 1; 		
    }else{
		NOresponse_led2 = 0; //todo:DHT11未响应
	}	
}