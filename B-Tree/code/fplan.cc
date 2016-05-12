
#include <fstream>
#include <cstdio>
#include <cstring>
#include <climits>
#include <cassert>
#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

#include "fplan.h"

char line[100],t1[40],t2[40];
ifstream fs;
int TerminalCount;

FPlan::FPlan(double calpha=1){
  norm_area= 1;
  norm_wire= 1;
  cost_alpha=calpha;
}

void FPlan::packing(){
  //if(cost_alpha!=1);
    // calcWireLength();
}

void FPlan::clear(){
  Area = 0; 
  WireLength = 0;
}

double FPlan::getCost()
{
  double ar = getHeight()/getWidth();
  double ar_cost = 0;

  if( cost_alpha == 1 )
      return cost_alpha*(Area/norm_area) + ar_cost;


  return cost_alpha*(Area/norm_area) + (1-cost_alpha)*(WireLength/norm_wire) + k3*ar_cost;



}

double FPlan::getDeadSpace()
{
  return 100*(Area-TotalArea)/double(Area);
}

void FPlan::normalize_cost(int t)
{
  norm_area = norm_wire = 0;

  for(int i=0; i < t; i++)
  {
    perturb();
    packing();
    norm_area += Area;
    norm_wire += WireLength;
  }
  
  norm_area /= t;
  norm_wire /= t;
}



void FPlan::readInput(char *file)
{
	int BlockCount,i,dummy,t=0,s=0,w,h,j;

		char a[50],b,BlockName[6],terminalName[6];
		FILE *fp;
	    //block *root,*temp, *lroot;
		//open File
		fp=fopen(file,"r");
		if(fp==NULL)
		{
			printf("Unable to open file");
			exit(0);
		}
		//read line 1 which has soft rectangular blocks
		fscanf (fp, "%s %c %d",a,&b,&dummy);
		//printf("%s %c %d\n",a,b,dummy);

		//read line 2 which has hard rectilinear blocks
		fscanf(fp, "%s %c %d",a,&b,&BlockCount);
		//printf("%s %c %d\n",a,b,BlockCount);

		//read terminal count
		fscanf(fp, "%s %c %d\n",a,&b,&TerminalCount);
		//printf("%s %c %d\n",a,b,TerminalCount);

		//intialize ptr arry
		Module dummy_mod;
	    //read block names and dimensions
		for(i=0;i<BlockCount;i++)
		{
			fscanf(fp,"%s %s %d ",BlockName,a,&dummy);
			//printf("%s %c %d\n",a,b,dummy);
			modules.push_back(dummy_mod);
			Module &mod = modules.back();
			mod.id = i;
			mod.pins.clear();
			mod.no_rotate = false;

			//fill in block details
			strcpy(mod.name,BlockName);
			//p->vis=0;
			for(j=0;j<dummy;j++)
				{
					fscanf(fp,"%c %d %c %d %c ",&b,&w,&b,&h,&b);
					if(j==dummy-2)
					{
						mod.height=h;
						mod.width=w;
						mod.area=h*w;
					}
				}
				//printf("Block Name:%s\tBlock ID:%d\tBlock Width:%d\tBlock Height:%d\n",mod.name,mod.id,mod.width,mod.height);
		}
  root_module = modules.back();
  modules_N = modules.size();  
  modules_info.resize(modules_N);
  modules.resize(modules_N);


  TotalArea = 0;
  for(int i=0; i < modules_N; i++)
    TotalArea += modules[i].area;


}





/*
string query_map(map<string,int> M,int value){
  for(map<string,int>::iterator p=M.begin(); p != M.end(); p++){
    if(p->second == value)
      return p->first;
  }
  return "";
}

void FPlan::show_modules()
{
  for(int i=0; i < modules.size();i++){
    cout << "Module: " << modules[i].name << endl;
    cout << "  Width = " << modules[i].width;
    cout << "  Height= " << modules[i].height << endl;
    cout << "  Area  = " << modules[i].area << endl;
  }
}
*/


void FPlan::writeModuleOut(const char *filename){

  string info = filename;
  ofstream of(info.c_str());
  
  //of << modules_N << " " << Width << " " << Height << endl;
  for(int i=0; i < modules_N; i++){
	of << modules[i].name << " ";
    of << modules_info[i].x << " " ;
    of << modules_info[i].y << "\n";
  }
  of << endl;


}

void FPlan::optimize_terminals(const char *filename){
	int NumNets,NumPins,dummy,acc=0;

	char a[20],b[20];
	FILE *fp;
	fp=fopen(filename,"r");
	if(fp==NULL)
		printf("Error Opening File\n");
	fscanf(fp,"%s %s %d",a,b,&NumNets);
	fscanf(fp,"%s %s %d",a,b,&NumPins);
	int boundx=-1, boundy=-1;
	for(int i=0;i<modules_N;i++)
	{
		if(modules_info[i].x+modules[i].width>boundx)
			boundx=modules_info[i].x+modules[i].width;
		if(modules_info[i].y+modules[i].height>boundy)
			boundy=modules_info[i].y+modules[i].height;

	}
	terminal term;
	int arr[10]={0};
	for(int i=0;i<TerminalCount;i++)
	{
		fscanf(fp,"%s %s %d",a,b,&dummy);
		acc+=1;
		terminals.push_back(term);
		terminal &temp = terminals.back();
		int max_x=-1,max_y=-1,min_x=INT_MAX,min_y=INT_MAX;
		char ch;
		for(int m=0;m<dummy;m++)
		{
			fscanf(fp,"%s %s",a,b);
			if(a[0]!='p'){
			for(int j=0;j<modules_N;j++)
			{
				if(strcmp(modules[j].name,a)==0)
				{
					int x_mid=modules_info[j].x+((modules[j].width)/2);
					int y_mid=modules_info[j].y+((modules[j].height)/2);
					if(max_x<x_mid)
						max_x=x_mid;
					if(max_y<y_mid)
						max_y=y_mid;
					if(min_x>x_mid)
						min_x=x_mid;
					if(min_y>y_mid)
						min_y=y_mid;
					//printf("%s %s",modules[j].name,a);
					ch = a[2];
					int k=ch-'0';
					arr[k]++;
				}
			}

			}
			else{
			strcpy(temp.name,a);
			}
		}
		//printf("min max stuff %d %d %d %d\n",min_x,min_y,max_x,max_y);
		//printf("%s",temp.name);


		int minVal=INT_MAX,flag=-1,ind=0;
		//printf("min Val1  %d \n",minVal);
		if(minVal>min_x)
		{ minVal=min_x; temp.x=0; temp.y=(min_y+max_y)/2;}
		if(minVal>min_y)
		{	minVal=min_y; temp.x=(min_x+max_x)/2; temp.y=0; }
		if(minVal>(boundx-max_x))
		{	minVal=boundx-max_x; temp.x=boundx; temp.y=(min_y+max_y)/2; }
		if(minVal>boundy-max_y)
		{	minVal=min_x; temp.x=(min_x+max_x)/2; temp.y=boundy; }
		for(int k=0;k<acc;k++)
		{
			if(((terminals[k].x==temp.x)||(terminals[k].x==temp.x+1)||(terminals[k].x==temp.x-1))&&((terminals[k].y==temp.y)||(terminals[k].y==temp.y+1)||(terminals[k].y==temp.y-1))&&(!(strcmp(terminals[k].name,temp.name)==0)))
				flag=1;
		}
		int s_x=temp.x, s_y=temp.y, det=1;
		while(flag==1)
		{
			flag=-1;
			if(temp.x==0||temp.x==boundx)
			{
				if(temp.y+2<boundy&&det==1)
				{temp.y+=2;}
				else
				{temp.y=s_y;  det=-1;}
				if(temp.y-2>0&&det==-1)
					{temp.y-=2;}

				for(int k=0;k<acc;k++)
				{
					if(((terminals[k].x==temp.x)||(terminals[k].x==temp.x+1)||(terminals[k].x==temp.x-1))&&((terminals[k].y==temp.y)||(terminals[k].y==temp.y+1)||(terminals[k].y==temp.y-1))&&(!(strcmp(terminals[k].name,temp.name)==0)))
						flag=1;
				}
			}
			else if(temp.y==0||temp.y==boundy)
			{
				if(temp.x+2<boundy&&det==1)
					temp.x+=2;
				else
				{temp.x=s_x;  det=-1;}
				if(temp.x-2>0&&det==-1)
				{temp.x-=2;}
				for(int k=0;k<acc;k++)
				{
					if(((terminals[k].x==temp.x)||(terminals[k].x==temp.x+1)||(terminals[k].x==temp.x-1))&&((terminals[k].y==temp.y)||(terminals[k].y==temp.y+1)||(terminals[k].y==temp.y-1))&&(!(strcmp(terminals[k].name,temp.name)==0)))
						flag=1;
				}

			}
		}
	}
	fclose(fp);
	//printf("Bounds %d %d\n",boundx,boundy);
	for(int m=0;m<10;m++)
	{
		printf("%d\t",arr[m]);
	}
}


void FPlan::writeTerminalOut(const char *filename)
{

	FILE *fp = fopen(filename,"a");

	for(int i=0;i<TerminalCount;i++)
	{
		fprintf(fp,"%s %d %d\n",terminals[i].name,terminals[i].x,terminals[i].y);
	}
	fclose(fp);
}void error(char *msg,char *msg2/*=""*/)
{
  printf(msg,msg2);
  cout << endl;
  throw 1;
}

bool rand_bool()
{
	return bool( rand()%2 == 0 );
}

double rand_01()
{
  return double(rand()%10000)/10000;
}
