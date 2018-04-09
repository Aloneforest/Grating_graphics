#include <iostream>
#include <algorithm>
#include <stack>
#include <cmath>
#include <vector>
#include "graphics.h"
#define eps 1e-2
using namespace std;

color_t white = RGB(0xff, 0xff, 0xff);
color_t blue = RGB(200, 200, 200);
int POINTNUM = 7;  

typedef struct{  
    float x;  
    float y;  
}point;//二维坐标 

typedef struct{
	float x, y, z;
}point3d;//三维坐标 

typedef struct{
	point3d pt[3];
	color_t co;
}noodles;//面 

void Parameter(noodles p, float *A, float *B, float *C, float *D)//平面参数计算 
{
	*A = p.pt[0].y*(p.pt[1].z-p.pt[2].z)+p.pt[1].y*(p.pt[2].z-p.pt[0].z)+p.pt[2].y*(p.pt[0].z-p.pt[1].z);
	*B = p.pt[0].z*(p.pt[1].x-p.pt[2].x)+p.pt[1].z*(p.pt[2].x-p.pt[0].x)+p.pt[2].z*(p.pt[0].x-p.pt[1].x);
	*C = p.pt[0].x*(p.pt[1].y-p.pt[2].y)+p.pt[1].x*(p.pt[2].y-p.pt[0].y)+p.pt[2].x*(p.pt[0].y-p.pt[1].y);
	*D = -p.pt[0].x*(p.pt[1].y*p.pt[2].z-p.pt[2].y*p.pt[1].z)
		 -p.pt[1].x*(p.pt[2].y*p.pt[0].z-p.pt[0].y*p.pt[2].z)
		 -p.pt[2].x*(p.pt[0].y*p.pt[1].z-p.pt[1].y*p.pt[0].z);
}

bool InNoodles(noodles no, point p)//判断点是否在多边形投影面内，使用向量叉乘的方法
//当点在多边形投影面内，法向量同向，当点在多边形投影面外，法向量有一个与另外两个符号相反 
{
	float a = (no.pt[0].y-p.x)*(no.pt[1].z-no.pt[0].z) - (no.pt[0].z-p.y)*(no.pt[1].y-no.pt[0].y);
	float b = (no.pt[1].y-p.x)*(no.pt[2].z-no.pt[1].z) - (no.pt[1].z-p.y)*(no.pt[2].y-no.pt[1].y);
	float c = (no.pt[2].y-p.x)*(no.pt[0].z-no.pt[2].z) - (no.pt[2].z-p.y)*(no.pt[0].y-no.pt[2].y);
	if(a*b<0 || b*c<0 || a*c<0)
		return false;
	return true;
}

int Quadrant(point q)//象限判断 
{
	if(q.x>=0 && q.y>=0)
		return 1;
	if(q.x<0 && q.y>=0)
		return 2;
	if(q.x<0 && q.y<0)
		return 3;
	if(q.x>=0 && q.y<0)
		return 4;
}

int radian(int ra1, int ra2, int x)//根据象限变换，获得弧长变换 
{
	if(ra1-ra2==0)
		return 0;
	else if(ra1-ra2==-1 || ra1-ra2==3)
		return 1;
	else if(ra1-ra2==1 || ra1-ra2==-3)
		return -1;
	else
	{
		if(ra1-ra2==-2 && x<0 || ra1-ra2==2 && x>0)
			return 2;
		else 
			return -2;
	}
} 

bool InNoodles1(noodles no, point p)//判断点是否在多边形投影面内，使用弧长累加的方法
{
	int qua[3];
	for(int i=0; i<3; i++)//计算象限 
		qua[i] = Quadrant((point){no.pt[i].y-p.x,no.pt[i].z-p.y});
	int re=0, k=qua[2];
	point pk = (point){no.pt[2].y,no.pt[2].z};
	for(int i=0; i<3; i++)//遍历每一条边 
	{
		point pt = (point){no.pt[i].y,no.pt[i].z};
		int x = pk.x-(pk.y-p.y)*(pk.x-pt.x)/(pk.y-pt.y);//当象限跨度等于2时，需要判断顺逆时针 
		if(x-p.x==0 && 
			( ((p.x>=pk.x&&p.x<=pt.x)||(p.x<=pk.x&&p.x>=pt.x))
			&&((p.y>=pk.y&&p.y<=pt.y)||(p.y>=pk.y&&p.y<=pt.y))))//当当前像素在某条边上，则该像素一定在投影内 
			return true;
		re += radian(k, qua[i], x-p.x);//象限累加 
		k = qua[i];
		pk = pt;
	}	
	if(abs(re) == 4)//当累加和绝对值为4时，证明该像素在多边形内部 
		return true;
	else
		return false;
}

void Z_Buffur(noodles *no, int num)
{
	for(int i=0; i<1000; i++)
	{
		for(int j=0; j<500; j++)
		{
			int zb = -9999999; //当前像素深度 
			for(int k=0; k<num; k++)
			{
//				if(InNoodles(no[k], (point){i, j}))//向量法 
				if(InNoodles1(no[k], (point){i, j}))//判断点是否在多边形投影面内，弧长法 
				{
					float A, B, C, D; 
					Parameter(no[k], &A, &B, &C, &D);
					float dep = -(B*i+C*j+D)/A;//计算深度 
					if(dep > zb) //若深度为最大的，改变当前点的颜色 
					{
						zb = dep;
						putpixel(i, j, no[k].co);
					}
				}
			}			
		}	
	} 
}


//typedef struct XPT{
//	float a, b, c, d;
//	float vmax;
//	int PI;
//	color_t color;
//	XPT *next;
//}APT;
//
//typedef struct XEPT{
//	float a, b, c, d;
//	color_t color;
//	float vmax;
//	int PI;
//	XPT *next;
//}AEPT;
//
//void intescan(noodles *nood)
//{
//	float MaxZ = 0;
//	int i;
//	for(i=0; i<NOODNUM; i++)
//		for(j=0; j<3; j++)
//			MaxZ = max(MaxZ, nood[i].pt[j].z);
//	
//	APT *pAPT = new APT;
//	pAPT->next = NULL;			
//	
//	APT *pAPT[1024];
//	for(int i=0; i<=MaxZ; i++) //构造新多边形表  
//	{
//		pAPT[i] = new APT;
//		pAPT[i]->next = NULL;
//	}
//	
//	for(int i=0; i<MaxZ; i++) //添加新多边形表 
//	{
//		for(int j=0; j<NOODNUM; j++)
//		{
//			for(int k=0; k<3; k++)
//			{
//				if(nood[j][k].z == i)
//				{
//					addpAPT(i, j-1, j, nood, polypoint);
//					addpAPT(i, j+1, j, nood, polypoint);
//				}
//			}
//		}
//	}
//}


//typedef struct{
//	unsigned xmin, xmax, ymin, ymax;
//}Rectangles;//区域 
//
//stack<Rectangles> st; 
//
//void Warnock(noodles *no, int num)
//{
//	vector<Rectangles> ve;
//	for(int i=0; i<num; i++)
//	{
//		float xmin=2000, xmax=-1, ymin=1000, ymax=-1;
//		for(int j=0; j<3; j++)
//		{
//			xmin = min(xmin, no[i].pt[j].x);
//			xmax = max(xmax, no[i].pt[j].x);
//			ymin = min(ymin, no[i].pt[j].y);
//			ymax = max(ymax, no[i].pt[j].y);
//		}
//		ve.push_back((Rectangles){xmin, xmax, ymin, ymax});
//	}
//	
//	st.push((Rectangles){0,1000,0,500});
//	while(!st.empty())
//	{
//		Rectangles re = st.top();
//		st.pop();
//		for(int i=0; i<num; i++)
//		{
//			if(re.xmax<ve[i].xmin || re.xmin>ve[i].xmax || re.ymax<ve[i].ymin || re.ymin>ve[i].ymax)
//			{
//				continue;
//			}
//			else if(re.xmax>ve[i].xmax || re.xmin<ve[i].xmin || re.ymax>ve[i].ymax || re.ymin<ve[i].ymin)
//			{
//				
//			}
//		}
//	}
//} 

int main(int argc, char** argv) 
{
	initgraph(1100, 500);
	
    setcolor(RGB(0xff, 0xff, 0xff) );  
    setfillcolor(RGB(0, 0, 0xff) );
    setrendermode(RENDER_MANUAL);
    
    noodles no[2];//面集合 ，此次默认每个面都为三角形 
    no[0].pt[0] = (point3d){50,100,100};
    no[0].pt[1] = (point3d){50,200,200};
    no[0].pt[2] = (point3d){50,100,200};
    no[0].co = white;
    no[1].pt[0] = (point3d){100,200,100};
    no[1].pt[1] = (point3d){100,50,150};
    no[1].pt[2] = (point3d){20,150,150};
    no[1].co = blue;
    
    Z_Buffur(no, 2);
    	
	getch();
	closegraph();
	return 0;
}
