#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "DepTree.h"


CDepTree::CDepTree(const string &str)
{
	if (str.empty() || !Check(str))
	{
		m_pRoot = 0;
	}
	else
	{
		m_pRoot = Create(str);
		m_pRoot->parrent = 0;
	}
}


CDepTree::~CDepTree()
{
	Release(m_pRoot);
}

void CDepTree::Tokens(vector<CToken> &toks)
{
	toks.clear();
	RecurTokens(m_pRoot, toks);
}

void CDepTree::RecurTokens(CDepNode *root, vector<CToken> &toks)
{
	size_t kid;

	for (kid = 0; kid < root->leftKids.size(); ++kid)
	{
		RecurTokens(root->leftKids[kid], toks);
	}

	string word;
	string POS;
	const string &val = root->val;
	size_t split = val.rfind("/");

	if (split != string::npos)
	{
		word = val.substr(0, split);
		POS = val.substr(split + 1, val.length() - split - 1);
	}
	else
	{
		word = val;
		POS = "";
	}

	CToken tok(word, POS);

	toks.push_back(tok);

	for (kid = 0; kid < root->rightKids.size(); ++kid)
	{
		RecurTokens(root->rightKids[kid], toks);
	}
}

string CDepTree::Graph()
{
	return RecurGraph(m_pRoot, 0);
}

string CDepTree::RecurGraph(CDepNode *root, const int &lev)
{
	string graph;
	size_t kid;

	for (kid = 0; kid < root->leftKids.size(); ++kid)
	{
		graph += RecurGraph(root->leftKids[kid], lev + 1);
	}

	int i;

	for (i = 0; i < lev; ++i)
	{
		graph += "\t";
	}

	graph += root->val + "\n";

	for (kid = 0; kid < root->rightKids.size(); ++kid)
	{
		graph += RecurGraph(root->rightKids[kid], lev + 1);
	}

	return graph;
}

void CDepTree::DepPairs(vector<CDepPair> &deps)
{
	deps.clear();

	int leafID = 0;
	int rootID = 0;

	RecurDepPairs(m_pRoot, deps, leafID, rootID);
	deps.push_back(CDepPair(-1, rootID));
}

void CDepTree::RecurDepPairs(CDepNode *root,
							 vector<CDepPair> &deps,
							 int &leafID,
							 int &rootID)
{
	vector<int> subHeads;
	size_t kid;

	for (kid = 0; kid < root->leftKids.size(); ++kid)
	{
		int subHead;

		RecurDepPairs(root->leftKids[kid], deps, leafID, subHead);
		subHeads.push_back(subHead);
	}

	rootID = leafID;
	++leafID;

	for (kid = 0; kid < root->rightKids.size(); ++kid)
	{
		int subHead;

		RecurDepPairs(root->rightKids[kid], deps, leafID, subHead);
		subHeads.push_back(subHead);
	}

	size_t i;

	for (i = 0; i < subHeads.size(); ++i)
	{
		deps.push_back(CDepPair(rootID, subHeads[i]));
	}
}

bool CDepTree::Check(const string &str)
{
	size_t cntLB = count(str.begin(), str.end(), '(');
	size_t cntRB = count(str.begin(), str.end(), ')');

	if (cntLB != cntRB || cntLB == 0)
	{
		return false;
	}

	return true;
}

CDepNode *CDepTree::Root()
{
	return m_pRoot;
}

CDepNode *CDepTree::Create(const string &str)
{
	size_t p;

	for (p = 0; str[p] != '('; ++p);
	for (++p; str[p] == ' '; ++p);

	CDepNode *root = new CDepNode;
	size_t end;

	for (end = str.length() - 1; str[end] != ')'; --end);
	string s = str.substr(p, end-p+1);
	bool meetHead = false;

	while (p < end)
	{
		if (str[p] != '(')
		{
			string val;

			for (; p < end && str[p] != ' ' && str[p] != '('; ++p)
			{
				val += str[p];
			}

			root->val = val;
			meetHead = true;
		}
		else
		{
			string sub;
			int leftB = 0;
			int rightB = 0;

			for (; sub.empty() || leftB != rightB; ++p)
			{
				sub += str[p];

				if (str[p] == '(')
				{
					++leftB;
				}
				else if (str[p] == ')')
				{
					++rightB;
				}
			}

			CDepNode *subRoot = Create(sub);

			if (!meetHead)
			{
				root->leftKids.push_back(subRoot);
			}
			else
			{
				root->rightKids.push_back(subRoot);
			}

			subRoot->parrent = root;
		}

		for (; p < end && str[p] == ' '; ++p);
	}

	return root;
}

void CDepTree::Release(CDepNode *ptr)
{
	size_t kid;

	for (kid = 0; kid < ptr->leftKids.size(); ++kid)
	{
		Release(ptr->leftKids[kid]);
	}

	for (kid = 0; kid < ptr->rightKids.size(); ++kid)
	{
		Release(ptr->rightKids[kid]);
	}

	delete ptr;
	ptr = NULL;
}
