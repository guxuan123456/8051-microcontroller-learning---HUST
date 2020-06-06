#include<reg52.h>
#define uchar unsigned char
#define uint  unsigned int
#define ulong unsigned long

extern	int bcd_to_led(uint x);	//汇编子程序，传入DISPBUFF[6]中的数据

//函数定义
void init();  //初始化函数
void display();	//显示函数
void delayms();	 //延迟函数
void serial_data(uchar  dd);
void send_char_com(unsigned char ch);
void uart_send_str(unsigned char *s);

//全局变量
uchar idata DISPBUFF[]={5,8,5,8,8,6};	//初始化数据
sbit udata=P3^6;
sbit uclk=P3^7;
uchar  indata;

/********
main()函数
********/
main( )
{

	init();
    while(1)
    {
        display();
    }
}

void delayms()
{
	 uint j;
	 for(j=0;j<=30000;j++) ;
} 

void init()
{
	//9600bps@11.0592MHz
    SCON = 0x50;	//8位数据,可变波特率
    TMOD = 0x21;	//定时器1为方式2,定时器0为方式1
	TL1 = 0xFD;		//定时初值
	TH1 = 0xFD;		//定时初值
	TH0=-10000/256;	//一次计数中断为10ms，
	TL0=-10000%256; //计数初值   
	TR1 = 1;		//启动定时器1
	TR0= 0;			//定时器0不启动，开始停止计时
	ES = 1;			//启动串口中断 
	ET0= 1;			//启动定时器0中断
	EX0=1;			//启动外部中断0，按键1
	EX1=1;			//启动外部中断1,按键2	
	IT0=1;			//按键1下降沿触发
	IT1=1;			//按键2为下降沿触发
	EA = 1;			//启动总中断
	uart_send_str("秒表：按键1开启和暂停，按键2清零\r\n");
}

void display()
{
	 uchar  i;
	 for (i=0;i<=5;i++) 
	 {
	 	serial_data(bcd_to_led(DISPBUFF[5-i])&0xff);  //调用汇编子程序		
	 }		 
	 delayms();
    
}


/**************************
发送字符等串口相关程序
**************************/
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

//向串口发送一个字符 
void send_char_com(unsigned char ch)
{
    SBUF=ch;
    while(TI==0);
    TI=0;
}
// 发送字符串
void uart_send_str(unsigned char *s)
{
	while(*s != '\0')// '\0':字符串结束标志
	{
		send_char_com(*s);// 发送1个字节数据，1个字符占8位，1字节
		s++;// 指向下一个字符
	}
}

/****************************
中断处理程序
定时器0中断，实现秒表基本功能
****************************/
int timer0() interrupt 1 
{
	TH0=-10000/256;	//一次计数中断为10ms，需要中断100次到达1s
	TL0=-10000%256;

	DISPBUFF[5]++;
	if(DISPBUFF[5]==10)
	{
		DISPBUFF[5]=0;
	  	DISPBUFF[4]++;
	  	if(DISPBUFF[4]==10)
	  	{
	    	DISPBUFF[4]=0;
	    	DISPBUFF[3]++;
			if(DISPBUFF[3]==10)
	    	{
	      		DISPBUFF[3]=0;
	      		DISPBUFF[2]++;
		  		if(DISPBUFF[2]==6)
	      		{
	        		DISPBUFF[2]=0;
	        		DISPBUFF[1]++;
					if(DISPBUFF[1]==10)
	        		{		 
	          			DISPBUFF[1]=0;
	          			DISPBUFF[0]++;
			  			if(DISPBUFF[0]==6)	DISPBUFF[0]=0;
					}
		  		}
			}
	  	}
	}
}    

int mode1() interrupt 0	   //按键1中断，改变计时器状态，即关闭或者打开
{
   if(TR0==1)
   {
   		TR0=0;
	 	uart_send_str("STATUS: STOP\r\n");
		uart_send_str("\r\r当前计时: \r");
		send_char_com(DISPBUFF[0]+48);
		send_char_com(DISPBUFF[1]+48);
		send_char_com('-');
		send_char_com(DISPBUFF[2]+48);
		send_char_com(DISPBUFF[3]+48);
		send_char_com('-');
		send_char_com(DISPBUFF[4]+48);
		send_char_com(DISPBUFF[5]+48);
		send_char_com('\n');
   }
   else 
   {
     	TR0=1;
	 	uart_send_str("STATUS: START\r\n");
   }
}

int mode2() interrupt 2		 //按键2中断
{
   uint i;	 
   TR0=0;
   for(i=0;i<=5;i++)	DISPBUFF[i]=0;
   uart_send_str("\r\nSTATUS: CLEAR\r\n");
}

void usart() interrupt 4 using 1   // 串口中断服务程序，这个只在输入时显示，都可以直接删掉 
{     
  if(RI)
  { 
  	RI=0;
    indata=SBUF;
  }
}
