#include<reg51.h>
#include"hanzi.h"
#include"char.h"

//函数声明
void delayms (void);	  /* 函数说明 */
void delay_m(unsigned int n);	//延时ns
void display(void);
void hanzi_leddisp(char *ascii,char length); 
uchar DISPBUFF[10]={0,1,2,3,4,5,6,7,8,9};           
void zifu_leddisp(char *ascii,char length);

//全局变量定义
uint mode=0,add=0,number=0;//mode是否能进行切换，add按钮进行切换，number计数器计数

void main (void)
{
	//uint h;
	// 定时器设置
	IE = 0x87;	    //1000 0111：开总中断，开中断INT1,T0,INT0
	TCON = 0x15;    //0001 0101-->中断INT0和INT1选择下降沿触发，定时器T0启动。
	TMOD = 0x01;    //工作模式1,16位定时计数器
	TH0=-50000/256;	//一次计数中断为50ms，需要中断800次到达40s
	TL0=-50000%256;
	while(1)  //无限循环，等待中断
	{
		display();
		delay_m(5);	//每次展示完的停顿
	}			
}

void display(void)
{

   if(add == 0)//第一个字符串
   {
   	  hanzi_leddisp(DISPBUFF,7);
	  delayms();
   }
   else if(add == 1)//第二个字符串
   {
   	  zifu_leddisp("I-LOVE-YOU!",11);
	  delayms();
   }   
   else if(add == 2)//第三个字符串
   {
   	  zifu_leddisp("HUST-NIUBI",10);
	  delayms();
   }   
   else if(add == 3)//第四个字符串
   {
   	  zifu_leddisp("WUHAN-JIAYOU",12);
	  delayms();
   }   
   else if(add == 4)//第五个字符串
   {
   	  zifu_leddisp("DANPIANJI",9);
	  delayms();
   }   
   else if(add == 5)//第六个字符串
   {
   	  zifu_leddisp("ZHONGGUO-NIUBI",14);
	  delayms();
   }
  
}

//延时n秒,具体精确也不确定
void delay_m(unsigned int n)
{
    unsigned int i=0,j=0;
    for(i=0;i<n;i++)
        for(j=0;j<21738;j++);
}


//中断处理程序
int timer0() interrupt 1	  //定时器0中断
{	
	TH0=-50000/256;	//一次计数中断为50ms，需要中断800次到达40s
	TL0=-50000%256;	//中断时设置初始值
	number++;	//中断次数加1

	if(number == 800)	//40s，切换下一个字符
	{		
	   number = 0;	//计数值设为0
	   if(mode == 0) //不处于按钮调整
	   {
	   		hanzi_leddisp(DISPBUFF,0);
	   		zifu_leddisp("--",2);  //显示字符清零，准备进入下一个字符串
	   		if(add ==5 ) add=0;
			else add++;
	   }
	}		
}

int MODE1() interrupt 0	//外部mode中断
{
  if(mode == 1)//中断时要变为0，此时按钮add不可用
  {
  	 mode = 0;//切换为定时器40s改变一次字符串

	 //定时器初始化
	 TH0=-50000/256;	//一次计数中断为50ms，需要中断800次到达40s
	 TL0=-50000%256;	//中断时设置初始值
	 number = 0;	//中断次数为0
  }
  else mode = 1; //切换为按钮改变字符串

}

//没有将显示字符的函数放在中断处理函数中，考虑到显示时间较长，这样的话下一次add可能无法触发中断
int ADD1() interrupt 2  //外部中断,ADD按钮改变ADD的值
{
   if(mode == 1) //允许按钮改变字符
   {
   	   if(add <5) add++;
	   else add = 0;

	   //add按钮改变字符串，先清空显示字符。
	   zifu_leddisp("--",2);  //显示字符清零，准备进入下一个字符串 		
   }
}


 // 延时函数
void delayms (void)  /* 延时 */
{
	uchar i;
	for (i=200;i>0;i--){;}
} 

 // 输入汉字字符串和长度，显示汉字。
void hanzi_leddisp(char *ascii,char length)                   
{
	uchar i,j,k,offset;
	uint  sel;
	 for(k=0;k<length-1;k++)
 	{
 		 for(j=0;j<160;j++)
   		{  
			offset=j/10;
     		sel=0xfffe;
   			for(i=0;i<16;i++)
      		{ 
        		if((i+offset)<16 )
         		{	 
					P1=0x01;  P2=0;
         			P2=hanzi[ascii[k]][2*(i+offset)];
         			P1=0x02;   P2=0;
         			P2=hanzi[ascii[k]][2*(i+offset)+1];
          		}
       			else
        		{ 
					P1=0x01;  P2=0;
         			P2=hanzi[ascii[k+1]][2*(i+offset-16)];
          			P1=0x02;   P2=0;
         			P2=hanzi[ascii[k+1]][2*(i+offset-16)+1];
        		}
      			P1=0x03;     P2=sel;
      			P1=0x04;    P2=sel>>8;
      			P1=0x00;
      			sel=sel<<1|0x01;
      			delayms( );
     		}
    	}
   	}
}

//输入字母字符串和长度，显示字符串
void zifu_leddisp(char *ascii,char length)                   
{
	uchar i,j,k,offset;
	uint  sel;
	for(k=0;k<length/2;k++)
	{
		for(j=0;j<160;j++)
		{  
			offset=j/20;
			sel=0xfffe;
			for(i=0;i<16;i++)
			{ 
				if(i<8)
				{
					if((i+offset)<8 )
					{	 
						P1=0x01;  P2=0;
						P2=zifu[ascii[2*k]-32][2*(i+offset)];
						P1=0x02;   P2=0;
						P2=zifu[ascii[2*k]-32][2*(i+offset)+1];
					}
					else
					{	 
						P1=0x01;  P2=0;
						P2=zifu[ascii[2*k+1]-32][2*(i+offset-8)];
						P1=0x02;   P2=0;
						P2=zifu[ascii[2*k+1]-32][2*(i+offset-8)+1];
					}
				}
				else
				{
					if((i+offset-8)<8 )
					{	 
						P1=0x01;  P2=0;
						P2=zifu[ascii[2*k+1]-32][2*(i+offset-8)];
						P1=0x02;   P2=0;
						P2=zifu[ascii[2*k+1]-32][2*(i+offset-8)+1];
					}
					else
					{	 
						P1=0x01;  P2=0;
						P2=zifu[ascii[2*k+2]-32][2*(i+offset-8-8)];
						P1=0x02;   P2=0;
						P2=zifu[ascii[2*k+2]-32][2*(i+offset-8-8)+1];
					}
				}
				P1=0x03;     P2=sel;
				P1=0x04;    P2=sel>>8;
				P1=0x00;
				sel=sel<<1|0x01;
				delayms( );
			}
		}
	}
}