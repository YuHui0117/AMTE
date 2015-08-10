#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <string>
#include <vector>

using namespace std;

class CSimulator
{
public:
	CSimulator();
	~CSimulator();
	void Simulate(const vector<CDepPair> &deps, vector<string> &ops);

private:
	void GetDepInfo(const vector<CDepPair> &deps);
	void InitState(const vector<CDepPair> &deps);
	void TraceState(vector<string> &ops);
	void Transition(const string &op, vector<string> &ops);

	int m_nLen;
	int m_nCurPtr;
	vector<int> m_vStack;
	vector<int> m_vDepRight;
	vector<vector<int> > m_vvDep;
};

#endif
