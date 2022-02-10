#pragma once
#include <vector>
#include <algorithm>
#include "child.hpp"
#include "param.hpp"
#include "tool.hpp"

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
void add_switch(hostswitch &child)
{
  //Search for open ports
  child.check_port();
  int s_num = param::hosts + child.switches;
  //generat label
  vector<int> label;
  for(unsigned int i = 0; i < child.edges.size(); i++)
  {
    if(child.port_f != child.edges[i][0] && child.port_f != child.edges[i][1])
      label.push_back(i);
  }
  //select edges
  shuffle(label.begin(), label.end(), randomseed);

  //connect to new switch
  for(int i = 0; i < param::radix / 2; i++)
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
void reduce_switch(hostswitch &child)
{
  if((child.switches - 1) * (param::radix - 2) + 2 <= param::hosts)
    return;
  vector<vector<int>> temp_edges;
  vector<int> index, ind_val;
  int f = 1, s_num, aj_count = 0;
  //decide switch
  vector <int> label(param::hosts + child.switches, 0);
  vector <int> ck(param::hosts + child.switches, 0); 
  for(unsigned int i = param::hosts; i < child.edges.size(); i++)
  {
    label[child.edges[i][0]]++;
    label[child.edges[i][1]]++;
    if(child.edges[i][0] == child.edges[i][1])
      ck[child.edges[i][1]]++;
  }
  vector <int> aj_label;
  for(unsigned int i = param::hosts; i < label.size(); i++)
  {
    if(label[i] - ck[i] * 2 >= param::radix - label[i])
      aj_label.push_back(i);
  }
  if(aj_label.size() == 0)
    return;

  shuffle(aj_label.begin(), aj_label.end(), randomseed);
  do
  {
    index.clear();
    ind_val.clear();
    temp_edges.clear();
    s_num = aj_label[aj_count];
    int h_count = 0;
    for(unsigned int i = 0; i < child.edges.size(); i++)
    {
      if(child.edges[i][0] == s_num || child.edges[i][1] == s_num)
      {
        if(child.edges[i][0] != child.edges[i][1])
        {
          index.push_back(i);
          ind_val.push_back(child.edges[i][0] + child.edges[i][1] - s_num);
          if(ind_val.back() < param::hosts)
            h_count++;
        }
        else
          index.push_back(i);
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

int n_search_rand(hostswitch &indiv)
{
  int f = 0;
  f = rand() % 4;
  if(f == 0)
    add_switch(indiv);
  else if(f == 1)
    reduce_switch(indiv);
  else if(f == 2)
    swap(indiv);
  else if(f == 3)
    swing(indiv);
    
  return f;
}

int n_search_each(hostswitch &indiv)
{
  int f = 0;
  f = rand() % 3;
  if(f == 0)
    add_switch(indiv);
  else if(f == 1)
    reduce_switch(indiv);
  f = rand() % 2;
  if(f == 0)
    swap(indiv);
  f = rand() % 2;
  if(f == 0)
    swing(indiv);

  return f;
}

void n_search_each(vector<hostswitch> &group)
{
  int f = 0;
  for(unsigned int i = 0; i < group.size(); i++)
  {
    f = rand() % 3;
    if(f == 0)
      add_switch(group[i]);
    else if(f == 1)
      reduce_switch(group[i]);        
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
      add_switch(group[i]);
    else if(f == 1)
      reduce_switch(group[i]);        
    else if(f == 2)
      swap(group[i]);
    else if(f == 3)
      swing(group[i]);
  }
}