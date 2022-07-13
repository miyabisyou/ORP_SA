#pragma once
#include <math.h>

using namespace std;

//hostswitch
#define HOSTS 32
#define RADIX 4
#define SEED 0
#define OFFSET 0
#define TES 1
#define SAFE 1000
#define TYPE 0			//0 -> Random, 1 -> Bias (Add_switch, Remove_switch) 

//SA
#define MAX_TEMP 400			//初期温度
#define MIN_TEMP 0.25			//終了温度
#define COOL_RATE 0.995			//冷却率
#define ITERATIONS 1			//反復回数
#define NCALCS 1000000		//計算回数(if don't used this value is -1)
#define SET_TEMP 1		//0 -> 温度自動設定オフ, 1 -> 温度自動設定オン
#define SET_TEMP_F true

//TEXT_LOG
#define GRAPH_LOG 0		//0 -> グラフ出力オフ, 1 -> グラフ出力オン
#define EDGES_OUT 1		//0 -> エッジ出力オフ, 1 -> エッジ出力オン
#define RESULT_LOG 1	//0 -> 結果出力オフ, 1 -> 結果出力オン
#define CSV_OUT 1		//0 -> 結果(csv)出力オフ, 1 -> 結果(csv)出力オン
#define DoNS 0		//0 -> 近傍差出力オフ, 1 -> 近傍差出力オン
#define Accept_rate 0		//0 -> 受理率出力オフ, 1 -> 受理率出力オン
#define NUM_PLOT 100
#define NUM_OF_SLME 0

namespace param
{
	int hosts = HOSTS;
	int radix = RADIX;
	int seed = SEED;
	int offset = OFFSET;
	int tes = TES;

	int type = TYPE; 
}

namespace param_sa
{
	double temp0 = MAX_TEMP;
	double tempF = MIN_TEMP;
	int iteration = ITERATIONS;
	double cool_rate = COOL_RATE;
	double ncalcs = NCALCS;
	//double cool_rate = pow(MIN_TEMP / MAX_TEMP, (double)1.0 / NCALCS * iteration);
	int auto_temp = SET_TEMP;
}

void output_setting() {
    cout << "----- settings parameters -----" << endl;
    cout << "hosts : " << param::hosts << endl;
    cout << "radix : " << param::radix << endl;
    cout << "offset : " << param::offset << endl;
	cout << "number of test : " << param::tes << endl;
    cout << "seed : " << param::seed << endl;
    cout << "max temperature : " << param_sa::temp0 << endl;
	cout << "min temperature : " << param_sa::tempF << endl;
    cout << "cooling rate : " << param_sa::cool_rate << endl;
    cout << "number of iterations : " << param_sa::iteration << endl;
	cout << "number of evaluation calculations : " << fixed << setprecision(0) << param_sa::ncalcs << endl;
	cout << "auto setting temperature(0 -> off, 1 -> on) : " << param_sa::auto_temp << endl;
	cout << "Add_switch, Remove_switch(0 -> Random, 1 -> Bias): " << param::type << endl;
	cout << "-------------------------------" << std::endl;
}