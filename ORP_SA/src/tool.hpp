#pragma once
#include "child.hpp"
#include "param.hpp"
#include <float.h>

using namespace std;

#define ERROR(...) do{fprintf(stderr,__VA_ARGS__); exit(1);}while(0)
#define MAX(a, b) ((a) > (b) ? (a) : (b))

//Optimize_switches
double moore_bound(const double nodes, const double degree)
{
	if (degree + 1 >= nodes)
		return 1;

	double diam = -1, n = 1, r = 1, aspl = 0.0, prev_tmp;
	while (1) {
		double tmp = n + degree * pow(degree - 1, r - 1);
		if (tmp >= nodes || (r > 1 && prev_tmp == tmp))
			break;

		n = tmp;
		aspl += r * degree * pow(degree - 1, r - 1);
		diam = r++;
		prev_tmp = tmp;
	}

	diam++;
	aspl += diam * (nodes - n);
	aspl /= (nodes - 1);

	return aspl;
}
static double continuous_moore_bound(const int hosts, const int switches, const int radix)
{
	double h = hosts;
	double s = switches;
	double r = radix;
	return moore_bound(s, r - h / s)*(s*h - h) / (s*h - s) + 2;
}
int ORP_Optimize_switches(const int hosts, const int radix)
{
	if (hosts < 3)
		ERROR("Hosts (%d) >= 3\n", hosts);
	else if (radix < 3)
		ERROR("Radix (%d) >= 3\n", radix);

	int s = 3;
	double prev = DBL_MAX;
	while (1) {
		if (s*radix - 2 * (s - 1) >= hosts) {
			double tmp = continuous_moore_bound(hosts, s, radix);
			if (prev <= tmp)
				break;
			prev = tmp;
		}
		s++;
	}
	return s - 1;
}


void intialize(vector<hostswitch> &group)
{
	group.resize(param::pop);
	for (unsigned int i = 0; i < group.size(); i++)
	{
		group[i].switches = ORP_Optimize_switches(param::hosts, param::radix) + param::offset; 
		group[i].Init();
	}

}

void all_eva(vector<hostswitch> &group)
{
	for(unsigned int i = 0; i < group.size(); i++)
		group[i].evaluation();
}

void copy_HS(const hostswitch &A, hostswitch &B)//A->B
{
	B.switches = A.switches;
	B.lines = A.lines;
	B.edges = A.edges;
	B.port_f = A.port_f;
	B.ASPL = A.ASPL;
	B.low_ASPL = A.low_ASPL;
	B.diameter = A.diameter;
}

void copy_group(const vector<hostswitch> &group, vector<hostswitch> &child_group)
{
	if(group.size() >= child_group.size())
	{
		for(unsigned int i = 0; i < child_group.size(); i++)
			copy_HS(group[i], child_group[i]);
	}
	else
	{
		for(unsigned int i = 0; i < child_group.size(); i++)
			copy_HS(group[i % group.size()], child_group[i]);
	}
}
void copy_grouptoc(const vector<hostswitch> &group, vector<vector<hostswitch>> &child_group)
{
	for(unsigned int i = 0; i < group.size(); i++)
	{
		for(unsigned int j = 0; j < child_group[i].size(); j++)
			copy_HS(group[i], child_group[i][j]);
	}
}

void update_group(const vector<vector<hostswitch>> &child_group, vector<hostswitch> &group)
{
	for(unsigned int i = 0; i < group.size(); i++)
		copy_HS(child_group[i][0], group[i]);
}

void select_child(vector<hostswitch> &group, const vector<vector<hostswitch>> &child_group)
{
	vector<hostswitch> temp(param::pop);



	copy_group(temp, group);
}

int min_ASPL(const vector<hostswitch> &group)
{
	int min;
	double min_aspl=DBL_MAX;
	for(unsigned int i = 0; i < group.size(); i++)
	{
		if(min_aspl > group[i].ASPL)
		{
			min = i;
			min_aspl = group[i].ASPL;
		}
	}
	return min;
}

bool comp1(const hostswitch &a, const hostswitch &b)
{
    return a.ASPL < b.ASPL;
}
bool comp2(const hostswitch &a, const hostswitch &b)
{
    return a.diameter < b.diameter;
}

void sort_group(vector<hostswitch> &group)
{
	sort(group.begin(), group.end(), comp1);
    sort(group.begin(), group.end(), comp2);
}

void sort_all(vector<vector<hostswitch>> &child_group)
{
	for(unsigned int i = 0; i < child_group.size(); i++)
		sort_group(child_group[i]);
}

bool comp3(const vector<int> &edges_a, const vector<int> &edges_b)
{
	return edges_a[0] < edges_b[0];
}

void sort_edges(hostswitch &child)
{
	for(unsigned int i = 0; i < child.edges.size(); i++)
		sort(child.edges[i].begin(), child.edges[i].end());
	sort(child.edges.begin(), child.edges.end(), comp3);
}

bool accept(const int hosts, const int switches, const int current_diameter, const int diameter, const double current_ASPL, const double ASPL, double *max_diff_energy)
{
  if(diameter < current_diameter){
    return true;
  }
  else if(diameter > current_diameter){
    return false;
  }
  //  diameter == current_diameter
  double diff = (current_ASPL-ASPL)*switches*hosts;
  *max_diff_energy = MAX(*max_diff_energy, -1.0 * diff);
  return (ASPL <= current_ASPL);
}

static double uniform_rand()
{
  return ((double)random()+1.0)/((double)RAND_MAX+2.0);
}

void set_temp(void)
{
  int hosts = param::hosts, switches, radix = param::radix, seed = param::seed;
  int lines, diameter, current_diameter, low_diameter;
  long sum, best_sum, ncalcs = param_sa::ncalcs;
  double ASPL, current_ASPL, low_ASPL, max_diff_energy = 0;
  ORP_Restore r;

  switches = ORP_Optimize_switches(hosts, radix) + param::offset;
  
  int *h_degree = (int*)malloc(sizeof(int) * switches);
  int *s_degree = (int*)malloc(sizeof(int) * switches);
  int *edge     = (int*)ORP_Generate_random(hosts, switches, radix, SET_TEMP_F, &lines, h_degree, s_degree);
  int *adjacency = (int*)malloc(sizeof(int) * switches * radix);

  ORP_Conv_edge2adjacency(hosts, switches, radix, lines, edge, adjacency);
  ORP_Init_aspl(hosts, switches, radix);
  ORP_Set_aspl(h_degree, s_degree, adjacency, &diameter, &sum, &ASPL);
  current_diameter = diameter;
  current_ASPL     = ASPL;

  ORP_Set_lbounds(hosts, radix, &low_diameter, &low_ASPL);
  if(diameter == low_diameter && ASPL == low_ASPL){
    printf("Find optimum solution\n");
  }
  else{
    for(long i=0;i<ncalcs;i++){
      if(uniform_rand() > 0.5)
        ORP_Swap_adjacency(switches, radix, s_degree, &r, adjacency);
      else
        ORP_Swing_adjacency(switches, radix, h_degree, s_degree, &r, adjacency); 
      ORP_Set_aspl(h_degree, s_degree, adjacency, &diameter, &sum, &ASPL);
      if(accept(hosts, switches, current_diameter, diameter, current_ASPL, ASPL, &max_diff_energy)){
	current_diameter = diameter;
	current_ASPL     = ASPL;
      }
      else{
        ORP_Restore_adjacency(r, radix, h_degree, s_degree, adjacency);
      }
    }
  }
  ORP_Finalize_aspl();

  // Set max temperature to accept it   50% in maximum diff energy.
  printf("Proposed max temperature is %f\n", (-1.0 * max_diff_energy) / log(0.5));
  // Set min temperature to accept it 0.01% in minimum diff energy.
  printf("Proposed min temperature is %f\n", (-2.0) / log(0.0001));
  
  param_sa::temp0 = (-1.0 * max_diff_energy) / log(0.5);
  param_sa::tempF = (-2.0) / log(0.0001);                           
  
  return;
}