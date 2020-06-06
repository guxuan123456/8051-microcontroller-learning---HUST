#include<reg51.h>
#include<absacc.h>
#define uchar unsigned char
#define uint unsigned int

//函数定义
extern	int bcd_to_led(uint x);	//汇编子程序，传入DISPBUFF[6]中的数据
void delayms();	//延时函数
void display();	//显示函数
void time_jinwei_chuli();	//时间进位处理函数


// 全局变量定义
uint mode = 0; //1表示调整小时，2表示调整分钟，0表示不调整
uint second = 36,minute = 48,hour = 22,number = 0;   //秒，分，时，时间计数变量
uint DISPBUFF[6]={2,2,4,8,3,6};	  //初始化数码管显示
uint dian_state[6]={0,0,0,0,0,0}; //数码管点的状态,亮或者不亮

main()
{
	// 定时器设置
	IE = 0x87;	    //1000 0111：开总中断，开中断INT1,T0,INT0
	TCON = 0x15;    //0001 0101-->中断INT0和INT1选择下降沿触发，定时器T0启动。
	TMOD = 0x01;    //工作模式0,13位定时计数器
	TH0=-10000/256;	//一次计数中断为10ms，需要中断100次到达1s
	TL0=-10000%256;
	while(1)  //无限循环，等待中断
	{
		display();	
	}			
}



void delayms()
{
   uchar i;
   for(i=200;i>0;i--){;}
}


void display()
{
	uchar i;
	P1=0xFE;//第一个灯
	for(i=0;i<6;i++)
	{
		P2=bcd_to_led(DISPBUFF[i])|dian_state[i];//调用汇编子程序变成数码管状态
		delayms();								 //和点状态想与得到最终显示状态
		P1=(P1<<1)+1;	
	}
}

// 中断函数设置

int timer0() interrupt 1	  //定时器0中断
{	
	TH0=-10000/256;	//一次计数中断为10ms，需要中断100次到达1s
	TL0=-10000%256;	//中断时设置初始值
	number++;	//中断次数加1

	if(number == 100)	//1s
	{		
	   number = 0;	//计数值设为0
	   second++;	//秒数加一
	   time_jinwei_chuli();	//时间进位处理函数
	}
		
}

int MODE1() interrupt 0	//外部mode中断
{
	delayms();
	switch(mode) //最开始不调整
	{
		//0表示不调整，1表示调小时，2表示调分钟
		case 0:	   //不调整
			{
				dian_state[0]=0x80;
				dian_state[1]=0x80;
				mode=1;	//按键，跳转下一状态，调整小时				
			};
			break;
		case 1:
			{
				dian_state[0]=0x00;
				dian_state[1]=0x00;
				dian_state[2]=0x80;
				dian_state[3]=0x80;
				mode=2;	//跳转下一状态，调整分钟
			};
			break;
		case 2:
			{
				dian_state[0]=0x00;
				dian_state[1]=0x00;
				dian_state[2]=0x00;
				dian_state[3]=0x00;
				mode=0; //跳转下一状态，不调整								
			};
			break;			
	}
}


int ADD() interrupt 2  //外部中断，调整时间
{	
	delayms();

	if(mode==1)	  //调小时
	{
		if(hour==23)hour=0;
		else hour++;
	}
	else if(mode==2)  //调分钟
	{
		if(minute==59)minute=0;
		else minute++;
	}
	time_jinwei_chuli();
}


void time_jinwei_chuli()  //时间进位处理函数
{
  	if(second==60)	 //满60s
	{
	 	second=0;
	  	minute++;
	  	
		if(minute==60)
	  	{
	    	minute=0;
	    	hour++;
			if(hour==24)
			{
		  		hour=0;
			}
	  	}
	}

	DISPBUFF[0]=hour/10;
	DISPBUFF[1]=hour%10;
	DISPBUFF[2]=minute/10;
	DISPBUFF[3]=minute%10;
	DISPBUFF[4]=second/10;
	DISPBUFF[5]=second%10;	
}









