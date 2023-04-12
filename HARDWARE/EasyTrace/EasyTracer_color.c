#include "EasyTracer_color.h"
#include "LCD.h"
#define min3v(v1, v2, v3)   ((v1)>(v2)? ((v2)>(v3)?(v3):(v2)):((v1)>(v3)?(v3):(v1)))//ȡ���ֵ
#define max3v(v1, v2, v3)   ((v1)<(v2)? ((v2)<(v3)?(v3):(v2)):((v1)<(v3)?(v3):(v1)))//ȡ��Сֵ

/*  
	���������ͱ�ע��STM32F4����
		typedef unsigned          char uint8_t;  u8
		typedef unsigned short     int uint16_t; u16 (int��ʡ)Ϊ16λ�������ͣ�ռ�����ֽڣ���ΧΪ0~65535
		typedef unsigned           int uint32_t; u32
		typedef unsigned       __INT64 uint64_t;
		
	��static���ͺ�����static��������ͨ����������
����	��static���εĺ������޶��ڱ�Դ���ļ��У����ܱ���Դ���ļ�����Ĵ����ļ����á�����ͨ�ĺ�����Ĭ����extern�ģ������Ա����������ļ����øú�����
����	�ں����ķ�������ǰ���Ϲؼ���static�������ͱ������Ϊ��̬��������ͨ�����Ķ��������Ĭ���������extern�ģ�����̬����ֻ�������������ļ����пɼ���
			���ܱ������ļ����á���˶��徲̬�����������ŵ㣺
����<1> �����ļ��п��Զ�����ͬ���ֵĺ��������ᷢ����ͻ��
����<2> ��̬�������ܱ������ļ����á�
  ��constͨ�����γ������ͣ������/�����ֵ���ܱ��ı䡣���������¼��㣺
		<1> ���ζ�����в��ɱ��ԣ����ڽ������ͼ�飬�����޸�ʱ�������ɲ����ǿ�˳���Ľ�׳��
����<2> ��ʡ�ռ䣬���ⲻ��Ҫ���ڴ���䣬���Ч��
  ��RGB��HSL��Ҳ��HSB/HSV��������ɫ�ʿռ䣬������,��,����Red,Green,Blue����ɫ��,���Ͷ�,���ȣ�Hue,Saturation,Lightness��Brightness��Value����
	  RGB�����ڻ���������Ŀǰ����ʾ����ɫ���������ֻ�ɫ���ɣ���HSL�����������ֱ�۸о���
		������һ����һ����ɫ������˵���е�Ũ�İ���ɫ��������˵��ɫռ���٣���ɫռ���٣���ɫռ����;
		<1>���Ƚ���ͼ��������ɫ�ɷֺ����ٵ���ɫ�ɷֵ������йء�����Խ�ߣ�ͼ��Խ���������İ�ɫ
		<2>���Ͷ���ͼ��������ɫ�ɷֺ����ٵ���ɫ�ɷֵĲ����йء����Ͷ�ԽС��ͼ��Խ���ڻҶ�ͼ�񡣱��Ͷ�Խ��ͼ��Խ���ޣ����˵ĸо��ǲ�ɫ��
		<3>ɫ���������˶�ͼ��Ĳ�ͬ����ɫ���ܡ� 
*/

//RGB��ɫ��ʽ�ṹ��
typedef struct
	  {
    unsigned char  red;             // [0,255]
    unsigned char  green;           // [0,255]
    unsigned char  blue;            // [0,255]
    }COLOR_RGB;//RGB��ʽ��ɫ
		
//ɫ��(H)�����Ͷ�(S)������(L)��ɫ��ʽ�ṹ��
typedef struct
	  {
    unsigned char hue;              // [0,240] ɫ��
    unsigned char saturation;       // [0,240] ���Ͷ�
    unsigned char luminance;        // [0,240] ����
    }COLOR_HSL;//HSL��ʽ��ɫ

//����ͼ�񴰿�����ͼ��X/Y��ʼ����
typedef struct
	  {
    unsigned int X_Start;              
    unsigned int X_End;
	  unsigned int Y_Start;              
    unsigned int Y_End;
    }SEARCH_AREA;//����

//��ȡRBG��ʽ��ɫ��Ψһ��Ҫ��ֲ�ĺ���
extern unsigned short LCD_ReadPoint(unsigned short x,unsigned short y);//��ĳ����ɫ

/***************************************************************************
	*�������ƣ�ReadColor
	*�������ܣ���LCD��Ļĳ����ɫ��֮ת��Ϊ RGB��ʽ ���洢�� RGB��ɫ��ʽ�ṹ�� ��
	*��ڲ�����x,yҪ��ȡ��ɫ��LCD��Ļ���꣬COLOR_RGB�����R��G��Bֵ�Ľṹ��
	*���ز�������               
****************************************************************************/
static void ReadColor(unsigned int x,unsigned int y,COLOR_RGB *Rgb)
	{
		unsigned short C16; //��Color16��R5+G6+B5=16

		C16 = LCD_ReadPoint(x,y);     //��ĳ����ɫ

		Rgb->red   =	 (unsigned char)((C16&0xf800)>>8);
		Rgb->green =	 (unsigned char)((C16&0x07e0)>>3);
		Rgb->blue  =   (unsigned char)((C16&0x001f)<<3);
	}
/***************************************************************************
	*�������ƣ�RGBtoHSL
	*�������ܣ���ɫ��ʽת����RGBתHSL
	*��ڲ�����COLOR_RGB��Ҫת����RGB�ṹ���ַ		COLOR_HSL��ת����HSL���洢����HSL�ṹ���ַ
	*���ز�������               
****************************************************************************/
static void RGBtoHSL(const COLOR_RGB *Rgb, COLOR_HSL *Hsl)
	{
			int h,s,l,maxVal,minVal,difVal;
			int r  = Rgb->red;
			int g  = Rgb->green;
			int b  = Rgb->blue;   //��ȡR��G��Bֵ
		
				maxVal = max3v(r, g, b);
				minVal = min3v(r, g, b);//��ȡRGB�������/��Сֵ
				
				difVal = maxVal-minVal; //���������Сֵ�Ĳ�ֵ

		//��������   
			l = (maxVal+minVal)*240/255/2;
		
		if(maxVal == minVal)//��r=g=b
			{
				h = 0; 
				s = 0;
			}
		else
		{
			//����ɫ��
			if(maxVal==r)
			{
				if(g>=b)
					h = 40*(g-b)/(difVal);
				else
					h = 40*(g-b)/(difVal) + 240;
			}
			else if(maxVal==g)
					h = 40*(b-r)/(difVal) + 80;
			else if(maxVal==b)
					h = 40*(r-g)/(difVal) + 160;
			//���㱥�Ͷ�
			if(l == 0)
				//	s = 0;
			   s=(difVal)*240/(511 - (maxVal+minVal));
			else if(l<=120)
					s = (difVal)*240/(maxVal+minVal);
			else
					s = (difVal)*240/(511 - (maxVal+minVal));
		}
			Hsl->hue =        (unsigned char)(((h>240)? 240 : ((h<0)?0:h)));//ɫ��
			Hsl->saturation = (unsigned char)(((s>240)? 240 : ((s<0)?0:s)));//���Ͷ�
			Hsl->luminance =  (unsigned char)(((l>240)? 240 : ((l<0)?0:l)));//����
	}
/***************************************************************************
	*�������ƣ�ColorMatch
	*�������ܣ���ɫƥ��
	*��ڲ�����COLOR_HSL��Ҫ����ƥ���HSL	TARGET_CONDI��ƥ�䣨�ж��ģ�Ŀ������
	*���ز������ɹ�����1��ʧ�ܷ���0              
****************************************************************************/
static int ColorMatch(const COLOR_HSL *Hsl,const TARGET_CONDI *Condition)
	{
		if( 
					Hsl->hue		>	Condition->H_MIN &&
					Hsl->hue		<	Condition->H_MAX &&
					Hsl->saturation	>	Condition->S_MIN &&
					Hsl->saturation	<   Condition->S_MAX &&
					Hsl->luminance	>	Condition->L_MIN &&
					Hsl->luminance	<   Condition->L_MAX 
			)          //hueΪɫ����saturationΪ���Ͷ� ��luminanceΪ����
				return 1;
		else
				return 0;
	}

/***************************************************************************
	*�������ƣ�SearchCentre
	*�������ܣ���������ɫƥ�䣬����Ŀ������Ĵ������
	*��ڲ�����*x��*yΪ������Ŀ���洢Ŀ�������ָ���ַ��TARGET_CONDIΪҪ�ж���Ŀ��������SEARCH_AREAΪҪ��������������
	*���ز������ɹ�����1��ʧ�ܷ���0              
****************************************************************************/
static int SearchCentre(unsigned int *x,unsigned int *y,const TARGET_CONDI *Condition,const SEARCH_AREA *Area)
	{
			unsigned int SpaceX,SpaceY,i,j,k,FailCount=0;
			COLOR_RGB Rgb;
			COLOR_HSL Hsl;
			
			SpaceX = Condition->WIDTH_MIN/3;   //Ŀ����С���
			SpaceY = Condition->HIGHT_MIN/3;   //Ŀ����С�߶�
			//�ֱ�ȡĿ����С�ߡ���ȵ�����֮һΪ�������ƶ��������ƶ������ƶ���������Ϊ��ʼ���������³����Ρ�����ֱ�Ϊ�����Ჽ��������ȡʮ�������е�������
				for(i=Area->Y_Start;i<Area->Y_End;i+=SpaceY)//i����������ʼ�㣬iС����������㣬iÿ���Լ�Ŀ����С�߶ȵ�1/3
				{
					for(j=Area->X_Start;j<Area->X_End;j+=SpaceX)//j���ں�����ʼ��,jС�ں�������㣬jÿ���Լ�Ŀ����С���1/3
					{
						FailCount=0; //ʧ�ܴ�����ʼ��
						for(k=0;k<SpaceX+SpaceY;k++)//kС��Ŀ����С�����Ŀ����С�߶Ⱥ�  
						{
							if(k<SpaceX)//���kС��Ŀ����С���
								ReadColor(j+k,i+SpaceY/2,&Rgb);//����j+k,i+SpaceY/2����ɫ
							else//����
								ReadColor(j+SpaceX/2,i+(k-SpaceX),&Rgb);//����j+SpaceX/2,i+(k-SpaceX)����ɫ
							
							RGBtoHSL(&Rgb,&Hsl);//�����󣬽�RGBתHSL
							if(!ColorMatch(&Hsl,Condition))//������ɫƥ�䣬�����ɫƥ�䲻�ɹ�
							FailCount++;	//ʧ�ܼ���	++		
							if(FailCount>((SpaceX+SpaceY)>>ALLOW_FAIL_PER))//���ʧ�ܴ��������ݴ���
								break;                                       //�����ó��������������ѭ�����ƶ�����һ������
						}
							   
						if(k==SpaceX+SpaceY)//���ĳ������ƥ��ɹ�����ôk�ص���SpaceX+SpaceY��������������ĵ㸳ֵ���洢�����ָ���ַ�������سɹ�ֵ1
						{
							*x = j+SpaceX/2;
							*y = i+SpaceY/2;
							return 1;
						}
					}
				}
		return 0;
	}
/***************************************************************************
	*�������ƣ�Corrode
	*�������ܣ��Ӹ�ʴ�������ⸯʴ���õ��µĸ�ʴ����
	*��ڲ�����oldx��oldyΪ�ϴ�������Ŀ�����ĵ�ַ��TARGET_CONDIΪҪ�ж���Ŀ��������RESULTΪҪ�洢�Ľ��ֵ�ṹ��
	*���ز������ɹ�����1��ʧ�ܷ���0  
	*��ע��((Condition->WIDTH_MIN+Condition->WIDTH_MAX)>>2),�����С�͵�ƽ������С������4
****************************************************************************/	
static int Corrode(unsigned int oldx,unsigned int oldy,const TARGET_CONDI *Condition,RESULT *Resu)
{
	unsigned int Xmin,Xmax,Ymin,Ymax,i,FailCount=0;
	COLOR_RGB Rgb;
	COLOR_HSL Hsl;
	
	for(i=oldx;i>IMG_X;i--) //��ʴ��y����
		{
				ReadColor(i,oldy,&Rgb);//������ɫ
				RGBtoHSL(&Rgb,&Hsl); //RGBת��HSL
				if(!ColorMatch(&Hsl,Condition))//������ɫƥ��
					FailCount++;//�����������
				if(FailCount>(((Condition->WIDTH_MIN+Condition->WIDTH_MAX)>>2)>>ALLOW_FAIL_PER))
					break;	
		}
	Xmin=i; //����X����С����ֵ
	FailCount=0;//��մ������
	
	for(i=oldx;i<IMG_X+IMG_W;i++)//�Ҹ�ʴ��y����
		{
				ReadColor(i,oldy,&Rgb);
				RGBtoHSL(&Rgb,&Hsl);
				if(!ColorMatch(&Hsl,Condition))
					FailCount++;
				if(FailCount>(((Condition->WIDTH_MIN+Condition->WIDTH_MAX)>>2)>>ALLOW_FAIL_PER))
					break;	
		}
	Xmax=i;//����X���������ֵ
	FailCount=0;//��մ������
	
	for(i=oldy;i>IMG_Y;i--)//�¸�ʴ��x����
		{
				ReadColor(oldx,i,&Rgb);
				RGBtoHSL(&Rgb,&Hsl);
				if(!ColorMatch(&Hsl,Condition))
					FailCount++;
				if(FailCount>(((Condition->HIGHT_MIN+Condition->HIGHT_MAX)>>2)>>ALLOW_FAIL_PER))
					break;	
		}
	Ymin=i;//����Y����С����ֵ
	FailCount=0;//��մ������
	
	for(i=oldy;i<IMG_Y+IMG_H;i++)//�ϸ�ʴ��x����
		{
				ReadColor(oldx,i,&Rgb);
				RGBtoHSL(&Rgb,&Hsl);
				if(!ColorMatch(&Hsl,Condition))
					FailCount++;
				if(FailCount>(((Condition->HIGHT_MIN+Condition->HIGHT_MAX)>>2)>>ALLOW_FAIL_PER))
					break;	
		}
	Ymax=i;//����Y���������ֵ
	FailCount=0;//��մ������
	//��������׼ȷ�ʵ�����
	Resu->x	= (Xmin+Xmax)/2;
	Resu->y	= (Ymin+Ymax)/2;
	Resu->w	= Xmax-Xmin;
	Resu->h	= Ymax-Ymin;
	//�鿴�����߶��Ƿ����
	if(
			 ((Xmax-Xmin)>(Condition->WIDTH_MIN)) && ((Ymax-Ymin)>(Condition->HIGHT_MIN)) &&\
			 ((Xmax-Xmin)<(Condition->WIDTH_MAX)) && ((Ymax-Ymin)<(Condition->HIGHT_MAX))
	   )
		  return 1;	
	else
		  return 0;	
}
/***************************************************************************
	*�������ƣ�Trace
	*�������ܣ��û���ʶ������д��Conditionָ��Ľṹ���У��ú���������Ŀ���x��y����ͳ���
	*��ڲ�����ʶ��������Conditionָ��Ľṹ�壻ʶ������д��Conditionָ��Ľṹ��
	*���ز���������1ʶ��ɹ�������0ʶ��ʧ��
***************************************************************************/

int Trace(const TARGET_CONDI *Condition,RESULT *Resu)
{
	unsigned int i;
	static unsigned int x0,y0,flag=0;  //��̬����
	static SEARCH_AREA Area={IMG_X,IMG_X+IMG_W,IMG_Y,IMG_Y+IMG_H};  //Ҫ����������ͼ������x���� y����  w���  h�߶�
	RESULT Result;	//RESULTʶ����
	
	if(flag==0)    //����״�ʹ�û���һ�θ�ʴʧ��
		{
			if(SearchCentre(&x0,&y0,Condition,&Area))  //������ʴ���Ĳ����ظ�x0��y0������ɹ�����������ôflag��1
				 flag=1;  
			else  //�����û��ʴ�ɹ�����ô�Ѹ�ʴ�����ٴ���������ͼ��Χ�ڽ��и�ʴ
				{
						Area.X_Start= IMG_X	       ;
						Area.X_End  = IMG_X+IMG_W  ;
						Area.Y_Start= IMG_Y		     ;
						Area.Y_End  = IMG_Y+IMG_H  ;

						if(SearchCentre(&x0,&y0,Condition,&Area))	//���������Χ��ʴ�ɹ�����ôflag��1
						{
							flag=1;//default 0
							return 1;
						}
						else
						{
							flag=0;//default 0
							return 0;
						}
				}
		}
	Result.x = x0;//���flag!=0��˵����һ���и�ʴ���Ľ��������ֱ��ʹ����һ�ν����ʴ���ɣ�������Ҫ�ٴα���ͼ��������ʴ����
	Result.y = y0;//��һ�εĸ�ʴ���ĸ�ֵ����ε�oldx��oldy
	
	for(i=0;i<ITERATE_NUM;i++)  //���и�ʴ��������
		 Corrode(Result.x,Result.y,Condition,&Result);
		
	if(Corrode(Result.x,Result.y,Condition,&Result))//�Ӹ�ʴ�������ⸯʴ�ɹ�
		{
			x0=Result.x;
			y0=Result.y; //���¸�ʴ���ģ��Ա��´�ʹ��
			Resu->x=Result.x;
			Resu->y=Result.y;
			Resu->w=Result.w;
			Resu->h=Result.h;//����/���ؽ��ֵ
			flag=1;

			Area.X_Start= Result.x - ((Result.w)<<1);
			Area.X_End  = Result.x + ((Result.w)<<1);
			Area.Y_Start= Result.y - ((Result.h)<<1);
			Area.Y_End  = Result.y + ((Result.h)<<1); //��С�´�������ʴ����ͼ��Χ
//			Area.X_Start= Result.x - ((Result.w)>>1);
//			Area.X_End  = Result.x + ((Result.w)>>1);
//			Area.Y_Start= Result.y - ((Result.h)>>1);
//			Area.Y_End  = Result.y + ((Result.h)>>1); //��С�´�������ʴ����ͼ��Χ

			return 1;
		}
	else//�����ʴʧ�ܣ���ô��־λflag��0������ʧ��ֵ0
		{
			flag=0;
			return 0;
		}

}

