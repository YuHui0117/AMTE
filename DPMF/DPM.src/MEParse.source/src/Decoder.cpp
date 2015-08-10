//#include <stdio.h>
#include <iostream>
#include <fstream>
#include <strstream>
#include <map>
#include <cmath>
#include <algorithm>
#include "../include/Decoder.h"
//using namespace std;
//分析
void CDecoder::Decode( maxent & me,
					  const int &beam,
					  const string &tokSeq, 
					  string &result, float &maxscore, 
                      int &lenh, 
                      vector<string> &vecRef, int &lenr)
{
	m_pModel = &me;
	m_nBeam = beam;
	SplitTokSeq(tokSeq, lenh);
    int dif = 1000;
    int len_all=0;
    for(int i=0; i<vecRef.size(); i++)
    {
       int len = Splitline(vecRef[i]);
       int temp = lenh - len;
       if(abs(temp) < dif) dif = temp;
       len_all += len;
    }
    //lenr = dif + lenh;
    lenr = len_all/vecRef.size();//use the average length
    //lenr= Splitline(vecRef[0]);//use the first ref length
    Search(result,maxscore);
}

//切分输入串
void CDecoder::SplitTokSeq(const string &tokSeq, int &len)
{
	m_vToken.clear();

	istrstream is(tokSeq.c_str());
	string part;

	while (is >> part)
	{
		size_t split = part.rfind("/");
		string word = part.substr(0, split);
		string POS = part.substr(split + 1, part.length() - split - 1);
        CToken tok(word, POS);

		m_vToken.push_back(tok);
	}
    len=m_vToken.size();
}
//compute the length of a sentence
int CDecoder::Splitline(const string &line)
{

    vector<string> vec;
	istrstream is(line.c_str());
	string part;

	while (is >> part)
	{
        vec.push_back(part);
	}
    return vec.size();
}
//移进-规约搜索
void CDecoder::Search(string &result, float &maxscore)
{
	m_vState.clear();

	//初始化状态栈
	CState ini;
	InitState(ini);
	m_vState.push_back(ini);

	//移进规约2N-1步
	int steps = 2 * int(m_vToken.size()) - 1;
	for (int i = 0; i < steps; ++i)
	{
		vector<CState> buf;

		for (size_t j = 0; j < m_vState.size(); ++j)
		{
			map<string, double> scores;
			GetScores(m_vState[j], scores);	

			vector<string> ops;
			GetOps(m_vState[j], ops);

			for (size_t k = 0; k < ops.size(); ++k)
			{
				CState next;
				Transition(m_vState[j], scores, ops[k], next);
				
				buf.push_back(next);
			}
		}
	
		sort(buf.begin(), buf.end());

		//选出最好的N个状态
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
    maxscore = m_vState[0].score;
	GetResult(m_vState[0], result);
}

//获取标准化分数
void CDecoder::GetScores(const CState &from, 
						 map<string, double> &scores)
{
	scores.clear();

	vector<string> feats;

	GenFeats(from, feats);
	vector<map<string,double> >v_cls_val;
	get_cls_val(feats,v_cls_val);

	double score_s = eval(v_cls_val, "SHIFT");
	double score_l = eval(v_cls_val,"LEFT");
	double score_r = eval(v_cls_val,"RIGHT");

	vector<double> v_scores;
	v_scores.push_back(score_s);
	v_scores.push_back(score_l);
	v_scores.push_back(score_r);
	normalize(v_scores);

	scores["SHIFT"] = v_scores[0]; 
	scores["LEFT"] = v_scores[1];
	scores["RIGHT"] = v_scores[2];
}

//初始状态
void CDecoder::InitState(CState &state)
{
	state.curPtr = 0;
	state.stack.clear();
	state.edges.clear();
	state.hisOps.clear();
	state.score = 0.0;
}

//获得在该状态下可行的操作
void CDecoder::GetOps(const CState &from, vector<string> &ops)
{
	ops.clear();

	if (from.curPtr < int(m_vToken.size()))
	{
		ops.push_back("SHIFT");
	}

	//栈内元素少于2个且队列不空
	if (from.stack.size() >= 2)
	{
		ops.push_back("LEFT");
		ops.push_back("RIGHT");
	}
}

//状态转移
void CDecoder::Transition(const CState &from,
						  map<string, double> &scores, 
						  const string &op, 
						  CState &to)
{
	to = from;
	to.score += scores[op];
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

//生成该状态下特征集
void CDecoder::GenFeats(const CState &from, vector<string> &feats)
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

//由动作序列生成分析结果
void CDecoder::GetResult(const CState &state, string &result)
{
	int root = state.stack.back().id;
	const vector<CDepPair> &edges = state.edges;
	vector<CDepNode> depNodes(m_vToken.size());
	size_t i;

	for (i = 0; i < edges.size(); ++i)
	{
		int head = edges[i].head;
		int mod = edges[i].modifier;

		if (mod < head)
		{
			depNodes[head].leftMods.push_back(mod);
		}
		else
		{
			depNodes[head].rightMods.push_back(mod);
		}
	}

	result = "(" + GenTree(depNodes, root) + ")";
}

string CDecoder::GenTree(const vector<CDepNode> &nodes, const int &root)
{
	string tree;
	int kid;

	for (kid = int(nodes[root].leftMods.size()) - 1; kid >= 0; --kid)
	{
		tree += "("
			+ GenTree(nodes, nodes[root].leftMods[kid])
			+ ") ";
	}

	tree += m_vToken[root].word + "/" + m_vToken[root].POS;

	for (kid = 0; kid < int(nodes[root].rightMods.size()); ++kid)
	{
		tree += " ("
			+ GenTree(nodes, nodes[root].rightMods[kid])
			+ ")";
	}

	return tree;
}

void CDecoder::get_cls_val( vector<string> & feat, vector<map<string,double> > & v_cls_val )
{
	v_cls_val.resize(feat.size());
	for (size_t i = 0;i < feat.size(); i++)
	{
		map<string,double> cls_val = m_pModel->prob(feat[i]);
		v_cls_val[i] = cls_val;
	}
}

double CDecoder::eval( vector<map<string,double> > & v_cls_val, const string & op)
{
	double score = 0.0;
	vector<double> v_val, v_score;
	v_val.resize(v_cls_val.size(),score);

	for (size_t i = 0;i < v_cls_val.size();i++)
	{
		map<string,double> cls_val = v_cls_val[i];
		map<string,double>::iterator it = cls_val.find(op);
		if (it != cls_val.end())
		{
			score += it->second;
			v_val[i] = it->second;
		}
	}

	return score;
}

void CDecoder::normalize( vector<double> & v_score )
{
	//avoid inf
	vector<double>::iterator it = max_element(v_score.begin(),v_score.end());
	double max_score = *it;

	//total score
	double sum = 0.0;
	for (size_t i = 0;i < v_score.size(); i++)
	{
        
		v_score[i] = exp(v_score[i]);
		//v_score[i] = exp(v_score[i] - max_score);
		sum += v_score[i];
	}

	//normalization
	for (size_t i = 0; i < v_score.size();i++)
	{
		//v_score[i] = v_score[i]/sum;
		v_score[i] = log(v_score[i]/sum);
	}
    //cout<<endl;

}
