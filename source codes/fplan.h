
#ifndef fplanH
#define fplanH
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <cstdio>
using namespace std;



struct Module{
  int id;
  float width,height;
  int x,y;
  float power;
  float area;

};
typedef vector<Module> Modules;

struct Module_Info{
  bool rotate, flip;
  int x,y;
  int rx,ry;
  struct Module_Info *next;
};
typedef vector<Module_Info> Modules_Info;



class FPlan{
  public:
    FPlan(double calpha);
    void read(char*);
    virtual void init()		=0;
    virtual void packing();
    virtual void perturb()	=0;    
    virtual void keep_sol()	=0;
    virtual void keep_best()	=0;
    virtual void recover()	=0;
    virtual void recover_best() =0;
    virtual double getCost();    
	void readInput(char *file);

    int    size()         { return modules_N; }
    double getTotalArea() { return TotalArea; }
    double getArea()      { return Area;      }
    
    double getWidth()     { return Width;     }
    double getHeight()    { return Height;    }
	//double getAR()        { return Width/Height; }
	double getAR()        { return Height/Width; }
    double  getDeadSpace();

    // outline
    double k3;
    double outline_width;
    double outline_height;
    bool isFit()
    {
      return ( Height <= outline_height && Width <= outline_width );
    }

    // information
    void writeModuleOut(const char *);
    void optimize_terminals(const char *);
    void writeTerminalOut(const char *);
    void show_modules();    
    void normalize_cost(int);
	void outDraw( const char* filename );		// output drawing file
    
  protected:
    void clear();
    
    void scaleIOPad(); 

    double Area;
    double Width,Height;
    double TotalArea;
    
    int modules_N;    
    Modules modules;
	
    Module  root_module;
    Modules_Info modules_info;    
	
	double Chip_Temp;
    double norm_area, norm_Chip_Temp;
    double cost_alpha;
    vector<vector<int> > connection;
    
  private:
    void read_dimension(Module&);
    void read_IO_list(Module&,bool parent);
    void read_network();
    void create_network();   

    map<string,int> net_table;
    string filename; 
};


void error(char *msg,char *msg2="");
bool rand_bool();
double rand_01();
double seconds();
      
//---------------------------------------------------------------------------
#endif
