 // 该程序实现频率计和测占空比，开始数据可能不准，等待数据刷新稳定即可。
#include "reg51.h"
#include "intrins.h"

// Define P2 pins，定义P2端口的输出
/*
#define DATA_BUS  P0 
sbit RS=P2^0;
sbit RW=P2^1;
sbit E=P2^2; */

extern	int bcd_to_led(uint x);	//汇编子程序，传入DISPBUFF[4]中的数据

// Define new types
typedef unsigned char   uchar;
typedef unsigned int    uint;

// 程序定义
void check_busy(void);
void write_command(uchar com);
void write_data(uchar ddata);
void LCD_init(void);
void string(uchar ad ,uchar *s);
void lcd_test(void);
void Init_zhongduan(void);	//中断初始化
void get_result();	//1s时计算频率，占空比，周期。
void delay(uint);	//延时函数
void display(void);	//显示函数
void buffer_display(uint x);	// 输出缓冲区数据

//全局变量定义
//button1_mode->输出模式选择，外部中断1调节，初始化为0.
//maichong_number-》1s内的脉冲数量，用于计算频率、周期
//timer0_number->定时器0溢出数量
//timer1_number->定时器1中断次数，20次到1s
//timer1_flag->达到1s的标志，到1s才可输出结果，删了此变量
uint button1_mode = 0,maichong_number=0,timer0_number=0,timer1_number=0,timer1_flag = 0;
uint frequent=0;	  //频率，占空比，周期
unsigned long int zhankongbi=0;
unsigned long zhouqi=0;
uint DISPBUFF[] = {0,0,0,0};	//设置6位数进行显示

sbit udata=P3^6;
sbit uclk=P3^7;

void delayms()
{
	 uint j;
	 for(j=0;j<=30000;j++) ;
}
/*****************************************

  主程序、初始化程序、显示程序

*****************************************/
void main(void)
 { 
 	//LCD_init(); //LCD初始化
	//IE = 0x85;
	//TCON = 0x05;
    Init_zhongduan();
    while(1)
    { 		
		display();
    }
 }

void Init_zhongduan(void)	//中断初始化 函数
{
    IE=0x8f;
    TCON=0x55;	//默认把外部中断也打开。
    TMOD=0x19;  //定时器0gate位置1	  
    TH0=0;
    TL0=0; //计时初始为0
    TH1=0x3c;
    TL1=0xb0;  //定时器初始值，定时50ms
}

void display()	//显示函数，不同模式显示不同结果
{
   
	if(button1_mode == 0)  //频率
	{
		 buffer_display(frequent);
	}
	else if(button1_mode == 1)	//周期
	{

	   buffer_display(zhouqi);

	}	
	else if(button1_mode == 2)	//占空比
	{
	   buffer_display(zhankongbi);

	}	
	delay(2500);	//刷新数值	
//  	write_command(0x01); //清屏
  	delay(1); 
}


void buffer_display(uint x)  //输入频率等结果，将结果显示在频幕上
{
   int i;
   for(i=3;i>=0;i--)
   {
        DISPBUFF[i]=x%10;
	    x=x/10;
   }//将数值换成buffer

	 for (i=0;i<=3;i++) 
	 {
	 	serial_data(bcd_to_led(DISPBUFF[3-i])&0xff);  //调用汇编子程序		
	 }		 
	 delayms();
}

void serial_data(uchar  dd)
{   
	uchar i;
	for (i=0;i<=7;i++)
	{
    	if((dd>>i)&0x01)	 udata=1;
	    else udata=0;
   		uclk=1;  uclk=0;
	}
}

void delay(uint j)
{ 
   uchar i = 60;
   for(; j>0; j--)
    { 
	  while(--i);
      i = 59;
      while(--i);
      i = 60;
    }
}

*/
/*************************************************

中断处理函数-----四个中断都用了

*************************************************/
int MODE1() interrupt 2 //按键控制外部1中断,切换显示模式
{
  	write_command(0x01); //显示清屏 
	if(button1_mode < 2) button1_mode++;
	else button1_mode = 0;

}

int mode0() interrupt 0	//外部中断0，输入正电平时产生中断。
{						//中断正电平数目+1

	 if(maichong_number == 0)  //0个周期了。最开始重置定时器
	 {
 	    TH0=0;
	    TL0=0; //计时初始为0
	    TH1=0x3c;
	    TL1=0xb0;  //定时器初始值，定时50ms			  
	 }
	 maichong_number++;	  //脉冲数量加一
}

int timer0() interrupt 1 //计数器0中断，溢出时才能中断，一旦来正脉冲就会计时。
{
   	 TH0=0;
     TL0=0; //计时初始为0。
	 timer0_number++;
}  //可以计算1s钟正脉冲所占时常


int timer1() interrupt 3	//定时器1中断，用来弄1s时间
{
	
   	TH1=0x3c;
  	TL1=0xb0;	//重新设置定时初值，50ms ，20次为1s
	timer1_number++;
    if(timer1_number == 20)	 //到达1s，计算频率占空比。
	{	
		get_result();
		timer1_flag = 1;
		maichong_number = 0; //脉冲数置0
		timer1_number = 0;	 //计数置0
		timer0_number = 0; //溢出次数置零
		Init_zhongduan(); //中断初始化
		TH0=0;
        TL0=0; //计时初始为0。
	}
}

void get_result(void)  //根据中断得出的数据得处结果
{
	//这边数字太大了，不能直接256*256，拆开算，可能单片机自己的问题。
	float t1,t2;
	t1 = (timer0_number*256/100)*256/100;// 
	t2 = (TH0*(256)+TL0)/10000;	  
    zhankongbi =(t1+t2);//结果为百分比			 
	frequent = maichong_number - maichong_number/1000-1; //频率，含有修正
/*	if (frequent < 20)	   //用ms表示周期
	{
	   	zhouqi = 1000/frequent;
	}
    else zhouqi = 1000000/frequent;	   */

}

