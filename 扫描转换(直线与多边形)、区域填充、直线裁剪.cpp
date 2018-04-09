/*使用方法
选择要进行的项目
直线裁剪：在界面内由鼠标确定两点，在裁剪框内显示裁剪后的直线
多边形扫描：输入多边形的边数，在界面内确定多边形的定点，结果为白色多边形
区域填充：鼠标点击需要填充的白色多边形，白色多边形会变色
多边形裁剪：操作同多边形扫描，结果为被裁剪框裁剪后的白色多边形*/ 
#include <iostream>
#include <algorithm>
#include <stack>
#include <cmath>
#include "graphics.h"
#define eps 1e-2
using namespace std;

color_t white = RGB(0xff, 0xff, 0xff);
color_t blue = RGB(200, 200, 200);
int POINTNUM = 7;  

typedef struct XET{
	float ymax;
	float x;
	float dx;
	XET *next;
}AET,NET;//边表 

typedef struct{  
    float x;  
    float y;  
}point;//点 

typedef struct{
	int y, xleft, xright;
}Span;//区域填充算法中的扫描区间 

typedef struct{
	unsigned all;
	unsigned num;
	unsigned left, right, top, bottom;
}OutCode;//线段裁剪算法中点的编码 

typedef struct{
	unsigned xmin, xmax, ymin, ymax;
}Rectangles;//矩阵边界 


void LineDDA(int x0, int y0, int x1, int y1)
{
	int x;
	float dx, dy, y, m;
	dx = x1-x0;
	dy = y1-y0;
	
	m = dy/dx;
	y = y0;
	for(x=x0; x<=x1; x++)
	{
		putpixel(x, (int)(y+0.5), white); 
		y += m;
	}
} 

void MidPointLine(int x0, int y0, int x1, int y1)
{
	int dx, dy, inceE, inceNE, d, x, y;
	dx = x1 - x0;
	dy = y1 - y0;
	d = dx - 2 * dy;
	
	inceE = -2*dy;
	inceNE = 2*(dx-dy);
	x = x0, y = y0;
	
	putpixel(x, y, white); 
	while(x < x1)
	{
		if(d > 0)
		 	d += inceE;
		else
		{
			d += inceNE;
			y++;
		}
		x++;
		putpixel(x, y, white);
	}
}

void BresnhamLine(point p0, point p1)//起点，终点 
{
	if(p1.x < p0.x)
	{
		point t = p1;
		p1 = p0;
		p0 = t;
	}
	
	int dx, dy, x, y, e;
	dx = p1.x - p0.x;
	dy = p1.y - p0.y;
	x = p0.x, y = p0.y;
	
	if(dx == 0)//当直线与y轴平行 
	{
		for(int i=min(p0.y, p1.y); i<max(p0.y, p1.y); i++)
			putpixel(x, i, white); 
		return;
	}
	
	putpixel(x, y, white); //起点 
	
	if(abs(dx)>abs(dy))//当斜率绝对值小于1 
	{
		if(dy>0)	e = -dx;//斜率大于0 
		else	e = dx;//斜率小于0
		while(x < p1.x)
		{
			if(dy>0)
			{
				e += 2*dy;
				if(e>0) e -= 2*dx, y++;	
			}
			else
			{
				e += 2*dy;
				if(e<0) e += 2*dx, y--;	
			}
			x++;
			putpixel(x, y, white);
		}
	}
	else//当斜率绝对值大于1 
	{
		if(p1.y < p0.y)
		{
			point t = p1;
			p1 = p0;
			p0 = t;
		}
		dx = p1.x - p0.x;
		dy = p1.y - p0.y;
		x = p0.x, y = p0.y;
		
		if(dx>0)	e = -dy;//斜率大于0 
		else	e = dy;//斜率小于0
		while(y < p1.y)
		{
			if(dx>0)
			{
				e += 2*dx;
				if(e>0) e -= 2*dy, x++;	
			}
			else
			{
				e += 2*dx;
				if(e<0) e += 2*dy, x--;	
			}
			y++;
			putpixel(x, y, white);
		}
	}
}



void addpNET(int x, int i, int j, NET **pNET, point *polypoint) // 当前扫描线，线段两端点标号，新边表，多边形定点集合 
{
	point k = polypoint[(i+POINTNUM)%POINTNUM];// 点循环 
	if(k.y > polypoint[j].y) // 另一段端点大于当前端点时加入 
	{
		NET *p = new NET;
		p->x = polypoint[j].x;
		p->ymax = k.y;
		p->dx = (k.x - polypoint[j].x)/(k.y - polypoint[j].y);
		p->next = pNET[x]->next;
		pNET[x]->next = p;
	}
}

void lsort(AET *p, AET *q, AET *pAET)//活性边表排序 
{
	while(p)
	{
		while(q->next && p->x >= q->next->x)
			q = q->next;
		NET *s = p->next;
		p->next = q->next;
		q->next = p;
		p = s;
		q = pAET;
	}
}

void polyscan(point *polypoint)// 多边形定点集合 
{
	float MaxY = 0;// 扫描线最大值 
	int i;
	for(i=0; i<POINTNUM; i++)
		MaxY = max(MaxY, polypoint[i].y);
	
	AET *pAET = new AET;//边的分类表 
	pAET->next = NULL;			
	
	NET *pNET[1024];//活化边表 
	for(int i=0; i<=MaxY; i++) //构造新边表结构 
	{
		pNET[i] = new NET;
		pNET[i]->next = NULL;
	}
	
	for(int i=0; i<MaxY; i++) //添加活化边表
	{
		for(int j=0; j<POINTNUM; j++)//扫描每个点 
		{
			if(polypoint[j].y == i)// 当扫描线到达该点时，将过该点的两条变加入活化边表
			{
				addpNET(i, j-1, j, pNET, polypoint);
				addpNET(i, j+1, j, pNET, polypoint);
			}
		}
	}
	
	for(i=0; i<=MaxY; i++)
	{
		AET *p = pAET->next;
		AET *q = pAET;
		
		while(p)//更新活性边表 
		{
			p->x = p->x + p->dx; 
			p = p->next;
		}
		
		p = pAET->next;
		q = pAET;
		q->next = NULL;
		lsort(p, q, pAET);//活性边表排序 
		
		p = pAET->next;
		q = pAET;
		while(p)//去除非活性边 
		{  
            if(p->ymax == i)
			{  
                q->next = p->next;  
                delete p;  
                p = q->next;  
            }  
            else
			{  
                q = q->next;  
                p = p->next; 
            }  
        }  
        
        p = pNET[i]->next;
        q = pAET;  
		lsort(p, q, pAET);//添加活性边表并排序 
		
        p = pAET->next;  
        while(p && p->next)//输出此行像素 
		{  
            for(int j=p->x; j<=p->next->x; j++)
				putpixel(j, i, white);  
            p = p->next->next;  
        }  
	}
}

stack<Span> st;

void polyline(int y, Span span, color_t oldcolor, color_t newcolor)//当前扫描线，当前扫描区间，旧颜色，新颜色 
{
	int xleft, xright;
	int i=span.xleft-1;
	bool isleftendset, spanneedfill;
	
	xright=span.xright;
	isleftendset=false;
	while(getpixel(i, y)==oldcolor)//填充当前点左边的点 
	{
		putpixel(i, y, newcolor);
		i--;
	}
	if(i!=span.xleft-1)//更新当前区间左边界 
	{
		isleftendset=true;
		xleft=i+1;
	}
	
	i=span.xleft;
	while(i<xright)
	{
		spanneedfill=false;
		while(getpixel(i,y)==oldcolor)//若遇到新区间，继续填充 
		{
			if(!spanneedfill)
			{
				spanneedfill = true;
				if(!isleftendset)
				{
					isleftendset=true;
					xleft=i;
				}
			}
			putpixel(i,y,newcolor);
			i++;
		}
		if(spanneedfill)//将新区间入栈 
		{
			st.push((Span){y, xleft, i-1});
			isleftendset=false;
			spanneedfill=false;
		}
		i++;
	}
}

void scanline(int x, int y, color_t oldcolor, color_t newcolor)//初始点横纵坐标，旧颜色，新颜色 
{
	int xleft, xright, i;
	Span span;

	i=x;
	while(getpixel(i, y)==oldcolor)//填充当前点右边的点 
	{
		putpixel(i, y, newcolor);
		i++;
	}
	span.xright=i-1;
	i=x-1;
	while(getpixel(i, y)==oldcolor)//填充当前点左边的点 
	{
		putpixel(i, y, newcolor);
		i--;
	}
	span.xleft=i+1;  
	span.y = y;
	
	st.push(span);
	while(!st.empty())
	{
		span = st.top();
		st.pop();
		polyline(span.y+1, span, oldcolor, newcolor);
		polyline(span.y-1, span, oldcolor, newcolor);
	}
}



void CompOutCode(point p, Rectangles * rect, OutCode * outcode)//点，边界，结果 （计算点的编码） 
{
	outcode->all = 0;//初始化 
	outcode->num = 0;
	
	outcode->top = outcode->bottom = 0;
	if(p.y>(float)rect->ymax)
	{
		outcode->top = 1;
		outcode->all += 1;
		outcode->num += 1<<0;
	}
	else if(p.y<(float)rect->ymin)
	{
		outcode->bottom = 1;
		outcode->all += 1;
		outcode->num += 1<<1;
	}
	
	outcode->right = outcode->left = 0;
	if(p.x>(float)rect->xmax)
	{
		outcode->right = 1;
		outcode->all += 1;
		outcode->num += 1<<2;
	}
	else if(p.x<(float)rect->xmin)
	{
		outcode->left = 1;
		outcode->all += 1;
		outcode->num += 1<<3;
	}
}

void CohenSutherlandLineClip(point p0, point p1, Rectangles * rect)//起点，终点，边界 （线段裁剪算法） 
{
	bool accept, done;
	OutCode outcode0, outcode1;
	OutCode * outcodeout;
	float x, y;
	
	accept = false;
	done = false;
	CompOutCode(p0, rect, &outcode0);//编码计算 
	CompOutCode(p1, rect, &outcode1);
	
	do{
		if((outcode0.num|outcode1.num) == 0)//窗口内 
		{
			accept = true;
			done = true;
		}
		else if((outcode0.num&outcode1.num) != 0)//窗口外 
		{
			done = true;
		}
		else
		{
			if(outcode0.all != 0)//判断那点在窗口外 
				outcodeout = &outcode0;
			else
				outcodeout = &outcode1;
			
			if(outcodeout->left)//线段与窗口左边求交 
			{
				y = p0.y+(p1.y-p0.y)*(rect->xmin-p0.x)/(p1.x-p0.x);
				x = (float)rect->xmin;
			}
			else if(outcodeout->right)//线段与窗口上边求交 
			{
				y = p0.y+(p1.y-p0.y)*(rect->xmax-p0.x)/(p1.x-p0.x);
				x = (float)rect->xmax;
			}
			else if(outcodeout->top)//线段与窗口右边求交 
			{
				x = p0.x+(p1.x-p0.x)*(rect->ymax-p0.y)/(p1.y-p0.y);
				y = (float)rect->ymax;
			}
			else if(outcodeout->bottom)//线段与窗口下边求交 
			{
				x = p0.x+(p1.x-p0.x)*(rect->ymin-p0.y)/(p1.y-p0.y);
				y = (float)rect->ymin;
			}
			
			if(outcodeout->num == outcode0.num)//判断那个点在窗口内，舍弃不可见线段，继续裁剪 
			{
				p0 = (point){x, y};
				CompOutCode(p0, rect, &outcode0);
			}
			else
			{
				p1 = (point){x, y};
				CompOutCode(p1, rect, &outcode1);
			}
		}
	}while(!done);
	
	if(accept)
		line((int)p0.x, (int)p0.y, (int)p1.x, (int)p1.y);
}

void MidLineClip(point p0, point p1, Rectangles * rect)//起点，终点，边界 （中点分割算法）
//该算法在实现上与书上的方法有差异，为了方便实现，使用了递归的方法，不建议这么写 
{
	bool accept, done;
	OutCode outcode0, outcode1, outcodemid;
	OutCode * outcodeout;
	
	accept = false;
	done = false;
	point l=p0, r=p1;
	CompOutCode(p0, rect, &outcode0);
	CompOutCode(p1, rect, &outcode1);
	
	if((outcode0.num|outcode1.num) == 0)
	{
		line((int)p0.x, (int)p0.y, (int)p1.x, (int)p1.y);
		return ;
	}
	else if((outcode0.num&outcode1.num) != 0)
	{
		return ;
	}
	else
	{
		point mid = {(l.x+r.x)/2, (l.y+r.y)/2};
		if(fabs(p0.x-mid.x)>eps || fabs(p0.y-mid.y)>eps)
			MidLineClip(p0, mid, rect); 
		if(fabs(p1.x-mid.x)>eps || fabs(p1.y-mid.y)>eps)
			MidLineClip(mid, p1, rect); 
	}
}

bool ClipT(float q, float d, float *t0, float *t1)//详见书116页 （计算参数方程参数） 
{
	float r;
	if(q<0)
	{
		r = d/q;
		if(r > *t1)
			return false;
		else if(r > *t0)
		{
			*t0 = r;
			return true;
		}
	}
	else  if(q>0)
	{
		r=d/q;
		if(r < *t0)
			return false;
		else if(r < *t1)
		{
			*t1 = r;
			return true;
		}
	}
	else if(d<0)
		return false;
	return true;
}

void LiangBarskyLineClip(point p0, point p1, Rectangles * rect)//起点，终点，边界 （梁算法） 
{
	float deltax, deltay, t0, t1;
	t0 = 0, t1 = 1;
	deltax = p1.x - p0.x;
	if(ClipT(-deltax, p0.x-rect->xmin, &t0, &t1))//计算与左边界交点参数 
		if(ClipT(deltax, rect->xmax-p0.x, &t0, &t1))//计算与右边界交点参数 
			{
				deltay = p1.y - p0.y;
				if(ClipT(-deltay, p0.y-rect->ymin, &t0, &t1))//计算与下边界交点参数  
					if(ClipT(deltay, rect->ymax-p0.y, &t0, &t1))//计算与上边界交点参数  
					{
						BresnhamLine(
							(point){(int)(p0.x+t0*deltax), (int)(p0.y+t0*deltay)},
							(point){(int)(p0.x+t1*deltax), (int)(p0.y+t1*deltay)});//画线 
						return ;
					}
			}
} 





void Intersect(point s, point p, point *line, point *I)//起点，终点，裁剪线，交点 （计算边与裁剪线的交点） 
{
	if(line[0].y == line[1].y)
	{
		I->y = line[0].y;
		I->x = (int)(s.x + (line[0].y - s.y) * (p.x - s.x)/(p.y - s.y));
	}
	else
	{
		I->x = line[0].x;
		I->y = (int)(s.y + (line[0].x - s.x) * (p.y - s.y)/(p.x - s.x));
	}
}

bool Inside(point t, point *line)//端点，裁剪线 （ 判断该点是否在裁剪线内测 ） 
{
	if(line[1].x > line[0].x)//下 
		if(t.y >= line[0].y)
			return true;
	if(line[1].x < line[0].x)//上 
		if(t.y <= line[0].y)
			return true;
	if(line[1].y > line[0].y)//右 
		if(t.x <= line[0].x)
			return true;
	if(line[1].y < line[0].y)//左 
		if(t.x >= line[0].x)
			return true;
	return false;
}

void Output(point newPoint, int outLength, point *OutArray)//新的端点， 裁剪后多边形定点数， 裁剪后多边形 （将裁减后的点加入输出多边形） 
{
	OutArray[outLength].x = newPoint.x;
	OutArray[outLength].y = newPoint.y;
}

int SutherlandHodgmanPloygonClip(int inLength, point *inArray, point *OutArray, point *line) 
//待裁剪多边形定点数，待裁剪多边形，裁剪后多边形，裁剪线 （多边形裁剪算法） 
{
	point s, p, I;
	int j, outLength=0;
	
	s = inArray[inLength-1];
	
	for(int j=0; j<inLength; j++)//遍历每条多边形的边 
	{
		p = inArray[j];
		if(Inside(p, line))//详见书119页 
		{
			if(Inside(s, line)) 
			{
				Output(p, outLength, OutArray);//情况1 
				outLength++;
			}
			else
			{
				Intersect(s, p, line, &I);
				Output(I, outLength, OutArray);//情况4 
				outLength++;
				Output(p, outLength, OutArray);
				outLength++;
			}
		} 
		else if(Inside(s, line))
		{
			Intersect(s, p, line, &I);//情况2 
			Output(I, outLength, OutArray);
			outLength++;
		}
		s = p;
	}
	return outLength;
}

void init()//初始化界面 
{
   	BresnhamLine((point){230,100}, (point){230,400});
   	BresnhamLine((point){230,100}, (point){800,100});
   	BresnhamLine((point){230,400}, (point){800,400});
   	BresnhamLine((point){800,400}, (point){800,100});
   	
	BresnhamLine((point){1000,0}, (point){1000,500});
	BresnhamLine((point){1000,50}, (point){1100,50});
	xyprintf(1010,10,"直线裁剪"); 
	BresnhamLine((point){1000,100}, (point){1100,100});
	xyprintf(1010,60,"多边形扫描"); 
	BresnhamLine((point){1000,150}, (point){1100,150});
	xyprintf(1010,110,"区域填充"); 
	BresnhamLine((point){1000,200}, (point){1100,200});
	xyprintf(1010,160,"多边形裁剪"); 
}

int main(int argc, char** argv) 
{
	initgraph(1100, 500);
	
    setcolor(RGB(0xff, 0xff, 0xff) );  
    setfillcolor(RGB(0, 0, 0xff) );
    setrendermode(RENDER_MANUAL);
    
    init(); //初始化界面 
    Rectangles rect={230,800,100,400};//边界 
    
    point pt[20]={0};//缓冲区 
	int i=0;
    int flag = 0; 
    int num = 0;
    for ( ; is_run(); delay_fps(100))
	{
		mouse_msg m ={0}; 
		int x, y;
		while (mousemsg())//鼠标信息 
		{
			m = getmouse();
			mousepos(&x,&y);
			if(x>=1000&&y>=0&&x<1100&&y<50 &&m.msg == mouse_msg_down)// 直线裁剪
				flag = 1, i=0;
			else if(x>=1000&&y>=50&&x<1100&&y<100 &&m.msg == mouse_msg_down)// 多边形扫描 
				flag = 2, i=0;
			else if(x>=1000&&y>=100&&x<1100&&y<150 &&m.msg == mouse_msg_down)// 区域填充 
				flag = 3, i=0;
			else if(x>=1000&&y>=150&&x<1100&&y<200 &&m.msg == mouse_msg_down)// 多边形裁剪 
				flag = 4, i=0;
				 
			
			if(flag == 1)
			{
				if(x>=0&&y>=0&&x<1000&&y<500 &&m.msg == mouse_msg_down)
				{
					pt[i++]=(point){x,y};
					if(i==2)
					{
						i=0;
						LiangBarskyLineClip(pt[0], pt[1], &rect);
					}
				 	break; 
				}
			}
			else if(flag == 2) 
			{
				if(num == 0)
				{
					char s[5];
					inputbox_getline("","",s,5);//弹框 
					num = atoi(s); 
				}
				if(x>=0&&y>=0&&x<1000&&y<500 &&m.msg == mouse_msg_down)
				{
					pt[i++]=(point){x,y};
					if(i==num)
					{
						i=0;
						POINTNUM = num; //多边形边数 
						polyscan(pt); 
						
						num = 0;
						flag = 0;
					}
				 	break; 
				}
			} 
			else if(flag == 3) 
			{
				if(x>=0&&y>=0&&x<1000&&y<500 &&m.msg == mouse_msg_down)
				{
					pt[i++]=(point){x,y};
					if(i==1)
					{
						i=0;
						scanline(pt[0].x, pt[0].y, white, blue); 
					}
				 	break; 
				}
			} 
			else if(flag == 4) 
			{
				if(num == 0)
				{
					char s[5];
					inputbox_getline("","",s,5);//弹框 
					num = atoi(s); 
				}
				
				if(x>=0&&y>=0&&x<1000&&y<500 &&m.msg == mouse_msg_down)
				{
					pt[i++]=(point){x,y};
					
					int inlen = num;//裁剪后多边形定点数 
					if(i==inlen)
					{
						i=0;
						
						point outps[20] = {0};
						
						point a[2]={rect.xmin,rect.ymin,rect.xmax,rect.ymin};//下边界 
						point b[2]={rect.xmax,rect.ymin,rect.xmax,rect.ymax};//上边界
						point c[2]={rect.xmax,rect.ymax,rect.xmin,rect.ymax};//右边界
						point d[2]={rect.xmin,rect.ymax,rect.xmin,rect.ymin};//左边界
						
						inlen = SutherlandHodgmanPloygonClip(inlen, pt, outps, a);//下 
						memcpy(pt, outps, sizeof(point)*inlen);
						inlen = SutherlandHodgmanPloygonClip(inlen, pt, outps, b);//上 
						memcpy(pt, outps, sizeof(point)*inlen);
						inlen = SutherlandHodgmanPloygonClip(inlen, pt, outps, c);//右 
						memcpy(pt, outps, sizeof(point)*inlen);
						inlen = SutherlandHodgmanPloygonClip(inlen, pt, outps, d);//左 
						
					 	POINTNUM = inlen;
						polyscan(outps);//扫描转换裁剪后的多边形 
						
						num = 0;
						flag = 0;
					}
				 	break; 
				}
			} 
		}
	}
	
	
	
	//LineDDA(50, 50, 50, 100);
	//LineDDA(50, 60, 100, 50);
	//MidPointLine(50, 80, 200, 180);
	
//	BresnhamLine(point{500, 200}, point{100, 100});
//	BresnhamLine(point{120,30}, point{250,50});
	
//	for(int i=0; i<POINTNUM+1; i++)
//		BresnhamLine(polypoint[i], polypoint[i+1]);
	
//	point pt[20]={230,220.35,230,156.64,253,153,324,212,272,248};
//	POINTNUM = 5;
//	polyscan(pt);
 
//	scanline(200, 110, white, blue);

//	Rectangles rect={230,800,100,400};
//	//Rectangles rect={0,1000,0,500};
//	line(230,100,230,400);
//	line(230,100,800,100);
//	line(230,400,800,400);
//	line(800,400,800,100);
//	
//	point ps[8][2]={
//	250,100,700,300,
//	200,150,100,400,
//	250,440,800,440,
//	200,50,300,300,
//	700,250,850,200,
//	500,480,880,360,
//	450,480,1000,390,
//	50,150,300,50
//	};
////	
//	for(int i=0; i<8; i++)
////		CohenSutherlandLineClip(ps[i][0], ps[i][1], &rect);
////		MidLineClip(ps[i][0], ps[i][1], &rect);
//		LiangBarskyLineClip(ps[i][0], ps[i][1], &rect);

//	//point inps[20] = {200,100,100,200,200,300,400,300,500,200,400,100,300,200};
//	point inps[20] = {200,300,300,120,500,300,300,350};
//	int inlen = 4;
//	point outps[20] = {0};
//	
//	point a[2]={rect.xmin,rect.ymin,rect.xmax,rect.ymin};
//	point b[2]={rect.xmax,rect.ymin,rect.xmax,rect.ymax};
//	point c[2]={rect.xmax,rect.ymax,rect.xmin,rect.ymax};
//	point d[2]={rect.xmin,rect.ymax,rect.xmin,rect.ymin};
//	
//	inlen = SutherlandHodgmanPloygonClip(inlen, inps, outps, a);//下 
//	memcpy(inps, outps, sizeof(point)*inlen);
//	inlen = SutherlandHodgmanPloygonClip(inlen, inps, outps, b);//上 
//	memcpy(inps, outps, sizeof(point)*inlen);
//	inlen = SutherlandHodgmanPloygonClip(inlen, inps, outps, c);//右 
//	memcpy(inps, outps, sizeof(point)*inlen);
//	inlen = SutherlandHodgmanPloygonClip(inlen, inps, outps, d);//左 
//	
// 	POINTNUM = inlen;
//	polyscan(outps);
	
	getch();
	closegraph();
	return 0;
}
