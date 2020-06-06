#include <reg51.h>
#include <math.h>
#define uchar unsigned char
#define uint unsigned int
#define KeyPort  P1

uchar code duanma[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e};// 显示段码值0~F和+-*/ 号
uchar code weima[]={0x3e,0x3d,0x3b,0x37,0x2f,0x1f};//位码,决定显示哪个LED
uchar tempdata[6]={0x3f,0x00,0x00,0x00,0x00,0x00}; //存储相应LED应该显示的值，初始第一位设置为0

void delayms(void);	//延时
uchar kbscan(void);	 //键盘扫描
uchar keypro(void);	 //键值计算				
void display(void);  //数码管显示



//由于要循环输入计算，所以要在一个while循环中
void main (void)	//逻辑部分
{
   uchar temp,tempbak;
   uint num1,num2,sym,value;
   int i,count,num,biaozhi;
   P1=0xff;
   count=0;
   num =0;
   num1=0;
   num2=0;
   temp=0;
   sym=0;
   biaozhi=0;
   tempbak=0;

  while(1)			//无限计算
  {
	temp=keypro();  //读取键盘值，判断是数字还是运算符
	if(temp!=tempbak)
	{
	    if(temp>=0&&temp<=9&&biaozhi==0)   //输入第一个操作数
		{
		  num1=num1*10+temp;
		  for(i=count;i>0;i--)
		  {
		  	tempdata[i] = tempdata[i-1];
		  }
		  tempdata[0]=duanma[temp];
		  count++;
	
		}
	
	
		else if(temp>=10&&temp<=13&&biaozhi==0)	 //输入了操作符，需保存第一个操作数num1和操作符
		{
		  sym=temp;	//保留操作符
	
		  for(i=count;i>0;i--)
		  {
		  	tempdata[i] = tempdata[i-1];
		  }
		  tempdata[0]=duanma[temp];
		  count++;
	
		  num=count;	//记录第一个数加上操作符的位数
		  biaozhi++;
		}
	
	
		else if(temp>=0&&temp<=9&&biaozhi==1)	//输入第二个操作数
		{
		  num2=num2*10+temp;	
		  	 
		  for(i=count;i>0;i--)
		  {
		  	tempdata[i] = tempdata[i-1];
		  }
		  tempdata[0]=duanma[temp];
		  count++;
	
		}	
				   
		else if(temp==14&&biaozhi==1)		//等号，需要输出结果
		{
	
		  //加减乘除
		  if(sym==10)value=num1+num2;
		  else if(sym==11)value=num1-num2;
		  else if(sym==12)value=num1*num2;
		  else if(sym==13)value=num1/num2;
		  for(i=0;i<6;i++)tempdata[i]=0x00;	  //前面显示的先变为0
	
		  //把结果存到tempdata[]中
	
		  if(value<10)tempdata[0]=duanma[value]; 
		  else if(value<100){
		  	tempdata[1]=duanma[value/10];
			tempdata[0]=duanma[value%10];
		  }	
		  
		  else if(value<1000){
			tempdata[2]=duanma[value/100];
		  	tempdata[1]=duanma[(value%100)/10];
			tempdata[0]=duanma[value%10];
		  }	
		 
		  else if(value<10000){
			tempdata[3]=duanma[value/1000];
			tempdata[2]=duanma[(value%1000)/100];
		  	tempdata[1]=duanma[(value%100)/10];
			tempdata[0]=duanma[value%10];
		  }	
		  
		  else if(value<100000){
			tempdata[4]=duanma[value/10000];
			tempdata[3]=duanma[(value%10000)/1000];
			tempdata[2]=duanma[(value%1000)/100];
		  	tempdata[1]=duanma[(value%100)/10];
			tempdata[0]=duanma[value%10];
		  }	
	
		  else if(value<1000000){
			tempdata[5]=duanma[value/100000];	
		    tempdata[4]=duanma[(value%100000)/10000];
			tempdata[3]=duanma[(value%10000)/1000];
			tempdata[2]=duanma[(value%1000)/100];
		  	tempdata[1]=duanma[(value%100)/10];
			tempdata[0]=duanma[value%10];
		  }
	
		  biaozhi=0;
		  count=0;
		  num=0;
		  num1=0;
		  num2=0;
		}
	
	
		else if(temp==15)			 //清除
		{
		  biaozhi=0;
		  count=0;
		  num=0;
		  num1=0;
		  num2=0;
		  for(i=0;i<6;i++)tempdata[i]=0x00;
		  tempdata[0]=0x3f;
		}
		  					
		
//	    delayms();
		tempbak=temp;
	} 
     display();			 //显示
  }
}


void display(void)
{
  uchar i;
  for(i=0;i<6;i++)  //六个数码管
	{
	  P2=0x00;
	  P2=tempdata[i]; //数码管显示的值
	  P3=weima[5-i];  //哪一个数码管显示
	  delayms();
	}
}

/* 
	键盘处理程序：
		1.确定是否有键按下-去抖。
		2.哪一个件按下代表那个数
		3.相应按键执行什么功能
*/

// 扫描键盘返回相应位置编码	
uchar kbscan(void) //键盘扫描函数
{ 
	uchar sccode,recode;
	P1=0xf0; // P1.0~P1.3发全0，P1.4~P1.7输入

	if ( (P1 & 0xf0)!=0xf0)  // * 如P1口高四位不全为1有键按下* /
	{
		delayms(); // * 延时去抖动* /
		if ((P1&0xf0)!=0xf0) // * 在读输入值* /
		{ 
			sccode =0xfe; // * 最低位置0 * /
			while((sccode&0x10)!=0)   // 不到最后一行循环
			{
				P1 =sccode;    //*P1口输出扫描码
				if ( (P1 & 0xf0)!=0xf0) //  如P1.4~P1.7不全为1，该行有键按下
				{
					recode = P1 & 0xf0; // * 保留P1口高四位值，低四位变为全1, 作为列值* /
					return((sccode&0x0f)|(recode));     //*行码+列值=键编码返回主程序
				}
		        else
			    {
					sccode =(sccode<<1)|0x01; // * 如该行无键按下，查下一行，行扫描值左移一位
				}
		    } 
		}
	}
	return( 0 );	  // * 无键按下，返回值为0 
} 


void delayms(void)	//延时函数
{
  uchar i;						 
  for(i=200;i>0;i--){;}
}


//根据键盘得到数字
unsigned char keypro(void)	 //键值转化函数
{
 	switch(kbscan())
	{
	  case 0xee:return 7;break;
	  case 0xde:return 8;break;
	  case 0xbe:return 9;break;
	  case 0x7e:return 13;break;// 除法
	  case 0xed:return 4;break;
	  case 0xdd:return 5;break;
	  case 0xbd:return 6;break;
	  case 0x7d:return 12;break;//乘法
	  case 0xeb:return 1;break;
	  case 0xdb:return 2;break;
	  case 0xbb:return 3;break;
	  case 0x7b:return 11;break;//减法
	  case 0xe7:return 15;break;//清除
	  case 0xd7:return 0;break;
	  case 0xb7:return 14;break;//等于
	  case 0x77:return 10;break;//加法
	  default:return 0xff;break;
   }
}
