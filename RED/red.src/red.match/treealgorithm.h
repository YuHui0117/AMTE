/*
 * =====================================================================================
 *
 *       Filename:  treealgorithm.h
 *
 *    Description:  algorithms
 *
 *        Version:  1.0
 *        Created:  05/09/12 20:20:32
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __TREEALGORITHM_H_
#define __TREEALGORITHM_H_
#include <map>
#include <iostream>
#include<algorithm>
#include <string>
#include <math.h>
#include "myalgorithm.h"
#include "treenode.h"
using namespace std;

float numberTopologyMatch(string &hypstring, SubLinkData & links,string refstring);
float getRedScore_sys(Config cfg);
vector<pair<float,float> > getRedScore(Config &cfg);
void getRefTrees(Config &cfg,FILE *ifbest,int &itref);
void printLinks(Config cfg);
void removeSameLink(Config &cfg);
#endif
