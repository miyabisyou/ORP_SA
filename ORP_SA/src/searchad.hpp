#pragma once
#include <vector>
#include <algorithm>
#include "child.hpp"
#include "param.hpp"
#include "tool.hpp"

//swap switch
void swap(hostswitch &child)
{
  int *adjacency = (int*)malloc(sizeof(int) * child.switches * param::radix);
	int *s_degree = (int*)malloc(sizeof(int) * child.switches);
	int *h_degree = (int*)malloc(sizeof(int) * child.switches);

  child.d_vtoa(h_degree, s_degree);
  child.adja_vtoa(adjacency);
  ORP_Swap_adjacency(child.switches, param::radix, s_degree, &child.r, adjacency); 
  child.adja_atov(adjacency);
  child.d_atov(h_degree, s_degree);

	free(adjacency);
	free(h_degree);
	free(s_degree);
}

//swap the host and switch
void swing(hostswitch &child)
{
	int *adjacency = (int*)malloc(sizeof(int) * child.switches * param::radix);
	int *s_degree = (int*)malloc(sizeof(int) * child.switches);
	int *h_degree = (int*)malloc(sizeof(int) * child.switches);

  child.d_vtoa(h_degree, s_degree);
  child.adja_vtoa(adjacency);
  ORP_Swing_adjacency(child.switches, param::radix, h_degree, s_degree, &child.r, adjacency);
  child.adja_atov(adjacency);
  child.d_atov(h_degree, s_degree);

	free(adjacency);
	free(h_degree);
	free(s_degree);
}

//increase switch
void add_switch(hostswitch &child)
{
  //Search for open ports
  child.check_port();
  int s_num = child.switches;
  int f, safe = 0;
  vector<int> p;
  int count = param::radix / 2;
  //generat label
  vector<int> label;
  for(unsigned int i = 0; i < child.adja.size(); i++)
  {
    if(child.port_f != i)
      label.push_back(i);
  }
  //select edges
  do
  {
    shuffle(label.begin(), label.end(), randomseed);
    p.clear();
    for(int i = 0; i < count; i++)
      p.push_back(rand() % param::radix);
    //check duplicate
    f = 0;
    for(int i = 0; i < count; i++)
    {
      for(int j = 0; j < count; j++)
      {
        if(i != j && child.adja[label[i]][p[i]] != -1)
        {
          if(label[j] == child.adja[label[i]][p[i]] || child.adja[label[j]][p[j]] == child.adja[label[i]][p[i]])
          {
            f = 1;
            safe++;
            break;
          }
        }
      }
      if(f == 1)
        break;
    }
  }while(f == 1 && safe < SAFE);
  if(safe == SAFE)
    return;
  //connect to new switch
  vector<int> temp;
  for(int i = 0; i < count; i++)
  {
    temp.push_back(child.adja[label[i]][p[i]]);
    temp.push_back(label[i]);
    if(child.adja[label[i]][p[i]] != -1)
      child.adja[child.adja[label[i]][p[i]]][distance(child.adja[child.adja[label[i]][p[i]]].begin(), find(child.adja[child.adja[label[i]][p[i]]].begin(), child.adja[child.adja[label[i]][p[i]]].end(), label[i]))] = s_num;
    child.adja[label[i]][p[i]] = s_num;
  }
  child.adja.push_back(temp);
  //when radix is odd(when there is a surplus port)
  if(param::radix % 2 == 1)
  {
    if(child.port_f != -1)
    {
      child.adja.back().push_back(child.port_f);
      child.adja[child.port_f][distance(child.adja[child.port_f].begin(), find(child.adja[child.port_f].begin(), child.adja[child.port_f].end(), -1))] = s_num;
    }
    else
    {
      child.adja.back().push_back(child.port_f);
      child.port_f = -1;
    }
  }
  child.set_degree();
  child.switches++;
  child.lines = (child.switches * param::radix - param::hosts) / 2 + param::hosts;;
  //child.show_adja();
}

//decrease switch
void reduce_switch(hostswitch &child)
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
  }/*
  for(unsigned int i = 0; i < label.size(); i++)
  {
    cout << label[i] << ", ";
  }cout << endl;*/
  vector <int> aj_label;
  for(unsigned int i = param::hosts; i < label.size(); i++)
  {
    if (label[i] >= (int)(param::radix / 2.0 + 0.5))
      aj_label.push_back(i);
  }
  if(aj_label.size() == 0)
    return;
  shuffle(aj_label.begin(), aj_label.end(), randomseed);/*
  cout << aj_label.size() << ",, ";
  for(unsigned int i = 0; i < aj_label.size(); i++)
  {
    cout << aj_label[i] << ", ";
  }cout << endl;*/
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

void search_rand(hostswitch &indiv)
{
  int f = 0;
  f = rand() % 2;
  //cout << "f=" << f << endl;
  //if(f == 0)
    //add_switch(indiv);/*
  /*else if(f == 1)
    reduce_switch(indiv);
  */if(f == 0)
    swap(indiv);
  else
    swing(indiv);//*/
}

void search_each(hostswitch &indiv)
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
}