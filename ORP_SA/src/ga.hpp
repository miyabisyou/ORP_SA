#pragma once
#include <vector>
#include <algorithm>
#include "child.hpp"
#include "param.hpp"
#include "tool.hpp"
#include "search.hpp"

void ga(vector<hostswitch> &group)
{
	int gene = 0, geneth = 0;
	//evaluation
	all_eva(group);
  vector<vector<hostswitch>> child_group;
  child_group.resize(param::pop);
  for(unsigned int i = 0; i < child_group.size(); i++)
    child_group[i].resize(param::off);
  sort_group(group);
  group[0].print_eva();
  while (gene != param::gen && geneth != param::genth)
	{
    shuffle(group.begin(), group.end(), randomseed);
    copy_grouptoc(group, child_group);

    for(unsigned int i = 0; i < child_group.size(); i++)
    {
      if(param::search_type == 0)
        n_search_rand(child_group[i]);
      else
        n_search_each(child_group[i]);
      all_eva(child_group[i]);
      sort_group(child_group[i]);
    }
    int f = 0;
    for(unsigned int i = 0; i < child_group.size(); i++)
    {
      if(child_group[i][0].ASPL < group[i].ASPL && child_group[i][0].diameter <= group[i].diameter)
      {
        copy_HS(child_group[i][0], group[i]);
        f = 1;
      }
    }
    if(f == 0)
      geneth++;
    else
      geneth = 0;
    gene++;
    printf("\rGEN:%8d(/%d), Generations without solution updates:%8d(/%d)", gene, param::gen, geneth, param::genth);
    fflush(stdout);
	}
  cout << endl;
  sort_group(group);
  group[0].print_eva();
}