/*
 * =====================================================================================
 *
 *       Filename:  myalgorithm.cpp
 *
 *    Description:  myalgorithm.cpp
 *
 *        Version:  1.0
 *        Created:  19/09/12 20:19:13
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "myalgorithm.h"
#include <vector>
#include <algorithm>
#include <math.h>
#include <sstream>
#include <iostream>
#include <assert.h>
using namespace std;
extern bool DBG_bool;
/*获得s中单词的个数*/
size_t getTokenNumber(string s)
{

    size_t number = 0;
    string word;
    istringstream iss(s); 
    while(iss>>word)
        number++;
    return number;
}
/*由s得到一个vector*/
vector<string> getVector(string s)
{
    vector<string> vecTokenized;
   // size_t number = 0;
    string word;
    istringstream iss(s);
    while(iss>>word)
    {
        vecTokenized.push_back(word);
    }
    return vecTokenized;
}
map<string,int> getGramCount(vector<string> & words, int ngram)
{
    map<string,int> ret;
    for(int i = 0; i <= (int)words.size() - ngram; i++)
    {
        string ss;

       for(int j = i; j < ngram +i  ; j++)
       {
            ss += words[j];
            if(j+1 < ngram +i) ss += " ";
       }

       if(ret.find(ss) == ret.end()){
           ret.insert(make_pair(ss,1));
       }else{
           ret[ss] += 1;
       }
       //cout<<ss<<ret.size()<<endl;
    }
    return ret;
}

map<string,int> getMatchGram(map<string,int> & ref, map<string,int> & tgt)
{//
    map<string,int> ret;
    for(map<string,int>::iterator it = tgt.begin();
            it != tgt.end();
            it++)
    {
        if(ref.find(it->first) != ref.end())
        {
            ret.insert(make_pair(it->first,min(it->second,ref[it->first])));
        }
    }
    return ret;
}
/*计算m中所有string的个数*/
size_t countall(map<string,int> & m)
{
    size_t ret=0;
    for(map<string,int>::iterator it = m.begin();
            it != m.end();
            it++)
    {
        ret += it->second;
    }
    return ret;
}
/*分解路径*/
vector<vector<int> >  decompress_path(vector<vector<int> >&pos_hyp_conpress)
{
    vector<vector<int> > tmpvv,pos_hyp_decomp,filt_pos_hyp_decomp;

    if(pos_hyp_conpress.empty())return pos_hyp_decomp;

    for(size_t iterv = 0 ; iterv < pos_hyp_conpress[0].size(); iterv++)
    {
        vector<int> tmp;
        tmp.push_back( pos_hyp_conpress[0][iterv]);
        pos_hyp_decomp.push_back(tmp);
    }


    for(size_t start =1; start<pos_hyp_conpress.size();start++)
    {
        tmpvv = pos_hyp_decomp;
        pos_hyp_decomp.clear();
        for(size_t iter = 0; iter < tmpvv.size(); iter++)
        {
            for(size_t iterv = 0 ; iterv < pos_hyp_conpress[start].size(); iterv++)
            {
                vector<int> tmp = tmpvv[iter];
                tmp.push_back( pos_hyp_conpress[start][iterv]);
                pos_hyp_decomp.push_back(tmp);
            }
        }
    }
    //filter the cases that cannot satisfy the order of the ref_link in pos_hyp_decomp
    
    for(int i=0; i<pos_hyp_decomp.size(); i++)
    {
        int tag=0;
        for(int j=0; j<pos_hyp_decomp[i].size()-1; j++)
        {

            if(pos_hyp_decomp[i][j]>pos_hyp_decomp[i][j+1])
            {
                tag=1; break;
            }
        }
        if(tag==0)
            filt_pos_hyp_decomp.push_back(pos_hyp_decomp[i]);
    }
    return filt_pos_hyp_decomp;
}

//vector<pair<float,float> > getRealBleu(float reflen,bool Smooth_bool, vector<map<string,int> > & ref_gramcount,vector<map<string,int> > & tgt_gramcount,int Ngram)
//{
//    float tgtlen = countall(tgt_gramcount[0]);
//    float BP = min(1-(float)reflen/(float)tgtlen,(float)0.0);
//    float BPexp = tgtlen > 0 ? exp(min(1-(float)reflen/(float)tgtlen,(float)0.0)) : 0;
//
//    vector<pair<float,float> > ngrm_bleu_score;
//    float score =0,iscore=0;
//    float smooth = 1;
//    if(DBG_bool) cout<<"BLEU:"<<endl;
//    for(int i = 0; i<Ngram; i++)
//    {
//        map<string,int> match = getMatchGram(ref_gramcount[i],tgt_gramcount[i]);
//
//        if(!Smooth_bool)
//        {
//            if(match.size() == 0)
//            {
//                ngrm_bleu_score.push_back(make_pair(0,0)) ;
//            }
//            else{
//                score += log( (float)countall(match)/(float)countall(tgt_gramcount[i]));
//                if(DBG_bool)cout<<"match "<<i+1<<" gram are: "<<countall(match)<<"  BP: "<<BP<<"  tgt word number: "<<countall(tgt_gramcount[i])<<endl;
//                //if(i==Ngram-1)
//					//ngrm_bleu_score.push_back(make_pair(exp(score/(i+1) + BP),(float)countall(match)));
//				//else 
//					ngrm_bleu_score.push_back(make_pair(score,(float)countall(match)));
//				
//			}
//        }else
//        {
//            double iscore_tmp=0;
//            //if(words_tgt.size() == 0)
//            if(tgt_gramcount[0].size() == 0)
//            {
//                iscore = 0;
//                //iscore_tmp=0;
//            }else if(match.size() == 0)
//            {
//                smooth *= 2;
//                //iscore_tmp=0.001/countall(tgt_gramcount[i]);
//                iscore = log( 1 / ( smooth * tgtlen) );
//            }
//            else{
//                //iscore = log((float) match.size() / (float)tgtlen );
//                //iscore_tmp=(float)countall(match)/(float)countall(tgt_gramcount[i]);
//				iscore = log( (float)countall(match)/(float)countall(tgt_gramcount[i]));
//                if(DBG_bool){
//                    cout<<"match "<<i+1<<" gram are: "<<countall(match)<<"  tgt word number: "<<countall(tgt_gramcount[i])<<endl;
//                   
//                }
//            }
//            score += iscore;
//		    ngrm_bleu_score.push_back(make_pair(score,(float)countall(match)));
//		    //ngrm_bleu_score.push_back(make_pair(score,exp(iscore)*BPexp));
//           // ngrm_bleu_score.push_back(make_pair(score,iscore_tmp));
//        }
//        }
//        //cout<<"bleu at "<<i<<" gram is "<<ngrm_bleu_score[i]<<endl;
//	if(!Smooth_bool) ngrm_bleu_score.push_back(make_pair(exp(score/Ngram + BP),BP));//only use the first value
//	else ngrm_bleu_score.push_back(make_pair(exp( score / Ngram ) * BPexp,BPexp));
//   // ngrm_bleu_score.push_back(make_pair(BPexp,BPexp));
//    return ngrm_bleu_score;
//
//}
//vector<pair<float,float> > getBLEU(vector<int> & ref_i_gramcount,vector<int> &tgt_i_gramcount, string ref, string tgt, int Ngram,bool Smooth_bool,
//        float &sys_bleurefleng,vector<map<string,int> > & vec_sys_bleutstCnt,vector<map<string,int> > &vec_sys_bleurefCnt)
//{
//    vector<map<string,int> > ref_gramcount,tgt_gramcount;
//    vector<string> words_ref = getVector(ref);
//    sys_bleurefleng += words_ref.size();
//    vector<string> words_tgt = getVector(tgt);
//    for(int i = 0 ; i < Ngram; i++)
//    {//bulid gramcount
//        map<string,int> gc = getGramCount(words_ref,i+1);
//        ref_gramcount.push_back(gc);
//        addInNgram(vec_sys_bleurefCnt[i], gc);
//        gc.clear();
//        gc = getGramCount(words_tgt,i+1);
//        tgt_gramcount.push_back(gc);
//        addInNgram(vec_sys_bleutstCnt[i] , gc);
//    }
//    
//    for(int i=0; i<Ngram; i++)
//    {
//        ref_i_gramcount.push_back(countall(ref_gramcount[i]));
//        tgt_i_gramcount.push_back(countall(tgt_gramcount[i]));
//    }
//    assert(words_ref.size() == countall(ref_gramcount[0]));
//
//    //int reflen = countall(ref_gramcount[0]);
//    float reflen = (float)countall(ref_gramcount[0]);
//    return getRealBleu(reflen, Smooth_bool, ref_gramcount, tgt_gramcount,Ngram);
//}

void addInNgram(map<string,int> &vec_map,map<string,int> gc)
{
    if(vec_map.empty()){
        vec_map = gc;
        return;
    }

    map<string,int> newmap;
    map<string,int>::iterator it2,it1;
    //it1 = vec_map.begin();
    it2 = gc.begin();
	for(it2=gc.begin();it2!=gc.end();it2++)
	{
		it1=vec_map.find(it2->first);
		if(it1!=vec_map.end())
			it1->second = it1->second+it2->second;
		else
			vec_map.insert(make_pair(it2->first,it2->second));
	}
    /*while(it1!= vec_map.end()&&it2!=gc.end())
    {
        if(it2->first < it1->first)
        {
            newmap.insert(make_pair(it2->first,it2->second));
            it2++;
        }
        else if(it2->first > it1->first){
            
            newmap.insert(make_pair(it1->first,it1->second));
            it1++;
        }else{
            newmap.insert(make_pair(it1->first,it1->second+it2->second));
			it1++; it2++;
        }
    }
	if(it1==vec_map.end()&&it2!=gc.end())
	{
		for(;it2!=gc.end();it2++)
			newmap.insert(make_pair(it2->first,it2->second));
	}
	if(it1!=vec_map.end()&&it2==gc.end())
	{
		for(;it1!=vec_map.end();it1++)
			newmap.insert(make_pair(it1->first,it1->second));
	}*/
    //vec_map.clear() ;
   // vec_map = newmap;
}
