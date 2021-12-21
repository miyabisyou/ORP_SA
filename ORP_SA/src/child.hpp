#pragma once
#include <vector>
#include <random>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <sys/stat.h>
#include "param.hpp"

typedef struct {
  int u[2], v[2], u_d[2], v_d[2], switches, symmetries, op;
} ORP_Restore;

extern "C"
{
	void ORP_Conv_edge2adjacency(const int hosts, const int switches, const int radix, const int lines, const int *edge, int *adjacency);
	void ORP_Init_aspl(const int hosts, const int switches, const int radix);
	void ORP_Set_aspl(const int* h_degree, const int* s_degree, const int* adjacency, int *diameter, long *sum, double *ASPL);
	void ORP_Finalize_aspl();
	void ORP_Set_lbounds(const int hosts, const int radix, int *low_diameter, double *low_ASPL);
	void ORP_Set_degrees(const int hosts, const int switches, const int lines, const int *edge, int *h_degree, int *s_degree);
	void* ORP_Generate_random(const int hosts, const int switches, const int radix, const bool assign_evenly, int *lines, int *h_degree, int *s_degree);
	void ORP_Restore_adjacency(const ORP_Restore r, const int radix, int *h_degree, int *s_degree, int *adjacency);
	void ORP_Swap_adjacency(const int switches, const int radix, const int* s_degree, ORP_Restore *r, int *adjacency);
	void ORP_Swing_adjacency(const int switches, const int radix, int *h_degree, int *s_degree, ORP_Restore *r, int *adjacency);
	void ORP_Conv_adjacency2edge(const int hosts, const int switches, const int radix, const int *h_degree,const int *s_degree, const int *adjacency, int *edge);
	int ORP_Optimize_switches(const int hosts, const int radix);
}

using namespace std;
mt19937 randomseed(param::seed);

int check_edges(vector<vector<int>> edges)
{
	int f = 1;
	for (unsigned int i = 0; i < edges.size(); i++)
	{
		if (edges[i][0] == edges[i][1])
		{
			f = 0;
			break;
		}
	}
	for (unsigned int i = param::hosts; i < edges.size() - 1; i++)
	{
		for (unsigned int j = i + 1; j < edges.size(); j++)
		{
			if ((edges[i][0] == edges[j][0] && edges[i][1] == edges[j][1]) || (edges[i][0] == edges[j][1] && edges[i][1] == edges[j][0]))
			{
				f = 0;
				break;
			}
		}
	}
	return f;

}

class hostswitch
{
public:
	vector<vector<int>> edges;
	vector<vector<int>> adja;
	vector<int> h_deg, s_deg;
	int switches, lines, diameter = -1, low_diameter = -1;
	double ASPL = -1, low_ASPL = -1;
	long sum;
	int port_f;
	ORP_Restore r;

	void Initialize(void)
	{
		int f, k;
		lines = (switches * param::radix - param::hosts) / 2 + param::hosts;
		edges.assign(lines, vector<int>(2, 0));
		vector<int> sw;
		for (int i = 0; i < switches; i++)
		{
			for (int j = 0; j < param::radix; j++)
				sw.push_back(i + param::hosts);
		}
		do
		{
			f = 1;
			shuffle(sw.begin(), sw.end(), randomseed);
			for (int i = 0; i < param::hosts; i++)
			{
				edges[i][0] = i;
				edges[i][1] = sw[i];
			}
			k = param::hosts;
			for (unsigned int i = param::hosts; i < edges.size(); i++)
			{
				edges[i][0] = sw[k];
				edges[i][1] = sw[++k];
				k++;
			}
			f = check_edges(edges);
			cout<<"1, " ;
		} while (f == 0);
	}

	void arr_to_vec(int *edge_arr)
	{
		for(int i = 0; i < edges.size(); i++)
		{
			edges[i][0] = edge_arr[i * 2];
			edges[i][1] = edge_arr[i * 2 + 1];
		}
	}

	void vec_to_arr(int *edge_arr)
	{
		for(int i = 0; i < edges.size(); i++)
		{
			edge_arr[i * 2] = edges[i][0];
			edge_arr[i * 2 + 1] = edges[i][1];
		}
	}

	void d_atov(int *h_degree, int *s_degree)
	{
		for(int i = 0; i < switches; i++)
		{
			h_deg[i] = h_degree[i];
			s_deg[i] = s_degree[i];
		}
	}

	void d_vtoa(int *h_degree, int *s_degree)
	{
		for(int i = 0; i < switches; i++)
		{
			h_degree[i] = h_deg[i];
			s_degree[i] = s_deg[i];
		}
	}

	void adja_atov(int *adjacency)
	{
		for(int i = 0; i < switches; i++)
		{
			for(int j = 0; j < param::radix; j ++)
			{
				adja[i][j] = adjacency[i * param::radix + j];
			}
		}
	}

	void adja_vtoa(int *adjacency)
	{
		for(int i = 0; i < switches; i++)
		{
			for(int j = 0; j < param::radix; j ++)
			{
				adjacency[i * param::radix + j] = adja[i][j];
			}
		}
	}

	void Init(void)
	{
		bool assign_evenly = false;
		lines = (switches * param::radix - param::hosts) / 2 + param::hosts;
		int *s_degree = (int*)malloc(sizeof(int) * switches);
  		int *h_degree = (int*)malloc(sizeof(int) * switches);
		int *adjacency = (int*)malloc(sizeof(int) * switches * param::radix);
		//int *edge = (int*)malloc(sizeof(int) * lines * 2);

		//edges.assign(lines, vector<int>(2, 0));
		//ORP_Srand(SEED);
  		int *edge = (int*)ORP_Generate_random(param::hosts, switches, param::radix, assign_evenly, &lines, h_degree, s_degree);
		//arr_to_vec(edge);

		ORP_Conv_edge2adjacency(param::hosts, switches, param::radix, lines, edge, adjacency);
		adja.assign(switches, vector<int>(param::radix, 0));
		adja_atov(adjacency);

		h_deg.resize(switches);
		s_deg.resize(switches);
		d_atov(h_degree, s_degree);////////////

		free(edge);
		free(h_degree);
		free(s_degree);
	}

	/*void Init(string filename)
	{
		ifstream input_file("./../input/" + filname);
		if(!input_File)
    	{
        	std::cout << "dose not open the input file." << std::endl;
        	exit(0);
    	}

		string str, ss1, ss2;
		vecter<int> temp; 
		getline(input_file, str);
		ss1 = str.substr(str.find(' ') + 1, str.rfind(' ') - str.find(' ') - 1);
		switches = temp[1];
		while(getline(input_file, str)){
			
		}
	}*/

	void print_eva(void)
	{
		printf("Hosts = %d, Switches %d, Radix = %d\n", param::hosts, switches, param::radix);
		printf("Diameter     = %d\n", diameter);
		printf("Diameter Gap = %d (%d - %d)\n", diameter - low_diameter, diameter, low_diameter);
		printf("ASPL         = %.10f (%ld/%.0f)\n", ASPL, sum, (double)param::hosts*(param::hosts - 1) / 2);
		printf("ASPL Gap     = %.10f (%.10f - %.10f)\n", ASPL - low_ASPL, ASPL, low_ASPL);
	}
	void show_edges(void)
	{
		for(unsigned int i = 0; i < edges.size(); i++)
			printf("%d : %d, %d\n", i, edges[i][0], edges[i][1]);
		printf("\n");
	}

	void show_adja(void)
	{
		for(unsigned int i = 0; i < adja.size(); i++)
		{
			cout << i << " : " << adja[i][0];
			for(unsigned int j = 1; j < adja[j].size(); j++)
				cout << ", " << adja[i][j];
			cout << endl;  
		}
	}

	void check_port(void)
	{
		port_f = -1;
		int sum;
      	for(int i = 0; i < switches; i++)
      	{
          	if(h_deg[i] + s_deg[i] != param::radix)
    	    {
          		port_f = i;
          		break;
        	}
      	}
	}

	void set_degree(void)
	{
		int temp_s, temp_h;
		for(unsigned int i = 0; i < adja.size(); i++)
		{
			temp_s = 0;
			temp_h = 0;
			for(unsigned int j = 0; j < adja[i].size(); j++)
			{
				if(adja[i][j] == -1)
					temp_s++;
				else
					temp_h++;
			}
			s_deg[i] = temp_s;
			h_deg[i] = temp_h;
		}
		if(port_f != -1)
			h_deg[port_f]--;
	}

	void evaluation(void)
	{
		//update of degree and adjacency
		//lines = (switches * param::radix - param::hosts) / 2 + param::hosts;
		//int *edge = (int*)malloc(sizeof(int) * lines * 2);
		int *adjacency = (int*)malloc(sizeof(int) * switches * param::radix);
		int *s_degree = (int*)malloc(sizeof(int) * switches);
		int *h_degree = (int*)malloc(sizeof(int) * switches);

		//vec_to_arr(edge);

		//ORP_Conv_edge2adjacency(param::hosts, switches, param::radix, lines, edge, adjacency);

		ORP_Init_aspl(param::hosts, switches, param::radix);
		//ORP_Set_degrees(param::hosts, switches, lines, edge, h_degree, s_degree);
		d_vtoa(h_degree, s_degree);
		adja_vtoa(adjacency);
		ORP_Set_aspl(h_degree, s_degree, adjacency, &diameter, &sum, &ASPL);
		ORP_Set_lbounds(param::hosts, param::radix, &low_diameter, &low_ASPL);
		ORP_Finalize_aspl();

		//print_eva();
		//free
		free(adjacency);
		free(h_degree);
		free(s_degree);
		//free(edge);
	}

	void output(string filename)
	{
		mkdir("./../result", S_IRUSR|S_IWUSR|S_IXUSR);
		ofstream Output_File("./../result/" + filename + ".edges");
		if(!Output_File)
    	{
        	std::cout << "dose not open the output file." << std::endl;
        	exit(0);
    	}

		int *edge = (int*)malloc(sizeof(int) * lines * 2);
		int *adjacency = (int*)malloc(sizeof(int) * switches * param::radix);
		int *s_degree = (int*)malloc(sizeof(int) * switches);
		int *h_degree = (int*)malloc(sizeof(int) * switches);
		adja_vtoa(adjacency);
		d_vtoa(h_degree, s_degree);
		ORP_Conv_adjacency2edge(param::hosts, switches, param::radix, h_degree, s_degree, adjacency, edge);
		edges.assign(lines, vector<int>(2, 0));
		arr_to_vec(edge);
		
		Output_File << param::hosts << " " << switches << " " << param::radix << endl;
		for(unsigned int i = 0; i < edges.size(); i++)
			Output_File << edges[i][0] << " " << edges[i][1] << endl;
		
		free(adjacency);
		free(h_degree);
		free(s_degree);
		free(edge);
	}

	void outputlog(string filename, double time)
	{
		mkdir("./../log", S_IRUSR|S_IWUSR|S_IXUSR);
		ofstream Output_File("./../log/" + filename + ".logs");
		if(!Output_File)
    	{
        	std::cout << "dose not open the log file." << std::endl;
        	exit(0);
    	}
		
		Output_File << "----- settings parameters -----" << endl;
		#if SOLUTION == 0
    		Output_File << "max generation count : " << param::gen << endl;
			Output_File << "largest institution without renewal : " << param::genth << endl;
    		Output_File << "population size : " << param::pop << endl;
    		Output_File << "offspring size : " << param::off << endl;
		#else
        	Output_File << "max temperature : " << param_sa::temp0 << endl;
			Output_File << "min temperature : " << param_sa::tempF << endl;
    		Output_File << "cooling rate : " << param_sa::cool_rate << endl;
    		Output_File << "number of iterations : " << param_sa::iteration << endl;
			Output_File << "number of evaluation calculations : " << fixed << setprecision(0) << param_sa::ncalcs << endl;
			Output_File << "auto setting temperature(0 -> off, 1 -> on) : " << param_sa::auto_temp << endl;
		#endif
	    	Output_File << "search type(0 -> randam, 1 -> each) : " << param::search_type << std::endl;
    	Output_File << "-------------------------------" << std::endl;
		
		Output_File << "Hosts = " << param::hosts << ", Switches = " << switches << ", Radix = " << param::radix << endl;
		Output_File << "Diameter     = " << diameter << endl;
		Output_File << "Diameter Gap = " << diameter - low_diameter << " (" << diameter << " - " << low_diameter << ")" << endl;
		Output_File << "ASPL         = " << setprecision(10) << ASPL << " (" << fixed << setprecision(0) << sum << "/" << fixed << setprecision(0) << (double)param::hosts * (param::hosts - 1) / 2 << ")"<<endl;
		Output_File << "ASPL Gap     = " << setprecision(10) << ASPL - low_ASPL << " (" << ASPL << " - " << low_ASPL << ")" << endl;
		Output_File << "computingtime : " << time << "sec.\n";
	}

	bool constrain_edges(int a, int b)
	{
		for(unsigned int i = 0; i < edges.size(); i++)
		{
			if((edges[i][0] == a || edges[i][1] == a) && (edges[i][0] == b || edges[i][1] == b))
				return true;
		}
		return false;
	}

	int count_host(void)
	{
		int count = 0;
		for(unsigned int i = 0; i < param::hosts; i++)
		{
			if(edges[i][0] < param::hosts)
				count++;
		}
		return count;
	}
};