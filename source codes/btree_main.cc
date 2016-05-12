//main
#include <iostream>             
#include <cstring>
#include <ctime>
#include <cmath>
#include<string.h>
#include <stdlib.h>
#include <cassert>
#include <iomanip>
using namespace std;

#include "btree.h"

extern double init_avg;
extern double avg_ratio;
extern double lamda;
extern double P;

void SimAnneal(FPlan &fp, int k, int local=0, double term_T=0.1);
double Random_Floorplan(FPlan &fp,int times);
double init_avg = 0.00001;
int hill_climb_stage = 7;
double avg_ratio=150;
double lamda=1.3;
bool forgeFlag = false;

double P=0.9;


double mean(vector<double> &chain)
{
  double sum=0;
  for(int i=0; i < chain.size();i++)
     sum+= chain[i];
  return sum/chain.size();
}

double var(vector<double> &chain)
{
  double m = mean(chain);
  double sum=0;
  double var;
  int N= chain.size();

  for(int i=0; i < N;i++)
     sum += (chain[i]-m)*(chain[i]-m);

  var = sum/(N);
  return var;
}


void SimAnneal(FPlan &fp, int k, int local/*=0*/, double term_T/*=0.1*/)
{
  int MT,uphill,reject;
  double pre_cost,best,cost;
  float d_cost,reject_rate=0;

  int N = k * fp.size();
  //float P=0.9;
  float T,actual_T=1;
  double avg=init_avg;
  float conv_rate = 0.99;
  double time_start;


  avg = Random_Floorplan( fp,N);
  cout<<"\nProblem is here\n";
  avg_ratio = avg;



  T = avg / log(1/P);
  actual_T = T;

  fp.packing();
  fp.keep_sol();
  fp.keep_best();
  pre_cost =  best = fp.getCost();

  int good_num=0,bad_num=0;
  double total_cost=0;
  int count=0;
  float p_avg = 0;
  float dcost_avg = 0;
  int bad_count=0;
  int good_count = 0;
  int itcount = 0;
    bool first_feasible = false;

  do{
   count++;
   total_cost = 0;
   MT=uphill=reject=0;

   vector<double> chain;
   good_count = 0;
   p_avg = 0;
   dcost_avg = 0;

   for(; uphill < N && MT < 2*N; MT++)
   {
       fp.perturb();
       fp.packing();
       cost = fp.getCost();
       d_cost = cost - pre_cost;
       float p = 1/(exp(d_cost/T));

       p = p>1? 1:p;

	   itcount++;
	   p_avg+=p;  // observ
       dcost_avg += fabs(d_cost); // observ

       chain.push_back(cost);

       if(d_cost <=0 || rand_01() < p )
	   {

         fp.keep_sol();
         pre_cost = cost;

         if(d_cost > 0)
		 {
           uphill++, bad_num++;
         }
		 else
			 if(d_cost < 0)
				 good_num++;

         if(cost < best)
		 {
             bool feasible = fp.isFit();

            if( first_feasible == false)
            {
                fp.keep_best();
                best = cost;
            }
            else
            {
                if( feasible )
                {
                    fp.keep_best();
                    best = cost;
                }
            }

            if( feasible )
                first_feasible = true;




            assert(fp.getArea() >= fp.getTotalArea());
	        good_count++;

         }
       }
       else
	   {
         reject++;
         fp.recover();
       }
   }
   if( good_count == 0 )
       bad_count++;
   else
       bad_count = 0;


   p_avg/= itcount;
   dcost_avg /=itcount;

   double sv = var(chain);

   reject_rate = float(reject)/MT;

   float r_t = exp(-T/sv);
   if( count < local )
   {
	   T =(actual_T/100)/((double)count/dcost_avg);
   }
   else
   {
  	   T =actual_T/((double)count/dcost_avg);
   }





  }while( ( reject_rate<conv_rate && T>1e-4 ) || bad_count<=3 );


  fp.recover_best();
  fp.packing(); 
  return;
  
 
}

double Random_Floorplan(FPlan &fp,int times)
{
  int N =times,t=0;
  double total_cost=0,pre_cost,cost,best;

  fp.packing();
  pre_cost = best = fp.getCost();
  fp.keep_best();

//do
 {
  for(int i=0; i < N;i++)
  {
    fp.perturb();
    fp.packing();

    cost = fp.getCost();
    if(cost-pre_cost > 0)
	{
      t++;
      total_cost += cost-pre_cost;
      pre_cost = cost;
    }

    if(cost < best)
	{
      best = cost;
   }
   
   total_cost==2;
  }
 }//while(total_cost==0);

  fp.recover_best();
  fp.packing();

  total_cost /= t;
  return total_cost;
}


int main(int argc,char **argv)
{

   char filename1[40],outfilename[40];
   strcpy(filename1,argv[1]);
   strcpy(outfilename,argv[2]);
   int rand_seed = (unsigned)time( NULL );
   srand( rand_seed );

     clock_t begin=clock();
     B_Tree fp(1);
     fp.readInput(filename1);


     fp.k3 = 1.0;
     fp.init();
	 
	 system("hotspot");
	
	 fp.normalize_cost( 1000 );

     SimAnneal(fp, 10, 7, 0.1);
     fp.writeModuleOut(outfilename);
     clock_t done=clock();
     cout<<"\nExecution time is "<<double(done-begin)/CLOCKS_PER_SEC<<" seconds\n";
       return 1;
}
