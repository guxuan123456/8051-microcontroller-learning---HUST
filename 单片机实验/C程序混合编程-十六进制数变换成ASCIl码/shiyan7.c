#include<absacc.h> 
extern  void turn_to_ascii(unsigned char p1, unsigned char p2);//定义汇编函数
extern  void init();  
void main(){ 
	unsigned char p1;  //p1为片外储存转移地址
	unsigned char p2;  //片内存储地址
	unsigned char num;   //计数器
	p1 = 0x00; //偏移地址为0
	p2 = 0x30; //片内30H开始
	init();    //初始化数据，调用汇编
	for(num = 0x00;num<0x14;num++){
		turn_to_ascii(p1,p2); //传递地址
		p1 = p1 + 2;		  //片外地址转移2
		p2++;	//片内转移1
	}	
}
