#pragma once
#include <vector>
#include <algorithm>
#include <iostream>
#include "child.hpp"
#include "param.hpp"
#include "tool.hpp"
#include "search.hpp"

double Fx_A(double delta_E, double T, int switches);

void sa(hostswitch &indiv)
{
  #if GRAPH_LOG == 1
    mkdir("./../graph", S_IRUSR|S_IWUSR|S_IXUSR);
	  ofstream graph_File("./../graph/sa_host" + to_string(param::hosts) + "radix" + to_string(param::radix) + "seed" + to_string(param::seed) + "offset" + to_string(param::offset) + ".txt");
	  if(!graph_File)
    {
    	std::cout << "dose not open the graph file." << std::endl;
    	exit(0);
    }
  #endif
  #if DoNS == 1
    mkdir("./../DoNS", S_IRUSR|S_IWUSR|S_IXUSR);
    string name = "./../DoNS/(" + to_string(param::hosts) + ", " + to_string(param::radix) + ")";
    mkdir(name.c_str(), S_IRUSR|S_IWUSR|S_IXUSR);
	  ofstream add_File("./../DoNS/(" + to_string(param::hosts) + ", " + to_string(param::radix) + ")/sa_add_host" + to_string(param::hosts) + "radix" + to_string(param::radix) + "seed" + to_string(param::seed) + "offset" + to_string(param::offset) + ".txt");
	  if(!add_File)
    {
  	  std::cout << "dose not open the add file." << std::endl;
   	  exit(0);
    }
    ofstream reduce_File("./../DoNS/(" + to_string(param::hosts) + ", " + to_string(param::radix) + ")/sa_reduce_host" + to_string(param::hosts) + "radix" + to_string(param::radix) + "seed" + to_string(param::seed) + "offset" + to_string(param::offset) + ".txt");
	  if(!reduce_File)
    {
    	std::cout << "dose not open the reduce file." << std::endl;
   	  exit(0);
    }
    ofstream swap_File("./../DoNS/(" + to_string(param::hosts) + ", " + to_string(param::radix) + ")/sa_swap_host" + to_string(param::hosts) + "radix" + to_string(param::radix) + "seed" + to_string(param::seed) + "offset" + to_string(param::offset) + ".txt");
	  if(!swap_File)
    {
    	std::cout << "dose not open the swap file." << std::endl;
   	  exit(0);
    }
    ofstream swing_File("./../DoNS/(" + to_string(param::hosts) + ", " + to_string(param::radix) + ")/sa_swing_host" + to_string(param::hosts) + "radix" + to_string(param::radix) + "seed" + to_string(param::seed) + "offset" + to_string(param::offset) + ".txt");
	  if(!swing_File)
    {
    	std::cout << "dose not open the swing file." << std::endl;
     	exit(0);
    }
    ofstream ave_File("./../DoNS/(" + to_string(param::hosts) + ", " + to_string(param::radix) + ")/sa_ave_host" + to_string(param::hosts) + "radix" + to_string(param::radix) + "seed" + to_string(param::seed) + "offset" + to_string(param::offset) + ".txt");
	  if(!ave_File)
    {
    	std::cout << "dose not open the ave file." << std::endl;
   	  exit(0);
    }
  #endif

  double max_temp = param_sa::temp0, min_temp = param_sa::tempF, cool_rate = param_sa::cool_rate;
	int gene = 0, f;
  double temperature = max_temp;
  vector<vector <double>> dons;
	//evaluation
	indiv.evaluation();
  indiv.print_eva();

  #if GRAPH_LOG == 1
    graph_File << "0, " << indiv.diameter << ", " << indiv.ASPL << ", " << indiv.switches << endl;
  #endif
  
  hostswitch child, best_indiv;
  copy_HS(indiv, best_indiv);   //generate best_invid

  //indiv.show_edges_graph();

	while (temperature > min_temp)
	{
    copy_HS(indiv, child);
    if(param::search_type == 0)
      f = n_search_rand(child);
    else
      f = n_search_each(child);
    sort_edges(child);
    child.evaluation();
    //Difference of neighborhood solution
    #if DoNS == 1
      dons.push_back(vector<double>({(double)f, (double)child.diameter - indiv.diameter, child.ASPL - indiv.ASPL}));
    #endif
    //memory best solution
    if(child.diameter < best_indiv.diameter || (child.diameter == best_indiv.diameter && child.ASPL < best_indiv.ASPL))
      copy_HS(child, best_indiv);
    //accept
    //if((child.ASPL < indiv.ASPL || Fx_A(indiv.ASPL - child.ASPL, temperature, child.switches) >= (double)rand()/RAND_MAX) && child.diameter <= indiv.diameter)
    if(child.diameter < indiv.diameter || ((child.ASPL <= indiv.ASPL || Fx_A(indiv.ASPL - child.ASPL, temperature, child.switches) >= (double)rand()/RAND_MAX) && child.diameter == indiv.diameter))
    {
      copy_HS(child, indiv);

      //cout << "assept" << endl;

      #if GRAPH_LOG == 1
      graph_File << gene + 1 << ", " << indiv.diameter << ", " << indiv.ASPL << ", " << indiv.switches << endl;
      #endif
    }
    gene++;
    if(gene % param_sa::iteration == 0)
      temperature *= cool_rate;
    printf("\rGEN:%8d, temperature:%12.6f, switch:%4d, ASPL:%12.6f", gene, temperature, indiv.switches, indiv.ASPL);
    //printf("\rGEN:%8d, temperature:%12.6f", gene, temperature);
    fflush(stdout);
	}
  cout << endl;
  copy_HS(best_indiv, indiv);
  indiv.evaluation();
  indiv.print_eva();

  #if DoNS == 1
  for(unsigned int i = 0; i < dons.size(); i++)
  {
    if(dons[i][0] == 0)
      add_File << i << ", " << dons[i][1] << ", " << dons[i][2] << endl;
    else if(dons[i][0] == 1)
      reduce_File << i << ", " << dons[i][1] << ", " << dons[i][2] << endl;
    else if(dons[i][0] == 2)
      swap_File << i << ", " << dons[i][1] << ", " << dons[i][2] << endl;
    else
      swing_File << i << ", " << dons[i][1] << ", " << dons[i][2] << endl; 
  }
  //ave_File << "Kind of Operator : Ave_Diameter, Ave_ASPL  (0 -> Add_switch, 1 -> Reduce_switch, 2 -> swap, 3 -> swing)" << endl;
  ave_File << param::seed;
  for(int i = 0; i < 4; i++)
  {
    vector<double> temp;
    temp = ave_dons(i, dons);
    //ave_File << i << " : " << setprecision(10) << temp[0] << ", " << setprecision(10) << temp[1] << endl;
    ave_File << ", " << setprecision(10) << temp[0] << ", " << setprecision(10) << temp[1];
  }
  ave_File << endl;
  #endif
}

double Fx_A(double delta_E, double T, int switches)
{
  return exp(delta_E * switches * (param::hosts - 1) / T);
}