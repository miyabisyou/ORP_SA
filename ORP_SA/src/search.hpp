#pragma once
#include <vector>
#include <algorithm>
#include "child.hpp"
#include "param.hpp"
#include "tool.hpp"

void swap_int(int *a, int *b)
{
  int temp = *a;
  *a = *b;
  *b = temp;
}

//swap switch
void swap(hostswitch &child)
{
  int *edge = (int*)malloc(sizeof(int) * child.lines * 2);
	int *adjacency = (int*)malloc(sizeof(int) * child.switches * param::radix);
	int *s_degree = (int*)malloc(sizeof(int) * child.switches);
	int *h_degree = (int*)malloc(sizeof(int) * child.switches);

  child.vec_to_arr(edge);
  ORP_Conv_edge2adjacency(param::hosts, child.switches, param::radix, child.lines, edge, adjacency);
  ORP_Set_degrees(param::hosts, child.switches, child.lines, edge, h_degree, s_degree);
  ORP_Swap_adjacency(child.switches, param::radix, s_degree, &child.r, adjacency);
  ORP_Conv_adjacency2edge(param::hosts, child.switches, param::radix, h_degree, s_degree, adjacency, edge);
  child.arr_to_vec(edge);

	SAFE_FREE(h_degree);
  SAFE_FREE(s_degree);
	SAFE_FREE(edge);
  SAFE_FREE(adjacency);
}

//swap the host and switch
void swing(hostswitch &child)
{
   int *edge = (int*)malloc(sizeof(int) * child.lines * 2);
	int *adjacency = (int*)malloc(sizeof(int) * child.switches * param::radix);
	int *s_degree = (int*)malloc(sizeof(int) * child.switches);
	int *h_degree = (int*)malloc(sizeof(int) * child.switches);

  child.vec_to_arr(edge);
  ORP_Conv_edge2adjacency(param::hosts, child.switches, param::radix, child.lines, edge, adjacency);
  ORP_Set_degrees(param::hosts, child.switches, child.lines, edge, h_degree, s_degree);
  ORP_Swing_adjacency(child.switches, param::radix, h_degree, s_degree, &child.r, adjacency);
  ORP_Conv_adjacency2edge(param::hosts, child.switches, param::radix, h_degree, s_degree, adjacency, edge);
  child.arr_to_vec(edge);

	SAFE_FREE(h_degree);
  SAFE_FREE(s_degree);
	SAFE_FREE(edge);
  SAFE_FREE(adjacency);
}

//increase switch
void inc_switch(hostswitch &child)
{
  //Search for open ports
  child.check_port();
  int s_num = param::hosts + child.switches;
  int f, safe = 0;
  int count = param::radix / 2;
  //generat label
  vector<int> label;
  for(unsigned int i = 0; i < child.edges.size(); i++)
  {
    if(child.port_f != child.edges[i][0] && child.port_f != child.edges[i][1])
      label.push_back(i);
  }
  //select edges
  do
  {
    shuffle(label.begin(), label.end(), randomseed);
    //check duplicate
    f = 0;
    for(int i = 0; i < count - 1; i++)
    {
      for(int j = i + 1; j < count; j++)
      {
        if(child.edges[label[i]][0] == child.edges[j][0] || child.edges[label[i]][0] == child.edges[label[j]][1] || child.edges[label[i]][1] == child.edges[label[j]][0] || child.edges[i][1] == child.edges[label[j]][1])
        {
          f = 1;
          safe++;
          break;
        }
      }
      if(f == 1)
        break;
    }
  }while(f == 1 && safe < SAFE);
  if(safe == SAFE)
    return;
  //connect to new switch
  for(int i = 0; i < count; i++)
  {
    child.edges.push_back(vector<int>({child.edges[label[i]][1], s_num}));
    child.edges[label[i]][1] = s_num;
  }
  //when radix is odd(when there is a surplus port)
  if(child.port_f != -1)
    child.edges.push_back(vector<int>({child.port_f, s_num}));
  //generate edges between surplus ports
  child.switches++;
  child.lines = (int)child.edges.size();
}

//decrease switch
void de_switch(hostswitch &child)
{
  if((child.switches - 1) * (param::radix - 2) + 2 < param::hosts)
    return;
  vector<vector<int>> temp_edges;
  vector<int> index, ind_val;
  int f = 1, s_num, aj_count = 0;
  int safe = 0;
  //decide switch
  vector <int> label(param::hosts + child.switches, 0);
  for(unsigned int i = param::hosts; i < child.edges.size(); i++)
  {
    label[child.edges[i][0]]++;
    label[child.edges[i][1]]++;
  }
  vector <int> aj_label;
  for(unsigned int i = param::hosts; i < label.size(); i++)
  {
    if (label[i] >= (int)(param::radix / 2.0 + 0.5))
      aj_label.push_back(i);
  }
  if(aj_label.size() == 0)
    return;
  shuffle(aj_label.begin(), aj_label.end(), randomseed);
  do
  {
    index.clear();
    ind_val.clear();
    s_num = aj_label[aj_count];
    int h_count = 0;
    for(unsigned int i = 0; i < child.edges.size(); i++)
    {
      if(child.edges[i][0] == s_num || child.edges[i][1] == s_num)
      {
        index.push_back(i);
        ind_val.push_back(child.edges[i][0] + child.edges[i][1] - s_num);
        if(ind_val.back() < param::hosts)
          h_count++;
      }
    }
    sort(ind_val.begin(), ind_val.end());
    shuffle(ind_val.begin() + h_count, ind_val.end(), randomseed);
    if(ind_val.size() % 2 == 0)
    {
      for(unsigned int i = 0; i < (int)(ind_val.size() / 2); i++)
        temp_edges.push_back(vector<int>({ind_val[i], ind_val[ind_val.size() - i - 1]}));
    }
    else
    {
      //Search for open ports
      child.check_port();
      int bias = 1;
      if(child.port_f != -1)
      {
        temp_edges.push_back(vector<int>({child.port_f, ind_val.back()}));
        bias = 2;
      }
      for(int i = 0; i < (int)(ind_val.size() / 2); i++)
        temp_edges.push_back(vector<int>({ind_val[i], ind_val[ind_val.size() - i - bias]}));
      
      if(child.port_f == ind_val.back())
        f = 0;
    }
    //new edges check
    for(unsigned int i = 0; i < temp_edges.size(); i++)
    {
      if(f == 0)
        break;
      if(child.constrain_edges(temp_edges[i][0], temp_edges[i][1]))
      {
        f = 0;
        break;
      }
    }
    aj_count++;
  }while(f == 0 && aj_count < (int)aj_label.size());
  if(aj_count == (int)aj_label.size())
    return;
  //Mounting of new edges and erase of switch
  sort(index.begin(), index.end());
  for(int i = index.size() - 1; i >= 0; i--)
    child.edges.erase(child.edges.begin() + index[i]);
  for(unsigned int i = 0; i < temp_edges.size(); i++)
    child.edges.push_back(temp_edges[i]);
  if(s_num != param::hosts + child.switches - 1)
  {
    for(unsigned int i = 0; i < child.edges.size(); i++)
    {
      if(child.edges[i][0] == param::hosts + child.switches - 1)
        child.edges[i][0] = s_num;
      if(child.edges[i][1] == param::hosts + child.switches - 1)
        child.edges[i][1] = s_num;
    }
  }
  child.switches--;
  child.lines = (int)child.edges.size();
}

void n_search_rand(hostswitch &indiv)
{
  int f = 0;
  f = rand() % 3;
  //cout << "f=" << f << endl;
  //indiv.show_edges();
  //cout << indiv.switches << ", " << indiv.port_f << endl;
  //indiv.check_edge();
  //indiv.show_edges();
  //cout << "f=" << f << endl;
  if(f == 0)
    inc_switch(indiv);
  else if(f == 1)
    de_switch(indiv);
  else if(f == 2)
    swap(indiv);
  else if(f == 3)
    swing(indiv);
}

void n_search_each(hostswitch &indiv)
{
  int f = 0;
  f = rand() % 3;
  if(f == 0)
    inc_switch(indiv);
  else if(f == 1)
    de_switch(indiv);
  f = rand() % 2;
  if(f == 0)
    swap(indiv);
  f = rand() % 2;
  if(f == 0)
    swing(indiv);
}

void n_search_each(vector<hostswitch> &group)
{
  int f = 0;
  for(unsigned int i = 0; i < group.size(); i++)
  {
    f = rand() % 3;
    if(f == 0)
      inc_switch(group[i]);
    else if(f == 1)
      de_switch(group[i]);        
    f = rand() % 2;
    if(f == 0)
      swap(group[i]);
    f = rand() % 2;
    if(f == 0)
      swing(group[i]);
  }
}

void n_search_rand(vector<hostswitch> &group)
{
  int f = 0;
  for(unsigned int i = 0; i < group.size(); i++)
  {
    f = rand() % 4;
    if(f == 0)
      inc_switch(group[i]);
    else if(f == 1)
      de_switch(group[i]);        
    else if(f == 2)
      swap(group[i]);
    else if(f == 3)
      swing(group[i]);
  }
}