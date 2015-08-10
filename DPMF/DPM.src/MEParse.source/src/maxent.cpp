#include "../include/maxent.h"
#include <iterator>
#include <algorithm>

int maxent::load_maxent_model(char* me_file)
{
	ifstream f_in(me_file);
	if (!f_in)
	{
		cerr << "MaxEnt model --" << me_file << "-- read error!" << endl;
		return 0;
        //exit(1);
	}
	cout << "loading  maxent model from file: " << me_file  << "..." ;

	string str_line;
	//跳过注释行和空行
	while(getline(f_in, str_line))
	{
		if ( str_line.empty() || '#'== str_line[0]   )
			continue;
		else 
			break;
	}

	//1. 读取特征
	unsigned feats_num = atoi(str_line.c_str());
	v_feat.resize(feats_num);
	for ( unsigned i = 0; i < feats_num ; ++i )
	{
		getline(f_in, str_line);
		v_feat[i] = str_line;
	}
	
	//2. 读取类别
	getline(f_in, str_line);
	unsigned cls_num = atoi(str_line.c_str());
	v_cls.reserve(cls_num);
	for (unsigned i = 0; i < cls_num; ++i)
	{
		getline(f_in,str_line);
		v_cls.push_back(str_line);
	}

	//3. 读取特征分类信息
	vector<vector<int> > v_feat_cls;
	v_feat_cls.reserve(feats_num);
	size_t total_cls_num = 0;	//记录总的分类数量
	for (unsigned i = 0; i < feats_num ; ++i )
	{
		getline(f_in, str_line);

		//处理一个特征分类信息
		istringstream istr(str_line);
		vector<int> v_type;
		int type;
		bool b_first = true;
		while( istr >> type )
		{
			if(b_first == true)
			{
				v_type.reserve(type);
				b_first = false;
				continue;
			}
			v_type.push_back(type);
			++total_cls_num;
		}
		v_feat_cls.push_back(v_type);
	}

	//4. 读入特征对应分类的概率
	getline(f_in, str_line);
	size_t feat_prob_num = atoi(str_line.c_str());
	assert(total_cls_num == feat_prob_num);
	
	size_t total_prob_num = 0;	//记录概率数目
	for (size_t i = 0 ; i < v_feat_cls.size() ; i++)
	{
		// 处理一个特征分类信息
		string feat = v_feat[i];	//特征
		vector<int> v_type = v_feat_cls.at(i);	//分类
		
		map<string,double> map_cls_val;
		for (size_t j = 0; j < v_type.size(); j++)
		{
			getline(f_in,str_line);
			++total_prob_num;

			//string to double
			double prob = atof(str_line.c_str());
		
			int idx_cls = v_type[j];
			string str_cls = v_cls[idx_cls];
			
			map_cls_val.insert(make_pair(str_cls,prob));
		}
		m_model.insert(make_pair(feat,map_cls_val));
	}

	assert(total_prob_num == total_cls_num);
	
	f_in.close();
	cout << "OK!" << endl;
    return 1;
}

double maxent::prob( const string &feat, const string & cls )
{
	double prob = 0.0;
	map<string,map<string,double> >::iterator it = m_model.find(feat);
	if (it != m_model.end())
	{
		map<string,double>::iterator it2 = it->second.find(cls);
		if (it2 != it->second.end())
		{
			prob = it2->second;
		}
	}
	return prob;
}

map<string,double> maxent::prob( const string & feat )
{
	map<string,double> cls_prob;
	for (size_t i = 0;i < v_cls.size(); i++)
	{
		cls_prob.insert(make_pair(feat,0.0));
	}
	
	map<string,map<string,double> >::iterator it = m_model.find(feat);
	if (it != m_model.end())
	{
		cls_prob = it->second;
        map<string,double> tmp=it->second;
        map<string,double>::iterator itt=tmp.begin();
        //cout<<"feat: "<<feat<<" cal: "<<itt->first<<" s:"<<itt->second<<endl;
	}
	return cls_prob;
}

void maxent::print(ostream & out)
{
	out << "#txt,maxent" << endl;
	out << v_feat.size() << endl;
	copy(v_feat.begin(),v_feat.end(),ostream_iterator<string>(out,"\n"));
	out << v_cls.size() << endl;
	copy (v_cls.begin(),v_cls.end(),ostream_iterator<string>(out,"\n"));

	vector<double> v_val;
	for (size_t i = 0;i < v_feat.size(); i++)
	{
		string feat = v_feat[i];
		map<string,map<string,double> >::iterator it = m_model.find(feat);
		
		map<string,double> m_cls_val = it->second;
		
		vector<pair<int,double> > v_cls_val;
		map<string,double>::iterator it2;
		for (it2 = m_cls_val.begin(); it2 != m_cls_val.end(); it2++)
		{
			string cls = it2->first;
			vector<string>::iterator it_cls = find(v_cls.begin(),v_cls.end(),cls);
			int pos = it_cls - v_cls.begin();
			v_cls_val.push_back(make_pair(pos,it2->second));
		}

		sort(v_cls_val.begin(),v_cls_val.end());
		
		vector<int> v_cls_info;
		v_cls_info.push_back(v_cls_val.size());
		for(size_t i = 0;i < v_cls_val.size();i++)
		{
			v_cls_info.push_back(v_cls_val.at(i).first);
			v_val.push_back(v_cls_val.at(i).second);
		}
		
		copy(v_cls_info.begin(),v_cls_info.end(),ostream_iterator<int>(out," "));
		out << endl;
	}
	out << v_val.size() << endl;
	copy(v_val.begin(),v_val.end(),ostream_iterator<double>(out,"\n"));
}
