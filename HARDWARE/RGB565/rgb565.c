#include <rgb565.h>
#include "EasyTracer_color.h"
#include "lcd.h"
const u8*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7����Ч  
/*			4.3�磬800*480
				OV2640_Window_Set   ����ͼ��������� 
						�������������������û�������������������1632*1220���ĸ���Ȥ���֣�Ҳ�����ڴ��������濪����������Χ��2*2~1632*1220���������ã�����Ҫ��������ڱ�����ڵ���������õ�ͼ��ߴ� 
				OV2640_ImageSize_Set ����ͼ��ߴ��С,Ҳ������ѡ��ʽ������ֱ���
						ͼ��ߴ����ã�Ҳ����DSP��������������LCD�ģ�ͼ������ߴ磬�óߴ�ҪС�ڵ���ǰ�����Ǵ����������������趨�Ĵ��ڳߴ硣
						
				OV2640_ImageWin_Set ����ͼ�񿪴���С
						ͼ�񴰿�������ʵ��ǰ��Ĵ����������������ƣ�ֻ�����������������ǰ�����õ�ͼ��ߴ����棬��һ�����ô��ڴ�С���ô��ڱ���С�ڵ���ǰ�����õ�ͼ��ߴ硣���ú��ͼ��Χ��������������ⲿ��
				OV2640_OutSize_Set	����ͼ�������С
						ͼ�������С���ã���������������ⲿ��ͼ��ߴ硣�����ý�ͼ�񴰿������������Ĵ��ڴ�С��ͨ���ڲ�DSP�������ų�����������ⲿ��ͼ���С��
						�����ý����ͼ��������Ŵ���������õ�ͼ�������С������ͼ�񴰿�����ͼ���С����ôͼ��ͻᱻ���Ŵ���ֻ������������һ�����ʱ�������������1��1��
			lcddev.width=800;
			lcddev.height=480;
*/
  int x=0,y=0;
  TARGET_CONDI Conditionred={215,240,20,240,30,160,15,15,200,200};	//��ɫ1   API����  hsl����ֵ��ʶ��ʱ�õ�
	RESULT Resured;//�ж�Ϊ��Ŀ������
  u8 ov_frame=0;//֡��	
void rgb565_test(void)
{   
	OV2640_RGB565_Mode();	//RGB565ģʽ
	My_DCMI_Init();			//DCMI����
	DCMI_DMA_Init((u32)&LCD->LCD_RAM,1,DMA_MemoryDataSize_HalfWord,DMA_MemoryInc_Disable);//DCMI DMA����  
//	OV2640_ImageWin_Set(u16 offx,u16 offy,u16 width,u16 height);
 	OV2640_OutSize_Set(240,300); //480*600
	DCMI_Start(); 		//��������	  
  while(1)
	{
	 delay_ms(10); 
	}
}

 //DCMI�жϷ�����
void DCMI_IRQHandler(void)
{
	if(DCMI_GetITStatus(DCMI_IT_FRAME)==SET)//����һ֡ͼ��
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
	DCMI_ClearITPendingBit(DCMI_IT_FRAME);//���֡�ж�
	LED1=!LED1;	
	}	
}
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{
		printf("framerate is %d \r\n",ov_frame);ov_frame=0;
		printf("x is %d ��y is %d \r\n",x,y);
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ 
}

