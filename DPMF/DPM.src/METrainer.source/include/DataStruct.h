#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <string>

using namespace std;

//¥ -¥ –‘∂‘
class CToken
{
public:
	string word;
	string POS;

	CToken(const string &w, const string &p)
	{
		word = w;
		POS = p;
	}
};

//“¿¥Ê∂‘
class CDepPair
{
public:
	int head;
	int modifier;

	CDepPair(const int &h, const int &m)
	{
		head = h;
		modifier = m;
	}
};

#endif
