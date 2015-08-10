#ifndef DECODER_H
#define DECODER_H

#include <string>
#include <vector>
#include "../include/maxent.h"

using namespace std;

//词-词性标记对
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

//依存对
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

//依存内部表示节点
class CDepNode
{
public:
	vector<int> leftMods;
	vector<int> rightMods;
};

//状态节点中依存树的节点表示
class CNode
{
public:
	int id;
	int leftMost;
	int rightMost;

	CNode &operator = (const CNode &f)
	{
		id = f.id;
		leftMost = f.leftMost;
		rightMost = f.rightMost;
		return *this;
	}

	CNode(const int &idVal)
	{
		id = idVal;
		leftMost = -1;
		rightMost = -1;
	}
};

//移进-规约状态
class CState
{
public:
	int curPtr;
	vector<CNode> stack;
	vector<CDepPair> edges;
	vector<string> hisOps;
	double score;

	CState &operator = (const CState &f)
	{
		curPtr = f.curPtr;
		stack = f.stack;
		edges = f.edges;
		hisOps = f.hisOps;
		score = f.score;
		return *this;
	}

	bool operator < (const CState &s2) const
	{
		return score < s2.score;
	}
};

class CDecoder
{
public:
	CDecoder(){}
	~CDecoder(){}

	void Decode(maxent &me, 
				const int &beam,
				const string &tokSeq, 
				string &result,
                float &maxscore,
                int &lenh,
                vector<string> &vecref,
                int &lenr);

private:
	void SplitTokSeq(const string &tokSeq,int &len);
    int Splitline(const string &line);
    void Search(string &result, float &maxscore);
	void InitState(CState &state);
	void GetOps(const CState &from, vector<string> &ops);
	void GetScores(const CState &from, map<string, double> &scores);
	void Transition(const CState &from, 
					map<string, double> &scores, 
					const string &op, 
					CState &to);
	void GenFeats(const CState &from, vector<string> &feats);
	void GetResult(const CState &state, string &result);
	string GenTree(const vector<CDepNode> &nodes, const int &root);

	double eval(vector<map<string,double> > & op_val,const string & op);
	
	void get_cls_val(vector<string> & feat, vector<map<string,double> > & v_cls_val);

	void normalize(vector<double> & v_score);

	vector<CState> m_vState;
	vector<CToken> m_vToken;
	int m_nBeam;
	maxent *m_pModel;
};

#endif
