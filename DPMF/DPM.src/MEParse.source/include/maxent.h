#ifndef _MAXENT_H_
#define _MAXENT_H_

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <cassert>
#include <sstream>

using namespace std;

class maxent
{	
public:
	maxent(){}
	~maxent(){}

	//load maxent model from me_file
	int load_maxent_model(char* me_file);

	//get prob of feat taking cls
	double prob(const string &feat, const string & cls);

	map<string,double> prob(const string & feat);

private:
	void print(ostream & out = cout);

private:
	map<string,map<string,double> > m_model;	//(feat,cls,val)
	vector<string> v_cls;
	vector<string> v_feat;
};

#endif

