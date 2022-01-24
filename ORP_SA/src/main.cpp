#include <iostream>
#include <vector>
#include <time.h>
#include <random>
#include "child.hpp"
#include "tool.hpp"
#include "param.hpp"
#include "ga.hpp"
#include "sa.hpp"
#include "option.hpp"

using namespace std;

int main(int argc, char * argv[])
{	
	set_param(argc, argv);
	output_setting();
	#if CSV_OUT == 1
		mkdir("./../CSV", S_IRUSR|S_IWUSR|S_IXUSR);
		#if SOLUTION == 0
			ofstream csv_File("./../CSV/ga_host" + to_string(param::hosts) + "radix" + to_string(param::radix) + "seed" + to_string(param::seed) + "offset" + to_string(param::offset) + ".csv");
		#else
			ofstream csv_File("./../CSV/sa_host" + to_string(param::hosts) + "radix" + to_string(param::radix) + "seed" + to_string(param::seed) + "offset" + to_string(param::offset) + ".csv");
		#endif
		if(!csv_File)
		{
			std::cout << "dose not open the csv file." << std::endl;
			exit(0);
  		}
		csv_File << "Seed, Diametar, Diameter Gap, ASPL, ASPL Gap, switches, computing-time(sec.)" << endl;
	#endif
	for (int test = 0; test < param::tes; test++)
	{
		srand(param::seed);
		randomseed = mt19937(param::seed);
		cout << "seed = " << param::seed << endl;
		clock_t start = clock();
		#if SOLUTION == 0		//GA
			vector<hostswitch>  group;
			intialize(group);
			ga(group);
			string fname = "ga_host" + to_string(param::hosts) + "radix" + to_string(param::radix) + "seed" + to_string(param::seed) + "pop" + to_string(param::pop) + "off" + to_string(param::off) + "offset" + to_string(param::offset);
			clock_t end = clock();
			#if EDGES_OUT == 1
				group[0].output(fname);
			#endif
			#if RESULT_LOG == 1
				group[0].outputlog(fname, (double)(end - start) / CLOCKS_PER_SEC);
			#endif
			#if CSV_OUT == 1
				csv_File << param::seed << ", " << group[0].diameter << ", " << group[0].diameter - group[0].low_diameter << ", " << setprecision(10) << group[0].ASPL << ", " << setprecision(10) << group[0].ASPL - group[0].low_ASPL << ", " << group[0].switches << ", " << (double)(end - start) / CLOCKS_PER_SEC << endl;
			#endif
		#else		//SA
			hostswitch indiv;
			indiv.switches = ORP_Optimize_switches(param::hosts, param::radix) + param::offset; 
			indiv.Init();
			//indiv.show_edges();
			sa(indiv);
			string fname = "sa_host" + to_string(param::hosts) + "radix" + to_string(param::radix) + "seed" + to_string(param::seed) + "offset" + to_string(param::offset);
			clock_t end = clock();
			#if EDGES_OUT == 1
				indiv.output(fname);
			#endif
			#if RESULT_LOG == 1
				indiv.outputlog(fname, (double)(end - start) / CLOCKS_PER_SEC);
			#endif
			#if CSV_OUT == 1
				csv_File << param::seed << ", " << indiv.diameter << ", " << indiv.diameter - indiv.low_diameter << ", " << setprecision(10) << indiv.ASPL << ", " << setprecision(10) << indiv.ASPL - indiv.low_ASPL << ", " << indiv.switches << ", " << (double)(end - start) / CLOCKS_PER_SEC << endl;
			#endif
		#endif
		cout << "computingtime : " << (double)(end - start) / CLOCKS_PER_SEC << "sec.\n";
		param::seed++;
	}
}
