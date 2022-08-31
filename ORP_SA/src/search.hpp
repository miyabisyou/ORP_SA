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
  if (param::type == 0)
  {
    for(unsigned int i = 0; i < child.edges.size(); i++)
      label.push_back(i);
  }
  else
  {
    vector<int> temp;
    temp = check_num_of_me(child.edges);
    label = temp;
    temp = check_num_self_loop(child.edges);
    label.insert(label.end(), temp.begin(), temp.end());
    if(label.size() > 1)
    {
      for(int i = 0; i < label.size() - 1; i++)
      {
        for(unsigned int j = i + 1; j < label.size(); j++)
        {
          if(label[i] == label[j])
          {
            label.erase(label.begin() + j);
            j--;
          }
        }
      } 
    }
  
    if(label.size() < param::radix / 2)
    {
      temp.clear();
      temp.resize(child.edges.size());
      for(unsigned int i = 0; i < child.edges.size(); i++)
        temp[i] = i;
      for(unsigned int i = 0; i < label.size(); i++)
        temp.erase(find(temp.begin(), temp.end(), label[i]));
      if(temp.size() > 1)
        shuffle(temp.begin(), temp.end(), randomseed);
      label.insert(label.end(), temp.begin(), temp.begin() + param::radix / 2 - label.size());
    }
  }
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
void remove_switch(hostswitch &child)
{
  if((child.switches - 1) * (param::radix - 2) + 2 <= param::hosts)
    return;

  //select switch
  int s_num;
  if(param::type == 0)
    s_num = rand() % child.switches + param::hosts;
  else
    s_num = select_SLME_num(child.edges, child.switches);

  //Count of hosts and switches on the switch
  vector <int> h_count, s_count;
  int count = 0;
  for(unsigned int i = 0; i < child.edges.size(); i++)
  {
    if(child.edges[i][0] == s_num || child.edges[i][1] == s_num)
    {
      if(child.edges[i][0] + child.edges[i][1] - s_num < param::hosts)
        h_count.push_back(child.edges[i][0] + child.edges[i][1] - s_num);
      else if(child.edges[i][0] != child.edges[i][1])
        s_count.push_back(child.edges[i][0] + child.edges[i][1] - s_num);
      count++;
    }
    if(count == param::radix)
      break;
  }

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
    if(h_count.size() > 0)
    {
      vector<int> temp = check_num_of_me(child.edges);
      vector<int> label = temp;
      temp = check_num_self_loop(child.edges);
      label.insert(label.end(), temp.begin(), temp.end());
      for(int i = 0; i < label.size(); i++)
      {
        if(child.edges[label[i]][0] == s_num || child.edges[label[i]][1] == s_num)
        {
          label.erase(label.begin() + i);
          i--;
        }
      }
      for(int i = 0; i < (int)label.size() - 1; i++)
      {
        for(unsigned int j = i + 1; j < label.size(); j++)
        {
          if(label[i] == label[j])
          {
            label.erase(label.begin() + j);
            j--;
          }
        }
      }
      
      if(label.size() < (h_count.size() + 1) / 2)
      {
        temp.clear();
        temp.resize(child.edges.size());
        for(unsigned int i = param::hosts; i < child.edges.size(); i++)
          temp[i] = i;
        for(unsigned int i = 0; i < label.size(); i++)
          temp.erase(find(temp.begin(), temp.end(), label[i]));
        for(unsigned int i = 0; i < temp.size(); i++)
        {
          if(child.edges[temp[i]][0] < param::hosts || child.edges[temp[i]][0] == s_num || child.edges[temp[i]][1] == s_num)
          {
            temp.erase(temp.begin() + i);
            i--;
          }
        }
        if(temp.size() > 1)
          shuffle(temp.begin(), temp.end(), randomseed);
        label.insert(label.end(), temp.begin(), temp.begin() + (h_count.size() + 1) / 2 - label.size());
        shuffle(label.begin(), label.end(), randomseed);
      }
      else
      {
        shuffle(label.begin(), label.end(), randomseed);
        label.erase(label.begin(), label.begin() + label.size() - (h_count.size() + 1) / 2);
      } 

      for(unsigned int i = 0; i < label.size() - 1; i++)
      {
        new_edges.push_back(vector<int>({h_count.back(), child.edges[label[i]][0]}));
        h_count.pop_back();
        new_edges.push_back(vector<int>({h_count.back(), child.edges[label[i]][1]}));
        h_count.pop_back();
      }
      new_edges.push_back(vector<int>({h_count.back(), child.edges[label.back()][0]}));
      h_count.pop_back();
      if(h_count.size() == 1)
      {
        new_edges.push_back(vector<int>({h_count.back(), child.edges[label.back()][1]}));
        h_count.pop_back();
      }

      sort(label.begin(), label.end());
      for(int i = label.size(); i > 0; i--)
        child.edges.erase(child.edges.begin() + label[i - 1]);
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

int n_search(hostswitch &indiv)
{
  int f = 0;
  f = rand() % 4;
  if(f == 0)
    add_switch(indiv);
  else if(f == 1)
    remove_switch(indiv);
  else if(f == 2)
    swap(indiv);
  else if(f == 3)
    swing(indiv);
  return f;
}

int n_search_exc(hostswitch &indiv)
{
  int f = 0;
  f = rand() % 2;
  if(f == 0)
    swap(indiv);
  else if(f == 1)
    swing(indiv);
  return f;
}