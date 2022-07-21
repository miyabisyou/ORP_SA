#include <iostream>
#include <vector>
#include <time.h>
#include <random>
#include "child.hpp"
#include "tool.hpp"
#include "param.hpp"
#include "sa.hpp"
#include "option.hpp"

using namespace std;

int main(int argc, char * argv[])
{	
	printf("BIAS of host?   = %s\n", (ORP_Is_bias())? "Yes" : "No");
	set_param(argc, argv);
	output_setting();
	#if CSV_OUT == 1
		mkdir("./../CSV", S_IRUSR|S_IWUSR|S_IXUSR);
		set_fname();
		ofstream csv_File("./../CSV/" + param_sa::fname + ".csv");
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

		hostswitch indiv;
		indiv.switches = ORP_Optimize_switches(param::hosts, param::radix) + param::offset; 
		indiv.Init();
		set_fname();
		sa(indiv);
		clock_t end = clock();
		#if EDGES_OUT == 1
			indiv.output();
		#endif
		#if RESULT_LOG == 1
			indiv.outputlog((double)(end - start) / CLOCKS_PER_SEC);
		#endif
		#if CSV_OUT == 1
			csv_File << param::seed << ", " << indiv.diameter << ", " << indiv.diameter - indiv.low_diameter << ", " << setprecision(10) << indiv.ASPL << ", " << setprecision(10) << indiv.ASPL - indiv.low_ASPL << ", " << indiv.switches << ", " << (double)(end - start) / CLOCKS_PER_SEC << endl;
		#endif
		
		cout << "computingtime : " << (double)(end - start) / CLOCKS_PER_SEC << "sec.\n";
		param::seed++;
	}
}
