#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usmart.h"  
#include "usart2.h"  
#include "timer.h" 
#include "ov2640.h" 
#include "dcmi.h" 
#include "rgb565.h"

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  			//初始化延时函数
	uart_init(115200);			//初始化串口波特率为115200
	LED_Init();							//初始化LED 
 	LCD_Init();							//LCD初始化  
	TIM3_Int_Init(10000-1,8400-1);//10Khz计数,1秒钟中断一次 
// 	usmart_dev.init(84);		//初始化USMART
 	POINT_COLOR=RED;				//设置字体为红色  
	if(lcddev.height>600)
    {
        lcddev.height=600;      //SVGA模式,必须不能大于600.
        LCD_Set_Window(0,0,240,300);//设置开窗口为480*600.
    }
	while(OV2640_Init())		//初始化OV2640
	{
		LCD_ShowString(30,90,240,16,16,"OV2640 ERR");
		delay_ms(200);
	  LCD_Fill(30,130,239,170,WHITE);
		delay_ms(200);
	}
	LCD_ShowString(30,90,200,16,16,"OV2640 OK");  
	LCD_Clear(WHITE);
	rgb565_test(); 
}
