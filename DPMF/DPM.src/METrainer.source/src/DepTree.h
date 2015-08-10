#ifndef DEPTREE_H
#define DEPTREE_H

#include <string>
#include <vector>
#include "DataStruct.h"

using namespace std;

class CDepNode
{
public:
	CDepNode *parrent;
	vector<CDepNode *> leftKids;
	vector<CDepNode *> rightKids;
	string val;
};

class CDepTree
{
public:
	CDepTree(const string &str);
	~CDepTree();
	CDepNode *Root();
	string Graph();
	void Tokens(vector<CToken> &toks);
	void DepPairs(vector<CDepPair> &deps);

private:
	void RecurTokens(CDepNode *root, vector<CToken> &toks);
	void RecurDepPairs(CDepNode *root,
		vector<CDepPair> &deps,
		int &leafID,
		int &rootID);
	string RecurGraph(CDepNode *root, const int &lev);
	CDepNode *Create(const string &str);
	bool Check(const string &str);
	void Release(CDepNode *ptr);

	CDepNode *m_pRoot;
};

#endif

