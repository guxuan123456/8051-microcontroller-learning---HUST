# include <absacc.h>

main()
{
  char xdata *p;	//存储数据初始位置
  unsigned char begin = 0x16;
  unsigned int sum = 0;

  for(p=0x4050;p<0x4060;p++)	 //数据初始化
  {
  	 *p = begin;
	 begin++;
  }

  // 循环求取最大最小值和平均数
  XBYTE[0x4090] = 0;//最大值初始化
  XBYTE[0x4091] = 0xFF;//最小值初始化

  for(p=0x4050;p<0x4060;p++)	 //数据初始化
  {
  	 sum+=*p;  //求和

	 if(XBYTE[0x4090]<*p){
	   XBYTE[0x4090] = *p;
	 }

	 if(XBYTE[0x4091]>*p){
	   XBYTE[0x4091] = *p;
	 }
  }

   //求平均值和余数
    XBYTE[0x4080]  = sum/16;
	XBYTE[0x4081]  = sum%16;

  return 0;
}
