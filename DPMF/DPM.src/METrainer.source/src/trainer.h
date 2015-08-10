#ifndef TRAINER_H
#define TRAINER_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <fstream>
#include "DepTree.h"
#include "Simulator.h"

using namespace std;

//训练实例
class CInst
{
public:
	vector<CToken> tokens;
	vector<CDepPair> depPairs;
	vector<string> refOps;
};

//状态中的节点表示
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
	bool partOK;
	int featsPtr;

	CState &operator = (const CState &f)
	{
		curPtr = f.curPtr;
		stack = f.stack;
		edges = f.edges;
		hisOps = f.hisOps;
		score = f.score;
		partOK = f.partOK;
		featsPtr = f.featsPtr;
		return *this;
	}

	bool operator < (const CState &s2) const
	{
		return score < s2.score;
	}
};

class CTrainer
{
public:
	CTrainer();
	~CTrainer();
	void Train(const string &instFile,
			   const string &devFile);

private:
	bool ReadInsts(const string &instFile);
	bool ReadDev(const string &devFile);
	void RandSeq(vector<int> &seq);
	void Round();
	double TestOnDev();
	int FitInst(const CInst &inst);
	bool Search(const CInst &inst);
	bool TryDecode(const CInst &dev);
	void InitState(CState &state);
	void GetOps(const CState &from, vector<string> &ops);
	void GetScores(const CState &from, 
				   vector<string> &feats,
				   map<string, double> &scores);
	void GetAvgScores(const CState &from, map<string, double> &scores);
	void Transition(const CState &from, 
					map<string, double> &scores, 
					const string &op, 
					CState &to);
	void GatherUpdates(const vector<string> &feats,
					   const string &op,
					   const double &inc,
					   map<string, double> &updates);
	void GenFeats(const CState &from, vector<string> &feats);
	int MatchCnt(const vector<CDepPair> &refDeps,
				 const vector<CDepPair> &bestDeps);
	void GetDepTo(const vector<CDepPair> &deps, map<int, int> &depTo);
	double EvalOp(const vector<string> &feats, const string &op);
	double AvgEvalOp(const vector<string> &feats, const string &op);
	void Update(const map<string, double> &updates);
	double GetNonAvg(const string &pred);
	double GetAvg(const string &pred);
	void OutputNonAvg(const string &fWeight);
	void OutputAvg(const string &fWeight);

	vector<CState> m_vState;
	vector<CToken> m_vToken;
	int m_nBeam;
	vector<CInst> m_vInst;
	int m_nWordCnt;
	vector<CInst> m_vDev;
	fstream m_fOut;
	int m_nCurRound;
	int m_nCurIndex;
	map<string, double> m_mPredWeight;
	map<string, double> m_mSumForAvg;
	map<string, pair<int, int> > m_mLastModify;
};

#endif
