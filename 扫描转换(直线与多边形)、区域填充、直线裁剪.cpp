/*ʹ�÷���
ѡ��Ҫ���е���Ŀ
ֱ�߲ü����ڽ����������ȷ�����㣬�ڲü�������ʾ�ü����ֱ��
�����ɨ�裺�������εı������ڽ�����ȷ������εĶ��㣬���Ϊ��ɫ�����
������䣺�������Ҫ���İ�ɫ����Σ���ɫ����λ��ɫ
����βü�������ͬ�����ɨ�裬���Ϊ���ü���ü���İ�ɫ�����*/ 
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
}AET,NET;//�߱� 

typedef struct{  
    float x;  
    float y;  
}point;//�� 

typedef struct{
	int y, xleft, xright;
}Span;//��������㷨�е�ɨ������ 

typedef struct{
	unsigned all;
	unsigned num;
	unsigned left, right, top, bottom;
}OutCode;//�߶βü��㷨�е�ı��� 

typedef struct{
	unsigned xmin, xmax, ymin, ymax;
}Rectangles;//����߽� 


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

void BresnhamLine(point p0, point p1)//��㣬�յ� 
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
	
	if(dx == 0)//��ֱ����y��ƽ�� 
	{
		for(int i=min(p0.y, p1.y); i<max(p0.y, p1.y); i++)
			putpixel(x, i, white); 
		return;
	}
	
	putpixel(x, y, white); //��� 
	
	if(abs(dx)>abs(dy))//��б�ʾ���ֵС��1 
	{
		if(dy>0)	e = -dx;//б�ʴ���0 
		else	e = dx;//б��С��0
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
	else//��б�ʾ���ֵ����1 
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
		
		if(dx>0)	e = -dy;//б�ʴ���0 
		else	e = dy;//б��С��0
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



void addpNET(int x, int i, int j, NET **pNET, point *polypoint) // ��ǰɨ���ߣ��߶����˵��ţ��±߱�����ζ��㼯�� 
{
	point k = polypoint[(i+POINTNUM)%POINTNUM];// ��ѭ�� 
	if(k.y > polypoint[j].y) // ��һ�ζ˵���ڵ�ǰ�˵�ʱ���� 
	{
		NET *p = new NET;
		p->x = polypoint[j].x;
		p->ymax = k.y;
		p->dx = (k.x - polypoint[j].x)/(k.y - polypoint[j].y);
		p->next = pNET[x]->next;
		pNET[x]->next = p;
	}
}

void lsort(AET *p, AET *q, AET *pAET)//���Ա߱����� 
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

void polyscan(point *polypoint)// ����ζ��㼯�� 
{
	float MaxY = 0;// ɨ�������ֵ 
	int i;
	for(i=0; i<POINTNUM; i++)
		MaxY = max(MaxY, polypoint[i].y);
	
	AET *pAET = new AET;//�ߵķ���� 
	pAET->next = NULL;			
	
	NET *pNET[1024];//��߱� 
	for(int i=0; i<=MaxY; i++) //�����±߱�ṹ 
	{
		pNET[i] = new NET;
		pNET[i]->next = NULL;
	}
	
	for(int i=0; i<MaxY; i++) //��ӻ�߱�
	{
		for(int j=0; j<POINTNUM; j++)//ɨ��ÿ���� 
		{
			if(polypoint[j].y == i)// ��ɨ���ߵ���õ�ʱ�������õ������������߱�
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
		
		while(p)//���»��Ա߱� 
		{
			p->x = p->x + p->dx; 
			p = p->next;
		}
		
		p = pAET->next;
		q = pAET;
		q->next = NULL;
		lsort(p, q, pAET);//���Ա߱����� 
		
		p = pAET->next;
		q = pAET;
		while(p)//ȥ���ǻ��Ա� 
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
		lsort(p, q, pAET);//��ӻ��Ա߱����� 
		
        p = pAET->next;  
        while(p && p->next)//����������� 
		{  
            for(int j=p->x; j<=p->next->x; j++)
				putpixel(j, i, white);  
            p = p->next->next;  
        }  
	}
}

stack<Span> st;

void polyline(int y, Span span, color_t oldcolor, color_t newcolor)//��ǰɨ���ߣ���ǰɨ�����䣬����ɫ������ɫ 
{
	int xleft, xright;
	int i=span.xleft-1;
	bool isleftendset, spanneedfill;
	
	xright=span.xright;
	isleftendset=false;
	while(getpixel(i, y)==oldcolor)//��䵱ǰ����ߵĵ� 
	{
		putpixel(i, y, newcolor);
		i--;
	}
	if(i!=span.xleft-1)//���µ�ǰ������߽� 
	{
		isleftendset=true;
		xleft=i+1;
	}
	
	i=span.xleft;
	while(i<xright)
	{
		spanneedfill=false;
		while(getpixel(i,y)==oldcolor)//�����������䣬������� 
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
		if(spanneedfill)//����������ջ 
		{
			st.push((Span){y, xleft, i-1});
			isleftendset=false;
			spanneedfill=false;
		}
		i++;
	}
}

void scanline(int x, int y, color_t oldcolor, color_t newcolor)//��ʼ��������꣬����ɫ������ɫ 
{
	int xleft, xright, i;
	Span span;

	i=x;
	while(getpixel(i, y)==oldcolor)//��䵱ǰ���ұߵĵ� 
	{
		putpixel(i, y, newcolor);
		i++;
	}
	span.xright=i-1;
	i=x-1;
	while(getpixel(i, y)==oldcolor)//��䵱ǰ����ߵĵ� 
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



void CompOutCode(point p, Rectangles * rect, OutCode * outcode)//�㣬�߽磬��� �������ı��룩 
{
	outcode->all = 0;//��ʼ�� 
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

void CohenSutherlandLineClip(point p0, point p1, Rectangles * rect)//��㣬�յ㣬�߽� ���߶βü��㷨�� 
{
	bool accept, done;
	OutCode outcode0, outcode1;
	OutCode * outcodeout;
	float x, y;
	
	accept = false;
	done = false;
	CompOutCode(p0, rect, &outcode0);//������� 
	CompOutCode(p1, rect, &outcode1);
	
	do{
		if((outcode0.num|outcode1.num) == 0)//������ 
		{
			accept = true;
			done = true;
		}
		else if((outcode0.num&outcode1.num) != 0)//������ 
		{
			done = true;
		}
		else
		{
			if(outcode0.all != 0)//�ж��ǵ��ڴ����� 
				outcodeout = &outcode0;
			else
				outcodeout = &outcode1;
			
			if(outcodeout->left)//�߶��봰������� 
			{
				y = p0.y+(p1.y-p0.y)*(rect->xmin-p0.x)/(p1.x-p0.x);
				x = (float)rect->xmin;
			}
			else if(outcodeout->right)//�߶��봰���ϱ��� 
			{
				y = p0.y+(p1.y-p0.y)*(rect->xmax-p0.x)/(p1.x-p0.x);
				x = (float)rect->xmax;
			}
			else if(outcodeout->top)//�߶��봰���ұ��� 
			{
				x = p0.x+(p1.x-p0.x)*(rect->ymax-p0.y)/(p1.y-p0.y);
				y = (float)rect->ymax;
			}
			else if(outcodeout->bottom)//�߶��봰���±��� 
			{
				x = p0.x+(p1.x-p0.x)*(rect->ymin-p0.y)/(p1.y-p0.y);
				y = (float)rect->ymin;
			}
			
			if(outcodeout->num == outcode0.num)//�ж��Ǹ����ڴ����ڣ��������ɼ��߶Σ������ü� 
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

void MidLineClip(point p0, point p1, Rectangles * rect)//��㣬�յ㣬�߽� ���е�ָ��㷨��
//���㷨��ʵ���������ϵķ����в��죬Ϊ�˷���ʵ�֣�ʹ���˵ݹ�ķ�������������ôд 
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

bool ClipT(float q, float d, float *t0, float *t1)//�����116ҳ ������������̲����� 
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

void LiangBarskyLineClip(point p0, point p1, Rectangles * rect)//��㣬�յ㣬�߽� �����㷨�� 
{
	float deltax, deltay, t0, t1;
	t0 = 0, t1 = 1;
	deltax = p1.x - p0.x;
	if(ClipT(-deltax, p0.x-rect->xmin, &t0, &t1))//��������߽罻����� 
		if(ClipT(deltax, rect->xmax-p0.x, &t0, &t1))//�������ұ߽罻����� 
			{
				deltay = p1.y - p0.y;
				if(ClipT(-deltay, p0.y-rect->ymin, &t0, &t1))//�������±߽罻�����  
					if(ClipT(deltay, rect->ymax-p0.y, &t0, &t1))//�������ϱ߽罻�����  
					{
						BresnhamLine(
							(point){(int)(p0.x+t0*deltax), (int)(p0.y+t0*deltay)},
							(point){(int)(p0.x+t1*deltax), (int)(p0.y+t1*deltay)});//���� 
						return ;
					}
			}
} 





void Intersect(point s, point p, point *line, point *I)//��㣬�յ㣬�ü��ߣ����� ���������ü��ߵĽ��㣩 
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

bool Inside(point t, point *line)//�˵㣬�ü��� �� �жϸõ��Ƿ��ڲü����ڲ� �� 
{
	if(line[1].x > line[0].x)//�� 
		if(t.y >= line[0].y)
			return true;
	if(line[1].x < line[0].x)//�� 
		if(t.y <= line[0].y)
			return true;
	if(line[1].y > line[0].y)//�� 
		if(t.x <= line[0].x)
			return true;
	if(line[1].y < line[0].y)//�� 
		if(t.x >= line[0].x)
			return true;
	return false;
}

void Output(point newPoint, int outLength, point *OutArray)//�µĶ˵㣬 �ü������ζ������� �ü������� �����ü���ĵ�����������Σ� 
{
	OutArray[outLength].x = newPoint.x;
	OutArray[outLength].y = newPoint.y;
}

int SutherlandHodgmanPloygonClip(int inLength, point *inArray, point *OutArray, point *line) 
//���ü�����ζ����������ü�����Σ��ü������Σ��ü��� ������βü��㷨�� 
{
	point s, p, I;
	int j, outLength=0;
	
	s = inArray[inLength-1];
	
	for(int j=0; j<inLength; j++)//����ÿ������εı� 
	{
		p = inArray[j];
		if(Inside(p, line))//�����119ҳ 
		{
			if(Inside(s, line)) 
			{
				Output(p, outLength, OutArray);//���1 
				outLength++;
			}
			else
			{
				Intersect(s, p, line, &I);
				Output(I, outLength, OutArray);//���4 
				outLength++;
				Output(p, outLength, OutArray);
				outLength++;
			}
		} 
		else if(Inside(s, line))
		{
			Intersect(s, p, line, &I);//���2 
			Output(I, outLength, OutArray);
			outLength++;
		}
		s = p;
	}
	return outLength;
}

void init()//��ʼ������ 
{
   	BresnhamLine((point){230,100}, (point){230,400});
   	BresnhamLine((point){230,100}, (point){800,100});
   	BresnhamLine((point){230,400}, (point){800,400});
   	BresnhamLine((point){800,400}, (point){800,100});
   	
	BresnhamLine((point){1000,0}, (point){1000,500});
	BresnhamLine((point){1000,50}, (point){1100,50});
	xyprintf(1010,10,"ֱ�߲ü�"); 
	BresnhamLine((point){1000,100}, (point){1100,100});
	xyprintf(1010,60,"�����ɨ��"); 
	BresnhamLine((point){1000,150}, (point){1100,150});
	xyprintf(1010,110,"�������"); 
	BresnhamLine((point){1000,200}, (point){1100,200});
	xyprintf(1010,160,"����βü�"); 
}

int main(int argc, char** argv) 
{
	initgraph(1100, 500);
	
    setcolor(RGB(0xff, 0xff, 0xff) );  
    setfillcolor(RGB(0, 0, 0xff) );
    setrendermode(RENDER_MANUAL);
    
    init(); //��ʼ������ 
    Rectangles rect={230,800,100,400};//�߽� 
    
    point pt[20]={0};//������ 
	int i=0;
    int flag = 0; 
    int num = 0;
    for ( ; is_run(); delay_fps(100))
	{
		mouse_msg m ={0}; 
		int x, y;
		while (mousemsg())//�����Ϣ 
		{
			m = getmouse();
			mousepos(&x,&y);
			if(x>=1000&&y>=0&&x<1100&&y<50 &&m.msg == mouse_msg_down)// ֱ�߲ü�
				flag = 1, i=0;
			else if(x>=1000&&y>=50&&x<1100&&y<100 &&m.msg == mouse_msg_down)// �����ɨ�� 
				flag = 2, i=0;
			else if(x>=1000&&y>=100&&x<1100&&y<150 &&m.msg == mouse_msg_down)// ������� 
				flag = 3, i=0;
			else if(x>=1000&&y>=150&&x<1100&&y<200 &&m.msg == mouse_msg_down)// ����βü� 
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
					inputbox_getline("","",s,5);//���� 
					num = atoi(s); 
				}
				if(x>=0&&y>=0&&x<1000&&y<500 &&m.msg == mouse_msg_down)
				{
					pt[i++]=(point){x,y};
					if(i==num)
					{
						i=0;
						POINTNUM = num; //����α��� 
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
					inputbox_getline("","",s,5);//���� 
					num = atoi(s); 
				}
				
				if(x>=0&&y>=0&&x<1000&&y<500 &&m.msg == mouse_msg_down)
				{
					pt[i++]=(point){x,y};
					
					int inlen = num;//�ü������ζ����� 
					if(i==inlen)
					{
						i=0;
						
						point outps[20] = {0};
						
						point a[2]={rect.xmin,rect.ymin,rect.xmax,rect.ymin};//�±߽� 
						point b[2]={rect.xmax,rect.ymin,rect.xmax,rect.ymax};//�ϱ߽�
						point c[2]={rect.xmax,rect.ymax,rect.xmin,rect.ymax};//�ұ߽�
						point d[2]={rect.xmin,rect.ymax,rect.xmin,rect.ymin};//��߽�
						
						inlen = SutherlandHodgmanPloygonClip(inlen, pt, outps, a);//�� 
						memcpy(pt, outps, sizeof(point)*inlen);
						inlen = SutherlandHodgmanPloygonClip(inlen, pt, outps, b);//�� 
						memcpy(pt, outps, sizeof(point)*inlen);
						inlen = SutherlandHodgmanPloygonClip(inlen, pt, outps, c);//�� 
						memcpy(pt, outps, sizeof(point)*inlen);
						inlen = SutherlandHodgmanPloygonClip(inlen, pt, outps, d);//�� 
						
					 	POINTNUM = inlen;
						polyscan(outps);//ɨ��ת���ü���Ķ���� 
						
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
//	inlen = SutherlandHodgmanPloygonClip(inlen, inps, outps, a);//�� 
//	memcpy(inps, outps, sizeof(point)*inlen);
//	inlen = SutherlandHodgmanPloygonClip(inlen, inps, outps, b);//�� 
//	memcpy(inps, outps, sizeof(point)*inlen);
//	inlen = SutherlandHodgmanPloygonClip(inlen, inps, outps, c);//�� 
//	memcpy(inps, outps, sizeof(point)*inlen);
//	inlen = SutherlandHodgmanPloygonClip(inlen, inps, outps, d);//�� 
//	
// 	POINTNUM = inlen;
//	polyscan(outps);
	
	getch();
	closegraph();
	return 0;
}
