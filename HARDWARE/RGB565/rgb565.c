#include <rgb565.h>
#include "EasyTracer_color.h"
#include "lcd.h"
const u8*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7种特效  
/*			4.3寸，800*480
				OV2640_Window_Set   设置图像输出窗口 
						传感器窗口设置允许用户设置整个传感器区域（1632*1220）的感兴趣部分，也就是在传感器里面开窗，开窗范围从2*2~1632*1220都可以设置，不过要求这个窗口必须大于等于随后设置的图像尺寸 
				OV2640_ImageSize_Set 设置图像尺寸大小,也就是所选格式的输出分辨率
						图像尺寸设置，也就是DSP输出（最终输出到LCD的）图像的最大尺寸，该尺寸要小于等于前面我们传感器窗口设置所设定的窗口尺寸。
						
				OV2640_ImageWin_Set 设置图像开窗大小
						图像窗口设置其实和前面的传感器窗口设置类似，只是这个窗口是在我们前面设置的图像尺寸里面，再一次设置窗口大小，该窗口必须小于等于前面设置的图像尺寸。设置后的图像范围，将用于输出到外部。
				OV2640_OutSize_Set	设置图像输出大小
						图像输出大小设置，控制最终输出到外部的图像尺寸。该设置将图像窗口设置所决定的窗口大小，通过内部DSP处理，缩放成我们输出到外部的图像大小。
						该设置将会对图像进行缩放处理，如果设置的图像输出大小不等于图像窗口设置图像大小，那么图像就会被缩放处理，只有这两者设置一样大的时候，输出比例才是1：1的
			lcddev.width=800;
			lcddev.height=480;
*/
  int x=0,y=0;
  TARGET_CONDI Conditionred={215,240,20,240,30,160,15,15,200,200};	//红色1   API参数  hsl的阈值，识别时用的
	RESULT Resured;//判定为的目标条件
  u8 ov_frame=0;//帧率	
void rgb565_test(void)
{   
	OV2640_RGB565_Mode();	//RGB565模式
	My_DCMI_Init();			//DCMI配置
	DCMI_DMA_Init((u32)&LCD->LCD_RAM,1,DMA_MemoryDataSize_HalfWord,DMA_MemoryInc_Disable);//DCMI DMA配置  
//	OV2640_ImageWin_Set(u16 offx,u16 offy,u16 width,u16 height);
 	OV2640_OutSize_Set(240,300); //480*600
	DCMI_Start(); 		//启动传输	  
  while(1)
	{
	 delay_ms(10); 
	}
}

 //DCMI中断服务函数
void DCMI_IRQHandler(void)
{
	if(DCMI_GetITStatus(DCMI_IT_FRAME)==SET)//捕获到一帧图像
	{
	 DCMI_Stop();
	 if(Trace(&Conditionred,&Resured)) 
	 {
		 LCD_Fill(Resured.x-Resured.w/2,Resured.y-Resured.h/2,Resured.x+Resured.w/2,Resured.y-Resured.h/2+1,0xf800);//u16 x,u16 y,u16 width,u16 hight,u16 Color
		 LCD_Fill(Resured.x-Resured.w/2,Resured.y-Resured.h/2,Resured.x-Resured.w/2+1,Resured.y+Resured.h/2,0xf800);
		 LCD_Fill(Resured.x-Resured.w/2,Resured.y+Resured.h/2,Resured.x+Resured.w/2,Resured.y+Resured.h/2+1,0xf800);
		 LCD_Fill(Resured.x+Resured.w/2,Resured.y-Resured.h/2,Resured.x+Resured.w/2+1,Resured.y+Resured.h/2,0xf800);
		 LCD_Fill(Resured.x-2,Resured.y-2,Resured.x+2,Resured.y+2,0xf800);	
		 x=Resured.x;
		 y=Resured.y; 
	 }
 		
	
  ov_frame++; 
	DCMI_Start();	
	DCMI_ClearITPendingBit(DCMI_IT_FRAME);//清除帧中断
	LED1=!LED1;	
	}	
}
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		printf("framerate is %d \r\n",ov_frame);ov_frame=0;
		printf("x is %d ，y is %d \r\n",x,y);
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位 
}

