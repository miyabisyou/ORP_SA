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
  
  //reduce multiple edges
  vector <vector<int>>mul_edges = check_double_edge(child.edges, s_num);
  if(mul_edges.size() > 0)
  {
    if(mul_edges.size() > 1)
      shuffle(mul_edges.begin(), mul_edges.end(), randomseed);
    int *edge = (int*)malloc(sizeof(int) * child.lines * 2);
	  int *adjacency = (int*)malloc(sizeof(int) * child.switches * param::radix);
	  int *s_degree = (int*)malloc(sizeof(int) * child.switches);
	  int *h_degree = (int*)malloc(sizeof(int) * child.switches);
    int selected_edge[] = {mul_edges[0][0], mul_edges[0][1]};

    child.vec_to_arr(edge);
    ORP_Conv_edge2adjacency(param::hosts, child.switches, param::radix, child.lines, edge, adjacency);
    ORP_Set_degrees(param::hosts, child.switches, child.lines, edge, h_degree, s_degree);
    
    if(rand() % 2 == 0)
      ORP_Swap_adjacency_with_one_edge(param::hosts, child.switches, param::radix, selected_edge, s_degree, &child.r, adjacency);
    else
      ORP_Swing_adjacency_with_one_edge(param::hosts, child.switches, param::radix, selected_edge, h_degree, s_degree, &child.r, adjacency);
    
    ORP_Conv_adjacency2edge(param::hosts, child.switches, param::radix, h_degree, s_degree, adjacency, edge);
    child.arr_to_vec(edge);

  	SAFE_FREE(h_degree);
    SAFE_FREE(s_degree);
  	SAFE_FREE(edge);
    SAFE_FREE(adjacency);
  }
}

//decrease switch
void reduce_switch(hostswitch &child)
{
  if((child.switches - 1) * (param::radix - 2) + 2 <= param::hosts)
    return;

  //Select Switch_number
  int s_num = rand() % child.switches + param::hosts;
  

  //Generate Edges_table
  vector <vector<int>> edge_table;
	edge_table.resize(param::hosts + child.switches);
	for(unsigned int i = 0; i < child.edges.size(); i++)
	{
		edge_table[child.edges[i][0]].push_back(child.edges[i][1]);
		edge_table[child.edges[i][1]].push_back(child.edges[i][0]);
	}

  //Count of hosts and switches on the switch
  vector <int> h_count, s_count;
  for(unsigned int i = 0; i < edge_table[s_num].size(); i++)
  {
    if(edge_table[s_num][i] < param::hosts)
      h_count.push_back(edge_table[s_num][i]);
    else if(edge_table[s_num][i] != s_num)
      s_count.push_back(edge_table[s_num][i]);
  }
  //cout << "s_num = " << s_num << endl;

  //Generate new edges
  vector <vector<int>> new_edges;
  shuffle(h_count.begin(), h_count.end(), randomseed);
  shuffle(s_count.begin(), s_count.end(), randomseed);
  if(h_count.size() <= s_count.size())
  {
    while(h_count.size() > 0)
    {
      new_edges.push_back(vector<int>({h_count.back(), s_count.back()}));
      h_count.pop_back();
      s_count.pop_back();
    }
    while(s_count.size() > 1)
    {
      new_edges.push_back(vector<int>({s_count.front(), s_count.back()}));
      s_count.erase(s_count.begin());
      s_count.pop_back();
    }
    child.check_port();
    if(s_count.size() == 1 && child.port_f != -1 && child.port_f != s_num)
      new_edges.push_back(vector<int>({s_count.front(), child.port_f}));
  }
  else
  {
    while(s_count.size() > 0)
    {
      new_edges.push_back(vector<int>({h_count.back(), s_count.back()}));
      h_count.pop_back();
      s_count.pop_back();
    }
    child.check_port();
    if(child.port_f != -1 && child.port_f != s_num)
    {      
      new_edges.push_back(vector<int>({h_count.back(), child.port_f}));
      h_count.pop_back();
    }
    
    //connect to bouble edge
    if(h_count.size() > 0)
    {
      unsigned int dl_count = 0;
      vector <vector<int>> dl_edge = check_double_edge(child.edges, s_num);
      if(dl_edge.size() > 0)
      {
        shuffle(dl_edge.begin(), dl_edge.end(), randomseed);
        while(dl_count < dl_edge.size() && h_count.size() > 1)
        {
          new_edges.push_back(vector<int>({h_count.back(), dl_edge[dl_count][0]}));
          h_count.pop_back();
          new_edges.push_back(vector<int>({h_count.back(), dl_edge[dl_count][1]}));
          h_count.pop_back();
          dl_count++;
        }
        if(dl_count < dl_edge.size() && h_count.size() == 1)
        {
          new_edges.push_back(vector<int>({h_count.back(), dl_edge[dl_count][0]}));
          h_count.pop_back();
          dl_count++;
        }
        while(dl_count > 0)
        {
          for(unsigned int i = 0; i < child.edges.size(); i++)
          {
            if(child.edges[i][0] == dl_edge[dl_count - 1][0] && child.edges[i][1] == dl_edge[dl_count - 1][1])
            {
              child.edges.erase(child.edges.begin() + i);
              break;
            }
          }
          dl_count--;
        }
      }
    }

    //connect to self loop
    if(h_count.size() > 0)
    {
      unsigned int sl_count = 0;
      vector <int> sl_edge = check_self_loop(edge_table, s_num);
      if(sl_edge.size() > 0)
      {
        shuffle(sl_edge.begin(), sl_edge.end(), randomseed);
        while(sl_count < sl_edge.size() && h_count.size() > 1)
        {
          new_edges.push_back(vector<int>({h_count.back(), sl_edge[sl_count]}));
          h_count.pop_back();
          new_edges.push_back(vector<int>({h_count.back(), sl_edge[sl_count]}));
          h_count.pop_back();
          sl_count++;
        }
        if(sl_count < sl_edge.size() && h_count.size() == 1)
        {
          new_edges.push_back(vector<int>({h_count.back(), sl_edge[sl_count]}));
          h_count.pop_back();
          sl_count++;
        }
        while(sl_count > 0)
        {
          for(unsigned int i = 0; i < child.edges.size(); i++)
          {
            if(child.edges[i][0] == sl_edge[sl_count - 1] && child.edges[i][1] == sl_edge[sl_count - 1])
            {
              child.edges.erase(child.edges.begin() + i);
              break;
            }
          }
          sl_count--;
        }
      }
    }

    //Randomly select edges
    if(h_count.size() > 0)
    {
      unsigned int ss_count = 0;
      vector <vector<int>> ss_edge = check_stos_edge(child.edges, s_num);
      if(ss_edge.size() > 0)
      {
        shuffle(ss_edge.begin(), ss_edge.end(), randomseed);
        while(h_count.size() > 1)
        {
          new_edges.push_back(vector<int>({h_count.back(), ss_edge[ss_count][0]}));
          h_count.pop_back();
          new_edges.push_back(vector<int>({h_count.back(), ss_edge[ss_count][1]}));
          h_count.pop_back();
          ss_count++;
        }
        if(h_count.size() == 1)
        {
          new_edges.push_back(vector<int>({h_count.back(), ss_edge[ss_count][0]}));
          h_count.pop_back();
          ss_count++;
        }
      }
      while(ss_count > 0)
      {
        for(unsigned int i = 0; i < child.edges.size(); i++)
        {
          if(child.edges[i][0] == ss_edge[ss_count - 1][0] && child.edges[i][1] == ss_edge[ss_count - 1][1])
          {
            child.edges.erase(child.edges.begin() + i);
            break;
          }
        }
        ss_count--;
      }
    }
  }
  for(unsigned int i = 0; i < child.edges.size(); i++)
  {
    if(child.edges[i][0] == s_num || child.edges[i][1] == s_num)
    {
      child.edges.erase(child.edges.begin() + i);
      i--;
    }
  }
  for(unsigned int i = 0; i < new_edges.size(); i++)
    child.edges.push_back(new_edges[i]);
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
  //cout  << endl;
  //cout << f << endl;
  if(f == 0)
    add_switch(indiv);
  else if(f == 1)
    reduce_switch(indiv);
  else if(f == 2)
    swap(indiv);
  else if(f == 3)
    swing(indiv);

  //indiv.show_edges_graph();
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