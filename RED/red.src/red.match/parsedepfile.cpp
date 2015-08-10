/*
 * =====================================================================================
 *
 *       Filename:  parsedepfile.cpp
 *
 *    Description:  parsedepfile.cpp
 *
 *        Version:  1.0
 *        Created:  19/09/12 20:20:53
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <sstream>
#include <iostream>
#include <fstream>
#include "parsedepfile.h"
#include "stdlib.h"
using namespace std;
/************************************************
    读取依存树
*************************************************/
vector<string> parseDepfile(string filename)
{
    fstream f;
    f.open(filename.c_str());
    string strtmp;
    string ss;
    if(!f.is_open()) {
        cout<<"can not open "<<filename.c_str()<<endl;
        exit(0);
    }

    vector<string> vecstring;
    for(getline(f,strtmp);f.good();getline(f,strtmp))
    {
        ss += strtmp+'\n';
        if(strtmp.length() < 2) 
        {
            vecstring.push_back(ss);
            ss.clear();
        }
    }
    return vecstring;
}
