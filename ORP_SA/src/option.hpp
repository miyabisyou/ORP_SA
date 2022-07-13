#pragma once

#include <map>
#include <string>
#include <functional>

#include "param.hpp"

void set_param(int argc, char * argv[]) {
    
    /* parameters
        -h, --hosts : number of host
        -r, --radix : number of radix
        -o, --offset : make a bias in the number of switches
        -s, --seed : seed value
        -t, --test : number of test
        
        -g, --generation : max generation count
        -l, --genth : Largest institution without renewal
        -p, --population : population size or particle size
        -c, --offspring : offspring size

        -x, --maxtemp : max temperature
        -n, --mintemp : min temperature
        -z, --coolrate : cooling rate
        -i, --iteration : Number of iterations
        -k, --ncalcs : Number of evaluation calculations

        -a, --autotemp : 0 -> 温度自動設定オフ, 1 -> 温度自動設定オン
        -p, --type : 0 -> Random, 1 -> Bias (Add_switch, Remove_switch)
    */

    std::map<std::string, std::function<void(std::string)>> settings;
    settings.emplace("-h", [](std::string args){ param::hosts = std::stoi(args); });
    settings.emplace("--host", [](std::string args){ param::hosts = std::stoi(args); });
    settings.emplace("-r", [](std::string args){ param::radix = std::stoi(args); });
    settings.emplace("--radix", [](std::string args){ param::radix = std::stoi(args); });
    settings.emplace("-o", [](std::string args){ param::offset = std::stoi(args); });
    settings.emplace("--offset", [](std::string args){ param::offset = std::stoi(args); });
    settings.emplace("-s", [](std::string args){ param::seed = std::stoi(args); });
    settings.emplace("--seed", [](std::string args){ param::seed = std::stoi(args); });
    settings.emplace("-t", [](std::string args){ param::tes = std::stoi(args); });
    settings.emplace("--test", [](std::string args){ param::tes = std::stoi(args); });

    settings.emplace("-x", [](std::string args){ param_sa::temp0 = std::stod(args); });
    settings.emplace("--maxtemp", [](std::string args){ param_sa::temp0 = std::stod(args); });
    settings.emplace("-n", [](std::string args){ param_sa::tempF = std::stod(args); });
    settings.emplace("--mintemp", [](std::string args){ param_sa::tempF = std::stod(args); });
    settings.emplace("-z", [](std::string args){ param_sa::cool_rate = std::stod(args); });
    settings.emplace("--coolrate", [](std::string args){ param_sa::cool_rate = std::stod(args); });
    settings.emplace("-i", [](std::string args){ param_sa::iteration = std::stoi(args); });
    settings.emplace("--iteration", [](std::string args){ param_sa::iteration = std::stoi(args); });
    settings.emplace("-k", [](std::string args){ param_sa::ncalcs = std::stoi(args); });
    settings.emplace("--ncalcs", [](std::string args){ param_sa::ncalcs = std::stoi(args); });

    settings.emplace("-a", [](std::string args){ param_sa::auto_temp = std::stoi(args); });
    settings.emplace("--autotemp", [](std::string args){ param_sa::auto_temp = std::stoi(args); });
    settings.emplace("-p", [](std::string args){ param::type = std::stoi(args); });
    settings.emplace("--type", [](std::string args){ param::type = std::stoi(args); });

    for(int i = 1; i < argc; i += 2){
        std::string opt(argv[i]);
        std::string args(argv[(i + 1) % argc]);
        std::cout << opt + " : " + args << std::endl;
        settings.at(opt)(args);
    }

    if(param_sa::auto_temp == 1)
        set_temp();
    if(param_sa::ncalcs != -1)
        param_sa::cool_rate = pow(param_sa::tempF / param_sa::temp0, (double)1.0 / param_sa::ncalcs * param_sa::iteration);

}