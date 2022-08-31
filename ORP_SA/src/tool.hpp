#pragma once
#include "child.hpp"
#include "param.hpp"
#include <float.h>

using namespace std;

#define ERROR(...) do{fprintf(stderr,__VA_ARGS__); exit(1);}while(0)
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define DoNS_K 0

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

vector<double> ave_dons(const int &f, const vector<vector <double>> &dons)
{
	double d_sum = 0, a_sum = 0;
	int d_count = 0, a_count = 0;

	//Sparat
	#if DoNS_K == 0
		for(unsigned int i = 0; i < dons.size(); i++)
		{
			if(dons[i][0] == f && dons[i][1] < 100000)
			{
				if(dons[i][1] == 0)
				{
					a_sum += dons[i][2];
					a_count++;
				}
				else
				{
					d_sum += dons[i][1];
					d_count++;
				}
			}
		}		
	#else
		for(unsigned int i = 0; i < dons.size(); i++)
		{
			if(dons[i][0] == f && dons[i][1] < 100000)
			{
				a_sum += dons[i][2];
				a_count++;
				d_sum += dons[i][1];
				d_count++;
			}
		}
	#endif
	return vector<double>({d_sum / d_count, a_sum / a_count});
}

bool comp1(const hostswitch &a, const hostswitch &b)
{
    return a.ASPL < b.ASPL;
}
bool comp2(const hostswitch &a, const hostswitch &b)
{
    return a.diameter < b.diameter;
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
  
  SAFE_FREE(h_degree);
  SAFE_FREE(s_degree);
  SAFE_FREE(edge);
  SAFE_FREE(adjacency);

  return;
}

vector <int> check_self_loop(const vector <vector<int>> &edge_table, int s_num)
{
	vector <int> sl_edge;
	for(unsigned int i = param::hosts; i < edge_table.size(); i++)
	{
		if(i != s_num)
		{
			for(unsigned int j = 0; j < edge_table[i].size(); j++)
    		{
      			if(edge_table[i][j] == i)
	  			{
        			sl_edge.push_back(i);
					break;
				}
			}
    	}
	}
 	return sl_edge;
}

vector <int> check_num_double_edge(const vector <vector<int>> &edges, int s_num)
{
	vector <int> num;
	for(unsigned int i = 0; i < edges.size() - 1; i++)
	{
		if(edges[i][0] == s_num || edges[i][1] == s_num)
		{
			for(unsigned int j = i + 1; j < edges.size(); j++)
			{
				if((edges[j][0] == s_num || edges[j][1] == s_num) && (edges[j][0] + edges[j][1] - s_num == edges[i][0] + edges[i][1] - s_num))
				{
					num.push_back(i);
					break;
				}
			}
		}
	}
	return num;
}

vector <int> check_num_self_loop(const vector <vector<int>> &edges)
{
	vector <int> num;
	for(unsigned int i = 0; i < edges.size(); i++)
	{
		if(edges[i][0] == edges[i][1])
			num.push_back(i);
	}
	return num;
}

vector <vector<int>> check_double_edge(const vector <vector<int>> &edges, int s_num)//Expect for s_num
{
 	vector <vector<int>> dl_edge;
	for(unsigned int i = 0; i < edges.size() - 1; i++)
	{
		if(edges[i][0] != s_num && edges[i][1] != s_num)
		{
			for(unsigned int j = i + 1; j < edges.size(); j++)
    		{
				if(edges[j][0] != s_num && edges[j][1] != s_num && ((edges[j][0] == edges[i][0] && edges[j][1] == edges[i][1]) || (edges[j][0] == edges[i][1] && edges[j][1] == edges[i][0])))
				{ 
	  				dl_edge.push_back(vector<int>({edges[i][0], edges[i][1]}));
					break;
				}
				
			}
    	}
	}
  return dl_edge;
}

vector<int> check_num_of_me(const vector <vector<int>> &edges)
{
 	vector<int> num;
	for(unsigned int i = 0; i < edges.size() - 1; i++)
	{
		for(unsigned int j = i + 1; j < edges.size(); j++)
   		{
			if((edges[j][0] == edges[i][0] && edges[j][1] == edges[i][1]) || (edges[j][0] == edges[i][1] && edges[j][1] == edges[i][0]))
			{ 
  				num.push_back(j);
				break;
			}		
		}
	}
  return num;
}

vector<int> exp_check_num_double_edge(const vector <vector<int>> &edges, int s_num)//Expect for s_num
{
 	vector<int> num;
	for(unsigned int i = 0; i < edges.size() - 1; i++)
	{
		if(edges[i][0] != s_num && edges[i][1] != s_num)
		{
			for(unsigned int j = i + 1; j < edges.size(); j++)
    		{
				if(edges[j][0] != s_num && edges[j][1] != s_num && ((edges[j][0] == edges[i][0] && edges[j][1] == edges[i][1]) || (edges[j][0] == edges[i][1] && edges[j][1] == edges[i][0])))
				{ 
	  				num.push_back(i);
					break;
				}
				
			}
    	}
	}
  return num;
}

vector <vector<int>> check_stos_edge(const vector <vector<int>> &edges, int s_num)
{
	vector <vector<int>> ss_edge;
	for(unsigned int i = 0; i < edges.size() - 1; i++)
	{
		if(edges[i][0] >= param::hosts && edges[i][1] >= param::hosts && edges[i][0] != s_num && edges[i][1] != s_num)
		ss_edge.push_back(vector<int>({edges[i][0], edges[i][1]}));
	}
	return ss_edge;
}

int Num_of_sl(const vector <vector<int>> &edges)
{
	int num = 0;
	for(unsigned int i = 0; i < edges.size(); i++)
	{
		if(edges[i][0] == edges[i][1])
			num++;
	}
	return num;
}

int Num_of_me(const vector <vector<int>> &edges)
{
 	int num = 0;
	for(unsigned int i = 0; i < edges.size() - 1; i++)
	{
		for(unsigned int j = i + 1; j < edges.size(); j++)
   		{
			if((edges[j][0] == edges[i][0] && edges[j][1] == edges[i][1]) || (edges[j][0] == edges[i][1] && edges[j][1] == edges[i][0]))
			{ 
  				num++;
				break;
			}		
		}
	}
  return num;
}

int RME(vector <vector <int>> &edges, int s_num)
{
	//reduce multiple edges
	//ver2.0
	vector <int> mul_edges = check_num_double_edge(edges, s_num);
  	vector <int> self_loop_num = check_num_self_loop(edges); //num = line num of child.edge
  	vector <int> another_mul_edges = exp_check_num_double_edge(edges, s_num);
  	if(mul_edges.size() >= 1)
  	{
    	int p, q, temp, f = 0;
    	if(self_loop_num.size() >= 1)
    	{
      		p = self_loop_num[rand() % self_loop_num.size()];
      		q = mul_edges[rand() % mul_edges.size()];
			f = 1;
    	}
    	else if(mul_edges.size() >= 2)
    	{
      		p = mul_edges[rand() % mul_edges.size()];
      		do{ q = mul_edges[rand() % mul_edges.size()]; }while(p == q);
			f = 1;
    	}
    	else if(another_mul_edges.size() >= 1)
   		{
      		p = mul_edges[rand() % mul_edges.size()];
      		q = another_mul_edges[rand() % another_mul_edges.size()];
			f = 1;
    	}
    	else
    	{
      		p = mul_edges[rand() % mul_edges.size()];
      		do{ q = rand() % edges.size(); }while(p == q);
			f = 1;
    	}
		if(rand() % 2 == 0 && f == 1)
   		{
   			temp = edges[p][1];
      		edges[p][1] = edges[q][1];
   	  		edges[q][1] = temp;  
   		}
    	else if(f == 1)
   		{
   			temp = edges[p][1];
   			edges[p][1] = edges[q][0];
   			edges[q][0] = temp;
   		}
  	}
	return mul_edges.size();
}

int select_SLME_num(const vector<vector<int>> &edges, int switches)
{
	vector<int> temp(param::hosts + switches, 0), temp2;
  	for(unsigned int i = param::hosts; i < edges.size() - 1; i++)
  	{
    	if(edges[i][0] == edges[i][1])
      		temp[edges[i][0]]++;
    	else
		{
      		for(unsigned int j = i + 1; j < edges.size(); j++)
      		{
        		if((edges[i][0] == edges[j][0] && edges[i][1] == edges[j][1]) || (edges[i][0] == edges[j][1] && edges[i][1] == edges[j][0]))
        		{
          			temp[edges[i][0]]++;
          			temp[edges[i][1]]++;
          			break;
        		}
      		}
		}
  	}
  	int max_slme = *max_element(temp.begin(),temp.end());
  	for(unsigned int i = param::hosts; i < temp.size(); i++)
  	{
    	if(temp[i] == max_slme)
      	temp2.push_back(i);
  	}
  	if(temp2.size() > 1)
    	shuffle(temp2.begin(), temp2.end(), randomseed);
  	return temp2[0];
}

int select_fewhost_num(const vector<vector<int>> &edges, int switches)
{
	int s_num = -1;
	vector<int> num_host;
	num_host.resize(param::hosts + switches);
	for(int i = 0; i < param::hosts; i++)
		num_host[edges[i][1]]++;	//スイッチごとのホストの数を数える

	vector<vector<int>> num_hostlist;
	num_hostlist.resize(param::radix);
	for(unsigned int i = param::hosts; i < num_host.size(); i++)
		num_hostlist[num_host[i]].push_back(i);

	for(unsigned int i = 1; i < num_hostlist.size(); i++)
	{
		if(num_hostlist[i].size() >= 1)
		{
			s_num = num_hostlist[i][rand() % num_hostlist[i].size()];
			break;
		}
	}
	if(s_num == -1)
		s_num = rand() % switches + param::hosts;
	return s_num;
}