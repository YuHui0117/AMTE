#include <fstream>
#include <strstream>
#include <iostream>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <iterator>
#include "Trainer.h"

CTrainer::CTrainer()
{
	m_fOut.open("train_log.txt", ios::out);
	m_fOut << "------------  beam_sr train log  ------------" << endl;
}

CTrainer::~CTrainer()
{
	m_fOut.close();
}

//ѵ��������
void CTrainer::Train(const string &instFile, 
					 const string &devFile)
{
	if (!ReadInsts(instFile))
	{
		return;
	}

	for (int i = 0; i < int(m_vInst.size()); ++i)
	{
		FitInst(m_vInst[i]);
	}


}

//����ѵ��ʵ��
bool CTrainer::ReadInsts(const string &instFile)
{
	ifstream fin(instFile.c_str());

	if (!fin)
	{
		cerr << "Can not open file: " << instFile << endl;
		return false;
	}

	m_nWordCnt = 0;

	string line;

	while (getline(fin, line))
	{
		if (line.empty())
		{
			continue;
		}

		CDepTree depTree(line);
		CInst inst;

		depTree.Tokens(inst.tokens);
		depTree.DepPairs(inst.depPairs);

		CSimulator simulator;

		simulator.Simulate(inst.depPairs, inst.refOps);
		m_vInst.push_back(inst);
		m_nWordCnt += int(inst.tokens.size());
	}

	cerr << "Succeed."
		 << "Instance count: " << int(m_vInst.size()) << "\n"
		 << "Word count: " << m_nWordCnt << endl;
	fin.close();
	return true;
}

//��ȡ������
bool CTrainer::ReadDev(const string &devFile)
{
	ifstream fin(devFile.c_str());

	if (!fin)
	{
		cerr << "Can not open file: " << devFile << endl;
		return false;
	}

	string line;

	while (getline(fin, line))
	{
		if (line.empty())
		{
			continue;
		}

		CDepTree depTree(line);
		CInst dev;

		depTree.Tokens(dev.tokens);
		depTree.DepPairs(dev.depPairs);

		CSimulator simulator;

		simulator.Simulate(dev.depPairs, dev.refOps);
		m_vDev.push_back(dev);
	}

	cerr << "Dev count: " << int(m_vDev.size()) << endl;
	fin.close();
	return true;
}

//һ��ѵ��
void CTrainer::Round()
{
	vector<int> idxSeq;

	RandSeq(idxSeq);

	int depOK = 0;
	int i;

	for (i = 0; i < int(m_vInst.size()); ++i)
	{
		m_nCurIndex = i;
		depOK += FitInst(m_vInst[idxSeq[i]]);

		if (i % 100 == 0)
		{
			cerr << i << endl;
		}
	}

	double P = double(depOK) / double(m_vInst.size());

	cerr << "Round finish" << endl;
	cerr << "TotalP = " << P * 100 << "%" << endl;
	m_fOut << "TotalP = " << P * 100 << "%" << endl;
}

//����ƽ��ģ���ڿ�����������
double CTrainer::TestOnDev()
{
	int okCnt = 0;
	size_t i;

	for (i = 0; i < m_vDev.size(); ++i)
	{
		if (TryDecode(m_vDev[i]))
		{
			++okCnt;
		}
	}

	return double(okCnt) / double(m_vDev.size());
}

//�������ѵ������
void CTrainer::RandSeq(vector<int> &seq)
{
	seq.clear();

	int i;

	for (i = 0; i < int(m_vInst.size()); ++i)
	{
		seq.push_back(i);
	}

	for (i = int(m_vInst.size()) - 1; i > 0; --i)
	{
		int r = rand() % (i + 1);

		int tmp = seq[i];
		seq[i] = seq[r];
		seq[r] = tmp;
	}
}

//��ϸ�ʵ��
int CTrainer::FitInst(const CInst &inst)
{
	bool succ = Search(inst);
	return succ;
}

//��ʼ״̬
void CTrainer::InitState(CState &state)
{
	state.curPtr = 0;
	state.stack.clear();
	state.edges.clear();
	state.hisOps.clear();
	state.score = 0.0;
	state.partOK = true;
	state.featsPtr = -1;
}

//�ƽ�-��Լ����
bool CTrainer::Search(const CInst &inst)
{
	const vector<string> &refOps = inst.refOps;

	m_vToken = inst.tokens;
	m_vState.clear();

	//��ʼ��״̬ջ
	CState ini;

	InitState(ini);
	m_vState.push_back(ini);

	CState cur = ini;
	vector<vector<string> > v_feat;
	//�ƽ���Լ2N-1��
	int steps = 2 * int(m_vToken.size()) - 1;
	for (int i = 0; i < steps; ++i)
	{
		map<string, double> scores;
		CState next;
		string op = refOps[i];
		vector<string> feat;
		GenFeats(cur, feat);
		feat.insert(feat.begin(),op);
		v_feat.push_back(feat);
		Transition(cur, scores, op, next);
		cur = next;
	}
	for (size_t i = 0;i < v_feat.size();i++)
	{
		copy(v_feat[i].begin(),v_feat[i].end(),ostream_iterator<string>(cout," "));
		cout << endl;
	}
	
	return true;
}

//���ԶԿ����������з���
bool CTrainer::TryDecode(const CInst &dev)
{
	const vector<string> &refOps = dev.refOps;

	m_vToken = dev.tokens;
	m_vState.clear();

	//��ʼ��״̬ջ
	CState ini;

	InitState(ini);
	m_vState.push_back(ini);

	//�ƽ���Լ2N-1��
	int steps = 2 * int(m_vToken.size()) - 1;
	int i;

	for (i = 0; i < steps; ++i)
	{
		vector<CState> buf;
		size_t j;

		for (j = 0; j < m_vState.size(); ++j)
		{
			map<string, double> scores;

			GetAvgScores(m_vState[j], scores);

			//ִ�����п��ܵ���һ������
			vector<string> ops;

			GetOps(m_vState[j], ops);

			size_t k;

			for (k = 0; k < ops.size(); ++k)
			{
				CState next;

				Transition(m_vState[j], scores, ops[k], next);

				if (ops[k] != refOps[i])
				{
					next.partOK = false;
				}

				buf.push_back(next);
			}
		}

		sort(buf.begin(), buf.end());

		//�Ƿ����Ȩ�ؼ��Ƿ���ǰ��������
		int okID = int(buf.size()) - 1;

		while (okID >=0 && !buf[okID].partOK)
		{
			--okID;
		}

		if (okID < 0)
		{
			cerr << "Search error!" << endl;
			return false;
		}

		if (int(buf.size()) - okID > m_nBeam)
		{
			return false;
		}

		//ѡ����õ�N��״̬
		m_vState.clear();

		int p = int(buf.size()) - 1;
		int cnt = 0;

		while (p >= 0 && cnt < m_nBeam)
		{
			m_vState.push_back(buf[p]);
			--p;
			++cnt;
		}
	}

	if (m_vState[0].partOK)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//�ڵ�ǰ״̬��ͳ��Ӧ�ý��еĸ�����
void CTrainer::GatherUpdates(const vector<string> &feats, 
							 const string &op, 
							 const double &inc, 
							 map<string, double> &updates)
{
	size_t i;

	for (i = 0; i < feats.size(); ++i)
	{
		string pred = feats[i] + "=>" + op;

		updates[pred] += inc;
	}
}

//����ڸ�״̬�¿��еĲ���
void CTrainer::GetOps(const CState &from, vector<string> &ops)
{
	ops.clear();

	if (from.curPtr < int(m_vToken.size()))
	{
		ops.push_back("SHIFT");
	}

	//ջ��Ԫ������2���Ҷ��в���
	if (from.stack.size() >= 2)
	{
		ops.push_back("LEFT");
		ops.push_back("RIGHT");
	}
}

//��ȡ����
void CTrainer::GetScores(const CState &from,
						 vector<string> &feats,
						 map<string, double> &scores)
{
	feats.clear();
	scores.clear();

	GenFeats(from, feats);

	scores["SHIFT"] = EvalOp(feats, "SHIFT");
	scores["LEFT"] = EvalOp(feats, "LEFT");
	scores["RIGHT"] = EvalOp(feats, "RIGHT");
}

//��ȡƽ��ģ�ͷ���
void CTrainer::GetAvgScores(const CState &from, 
							map<string, double> &scores)
{
	scores.clear();

	vector<string> feats;

	GenFeats(from, feats);

	scores["SHIFT"] = AvgEvalOp(feats, "SHIFT");
	scores["LEFT"] = AvgEvalOp(feats, "LEFT");
	scores["RIGHT"] = AvgEvalOp(feats, "RIGHT");
}

//״̬ת��
void CTrainer::Transition(const CState &from,
						  map<string, double> &scores, 
						  const string &op, 
						  CState &to)
{
	to = from;
	//to.score += scores[op];
	to.hisOps.push_back(op);

	if (op == "SHIFT")
	{
		CNode cur(to.curPtr);

		to.stack.push_back(cur);
		++to.curPtr;
	}
	else 
	{
		int topIdx = int(to.stack.size()) - 1;
		CNode topRight = to.stack[topIdx];
		CNode topLeft = to.stack[topIdx - 1];

		to.stack.pop_back();
		to.stack.pop_back();

		if (op == "LEFT")
		{
			to.edges.push_back(CDepPair(topRight.id, topLeft.id));
			topRight.leftMost = topLeft.id;
			to.stack.push_back(topRight);
		}
		else if (op == "RIGHT")
		{
			to.edges.push_back(CDepPair(topLeft.id, topRight.id));
			topLeft.rightMost = topRight.id;
			to.stack.push_back(topLeft);
		}
	}
}

//���ɸ�״̬��������
void CTrainer::GenFeats(const CState &from, vector<string> &feats)
{
	feats.clear();

	int sp = int(from.stack.size()) - 1;
	int qp = from.curPtr;
	int qBnd = int(m_vToken.size()) - 1;
	string s0w = (sp >= 0) ? m_vToken[from.stack[sp].id].word : "<s>";
	string s0t = (sp >= 0) ? m_vToken[from.stack[sp].id].POS : "<s>";
	string s0lct = (sp >= 0 && from.stack[sp].leftMost != -1)
		? m_vToken[from.stack[sp].leftMost].POS : "NONE";
	string s0rct = (sp >= 0 && from.stack[sp].rightMost != -1)
		? m_vToken[from.stack[sp].rightMost].POS : "NONE";
	string s1w = (sp - 1 >= 0) 
		? m_vToken[from.stack[sp - 1].id].word : "<s>";
	string s1t = (sp - 1 >= 0) 
		? m_vToken[from.stack[sp - 1].id].POS : "<s>";
	string s1lct = (sp - 1 >= 0 && from.stack[sp - 1].leftMost != -1)
		? m_vToken[from.stack[sp - 1].leftMost].POS : "NONE";
	string s1rct = (sp - 1 >= 0 && from.stack[sp - 1].rightMost != -1)
		? m_vToken[from.stack[sp - 1].rightMost].POS : "NONE";
	string s2w = (sp - 2 >= 0) 
		? m_vToken[from.stack[sp - 2].id].word : "<s>";
	string s2t = (sp - 2 >= 0) 
		? m_vToken[from.stack[sp - 2].id].POS : "<s>";
	string q0w = (qp <= qBnd) ? m_vToken[qp].word : "</s>"; 
	string q0t = (qp <= qBnd) ? m_vToken[qp].POS : "</s>"; 
	string q1w = (qp + 1 <= qBnd) ? m_vToken[qp + 1].word : "</s>"; 
	string q1t = (qp + 1 <= qBnd) ? m_vToken[qp + 1].POS : "</s>";
	string q2w = (qp + 2 <= qBnd) ? m_vToken[qp + 2].word : "</s>"; 
	string q2t = (qp + 2 <= qBnd) ? m_vToken[qp + 2].POS : "</s>";

	feats.push_back("s0w-s0t=" + s0w + "|" + s0t);
	feats.push_back("s0w=" + s0w);
	feats.push_back("s0t=" + s0t);

	feats.push_back("s1w-s1t=" + s1w + "|" + s1t);
	feats.push_back("s1w=" + s1w);
	feats.push_back("s1t=" + s1t);

	feats.push_back("s0w-s0t-s1w-s1t="
		+ s0w + "|" + s0t + "|" + s1w + "|" + s1t);
	feats.push_back("s0w-s0t-s1w=" + s0w + "|" + s0t + "|" + s1w);
	feats.push_back("s0w-s1w-s1t=" + s0w + "|" + s1w + "|" + s1t);
	feats.push_back("s0w-s0t-s1t=" + s0w + "|" + s0t + "|" + s1t);
	feats.push_back("s0t-s1w-s1t=" + s0t + "|" + s1w + "|" + s1t);
	feats.push_back("s0w-s1w=" + s0w + "|" + s1w);
	feats.push_back("s0t-s1t=" + s0t + "|" + s1t);

	feats.push_back("q0w-q0t=" + q0w + "|" + q0t);
	feats.push_back("q0w=" + q0w);
	feats.push_back("q0t=" + q0t);

	feats.push_back("s0t-q0t=" + s0t + "|" + q0t);
	feats.push_back("s0t-q0t-q1t=" + s0t + "|" + q0t + "|" + q1t);
	feats.push_back("s1t-s0t-q0t=" + s1t + "|" + s0t + "|" + q0t);
	feats.push_back("s2t-s1t-s0t=" + s2t + "|" + s1t + "|" + s0t);
	feats.push_back("s1t-s1lct-s0t=" + s1t + "|" + s1lct + "|" + s0t);
	feats.push_back("s1t-s1rct-s0t=" + s1t + "|" + s1rct + "|" + s0t);
	feats.push_back("s1t-s0t-s0lct=" + s1t + "|" + s0t + "|" + s0lct);
	feats.push_back("s1t-s0t-s0rct=" + s1t + "|" + s0t + "|" + s0rct);
	feats.push_back("s0w-q0t-q1t=" + s0w + "|" + q0t + "|" + q1t);
	feats.push_back("s1t-s0w-q0t=" + s1t + "|" + s0w + "|" + q0t);
	feats.push_back("s1t-s1lct-s0w=" + s1t + "|" + s1lct + "|" + s0w);
	feats.push_back("s1t-s1rct-s0w=" + s1t + "|" + s1rct + "|" + s0w);
	feats.push_back("s1t-s0w-s0lct=" + s1t + "|" + s0w + "|" + s0lct);
}

//�ο����뵱ǰ��������ƥ����
int CTrainer::MatchCnt(const vector<CDepPair> &refDeps,
					   const vector<CDepPair> &bestDeps)
{
	map<int, int> refDepTo;

	GetDepTo(refDeps, refDepTo);

	map<int, int> bestDepTo;

	GetDepTo(bestDeps, bestDepTo);

	int depOK = 0;
	map<int, int>::iterator it;

	for (it = refDepTo.begin(); it != refDepTo.end(); ++it)
	{
		int refMod = it->first;
		int refHead = it->second;

		map<int, int>::iterator bestIt = bestDepTo.find(refMod);

		if (bestIt != bestDepTo.end() && bestIt->second == refHead)
		{
			++depOK;
		}
	}

	return depOK;
}

//��������Է��������ʵ�head
void CTrainer::GetDepTo(const vector<CDepPair> &deps, 
						map<int, int> &depTo)
{
	depTo.clear();

	size_t i;

	for (i = 0; i < deps.size(); ++i)
	{
		int h = deps[i].head;
		int m = deps[i].modifier;

		if (h == -1)
		{
			continue;
		}

		depTo[m] = h;
	}
}

//��������Ԥ��������ĵ÷�
double CTrainer::EvalOp(const vector<string> &feats, const string &op)
{
	double score = 0.0;
	size_t i;

	for (i = 0; i < feats.size(); ++i)
	{
		string pred = feats[i] + "=>" + op;

		score += GetNonAvg(pred);
	}

	return score;
}


//��������Ԥ���������ƽ��ģ�͵÷�
double CTrainer::AvgEvalOp(const vector<string> &feats, const string &op)
{
	double score = 0.0;
	size_t i;

	for (i = 0; i < feats.size(); ++i)
	{
		string pred = feats[i] + "=>" + op;

		score += GetAvg(pred);
	}

	return score;
}

//�����ƽ��Ȩ������
void CTrainer::OutputNonAvg(const string &fWeight)
{
	ofstream fout(fWeight.c_str());
	if (!fout)
	{
		cout << "Can not open file: " << fWeight << endl;
		return;
	}

	map<string, double>::iterator it;

	for (it = m_mPredWeight.begin(); it != m_mPredWeight.end(); ++it)
	{
		fout << it->first << "\t" << it->second << endl;
	}

	fout.close();
}

//���ƽ��Ȩ������
void CTrainer::OutputAvg(const string &fWeight)
{
	ofstream fout(fWeight.c_str());
	if (!fout)
	{
		cout << "Can not open file: " << fWeight << endl;
		return;
	}

	double deno = double(m_nCurRound + 1) * double(m_vInst.size());
	map<string, double>::iterator it;

	for (it = m_mSumForAvg.begin(); it != m_mSumForAvg.end(); ++it)
	{
		fout << it->first << "\t" << it->second / deno << endl;
	}

	fout.close();
}

//��ø��ƶϵķ�ƽ����֪����ֵ
double CTrainer::GetNonAvg(const string &pred)
{
	//cout << "++GetNonAvg()" << endl;
	//cout << "pred: " << pred << " ";
	
	map<string, double>::iterator it = m_mPredWeight.find(pred);

	if (it != m_mPredWeight.end())
	{
		cout << it->second << endl;
		return it->second;
	}
	else
	{
		cout << 0.0 << endl;
		return 0.0;
	}
}

//��ø��ƶϵ�ƽ����֪����ֵ
double CTrainer::GetAvg(const string &pred)
{
	double deno = double(m_nCurRound + 1) * double(m_vInst.size());
	map<string, double>::iterator it = m_mSumForAvg.find(pred);

	if (it != m_mSumForAvg.end())
	{
		return it->second / deno;
	}
	else
	{
		return 0.0;
	}
}

//����Ȩ��
void CTrainer::Update(const map<string, double> &updates)
{
	cout << "++Update()" << endl;

	map<string, double>::const_iterator it;

	for (it = updates.begin(); it != updates.end(); ++it)
	{
		cout << "pred: " << it->first << " count: " << it->second << endl;
		int lastRnd = m_mLastModify[it->first].first;
		int lastIdx = m_mLastModify[it->first].second;
		cout << "modified: lastRnd:" << lastRnd << " lastIdx:" << lastIdx << endl;

		int incTime = int(m_vInst.size()) * (m_nCurRound - lastRnd) + (m_nCurIndex - lastIdx);
		cout << "cur_round:" << m_nCurRound << endl;
		cout << "cur_idx:" << m_nCurIndex << endl;
		cout << "IncTime: " << incTime << endl;

		double incAvg = double(incTime) * GetNonAvg(it->first) 
						+ it->second;
		cout << "incAvg:" << incAvg << endl;

		m_mSumForAvg[it->first] +=  incAvg;
		m_mPredWeight[it->first] += it->second;
		m_mLastModify[it->first].first = m_nCurRound;
		m_mLastModify[it->first].second = m_nCurIndex;
	}

	//���һ��ѵ������ʱˢ��ƽ��Ȩ��
	if (m_nCurIndex == int(m_vInst.size()) - 1)
	{
		map<string, double>::iterator it;

		for (it = m_mSumForAvg.begin(); it != m_mSumForAvg.end(); ++it)
		{
			int lastRnd = m_mLastModify[it->first].first;
			int lastIdx = m_mLastModify[it->first].second;
			int incTime = int(m_vInst.size()) * (m_nCurRound - lastRnd) 
				+ (m_nCurIndex - lastIdx);
			double incAvg = double(incTime) * GetNonAvg(it->first);

			m_mSumForAvg[it->first] +=  incAvg;
			m_mLastModify[it->first].first = m_nCurRound;
			m_mLastModify[it->first].second = m_nCurIndex;
		}
	}
}
