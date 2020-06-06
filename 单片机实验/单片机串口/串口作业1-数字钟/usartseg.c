#include<reg52.h>
#define uchar unsigned char
#define uint  unsigned int
#define ulong unsigned long

extern	int bcd_to_led(uint x);	//汇编子程序，传入DISPBUFF[6]中的数据
//全局变量定义
uchar idata DISPBUFF[6]={2,2,4,8,3,6};
uchar idata CLOCK[4]={0,0,0,0};
sbit udata=P3^6;
sbit uclk=P3^7;
sbit bell=P1^0;
uchar indata,mode=0;
uint number = 0;  // 1s计时要用
uint index_j=0,index_n=0;  //校时和闹钟设置时数组下标

//函数定义
void init();  //初始化函数
void display();	//显示函数
void delayms();
void serial_data(uchar  dd);
void send_char_com(unsigned char ch);
void uart_send_str(unsigned char *s);
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

/********************
初始化、显示、延时函数
********************/
void init()	  //初始化,显示和时钟等
{
  	bell=1;
  //9600bps@11.0592MHz
    SCON = 0x50;	//8位数据,可变波特率
    TMOD = 0x21;	//设定定时器1为8位自动重装方式，定时器0为1方式
	ET0= 1;			//允许T0中断
	TL1 = 0xFD;		
	TH1 = 0xFD;		
	TH0 = 0x3c;	
    TL0 = 0xb0;     //计数初值50ms
	TR1 = 1;		//启动定时器1
	TR0= 1;			//启动定时器0
	ES = 1;			//启动串口中断 
	EA = 1;			//启动总中断
   	uart_send_str("串口作业、顾轩、U201713499\n\n");
	uart_send_str("\r\r输入格式如下：");
    uart_send_str("\r\r 时钟校时：j + xxxxxx + e");
	uart_send_str("\r\r 闹钟设置：n + xxxx + e\r");	
} 

void display()
{
	uchar  i;
	
	for (i=0;i<=2;i++)
	{
     	serial_data(((DISPBUFF[4-2*i]<<4)|DISPBUFF[5-2*i])&0x000000ff);
    }
    delayms();

	if(DISPBUFF[0]==CLOCK[0]&&DISPBUFF[1]==CLOCK[1]&&DISPBUFF[2]==CLOCK[2]&&DISPBUFF[3]==CLOCK[3])	
	{
		bell=0;
	}
	else bell=1;	
}

void delayms()
{
	uint j;
	for(j=0;j<=20000;j++);
}


/**************************
发送字符等串口相关程序
**************************/
void serial_data(uchar  dd)
{   
  uchar i;
  for (i=0;i<=7;i++)
  {
    if(bcd_to_led(dd>>i)) udata=1;
    else udata=0;
    uclk=1;  
    uclk=0;
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


/*******************
定时器中断，1s时间加一
串口中断，处理输入字符
*******************/

timer0() interrupt 1
{
  TH0 = 0X3c;
  TL0 = 0xb0;//设置计数初值，定时50ms

  number++;

  if(number==20)
  {
     number=0;
	 DISPBUFF[5]++;
	 if(DISPBUFF[5]==10)
	 {
	  	 DISPBUFF[5]=0;
	  	 DISPBUFF[4]++;
	  	 if(DISPBUFF[4]==6)
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
					if(DISPBUFF[1]==10&&DISPBUFF[0]<2)
	        		{		 
	          			DISPBUFF[1]=0;
	          			DISPBUFF[0]++;
					}
					else if(DISPBUFF[0]==2&&DISPBUFF[1]==4)
					{
			  			DISPBUFF[0]=0;
			  			DISPBUFF[1]=0;
					}
		  		}
			}
	  	 }
	  }
	}
}    

void usart() interrupt 4 using 1   // 串口中断服务程序 
{      
	if(RI)
  	{ 
		RI=0;
    	indata=SBUF;
		send_char_com(indata);
		if((indata<=57&&indata>=48)|indata=='n'|indata=='j'|indata=='e')
		{
   		if(indata=='j')	   //校时模式
   		{
     		mode=1;
			TR0=0;	 //校时，时钟不动
   		}

   		if(indata=='n')	mode=2;		//闹钟模式

   		if(indata=='e')	  //两种模式输入结束
   		{
     		if(mode == 1)	//校时
			{
				index_j=0;
			 	uart_send_str("\r\r重置时间成功: \r");
				send_char_com(DISPBUFF[0]+48);
				send_char_com(DISPBUFF[1]+48);
				send_char_com('-');
				send_char_com(DISPBUFF[2]+48);
				send_char_com(DISPBUFF[3]+48);
				send_char_com('-');
				send_char_com(DISPBUFF[4]+48);
				send_char_com(DISPBUFF[5]+48);
				send_char_com('\n');
				uart_send_str("\r\r输入格式如下：");
    			uart_send_str("\r\r 时钟校时：j + xxxxxx + e");
				uart_send_str("\r\r 闹钟设置：n + xxxx + e \r");
			}
			if(mode == 2)
			{
				index_n=0;
				uart_send_str("\r\r设置闹钟成功：\r");
				send_char_com(CLOCK[0]+48);
				send_char_com(CLOCK[1]+48);
				send_char_com('-');
				send_char_com(CLOCK[2]+48);
				send_char_com(CLOCK[3]+48);
				send_char_com('\n');
				uart_send_str("\r\r输入格式如下：");
    			uart_send_str("\r\r 时钟校时：j + xxxxxx + e");
				uart_send_str("\r\r 闹钟设置：n + xxxx + e \r");
			}
			mode=0;
			TR0=1;	   //恢复定时器
   		}
   		
		if(indata<=57&&indata>=48&&mode==1&&index_j<6)	//	读取数据.校时
   		{
     		DISPBUFF[index_j]=indata-48;
	 		index_j++;
	 	//	if(index_j==6)	index_j=0;
   		}
   		if(indata<=57&&indata>=48&&mode==2&&index_n < 4)	//闹钟
   		{
     		CLOCK[index_n]=indata-48;
	 		index_n++;
	 		//if(index_n==4) index_n=0;
   		}
		}
		else
		{
			uart_send_str("\r\r输入格式错误\n\n");
			uart_send_str("\r\r输入格式如下：");
    		uart_send_str("\r\r 时钟校时：j + xxxxxx + e");
			uart_send_str("\r\r 闹钟设置：n + xxxx + e \r");
		}
  	}
}

