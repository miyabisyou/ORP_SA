#pragma once
#include <math.h>

using namespace std;

//hostswitch
#define HOSTS 8
#define RADIX 3
#define SEED 0
#define OFFSET 0
#define TES 1
#define SAFE 1000

//GA
#define GEN 50000
#define GENTH 10000
#define POP 1
#define OFF 1

//SA
#define MAX_TEMP 400			//初期温度
#define MIN_TEMP 0.25			//終了温度
#define COOL_RATE 0.995			//冷却率
#define ITERATIONS 1			//反復回数
#define NCALCS 10		//計算回数(if don't used this value is -1)
#define SET_TEMP 1		//0 -> 温度自動設定オフ, 1 -> 温度自動設定オン
#define SET_TEMP_F true

#define SOLUTION 1		//0 -> GA or HC, 1 -> SA
#define NSEARCH 0		//0 -> rand, 1 -> each

//TEXT_LOG
#define GRAPH_LOG 0		//0 -> グラフ出力オフ, 1 -> グラフ出力オン
#define EDGES_OUT 0		//0 -> エッジ出力オフ, 1 -> エッジ出力オン
#define RESULT_LOG 0	//0 -> 結果出力オフ, 1 -> 結果出力オン
#define CSV_OUT 0		//0 -> 結果(csv)出力オフ, 1 -> 結果(csv)出力オン

namespace param
{
	int hosts = HOSTS;
	int radix = RADIX;
	int seed = SEED;
	int offset = OFFSET;

	int gen = GEN;
	int genth = GENTH;
	int pop = POP;
	int off = OFF;
	int tes = TES;

	int search_type = NSEARCH;
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
	#if SOLUTION == 0
    	cout << "max generation count : " << param::gen << endl;
		cout << "largest institution without renewal : " << param::genth << endl;
    	cout << "population size : " << param::pop << endl;
    	cout << "offspring size : " << param::off << endl;
	#else
        cout << "max temperature : " << param_sa::temp0 << endl;
		cout << "min temperature : " << param_sa::tempF << endl;
    	cout << "cooling rate : " << param_sa::cool_rate << endl;
    	cout << "number of iterations : " << param_sa::iteration << endl;
		cout << "number of evaluation calculations : " << fixed << setprecision(0) << param_sa::ncalcs << endl;
		cout << "auto setting temperature(0 -> off, 1 -> on) : " << param_sa::auto_temp << endl;
	#endif
	    cout << "search type(0 -> randam, 1 -> each) : " << param::search_type << std::endl;
    cout << "-------------------------------" << std::endl;
}