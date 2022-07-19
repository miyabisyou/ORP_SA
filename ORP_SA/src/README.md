#ツール名

ORP_SA.exe

#概要

Order/Radix Problem(ORP)において最小の直径(Diameter)と平均ホスト間距離(h-ASPL)を持つグラフを近似解法を用いて探索するプログラムです

#機能

ーparam.hpp
    GRAPH_LOG   	h-ASPLとスイッチ数の遷移出力
    EDGES_OUT	    グラフの結果出力
    RESULT_LOG      コンソールの結果出力
    CSV_OUT 		結果(csv)出力
    DoNS 0		    近傍操作による目的関数値の差の遷移出力
    Accept_rate     受理率の遷移出力
    NUM_OF_SLME     自己ループ辺数と多重辺数の遷移出力

ーコマンドライン引数
    -h, --hosts : number of host
    -r, --radix : number of radix
    -o, --offset : make a bias in the number of switches
    -s, --seed : seed value
    -t, --test : number of test

    -x, --maxtemp : max temperature
    -n, --mintemp : min temperature
    -z, --coolrate : cooling rate
    -i, --iteration : Number of iterations
    -k, --ncalcs : Number of evaluation calculations

    -a, --autotemp : 0 -> 温度自動設定オフ, 1 -> 温度自動設定オン
    -p, --type : 0 -> Random, 1 -> Bias (Add_switch, Remove_switch)

#実行環境

ーコンパイラ
	g++ (GCC) 9.3.0
	Copyright (C) 2019 Free Software Foundation, Inc.
ーコンソール
	Cygwin 3.1.7

#インストール

$ git clone https://github.com/miyabisyou/ORP_SA --recursive
$ cd ORP_SA/ORP_SA/src
$ make
$ ./ORP.exe
BIAS of host?   = No
Proposed max temperature is 261.360495
Proposed min temperature is 0.217147
----- settings parameters -----
hosts : 32
radix : 4
offset : 0
number of test : 1
seed : 0
max temperature : 261.36
min temperature : 0.217147
cooling rate : 0.999993
number of iterations : 1
number of evaluation calculations : 1000000
auto setting temperature(0 -> off, 1 -> on) : 1
Add_switch, Remove_switch(0 -> Random, 1 -> Bias): 0
-------------------------------
seed = 0
Hosts = 32, Switches 26, Radix = 4
Diameter     = 11
Diameter Gap = 6 (11 - 5)
ASPL         = 6.4294354839 (3189/496)
ASPL Gap     = 2.2358870968 (6.4294354839 - 4.1935483871)
GEN: 1000000, temperature:    0.217147, switch:  22, ASPL:    4.963710
Hosts = 32, Switches 22, Radix = 4
Diameter     = 6
Diameter Gap = 1 (6 - 5)
ASPL         = 4.9637096774 (2462/496)
ASPL Gap     = 0.7701612903 (4.9637096774 - 4.1935483871)
computingtime : 62sec.
