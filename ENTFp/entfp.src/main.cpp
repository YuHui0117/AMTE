/*
 * =====================================================================================
 *
 *       Filename:  test.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/09/12 10:51:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <assert.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "entropy.cpp"
#include "rain_algorithm.h"
using namespace std;
using namespace rain;
vector<vector<int> > chunks_sys;

bool DBG_bool = false;
bool BP_bool = false;
bool ngramLength_bool=false;
int refnum = 1;
vector<float> weight(4,1);//exact stem synonym paraphrase
vector<float> params(4,1);//Fmean functionword lp  ent

int main(int argc, char* argv[]) {
    weight[0]=1.0;
    weight[1]=0.6;
    weight[2]=0.8;
    weight[3]=0.6;
    params[0]=0.85; 
    params[1]=0.25; 
    params[2]=1.2; 
    params[3]=1.2; 
    string alignFile,funFile;
    for (int i = 1; i < argc; i++) { 
        if (i != argc){ 
            if (string(argv[i]) == "-Align") {
                alignFile = argv[++i];
            }else if (string(argv[i]) == "-FunList"){
                funFile = argv[++i];
            }else if (string(argv[i]) == "-Weight"){
                weight[0]=atof(argv[++i]);
                weight[1]=atof(argv[++i]);
                weight[2]=atof(argv[++i]);
                weight[3]=atof(argv[++i]);
            }else if (string(argv[i]) == "-Params"){
                params[0]=atof(argv[++i]);
                params[1]=atof(argv[++i]);
                params[2]=atof(argv[++i]);
                params[3]=atof(argv[++i]);
            }else {
                std::cout << argv[i]<<"invalid arguments, please try again.\n";
                exit(0);
            }
        }
    }

    metricScore(alignFile,funFile);
    return 0;
}
