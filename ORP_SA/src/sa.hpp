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
	  ofstream graph_File("./../graph/" + param_sa::fname + ".txt");
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
	  ofstream add_File("./../DoNS/(" + to_string(param::hosts) + ", " + to_string(param::radix) + ")/add_" + param_sa::fname + ".txt");
	  if(!add_File)
    {
  	  std::cout << "dose not open the add file." << std::endl;
   	  exit(0);
    }
    ofstream reduce_File("./../DoNS/(" + to_string(param::hosts) + ", " + to_string(param::radix) + ")/reduce_" + param_sa::fname + ".txt");
	  if(!reduce_File)
    {
    	std::cout << "dose not open the reduce file." << std::endl;
   	  exit(0);
    }
    ofstream swap_File("./../DoNS/(" + to_string(param::hosts) + ", " + to_string(param::radix) + ")/swap_" + param_sa::fname + ".txt");
	  if(!swap_File)
    {
    	std::cout << "dose not open the swap file." << std::endl;
   	  exit(0);
    }
    ofstream swing_File("./../DoNS/(" + to_string(param::hosts) + ", " + to_string(param::radix) + ")/swing_" + param_sa::fname + ".txt");
	  if(!swing_File)
    {
    	std::cout << "dose not open the swing file." << std::endl;
     	exit(0);
    }
    ofstream ave_File("./../DoNS/(" + to_string(param::hosts) + ", " + to_string(param::radix) + ")/ave_" + param_sa::fname + ".txt");
	  if(!ave_File)
    {
    	std::cout << "dose not open the ave file." << std::endl;
   	  exit(0);
    }
  #endif
  #if Accept_rate == 1
    int num_add = 0, num_red = 0, num_swa = 0, num_swi = 0, a_num_add = 0, a_num_red = 0, a_num_swa = 0, a_num_swi = 0;
    int ave_add = 0, ave_red = 0, ave_swa = 0, ave_swi = 0, ave_count_add = 0, ave_count_red = 0, ave_count_swa = 0, ave_count_swi = 0;
    mkdir("./../Accept/", S_IRUSR|S_IWUSR|S_IXUSR);
	  ofstream accept_File("./../Accept/accept_" + param_sa::fname + ".csv");
	  if(!accept_File)
    {
  	  std::cout << "dose not open the accept file." << std::endl;
   	  exit(0);
    }
    accept_File << ", Add_switch, Reduce_switch, swap, swing" <<endl;
  #endif
  #if NUM_OF_SLME == 1
    mkdir("./../SLME", S_IRUSR|S_IWUSR|S_IXUSR);
	  ofstream slme_File("./../SLME/" + param_sa::fname + ".txt");
	  if(!slme_File)
    {
    	std::cout << "dose not open the slme file." << std::endl;
    	exit(0);
    }
  #endif

  double max_temp = param_sa::temp0, min_temp = param_sa::tempF, cool_rate = param_sa::cool_rate;
	int gene = 0, f;
  double temperature = max_temp;
  int last_switches, count_switches = 0, sw_f = 0; //switching
  vector<vector <double>> dons;
	//evaluation
	indiv.evaluation();
  indiv.print_eva();

  #if GRAPH_LOG == 1
    graph_File << "0, " << indiv.diameter << ", " << indiv.ASPL << ", " << indiv.switches << endl;
  #endif
  
  hostswitch child, best_indiv;
  copy_HS(indiv, best_indiv);   //generate best_invid

  last_switches = indiv.switches;

	while (temperature > min_temp)
	{
    copy_HS(indiv, child);
    if(sw_f == 0)
      f = n_search(child);
    else
      f = n_search_exc(child);

    sort_edges(child);
    child.evaluation();
    #if NUM_OF_SLME == 1
      slme_File << gene + 1 << ", " << Num_of_sl(child.edges) << ", " << Num_of_me(child.edges) << endl;
    #endif
    //Difference of neighborhood solution
    #if DoNS == 1
      dons.push_back(vector<double>({(double)f, (double)child.diameter - indiv.diameter, child.ASPL - indiv.ASPL}));
    #endif
    //memory best solution
    if(child.diameter < best_indiv.diameter || (child.diameter == best_indiv.diameter && child.ASPL < best_indiv.ASPL))
      copy_HS(child, best_indiv);
    //accept
    #if Accept_rate == 1
      if(f == 0)
      {
        num_add++;
        ave_count_add++;
      }
      //else if(f == 1 || f == -1)
      else if(f == 1)
      {
        num_red++;
        ave_count_red++;
      }        
      else if(f == 2)
      {
        num_swa++;
        ave_count_swa++;
      }        
      else if(f == 3)
      {
        num_swi++;
        ave_count_swi++;
      }        
    #endif
    if(child.diameter < indiv.diameter || ((child.ASPL <= indiv.ASPL || Fx_A(indiv.ASPL - child.ASPL, temperature, child.switches) >= (double)rand()/RAND_MAX) && child.diameter == indiv.diameter))
    {
      copy_HS(child, indiv);

      #if GRAPH_LOG == 1
        graph_File << gene + 1 << ", " << indiv.diameter << ", " << indiv.ASPL << ", " << indiv.switches << endl;
      #endif
      #if Accept_rate == 1
        if(f == 0)
        {
          a_num_add++;
          ave_add++;
        }
        else if(f == 1)
        {
          a_num_red++;
          ave_red++;
        }          
        else if(f == 2)
        {
          a_num_swa++;
          ave_swa++;
        }          
        else if(f == 3)
        {
          a_num_swi++;
          ave_swi++;
        }          
      #endif
    }
    #if Accept_rate == 1
      if((gene + 1) % (int)(param_sa::ncalcs / NUM_PLOT) == 0)
      {
        accept_File << gene + 1 << ", " << a_num_add / (double)num_add * 100 << ", " << a_num_red / (double)num_red * 100 << ", " << a_num_swa / (double)num_swa * 100 << ", " << a_num_swi / (double)num_swi * 100 << endl;
        a_num_add = 0;
        a_num_red = 0;
        a_num_swa = 0;
        a_num_swi = 0;
        num_add = 0;
        num_red = 0;
        num_swa = 0;
        num_swi = 0;
      }
    #endif

    //switching
    ///*
    if(last_switches == indiv.switches)
      count_switches++;
    else
    {
      last_switches = indiv.switches;
      count_switches = 0;  
    }
    if(count_switches >= param_sa::ncalcs * SWITCHING)
      sw_f = 1;//*/
    
    gene++;
    if(gene % param_sa::iteration == 0)
      temperature *= cool_rate;
    if(param_sa::display != 0 && gene % param_sa::display == 0)
    {
      //printf("\rGEN:%8d, temperature:%12.6f, switch:%4d, ASPL:%12.6f", gene, temperature, indiv.switches, indiv.ASPL);
      printf("\rGEN:%8d, temperature:%12.6f, switch:%4d, ASPL:%12.6f, sw_f:%d", gene, temperature, indiv.switches, indiv.ASPL, sw_f);
      fflush(stdout);
    }  
	}
  #if Accept_rate == 1
    accept_File << "AVERAGE, " << ave_add / (double)ave_count_add * 100 << ", " << ave_red / (double)ave_count_red * 100 << ", " << ave_swa / (double)ave_count_swa * 100 << ", " << ave_swi / (double)ave_count_swi * 100 << endl;
  #endif
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
    ave_File << ", " << setprecision(10) << temp[0] << ", " << setprecision(10) << temp[1];
  }
  ave_File << endl;
  #endif
}

double Fx_A(double delta_E, double T, int switches)
{
  return exp(delta_E * switches * (param::hosts - 1) / T);
}