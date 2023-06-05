#define _CRT_SECURE_NO_WARNINGS 1
#include<iostream>
#include<cstdlib>
#include<ctime>
#include<graphics.h>
#include<vector>
using namespace std;

//**************************************************************************************    
const int WM = 1080;                        
const int HM = 720;					
const int g = 10;							
const double max_v = sqrt(2 * g * (HM - 40));   
const int max_len = 40;					   
const double PI = acos(-1);				
const double K2 = pow(2, 0.5);				  
const int max_n = 5;					
const int fire_len = 50;					
//**************************************************************************class LightLine
class LightLine
{
public:
	LightLine();
	void Draw()const;
	void Move();
	bool stopped()const { return v == 0.00; }
	double GetX()const { return x; }
	double GetY()const { return y; }
private:
	double x;	
	double y;
	double v;	
	int len;		
	clock_t ct = 0;	 
};
LightLine::LightLine()
{
	x = rand() % 560 + 40.0;		
	y = HM;							
	v = (rand() % 20 + 75.0) / 100 * max_v; 
	len = int(v / max_v * max_len);
}
void LightLine::Draw()const
{
	for (int i = y; i < y + len; ++i)
	{
		float hsv_v = 0.2 + 0.8 * (1.0 - (i - y) / len);      
		setfillcolor(HSVtoRGB(0, 1, hsv_v));
		solidcircle(x, i, 1);		
	}
}
void LightLine::Move()
{
	if (v == 0.00)	  
		return;
	if (ct == 0)		
	{
		ct = clock();
		Draw();
		return;
	}
	clock_t t = clock() - ct; 
	ct = clock();		 
	double v0 = v;		
	v -= g * t / 100.0;   
	v = v > 0 ? v : 0; 
	y -= (v + v0) / 2 * t / 100.0;
	len = int(v / max_v * max_len); 
	Draw();
}
//*********************************************************************************class FireCracker
class FireCracker
{
	struct Spark
	{
		double x;       
		double y;
		double vy;    
		double vh;     
		double radian; 
		Spark(double xx, double yy, double vvy, double vvh, double r) :
			x(xx), y(yy), vy(vvy), vh(vvh), radian(r) {}
	};
public:
	FireCracker(double xx, double yy);	
	void Draw();
	void Move();
private:
	vector<Spark> vec;
	clock_t ct = 0;
	float hsv_h = 0;
	float hsv_v = 1;
};

FireCracker::FireCracker(double xx, double yy)
{
	double vvy = ((rand() % 20) / 20 * 0.1 + 0.1) * max_v;	
	double vvh = ((rand() % 20) / 20 * 0.1 + 0.6) * max_v;
	for (double r = 0; r < 2 * PI; r += PI / 12)
		vec.push_back(Spark(xx, yy, vvy, vvh, r));
	hsv_h = rand() % 360 * 1.0;
}
void FireCracker::Draw()
{
	setfillcolor(HSVtoRGB(hsv_h, 1, hsv_v > 1 ? 1 : hsv_v));
	for (auto i = vec.begin(); i != vec.end(); i++)
	{
		if (0 < i->x && i->x < WM && 0 < i->y && i->y < HM)
			solidcircle(int(i->x), int(i->y), 1);
	}
	hsv_v -= 0.05;
}
void FireCracker::Move()
{
	if (ct == 0)
	{
		ct = clock();
		Draw();
		return;
	}
	clock_t t = clock() - ct;
	ct = clock();

	for (auto i = vec.begin(); i != vec.end(); i++)
	{
		double vvy = i->vy;
		i->vy -= t * g / 1000.0;
		double ym = (vvy + i->vy) / 2 * t / 800.0;  
		double vvx = i->vh * cos(i->radian);
		double vvz = i->vh * sin(i->radian) * 0.5;
		double xm = vvx * t / 1000.0;
		double zm = vvz * t / 1000.0;

		xm -= zm / K2;
		ym -= zm / K2;

		i->x += xm;		
		i->y -= ym;
	}
	hsv_v = 1;
}
//***************************************************************************************class Fire
class Fire
{
public:
	Fire(double xx, double yy);
	void Draw();
	void Move();
	bool finish()const { return vec.size() == fire_len; }	
private:
	vector<FireCracker>vec;
};
Fire::Fire(double xx, double yy)
{
	vec.push_back(FireCracker(xx, yy));		
}
void Fire::Draw()
{
	int s = vec.size();
	for (auto i = (s > 20 ? vec.end() - 20 : vec.begin()); i != vec.end(); ++i)
	{			
		i->Draw();
	}
}
void Fire::Move()
{
	FireCracker temp = vec.back();	
	temp.Move();
	vec.push_back(temp);
	Draw();
}
//****************************************************************************************  main
int main()
{
	initgraph(WM, HM);	
	clock_t ct = clock();
	srand(time(0));		

	vector<LightLine> vec1;	
	vec1.push_back(LightLine());	
	vector<Fire>vec2;	

	BeginBatchDraw();	

	while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000))		
	{
		if (clock() - ct > 50)
		{
			cleardevice();	
			ct = clock();

			if (vec1.size() + vec2.size() < max_n && rand() % 100 < 5)
				vec1.push_back(LightLine());	
			for (auto i = vec1.begin(); i != vec1.end(); )
			{
				i->Move();
				if (i->stopped())		
				{
					vec2.push_back(Fire(i->GetX(), i->GetY()));
					i = vec1.erase(i);	
				}
				else
					++i;
			}

			for (auto i = vec2.begin(); i != vec2.end(); )
			{
				i->Move();		
				if (i->finish())	
					i = vec2.erase(i);
				else
					++i;
			}

			FlushBatchDraw();
		}

		Sleep(1);
	}
	EndBatchDraw();

	closegraph();

	return 0;
}