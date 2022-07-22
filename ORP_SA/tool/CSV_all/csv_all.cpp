#include <stdio.h>
#include <map>
#include <string>
#include <functional>
#include <iostream>
#include <fstream>

#define HOST 32
#define RADIX 4
#define OFFSET 0
#define SEED 0
#define TEST 10
#define NOI 100

using namespace std;

namespace param
{
    int hosts = HOST;
    int radix = RADIX;
    int offset = OFFSET;
    int seed = SEED;
    int test = TEST;
    int noi = NOI;

    string switch_bias = "(random)";
    string swing_bias = "(random)";
    string fname = "sa_host" + to_string(hosts) + "radix" + to_string(radix) + "seed" + to_string(seed) + "offset" + to_string(offset) + ".csv";

}

using namespace param;

void set_param(int argc, char * argv[]);
void set_fname(int i);

int main(int argc, char * argv[])
{
    set_param(argc, argv);
    set_fname(seed);
    ofstream csv_File("summary_" + fname);
	if(!csv_File)
	{
		cout << "dose not open the csv file." << endl;
		exit(0);
	}
    csv_File <<"Seed, Diametar, Diameter Gap, ASPL, ASPL Gap, switches, computing-time(sec.)" << endl; 
    
    string str;
    for(int i = 0; i < noi; i += test)
    {
        set_fname(seed);
        string filename = "./../../CSV/" + fname;
        ifstream input_file;
        input_file.open(filename);
        if (input_file.fail())
        {
            cerr << "dose not open the input file. seed : " << filename << endl;
            exit(0);
        }
        getline(input_file, str);
        while(getline(input_file, str))
            csv_File << str << endl;   
        seed++;
    }
    csv_File << "AVERAGE,=AVERAGE(B2:B"+ to_string(1 + noi) + "),=AVERAGE(C2:C"+ to_string(1 + noi) + "),=AVERAGE(D2:D"+ to_string(1 + noi) + "),=AVERAGE(E2:E"+ to_string(1 + noi) + "),=AVERAGE(F2:F"+ to_string(1 + noi) + "),=AVERAGE(G2:G"+ to_string(1 + noi)  + ")" << endl;
}

void set_param(int argc, char * argv[]) 
{
    /* parameters
        -h, --hosts : number of host
        -r, --radix : number of radix
        -o, --offset : make a bias in the number of switches
        -s, --seed : start seed value
        -t, --test : number of test
        -n, --noi : number of max test

        -q, --switch_bias : random or bias
        -w, --swing_bias : random or bias
    */

    map<string, function<void(string)>> settings;
    settings.emplace("-h", [](string args){ hosts = stoi(args); });
    settings.emplace("--host", [](string args){ hosts = stoi(args); });
    settings.emplace("-r", [](string args){ radix = stoi(args); });
    settings.emplace("--radix", [](string args){ radix = stoi(args); });
    settings.emplace("-o", [](string args){ offset = stoi(args); });
    settings.emplace("--offset", [](string args){ offset = stoi(args); });
    settings.emplace("-s", [](string args){ seed = stoi(args); });
    settings.emplace("--seed", [](string args){ seed = stoi(args); });
    settings.emplace("-t", [](string args){ test = stoi(args); });
    settings.emplace("--test", [](string args){ test = stoi(args); });
    settings.emplace("-n", [](string args){ noi = stoi(args); });
    settings.emplace("--noi", [](string args){ noi = stoi(args); });
    settings.emplace("-q", [](string args){ switch_bias = "(" + args + ")"; });
    settings.emplace("--switch_bias", [](string args){ switch_bias = "(" + args + ")"; });
    settings.emplace("-w", [](string args){ swing_bias = "(" + args + ")"; });
    settings.emplace("--swing_bias", [](string args){ swing_bias = "(" + args + ")"; });

    for(int i = 1; i < argc; i += 2){
        string opt(argv[i]);
        string args(argv[(i + 1) % argc]);
        cout << opt + " : " + args << endl;
        settings.at(opt)(args);
    }
}

void set_fname(int i)
{
    fname = "sa_host" + to_string(hosts) + "radix" + to_string(radix) + "seed" + to_string(i) + "offset" + to_string(offset) + "switch" + switch_bias + "swing" + swing_bias + ".csv";
}