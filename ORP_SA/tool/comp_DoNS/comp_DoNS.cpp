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

using namespace std;

namespace param
{
    int hosts = HOST;
    int radix = RADIX;
    int offset = OFFSET;
    int seed = SEED;
    int test = TEST;
    
    string switch_bias = "(random)";
    string swing_bias = "(random)";
    string fname = "ave_sa_host" + to_string(hosts) + "radix" + to_string(radix) + "seed" + to_string(seed) + "offset" + to_string(offset);
}

using namespace param;

void set_param(int argc, char * argv[]);
void set_fname(int i);

int main(int argc, char * argv[])
{
    set_param(argc, argv);
    ofstream csv_File("comp_" + fname + ".csv");
	if(!csv_File)
	{
		cout << "dose not open the csv file." << endl;
		exit(0);
	}
    csv_File<< ", Add_switch, , Reduce_switch, , Swap, , Swing" <<endl;
    csv_File<< "seed, Diameter, ASPL, Diameter, ASPL, Diameter, ASPL, Diameter, ASPL" <<endl;

    string str;
    for(int i = seed; i < test; i++)
    {
        set_fname(i);
        string filename = "./../../DoNS/(" + to_string(hosts) + ", " + to_string(radix) + ")/" + fname + ".txt";
        ifstream input_file;
        input_file.open(filename);
        if (input_file.fail())
        {
            cerr << "dose not open the input file. seed : " << i << endl;
            exit(0);
        }
        //getline(input_file, str);
        getline(input_file, str);
        csv_File << str << endl;
    }
    
    csv_File << "AVERAGE,=AVERAGE(B3:B"+ to_string(2 + test) + "),=AVERAGE(C3:C"+ to_string(2 + test) + "),=AVERAGE(D3:D"+ to_string(2 + test) + "),=AVERAGE(E3:E"+ to_string(2 + test) + "),=AVERAGE(F3:F"+ to_string(2 + test) + "),=AVERAGE(G3:G"+ to_string(2 + test) + "),=AVERAGE(H3:H"+ to_string(2 + test) + "),=AVERAGE(I3:I"+ to_string(2 + test) + ")" << endl;
}

void set_param(int argc, char * argv[]) 
{
    
    /* parameters
        -h, --hosts : number of host
        -r, --radix : number of radix
        -o, --offset : make a bias in the number of switches
        -s, --seed : start seed value
        -t, --test : number of test

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
    fname = "ave_sa_host" + to_string(hosts) + "radix" + to_string(radix) + "seed" + to_string(i) + "offset" + to_string(offset) + "switch" + switch_bias + "swing" + swing_bias;
}