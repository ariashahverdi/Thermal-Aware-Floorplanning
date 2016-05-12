
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
  norm_Chip_Temp= 1;
  cost_alpha=calpha;
}

void FPlan::packing(){
  //if(cost_alpha!=1);
    // calcWireLength();
}

void FPlan::clear(){
  Area = 0; 
  Chip_Temp = 0;
}

double FPlan::getCost()
{
  double ar = getHeight()/getWidth();
  double ar_cost = 0;

  if( cost_alpha == 1 )
      return cost_alpha*(Area/norm_area) + ar_cost;


  return cost_alpha*(Area/norm_area) + (1-cost_alpha)*(Chip_Temp/norm_Chip_Temp) + k3*ar_cost;



}

double FPlan::getDeadSpace()
{
  return 100*(Area-TotalArea)/double(Area);
}

void FPlan::normalize_cost(int t)
{
  norm_area = norm_Chip_Temp = 0;

  for(int i=0; i < t; i++)
  {
    perturb();
    packing();
    norm_area += Area;
    norm_Chip_Temp += Chip_Temp;
  }
  
  norm_area /= t;
  norm_Chip_Temp /= t;
}



void FPlan::readInput(char *file)
{
	int BlockCount,i,dummy,t=0,s=0,j;
	float w,h,p,tolerance;

		char a[50],b,BlockName[6],terminalName[6];
		FILE *fp_in;
	    //block *root,*temp, *lroot;
		
		//open Input File
		fp_in=fopen(file,"r");
		if (fp_in == NULL)
		{printf("Unable to open Input File\n");	exit(0);}

		fscanf(fp_in, "%d\n", &BlockCount);
		fscanf(fp_in, "%f\n", &tolerance);
		
		printf("Total Number of Blocks present in File is: %d \n",BlockCount);

	
		//intialize ptr arry
		Module dummy_mod;
		
	    //read block names and dimensions
		for(i=0;i<BlockCount;i++)
		{
			fscanf(fp_in, "%f %f %f\n", &w,&h,&p);
			modules.push_back(dummy_mod);
			Module &mod = modules.back();
			mod.id = i;
			mod.height=h;
			mod.width=w;
			mod.area=h*w;
			mod.power=p;
		}
  root_module = modules.back();
  modules_N = modules.size();  
  modules_info.resize(modules_N);
  modules.resize(modules_N);


  TotalArea = 0;
  for(int i=0; i < modules_N; i++)
    TotalArea += modules[i].area;

  printf("Total Area of Blocks present in File is: %f \n",TotalArea);
  printf("\n\n The Individual Block Details are \n");
  for(int i=0; i < modules_N; i++)
	  printf("w=%f h=%f p=%f\n",modules[i].width,modules[i].height,modules[i].power);
  
  
  
  //Create the INIT needed for HotSpot
  ofstream myfile ("sample.init");
  if (myfile.is_open())
  {
	for(int i=0; i < modules_N; i++){  
    myfile << i << " " << "0 \n";
	}
    myfile.close();
	
  }
  
  //Create the PTRACE needed for HotSpot
  ofstream myfile2 ("sample.ptrace");
  if (myfile2.is_open())
  {
	for(int i=0; i < modules_N; i++){ 
	myfile2 << i << " ";
	}
	
	myfile2 << " \n";
	
	for(int i=0; i < modules_N; i++){  
    myfile2 << modules[i].power << " ";
	}
    myfile2.close();
	
  }
  
	  
}



void FPlan::writeModuleOut(const char *filename){

  string info = filename;
  ofstream of(info.c_str());
  printf("\n\n The Output of the Algorithm is: \n");
  for(int i=0; i < modules_N; i++){
    of << modules_info[i].x << " " ;
    of << modules_info[i].y << "\n";
	printf("X=%f Y=%f\n",modules_info[i].x,modules_info[i].y);
  }
  of << endl;


}

void error(char *msg,char *msg2/*=""*/)
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
