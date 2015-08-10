/*
 * =====================================================================================
 *
 *       Filename:  myalgorithm.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  17/09/12 10:13:31
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "treenode.h"
#ifndef __MYALGORITHM_H__
#define __MYALGORITHM_H__
#include <vector>
#include <algorithm>
#include <string>
#include <math.h>
#include <map>
#include <iostream>
using namespace std;
template<class T>
class vectorDist
{
    vector<T> m_vec1,m_vec2, m_vec3, m_vec4;
    public:
    vectorDist(vector<T> & v1, vector<T> &v2, vector<T> & v3, vector<T> & v4):m_vec1(v1),m_vec2(v2),m_vec3(v3), m_vec4(v4){}
    vectorDist(vector<T> & v1, vector<T> &v2):m_vec1(v1),m_vec2(v2){}
    float cosDist();
    float euclidDist(Config cfg);
    float euclidDist_continue();
    float LengthDist();
};
/*计算匹配分数时，用绝对距离*/
template<class T>
float vectorDist<T>::LengthDist()
{
    if(m_vec2.size() < 2 || m_vec1.size() < 2 || m_vec1.size() != m_vec2.size()){
    //if( m_vec1.size() != m_vec2.size()){
        cout<<"not match of m_vec2 "<<m_vec2.size()<<" and m_vec1 "<<m_vec1.size()<<endl;
        return 0;
    }
    if(m_vec1.size()==1&&m_vec2.size()==1)  return 1;
    float  distance1=0,distance2=0;
    for(size_t i=0;i<m_vec1.size()-1;i++)
    {
        for(size_t j=i+1;j<m_vec1.size();j++)
        {
            if(fabs((float)(m_vec1[j]-m_vec1[i])) > distance1) distance1 = fabs((float)(m_vec1[j]-m_vec1[i]));
            if(fabs((float)(m_vec2[j]-m_vec2[i])) > distance2) distance2 = fabs((float)(m_vec2[j]-m_vec2[i]));
        }
    }

    int numberofsegment = m_vec1.size();
    return exp(-1*fabs(distance2 - distance1));  
}
template<class T>
float vectorDist<T>::euclidDist_continue()
{
   // if(m_vec2.size() < 2 || m_vec1.size() < 2 || m_vec1.size() != m_vec2.size()){
    if(m_vec1.size() != m_vec2.size()){
        cout<<"not match of m_vec2 "<<m_vec2.size()<<" and m_vec1 "<<m_vec1.size()<<endl;
        return 0;
    }
    if(m_vec1.size()==1 && m_vec2.size()==1)  return 1;
    float  distance=0;
    typename  vector<T>::iterator it1 = m_vec1.begin();
    typename vector<T>::iterator it2 = m_vec2.begin();
    it1++;it2++;

    while(it1!=m_vec1.end())
    {
        distance += fabs(fabs((float)(*it1 - *(it1-1))) - fabs((float)(*it2 - *(it2-1)))) ;
        it1++;it2++;
    }

    int numberofsegment = m_vec1.size()-1;

    return exp(-1*distance/numberofsegment);  
}
/*计算匹配分数时,用欧式距离*/
template<class T>
float vectorDist<T>::euclidDist(Config cfg)
{
    if(m_vec1.size() != m_vec2.size()){
        cout<<"not match of m_vec2 "<<m_vec2.size()<<" and m_vec1 "<<m_vec1.size()<<endl;
        return 0;
    }
    if(m_vec1.size()==1 && m_vec2.size()==1) { 
        float wf;
        if(m_vec4[0] == 0) wf=1-cfg.wfun;
        else wf=cfg.wfun;
        return 1*cfg.wMatchModule[m_vec3[0]]*wf;
    
    }
    if(cfg.Train_bool){
    for(int p=0; p<m_vec4.size()-1;p++)
    {
        cout<<m_vec4[p]<<"_";
    }
    cout<<m_vec4[m_vec4.size()-1]<<"-";
    for(int p=0; p<m_vec3.size()-1;p++)
    {
        cout<<m_vec3[p]<<"_";
    }
    cout<<m_vec3[m_vec3.size()-1]<<"-";
    }
    float  distance=0;
    typename  vector<T>::iterator it1 = m_vec1.begin();
    typename vector<T>::iterator it2 = m_vec2.begin();
    it1++;it2++;
    int cnt=1;
    vector<int> dis1,dis2;
    while(it1!=m_vec1.end())
    {
        int a=*it1 - *(it1-1);
        int b=*it2 - *(it2-1);
      //  float wm = (weight[m_vec3[cnt]] + weight[m_vec3[cnt-1]])/(float)2;
      //  float wf1,wf2;
      //  if(m_vec4[cnt] == 0) wf2=1-para_func;
      //  else wf2=para_func;
      //  if(m_vec4[cnt-1] == 0) wf1=1-para_func;
      //  else wf1=para_func;
      //  float wf = (wf1+wf2)/(float)2;
        distance += fabs(fabs((float)a) - fabs((float)b)) ;
        dis1.push_back(fabs(fabs((float)a) - fabs((float)b))); 
        //  cout<<m_vec1[cnt-1]<<"-"<<m_vec2[cnt-1]<<"-"<<m_vec3[cnt-1]<<"-"<<m_vec4[cnt-1]<<"-"<<wm<<"-"<<wf<<endl;;
        it1++;it2++;cnt++;
    }

    if(cfg.Train_bool){
    for(int p=0; p<dis1.size()-1;p++)
    {
        cout<<dis1[p]<<"_";
    }
    cout<<dis1[dis1.size()-1]<<" ";
    }

    float wf=0,wm=0,wf2;
    if(cfg.Entropy_bool && cfg.Function_bool)
    {
        for(int i=0; i<m_vec3.size(); i++)
        {
            if(m_vec4[cnt] == 0) wf2=1-cfg.wfun;
            else wf2=cfg.wfun;
            wf += wf2;
            wm += cfg.wMatchModule[m_vec3[i]];
        }
        wf = wf/(float)m_vec3.size();
        wm = wm/(float)m_vec4.size();
    }
    int numberofsegment = m_vec1.size()-1;
    if(cfg.Align_bool && cfg.Function_bool)
        return exp(-1*distance/numberofsegment)*wf*wm;  
    if(cfg.Align_bool && !cfg.Function_bool)
        return exp(-1*distance/numberofsegment)*wm;  
    if(!cfg.Align_bool && cfg.Function_bool)
        return exp(-1*distance/numberofsegment)*wf;  
    if(!cfg.Align_bool && !cfg.Function_bool)
        return exp(-1*distance/numberofsegment);  
}
/*计算匹配分数时，用cosin距离*/
template<class T>
float vectorDist<T>::cosDist(){

    if(m_vec2.size() <= 1 || m_vec1.size() <= 1 || m_vec1.size() != m_vec2.size()) return 0;

    float numerator= 0;float denominator1= 0;float denominator2=0;
    typename  vector<T>::iterator it1 = m_vec1.begin();
        typename vector<T>::iterator it2 = m_vec2.begin();
        it1++;it2++;

        while(it1!=m_vec1.end())
        {
            numerator += (*it1 - *(it1-1)) * (*it2 - *(it2-1)) ;
            denominator1 +=pow((*it1 - *(it1-1)),(float)2 );
            denominator2 +=pow((*it2 - *(it2-1)),(float)2 );
            it1++;it2++;
        }
        if(fabs(numerator/pow((denominator1*denominator2 + 0.00000001),0.5)) > 1 + 0.0001 ) 
        {
            cout<<"error consine vale "<<__LINE__<<endl;
            exit(0);
        }
        return fabs(numerator/pow((denominator1*denominator2+ 0.00000001),0.5));  
};

    template<class T>
std::vector<T> range(T curr, T const &up)
{
    std::vector<T> result;
    for (; curr <= up; ++curr)
        result.push_back(curr);
    return result;
}
template<class T>
std::vector< std::vector<T> > kSubsets(std::vector<T> vec, unsigned k)
{
    std::vector< std::vector<T> > result;
    if (!k)
        return result;
    int n = vec.size();
    std::vector<int> idx_vec = range<int>(0, k - 1);
    while (true)
    {
        int pos;
        result.push_back(idx_vec);
        for (pos = 0; pos < (int)result.back().size(); ++pos)
            result.back()[pos] = vec[result.back()[pos]];
        //next kSubset
        pos = k - 1;
        ++idx_vec[pos];
        while (idx_vec[pos] >= n + pos - (int)k + 1)
        {
            if (--pos < 0)
                return result;
            ++idx_vec[pos];
            for (int i = pos + 1; i < (int)k; ++i)
                idx_vec[i] = idx_vec[pos] + i - pos;
        }
    }
    // unreachable
    return result;
}

size_t getTokenNumber(string s);
vector<vector<int> >  decompress_path(vector<vector<int> >&pos_hyp_conpress);
vector<string> getVector(string s);
map<string,int> getMatchGram(map<string,int> & ref, map<string,int> & tgt);
size_t countall(map<string,int> & m);
//vector<pair<float,float> > getRealBleu(float reflen,bool Smooth_bool, vector<map<string,int> > & ref_gramcount,vector<map<string,int> > & tgt_gramcount,int Ngram);
//vector<pair<float,float> > getBLEU(vector<int> &a, vector<int> &b, string ref, string tgt, int Ngram,bool Smooth_bool,float &sys_bleurefleng,vector<map<string,int> > & vec_sys_bleutstCnt,vector<map<string,int> > &vec_sys_bleurefCnt);
void addInNgram(map<string,int> &vec_map,map<string,int> gc);
map<string,int> getGramCount(vector<string> & words, int ngram);
#endif
