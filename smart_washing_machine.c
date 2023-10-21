#include<reg51.h>
//电源
sbit dy=P3^3;
sbit dy_led=P0^1;
//普通洗衣
sbit xiyi_out=P3^1;
sbit xiyi_in=P3^4;
//加水
sbit js=P3^0;
//单脱水
sbit ts_in=P3^5;
sbit ts_out=P3^2;
//停止
sbit tz=P3^6;

sbit dian_led=P0^0;

//电动机
sbit motor_1=P0^6;
sbit motor_2=P0^7;

unsigned char mg=0,
			  ms=0,
			  fg=0,
			  fs=0,
			  time=0,  //中断控制变量
			  num=0,
			  num1=0,
			  dy1,	  //判断菜单是否打开
			  mode1=0,//洗衣模式选择的变量
			  mode2=0,
			  mode3=0,
			  key=0,//判断停止按键是否按下
			  num_1;
			  num_2;
			  return_1=0;
//共阴极段码
unsigned char code duanma[]=
{0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x40};
//************************************************************************
//延时
//************************************************************************
void delay(unsigned char x)
{
	unsigned char i,j,k;
	for(i=0;i<x;i++)
	for(j=0;j<40;j++)
	for(k=0;k<50;k++)
	;
}
//************************************************************************
//电动机工作与停止
//************************************************************************
void zheng()//正转
{
	motor_1=1;
	motor_2=0;
}
void fan()  //反转
{
	motor_1=0;
	motor_2=1;
}
void stop() //停止
{
	motor_1=0;
	motor_2=0;
}
//************************************************************************
//洗衣控制
//************************************************************************
void xiyi()
{
   
  //工作（正转，反转）
	if(num>=1&&num<60&&num1>6)
	{
		if(num%10==1)
		{
			if(num/10%2==0)  	//正转
			{
				zheng();
			}
			else if(num/10%2==1)//反转
			{
				fan();
			}
		}
		if(num%10==6)			//停止
		{
			stop();
		}
	}
	//脱水
	if(num1<=6&&num1>3)
	{
		zheng();
	}
	//停止
	if(num1<=3) 
	{
		stop();
	}
	
} 


//************************************************************************
//数码管输出显示
//************************************************************************
void show()
{
	if(num%2)
	{
		dian_led=0;
	}else
	{
	   	dian_led=1;
	}
	P1=duanma[mg];//阳极，图形码
	P2=0xf7;
	delay(1);
	P2=0xff;
	
	P1=duanma[ms];
	P2=0xfb;
	delay(1);
	P2=0xff;
	
	P1=duanma[fg];//阳极，图形码
	P2=0xfd;
	delay(1);
	P2=0xff;
	
	P1=duanma[fs];
	P2=0xfe;
	delay(1);
	P2=0xff;   
}
void show1()
{	unsigned char i;
    for(i=0;i<50;i++)
	{ 
	    dian_led=1;
		P1=duanma[num_1%10];//阳极，图形码
		P2=0xf7;
		delay(1);
		P2=0xff;
	
		P1=duanma[num_1/10];
		P2=0xfb;
		delay(1);
		P2=0xff;
		
		P1=duanma[num_2%10];//阳极，图形码
		P2=0xfd;
		delay(1);
		P2=0xff;
		
		P1=duanma[num_2/10];
		P2=0xfe;
		delay(1);
		P2=0xff; 
	}
	for(i=0;i<50;i++)
	{ 
	    dian_led=0;
		P1=0x00;
		P2=0x00;
		delay(1);
		P2=0xff;  
	}
	
}
//菜单
void dianyuan()
{
		if(dy==0)
	{
	  delay(1);
	  if(dy==0)
	  {
	  	dy1=1;
		dy_led=0;
		while(dy==0);
	  }
	}
}
void xiyi_mode()
{
	if(xiyi_in==0) //选择的模式为普通洗衣
	   {
		delay(10);
		if(xiyi_in==0)
		{
		xiyi_out=0;	//打开普通洗衣指示灯
		js=0;//打开加水闸门
		ts_out=1;	//关闭单脱水指示灯
		mode1=1;//洗衣模式选择普通洗衣的标记标记变量
		mode2=1;
		num=60;
		num1=45;
		while(xiyi_in==0);
		}
	   }	
}

void ts_mode()
{
 	if(ts_in==0) //选择的模式为单脱水
	   {
		delay(10);
		if(ts_in==0)
		{
		ts_out=0;	//打开单脱水指示灯
		xiyi_out=1;	//关闭普通洗衣指示灯
		js=1;//关闭加水闸门
		mode1=2;//洗衣模式选择单脱水的标记标记变量
		mode3=1;
		num=60;
		num1=25;		
		while(ts_in==0);
		}
	   }
}
//停止
void tingzhi()
{
	 	if(tz==0) //判断暂停是否按下
	   {
		delay(1);
		if(tz==0)
		{ key=1;
		 while(tz==0);
		}
		} 	  
}

//洗衣模式选择
void mode()
{
  if((dy1==1&&xiyi_out==0)&&(num!=0&&num1!=0))//选择普通洗衣模式
  {	mode1=0;	
     xiyi();
  }
    if((dy1==1&&ts_out==0)&&(num!=0&&num1!=0))//选择单脱水模式
  {	 mode1=0;
     zheng();
  }
  if(num==0&&num1==0)
  {stop();}
}
//************************************************************************
//中断初始化函数
//************************************************************************
void Interrupt_init()
{
	EA=1;                   //开总中断
    TMOD=0x01;              //使用定时器T0的，工作方式1

	ET0=1;                  //定时器T0中断允许   
	TH0=(65536-50000)/256;  //定时器T0的高8位赋初值
	TL0=(65536-50000)%256;  //定时器T0的低8位赋初值
	TR0=1;                  //定时器T0启动

	ET1=1;                  //定时器T1中断允许   
	TH1=(65536-50000)/256;  //定时器T1的高8位赋初值
	TL1=(65536-50000)%256;  //定时器T1的低8位赋初值
	TR1=1;                  //定时器T1启动
}
//************************************************************************
//主函数
//************************************************************************
void main()
{  	unsigned char i=1;
    dianyuan();
    Interrupt_init();//中断初始化函数	 
		while(i==1&&key!=1&&dy==1)
		    {	
			    show();
			    mode();	
			}
			if(key==1&&mode1==0&&dy==1)
		 {
		 	num_1=num;
			num_2=num1;
		    while(key==1&&mode1==0)
				 {
					stop();
					show1();	
				 }
		 }
  }		 

//************************************************************************
//中断函数
//************************************************************************
void T0_time() interrupt 1	 //t0定时器，中断1，控制数码管延时。
{		
	
	
	time++;
  	if(time==20&&mode2==1)//显示秒
	{
		time=0;
		num--;
		mg=num%10;
		ms=num/10;
		fg=num1%10;
    	fs=num1/10;
	
	}
    if(num==0)//显示分
	{
		num=59;
		num1--;
		if(num1==0&&num==0)
		{
		  TR0=0;
		}
			
	}
		
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;
}

void T1_time() interrupt 3
{	
   	if(num1==1)//自动加水满一分钟关闭闸门
	{
	 	js=1;
	}
	xiyi_mode();//普通洗衣
	ts_mode();//单脱水
	tingzhi();//停止
			 
 	TH1=(65536-50000)/256;  //定时器T1的高8位赋初值
	TL1=(65536-50000)%256;  //定时器T1的低8位赋初值
}
