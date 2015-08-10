#include <iostream>
#include <fstream>
#include <strstream>
#include <algorithm>
#include "DepTree.h"
#include "Simulator.h"

CSimulator::CSimulator()
{
}

CSimulator::~CSimulator()
{
}

//ģ�����״̬
void CSimulator::Simulate(const vector<CDepPair> &deps, 
						  vector<string> &ops)
{
	GetDepInfo(deps);
	InitState(deps);
	TraceState(ops);
}

//ͳ��ģ����������������Ϣ
void CSimulator::GetDepInfo(const vector<CDepPair> &deps)
{
	m_vDepRight.clear();
	m_vDepRight.resize(deps.size());
	m_vvDep.clear();
	m_vvDep.resize(deps.size());

	size_t i;

	for (i = 0; i < deps.size(); ++i)
	{
		m_vvDep[i].resize(deps.size());
	}

	for (i = 0; i < deps.size(); ++i)
	{
		int head = deps[i].head;
		int mod = deps[i].modifier;

		if (head != -1)
		{
			m_vvDep[head][mod] = 1;

			if (m_vDepRight[head] < mod)
			{
				m_vDepRight[head] = mod;
			}
		}
	}
}

//��ʼ���������ٵ�״̬
void CSimulator::InitState(const vector<CDepPair> &deps)
{
	m_nLen = int(deps.size());
	m_nCurPtr = 0;
	m_vStack.clear();
}

//���ٳ��������Ķ������в����ѵ��ʵ��
void CSimulator::TraceState(vector<string> &ops)
{
	ops.clear();

	while (true)
	{
		if (m_vStack.size() < 2)
		{
			if (m_nCurPtr < m_nLen)
			{
				Transition("SHIFT", ops);
			}
			else
			{
				break;
			}
		}
		else
		{
			int topIdx = int(m_vStack.size()) - 1;
			int topRight = m_vStack[topIdx];
			int topLeft = m_vStack[topIdx - 1];

			if (m_vvDep[topRight][topLeft] == 1)
			{
				Transition("LEFT", ops);
			}
			else if (m_vvDep[topLeft][topRight] == 1)
			{
				if (m_vDepRight[topRight] < m_nCurPtr)
				{
					Transition("RIGHT", ops);
				}
				else
				{
					if (m_nCurPtr < m_nLen)
					{
						Transition("SHIFT", ops);
					}
					else
					{
						cerr << "Tree Error" << endl;
						break;
					}
				}
			}
			else
			{
				if (m_nCurPtr < m_nLen)
				{
					Transition("SHIFT", ops);
				}
				else
				{
					cerr << "Tree Error" << endl;
					break;
				}
			}
		}
	}
}

//�������ͽ���״̬ת�Ʋ����ʵ��
void CSimulator::Transition(const string &op, vector<string> &ops)
{
	ops.push_back(op);

	if (op == "SHIFT")
	{
		m_vStack.push_back(m_nCurPtr);
		++m_nCurPtr;
	}
	else 
	{
		int topIdx = int(m_vStack.size()) - 1;
		int topRight = m_vStack[topIdx];
		int topLeft = m_vStack[topIdx - 1];

		m_vStack.pop_back();
		m_vStack.pop_back();

		if (op == "LEFT")
		{
			m_vStack.push_back(topRight);
		}
		else if (op == "RIGHT")
		{
			m_vStack.push_back(topLeft);
		}
	}
}
