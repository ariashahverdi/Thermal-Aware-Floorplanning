
#ifndef fplanH
#define fplanH
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <cstdio>
using namespace std;

struct Pin{
  int mod;
  int net;
  int x,y;    // relative position
  int ax,ay;  // absolute position
  Pin(int x_=-1,int y_=-1){ x=x_,y=y_; }
};
typedef vector<Pin> Pins;
typedef Pin* Pin_p;
typedef vector<Pin_p> Net;
typedef vector<Net > Nets;

enum Module_Type { MT_HARD, MT_SOFT, MT_RECLINEAR };

struct Module{
  int id;
  char name[20];
  int width,height;
  int x,y;
  int area;
  Pins pins;
  Module_Type type;
  bool no_rotate;

};
typedef vector<Module> Modules;

struct Module_Info{
  bool rotate, flip;
  int x,y;
  int rx,ry;
  struct Module_Info *next;
};
typedef vector<Module_Info> Modules_Info;

struct terminal{
	char name[20];
	int x;
	int y;
	struct Module *ptr;
};
typedef vector<terminal> Terminals;

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
    int    getWireLength(){ return WireLength;}
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
    double calcWireLength();
    void scaleIOPad(); 

    double Area;
    double Width,Height;
    int WireLength;
    double TotalArea;
    
    int modules_N;    
    Modules modules;
    Terminals terminals;
    Module  root_module;
    Modules_Info modules_info;    
    Nets network;
    double norm_area, norm_wire;
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
