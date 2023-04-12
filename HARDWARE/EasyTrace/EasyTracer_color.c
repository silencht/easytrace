#include "EasyTracer_color.h"
#include "LCD.h"
#define min3v(v1, v2, v3)   ((v1)>(v2)? ((v2)>(v3)?(v3):(v2)):((v1)>(v3)?(v3):(v1)))//取最大值
#define max3v(v1, v2, v3)   ((v1)<(v2)? ((v2)<(v3)?(v3):(v2)):((v1)<(v3)?(v3):(v1)))//取最小值

/*  
	①数据类型备注（STM32F4）：
		typedef unsigned          char uint8_t;  u8
		typedef unsigned short     int uint16_t; u16 (int可省)为16位数据类型，占两个字节，范围为0~65535
		typedef unsigned           int uint32_t; u32
		typedef unsigned       __INT64 uint64_t;
		
	②static类型函数：static函数与普通函数的区别：
　　	用static修饰的函数，限定在本源码文件中，不能被本源码文件以外的代码文件调用。而普通的函数，默认是extern的，即可以被其它代码文件调用该函数。
　　	在函数的返回类型前加上关键字static，函数就被定义成为静态函数。普通函数的定义和声明默认情况下是extern的，但静态函数只是在声明他的文件当中可见，
			不能被其他文件所用。因此定义静态函数有以下优点：
　　<1> 其他文件中可以定义相同名字的函数，不会发生冲突。
　　<2> 静态函数不能被其他文件所用。
  ③const通常修饰常量类型，其变量/对象的值不能被改变。作用有以下几点：
		<1> 修饰对象具有不可变性，便于进行类型检查，对象被修改时编译器可查错，增强了程序的健壮性
　　<2> 节省空间，避免不必要的内存分配，提高效率
  ④RGB和HSL（也叫HSB/HSV）是两种色彩空间，即：红,绿,蓝（Red,Green,Blue）和色调,饱和度,亮度（Hue,Saturation,Lightness或Brightness或Value），
	  RGB适用于机器采样，目前的显示器颜色即由这三种基色构成，而HSL更符合人类的直观感觉，
		比如人一般表达一个颜色会这样说：有点浓的暗红色，而不会说红色占多少，绿色占多少，蓝色占多少;
		<1>亮度仅与图像的最多颜色成分和最少的颜色成分的总量有关。亮度越高，图像越趋于明亮的白色
		<2>饱和度与图像的最多颜色成分和最少的颜色成分的差量有关。饱和度越小，图像越趋于灰度图像。饱和度越大，图像越鲜艳，给人的感觉是彩色的
		<3>色调决定了人对图像的不同的颜色感受。 
*/

//RGB颜色格式结构体
typedef struct
	  {
    unsigned char  red;             // [0,255]
    unsigned char  green;           // [0,255]
    unsigned char  blue;            // [0,255]
    }COLOR_RGB;//RGB格式颜色
		
//色相(H)、饱和度(S)、明度(L)颜色格式结构体
typedef struct
	  {
    unsigned char hue;              // [0,240] 色调
    unsigned char saturation;       // [0,240] 饱和度
    unsigned char luminance;        // [0,240] 亮度
    }COLOR_HSL;//HSL格式颜色

//搜索图像窗口区域，图像X/Y起始坐标
typedef struct
	  {
    unsigned int X_Start;              
    unsigned int X_End;
	  unsigned int Y_Start;              
    unsigned int Y_End;
    }SEARCH_AREA;//区域

//读取RBG格式颜色，唯一需要移植的函数
extern unsigned short LCD_ReadPoint(unsigned short x,unsigned short y);//读某点颜色

/***************************************************************************
	*函数名称：ReadColor
	*函数功能：读LCD屏幕某点颜色将之转化为 RGB格式 并存储在 RGB颜色格式结构体 中
	*入口参数：x,y要读取颜色的LCD屏幕坐标，COLOR_RGB，存放R、G、B值的结构体
	*返回参数：无               
****************************************************************************/
static void ReadColor(unsigned int x,unsigned int y,COLOR_RGB *Rgb)
	{
		unsigned short C16; //即Color16，R5+G6+B5=16

		C16 = LCD_ReadPoint(x,y);     //读某点颜色

		Rgb->red   =	 (unsigned char)((C16&0xf800)>>8);
		Rgb->green =	 (unsigned char)((C16&0x07e0)>>3);
		Rgb->blue  =   (unsigned char)((C16&0x001f)<<3);
	}
/***************************************************************************
	*函数名称：RGBtoHSL
	*函数功能：颜色格式转换：RGB转HSL
	*入口参数：COLOR_RGB：要转换的RGB结构体地址		COLOR_HSL：转换完HSL，存储到的HSL结构体地址
	*返回参数：无               
****************************************************************************/
static void RGBtoHSL(const COLOR_RGB *Rgb, COLOR_HSL *Hsl)
	{
			int h,s,l,maxVal,minVal,difVal;
			int r  = Rgb->red;
			int g  = Rgb->green;
			int b  = Rgb->blue;   //提取R、G、B值
		
				maxVal = max3v(r, g, b);
				minVal = min3v(r, g, b);//提取RGB三者最大/最小值
				
				difVal = maxVal-minVal; //计算最大最小值的差值

		//计算亮度   
			l = (maxVal+minVal)*240/255/2;
		
		if(maxVal == minVal)//若r=g=b
			{
				h = 0; 
				s = 0;
			}
		else
		{
			//计算色调
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
			//计算饱和度
			if(l == 0)
				//	s = 0;
			   s=(difVal)*240/(511 - (maxVal+minVal));
			else if(l<=120)
					s = (difVal)*240/(maxVal+minVal);
			else
					s = (difVal)*240/(511 - (maxVal+minVal));
		}
			Hsl->hue =        (unsigned char)(((h>240)? 240 : ((h<0)?0:h)));//色度
			Hsl->saturation = (unsigned char)(((s>240)? 240 : ((s<0)?0:s)));//饱和度
			Hsl->luminance =  (unsigned char)(((l>240)? 240 : ((l<0)?0:l)));//亮度
	}
/***************************************************************************
	*函数名称：ColorMatch
	*函数功能：颜色匹配
	*入口参数：COLOR_HSL：要进行匹配的HSL	TARGET_CONDI：匹配（判定的）目标条件
	*返回参数：成功返回1，失败返回0              
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
			)          //hue为色调，saturation为饱和度 ，luminance为亮度
				return 1;
		else
				return 0;
	}

/***************************************************************************
	*函数名称：SearchCentre
	*函数功能：条件、颜色匹配，搜索目标物体的大概坐标
	*入口参数：*x、*y为搜索到目标后存储目标坐标的指针地址，TARGET_CONDI为要判定的目标条件、SEARCH_AREA为要进行搜索的区域
	*返回参数：成功返回1，失败返回0              
****************************************************************************/
static int SearchCentre(unsigned int *x,unsigned int *y,const TARGET_CONDI *Condition,const SEARCH_AREA *Area)
	{
			unsigned int SpaceX,SpaceY,i,j,k,FailCount=0;
			COLOR_RGB Rgb;
			COLOR_HSL Hsl;
			
			SpaceX = Condition->WIDTH_MIN/3;   //目标最小宽度
			SpaceY = Condition->HIGHT_MIN/3;   //目标最小高度
			//分别取目标最小高、宽度的三分之一为横纵轴移动步长，移动中以移动到的坐标为起始，向其右下长方形【长宽分别为横纵轴步长】区域取十字型阵列点做分析
				for(i=Area->Y_Start;i<Area->Y_End;i+=SpaceY)//i等于纵轴起始点，i小于纵轴结束点，i每次自加目标最小高度的1/3
				{
					for(j=Area->X_Start;j<Area->X_End;j+=SpaceX)//j等于横轴起始点,j小于横轴结束点，j每次自加目标最小宽度1/3
					{
						FailCount=0; //失败次数初始化
						for(k=0;k<SpaceX+SpaceY;k++)//k小于目标最小宽度与目标最小高度和  
						{
							if(k<SpaceX)//如果k小于目标最小宽度
								ReadColor(j+k,i+SpaceY/2,&Rgb);//读点j+k,i+SpaceY/2的颜色
							else//否则
								ReadColor(j+SpaceX/2,i+(k-SpaceX),&Rgb);//读点j+SpaceX/2,i+(k-SpaceX)的颜色
							
							RGBtoHSL(&Rgb,&Hsl);//读完点后，将RGB转HSL
							if(!ColorMatch(&Hsl,Condition))//进行颜色匹配，如果颜色匹配不成功
							FailCount++;	//失败计数	++		
							if(FailCount>((SpaceX+SpaceY)>>ALLOW_FAIL_PER))//如果失败次数大于容错率
								break;                                       //结束该长方形区域的搜索循环，移动到下一个区域
						}
							   
						if(k==SpaceX+SpaceY)//如果某次区域匹配成功，那么k必等于SpaceX+SpaceY，将该区域的中心点赋值给存储坐标的指针地址，并返回成功值1
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
	*函数名称：Corrode
	*函数功能：从腐蚀中心向外腐蚀，得到新的腐蚀中心
	*入口参数：oldx、oldy为上次搜索的目标中心地址，TARGET_CONDI为要判定的目标条件、RESULT为要存储的结果值结构体
	*返回参数：成功返回1，失败返回0  
	*备注：((Condition->WIDTH_MIN+Condition->WIDTH_MAX)>>2),最大最小和的平均，减小误差除以4
****************************************************************************/	
static int Corrode(unsigned int oldx,unsigned int oldy,const TARGET_CONDI *Condition,RESULT *Resu)
{
	unsigned int Xmin,Xmax,Ymin,Ymax,i,FailCount=0;
	COLOR_RGB Rgb;
	COLOR_HSL Hsl;
	
	for(i=oldx;i>IMG_X;i--) //左腐蚀，y不变
		{
				ReadColor(i,oldy,&Rgb);//读点颜色
				RGBtoHSL(&Rgb,&Hsl); //RGB转换HSL
				if(!ColorMatch(&Hsl,Condition))//进行颜色匹配
					FailCount++;//错误次数自增
				if(FailCount>(((Condition->WIDTH_MIN+Condition->WIDTH_MAX)>>2)>>ALLOW_FAIL_PER))
					break;	
		}
	Xmin=i; //更新X轴最小坐标值
	FailCount=0;//清空错误次数
	
	for(i=oldx;i<IMG_X+IMG_W;i++)//右腐蚀，y不变
		{
				ReadColor(i,oldy,&Rgb);
				RGBtoHSL(&Rgb,&Hsl);
				if(!ColorMatch(&Hsl,Condition))
					FailCount++;
				if(FailCount>(((Condition->WIDTH_MIN+Condition->WIDTH_MAX)>>2)>>ALLOW_FAIL_PER))
					break;	
		}
	Xmax=i;//更新X轴最大坐标值
	FailCount=0;//清空错误次数
	
	for(i=oldy;i>IMG_Y;i--)//下腐蚀，x不变
		{
				ReadColor(oldx,i,&Rgb);
				RGBtoHSL(&Rgb,&Hsl);
				if(!ColorMatch(&Hsl,Condition))
					FailCount++;
				if(FailCount>(((Condition->HIGHT_MIN+Condition->HIGHT_MAX)>>2)>>ALLOW_FAIL_PER))
					break;	
		}
	Ymin=i;//更新Y轴最小坐标值
	FailCount=0;//清空错误次数
	
	for(i=oldy;i<IMG_Y+IMG_H;i++)//上腐蚀，x不变
		{
				ReadColor(oldx,i,&Rgb);
				RGBtoHSL(&Rgb,&Hsl);
				if(!ColorMatch(&Hsl,Condition))
					FailCount++;
				if(FailCount>(((Condition->HIGHT_MIN+Condition->HIGHT_MAX)>>2)>>ALLOW_FAIL_PER))
					break;	
		}
	Ymax=i;//更新Y轴最大坐标值
	FailCount=0;//清空错误次数
	//更新物体准确质点坐标
	Resu->x	= (Xmin+Xmax)/2;
	Resu->y	= (Ymin+Ymax)/2;
	Resu->w	= Xmax-Xmin;
	Resu->h	= Ymax-Ymin;
	//查看物体宽高度是否符合
	if(
			 ((Xmax-Xmin)>(Condition->WIDTH_MIN)) && ((Ymax-Ymin)>(Condition->HIGHT_MIN)) &&\
			 ((Xmax-Xmin)<(Condition->WIDTH_MAX)) && ((Ymax-Ymin)<(Condition->HIGHT_MAX))
	   )
		  return 1;	
	else
		  return 0;	
}
/***************************************************************************
	*函数名称：Trace
	*函数功能：用户将识别条件写入Condition指向的结构体中，该函数将返回目标的x，y坐标和长宽
	*入口参数：识别条件：Condition指向的结构体；识别条件写入Condition指向的结构体
	*返回参数：返回1识别成功，返回0识别失败
***************************************************************************/

int Trace(const TARGET_CONDI *Condition,RESULT *Resu)
{
	unsigned int i;
	static unsigned int x0,y0,flag=0;  //静态变量
	static SEARCH_AREA Area={IMG_X,IMG_X+IMG_W,IMG_Y,IMG_Y+IMG_H};  //要进行搜索的图像区域：x坐标 y坐标  w宽度  h高度
	RESULT Result;	//RESULT识别结果
	
	if(flag==0)    //如果首次使用或上一次腐蚀失败
		{
			if(SearchCentre(&x0,&y0,Condition,&Area))  //搜索腐蚀中心并返回给x0，y0，如果成功搜索到，那么flag置1
				 flag=1;  
			else  //如果还没腐蚀成功，那么把腐蚀区域再次扩大到整个图像范围内进行腐蚀
				{
						Area.X_Start= IMG_X	       ;
						Area.X_End  = IMG_X+IMG_W  ;
						Area.Y_Start= IMG_Y		     ;
						Area.Y_End  = IMG_Y+IMG_H  ;

						if(SearchCentre(&x0,&y0,Condition,&Area))	//如果整个范围腐蚀成功，那么flag置1
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
	Result.x = x0;//如果flag!=0，说明上一次有腐蚀中心结果，所以直接使用上一次结果腐蚀即可，而不需要再次遍历图像搜索腐蚀中心
	Result.y = y0;//上一次的腐蚀中心赋值给这次的oldx，oldy
	
	for(i=0;i<ITERATE_NUM;i++)  //进行腐蚀迭代计算
		 Corrode(Result.x,Result.y,Condition,&Result);
		
	if(Corrode(Result.x,Result.y,Condition,&Result))//从腐蚀中心向外腐蚀成功
		{
			x0=Result.x;
			y0=Result.y; //更新腐蚀中心，以便下次使用
			Resu->x=Result.x;
			Resu->y=Result.y;
			Resu->w=Result.w;
			Resu->h=Result.h;//更新/返回结果值
			flag=1;

			Area.X_Start= Result.x - ((Result.w)<<1);
			Area.X_End  = Result.x + ((Result.w)<<1);
			Area.Y_Start= Result.y - ((Result.h)<<1);
			Area.Y_End  = Result.y + ((Result.h)<<1); //缩小下次搜索腐蚀中心图像范围
//			Area.X_Start= Result.x - ((Result.w)>>1);
//			Area.X_End  = Result.x + ((Result.w)>>1);
//			Area.Y_Start= Result.y - ((Result.h)>>1);
//			Area.Y_End  = Result.y + ((Result.h)>>1); //缩小下次搜索腐蚀中心图像范围

			return 1;
		}
	else//如果腐蚀失败，那么标志位flag置0，返回失败值0
		{
			flag=0;
			return 0;
		}

}

