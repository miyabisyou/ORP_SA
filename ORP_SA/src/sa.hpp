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

  double max_temp = param_sa::temp0, min_temp = param_sa::tempF, cool_rate = param_sa::cool_rate;
	int gene = 0;
  double temperature = max_temp;
	//evaluation
	indiv.evaluation();
  indiv.print_eva();

  #if GRAPH_LOG == 1
  graph_File << "0, " << indiv.diameter << ", " << indiv.ASPL << ", " << indiv.switches << endl;
  //graph_File << "0, " << indiv.diameter << ", " << indiv.ASPL - indiv.low_ASPL << ", " << indiv.switches << endl;
  #endif
  
  hostswitch child;
	while (temperature > min_temp)
	{
    copy_HS(indiv, child);
    //child.show_edges();
    //cout<<child.port_f<<endl;
    //inc_switch(child);/*
    if(param::search_type == 0)
      n_search_rand(child);
    else
      n_search_each(child);//*/
    sort_edges(child);
    child.evaluation();
    if((child.ASPL < indiv.ASPL || Fx_A(indiv.ASPL - child.ASPL, temperature, child.switches) >= (double)rand()/RAND_MAX) && child.diameter <= indiv.diameter) 
    {
      copy_HS(child, indiv);
      #if GRAPH_LOG == 1
      graph_File << gene + 1 << ", " << indiv.diameter << ", " << indiv.ASPL << ", " << indiv.switches << endl;
      //graph_File << gene + 1 << ", " << indiv.diameter << ", " << indiv.ASPL - indiv.low_ASPL << ", " << indiv.switches << endl;
      #endif
    }
    gene++;
    if(gene % param_sa::iteration == 0)
      temperature *= cool_rate;
    printf("\rGEN:%8d, temperature:%12.6f", gene, temperature);
    fflush(stdout);
	}
  cout << endl;
  indiv.evaluation();
  indiv.print_eva();
}

double Fx_A(double delta_E, double T, int switches)
{
  return exp(delta_E * switches * (param::hosts - 1) / T);
}