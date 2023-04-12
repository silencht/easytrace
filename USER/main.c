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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  			//��ʼ����ʱ����
	uart_init(115200);			//��ʼ�����ڲ�����Ϊ115200
	LED_Init();							//��ʼ��LED 
 	LCD_Init();							//LCD��ʼ��  
	TIM3_Int_Init(10000-1,8400-1);//10Khz����,1�����ж�һ�� 
// 	usmart_dev.init(84);		//��ʼ��USMART
 	POINT_COLOR=RED;				//��������Ϊ��ɫ  
	if(lcddev.height>600)
    {
        lcddev.height=600;      //SVGAģʽ,���벻�ܴ���600.
        LCD_Set_Window(0,0,240,300);//���ÿ�����Ϊ480*600.
    }
	while(OV2640_Init())		//��ʼ��OV2640
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
