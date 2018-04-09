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
}point;//��ά���� 

typedef struct{
	float x, y, z;
}point3d;//��ά���� 

typedef struct{
	point3d pt[3];
	color_t co;
}noodles;//�� 

void Parameter(noodles p, float *A, float *B, float *C, float *D)//ƽ��������� 
{
	*A = p.pt[0].y*(p.pt[1].z-p.pt[2].z)+p.pt[1].y*(p.pt[2].z-p.pt[0].z)+p.pt[2].y*(p.pt[0].z-p.pt[1].z);
	*B = p.pt[0].z*(p.pt[1].x-p.pt[2].x)+p.pt[1].z*(p.pt[2].x-p.pt[0].x)+p.pt[2].z*(p.pt[0].x-p.pt[1].x);
	*C = p.pt[0].x*(p.pt[1].y-p.pt[2].y)+p.pt[1].x*(p.pt[2].y-p.pt[0].y)+p.pt[2].x*(p.pt[0].y-p.pt[1].y);
	*D = -p.pt[0].x*(p.pt[1].y*p.pt[2].z-p.pt[2].y*p.pt[1].z)
		 -p.pt[1].x*(p.pt[2].y*p.pt[0].z-p.pt[0].y*p.pt[2].z)
		 -p.pt[2].x*(p.pt[0].y*p.pt[1].z-p.pt[1].y*p.pt[0].z);
}

bool InNoodles(noodles no, point p)//�жϵ��Ƿ��ڶ����ͶӰ���ڣ�ʹ��������˵ķ���
//�����ڶ����ͶӰ���ڣ�������ͬ�򣬵����ڶ����ͶӰ���⣬��������һ�����������������෴ 
{
	float a = (no.pt[0].y-p.x)*(no.pt[1].z-no.pt[0].z) - (no.pt[0].z-p.y)*(no.pt[1].y-no.pt[0].y);
	float b = (no.pt[1].y-p.x)*(no.pt[2].z-no.pt[1].z) - (no.pt[1].z-p.y)*(no.pt[2].y-no.pt[1].y);
	float c = (no.pt[2].y-p.x)*(no.pt[0].z-no.pt[2].z) - (no.pt[2].z-p.y)*(no.pt[0].y-no.pt[2].y);
	if(a*b<0 || b*c<0 || a*c<0)
		return false;
	return true;
}

int Quadrant(point q)//�����ж� 
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

int radian(int ra1, int ra2, int x)//�������ޱ任����û����任 
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

bool InNoodles1(noodles no, point p)//�жϵ��Ƿ��ڶ����ͶӰ���ڣ�ʹ�û����ۼӵķ���
{
	int qua[3];
	for(int i=0; i<3; i++)//�������� 
		qua[i] = Quadrant((point){no.pt[i].y-p.x,no.pt[i].z-p.y});
	int re=0, k=qua[2];
	point pk = (point){no.pt[2].y,no.pt[2].z};
	for(int i=0; i<3; i++)//����ÿһ���� 
	{
		point pt = (point){no.pt[i].y,no.pt[i].z};
		int x = pk.x-(pk.y-p.y)*(pk.x-pt.x)/(pk.y-pt.y);//�����޿�ȵ���2ʱ����Ҫ�ж�˳��ʱ�� 
		if(x-p.x==0 && 
			( ((p.x>=pk.x&&p.x<=pt.x)||(p.x<=pk.x&&p.x>=pt.x))
			&&((p.y>=pk.y&&p.y<=pt.y)||(p.y>=pk.y&&p.y<=pt.y))))//����ǰ������ĳ�����ϣ��������һ����ͶӰ�� 
			return true;
		re += radian(k, qua[i], x-p.x);//�����ۼ� 
		k = qua[i];
		pk = pt;
	}	
	if(abs(re) == 4)//���ۼӺ;���ֵΪ4ʱ��֤���������ڶ�����ڲ� 
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
			int zb = -9999999; //��ǰ������� 
			for(int k=0; k<num; k++)
			{
//				if(InNoodles(no[k], (point){i, j}))//������ 
				if(InNoodles1(no[k], (point){i, j}))//�жϵ��Ƿ��ڶ����ͶӰ���ڣ������� 
				{
					float A, B, C, D; 
					Parameter(no[k], &A, &B, &C, &D);
					float dep = -(B*i+C*j+D)/A;//������� 
					if(dep > zb) //�����Ϊ���ģ��ı䵱ǰ�����ɫ 
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
//	for(int i=0; i<=MaxZ; i++) //�����¶���α�  
//	{
//		pAPT[i] = new APT;
//		pAPT[i]->next = NULL;
//	}
//	
//	for(int i=0; i<MaxZ; i++) //����¶���α� 
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
//}Rectangles;//���� 
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
    
    noodles no[2];//�漯�� ���˴�Ĭ��ÿ���涼Ϊ������ 
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
