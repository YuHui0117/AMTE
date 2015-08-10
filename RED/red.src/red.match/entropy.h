/*
 * =====================================================================================
 *
 *       Filename:  entropy.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2013/02/26 14时17分17秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __ENTROPY_H_
#define __ENTROPY_H_

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <strstream>
#include <vector>
#include <map>
#include <math.h>
#include <cmath>
#include "myalgorithm.h"
#include "rain_algorithm.h"
using namespace std;
extern bool DBG_bool;
extern vector<float> MET_weight;
bool concheck(vector<pair<int, int> > vecref, int s, int i)
{
  map<int,int> tmp;
  for(int j=s; j<i+1; j++)
    tmp.insert(make_pair(vecref[j].first, vecref[j].second));
  vector<pair<int,int> > vec;
  map<int,int>::iterator it;
  for(it=tmp.begin(); it!=tmp.end(); it++)
      vec.push_back(make_pair(it->first, it->second));
  if(vec.size()==1)
    return true;
  else {
    int m=1;
    while(m<vec.size() && vec[m-1].first+ vec[m-1].second ==vec[m].first){
        m++;
        
    }
    if(m==vec.size()) return true;
    else return false;
  }

}
vector<int> getchunks(vector<pair<int,int> > vechyp,vector<pair<int,int> > vecref)
{
//    cout<<vechyp.size()<<" "<<vecref.size()<<endl;
      vector<int> chunks;
       int i=0;
       int s=0;
       int t;
       int tagg=1;
       if(vechyp.size()==0) ;//chunks.push_back(0);
       else if(vechyp.size()==1) chunks.push_back(1);
       else {
         while(i<vechyp.size()){
           if(i<vechyp.size()-1){  
             if(vechyp[i+1].first==vechyp[i].first+vechyp[i].second){ 
                 i++;
             }
             else {
                //if(i==vechyp.size()-1) i++;
               while(i>=s){
                // cout<<"xwz"<<i<<" "<<s<<endl;
                 bool tag=concheck(vecref,s,i);
                 if(tag) {
                     //cout<<"ykuhui"<<i<<" "<<s<<endl;
                     int c=0;
                     for(int p=s; p<=i; p++)
                     {
                        c += vechyp[p].second; 
                     }
                     //chunks.push_back(i-s+1);
                     chunks.push_back(c);
                     i++; s=i;
                     break;
                 } 
                 else i--;
               }
             }
           }else{
             while(i>=s){
               bool tag=concheck(vecref,s,i);
               if(tag) {
                   //cout<<"yuhui "<<i <<" "<<s<<endl;
                     int c=0;
                     for(int p=s; p<=i; p++)
                     {
                        c += vechyp[p].second; 
                     }
                     //chunks.push_back(i-s+1);
                     chunks.push_back(c);
                   //chunks.push_back(i-s+1); 
                   i++; s=i;
                   break;
               } 
               else i--;
             }
           }  
         }
       }

       if(DBG_bool) {
           cout <<"length of every chunks: ";
           for(int i=0; i<chunks.size(); i++)
               cout<<chunks[i]<<"  ";
           cout << endl;
       } 
       return chunks;
}

int function_word(map<string, int> functionWords, string word)
{
    map<string,int>::iterator it;
        it=functionWords.find(word);
        if(it==functionWords.end())
            return 0;
        else
            return 1;
}
vector<int> matchhyp(map<string, int> functionWords, string line, string hyp, string ref, vector<pair<int,int> > &onegram_hyp, vector<pair<int,int> > & onegram_ref, vector<int> & chunks_bak)
{
      vector<string> vechyp = rain::split(hyp," "); 
      vector<string> vecref = rain::split(ref," "); 
    
      string word;
      vector<pair<int,int> > vechypmatch;
      vector<pair<int,int> > vecrefmatch;
      istringstream is(line);
      while(is >> word) {
       vector<string> vecstr= rain::split(word,"-");
       int aref = atoi(vecstr[0].c_str());
       int ahyp = atoi(vecstr[2].c_str());
       vechypmatch.push_back(make_pair(atoi(vecstr[2].c_str()),atoi(vecstr[3].c_str())));
       vecrefmatch.push_back(make_pair(atoi(vecstr[0].c_str()),atoi(vecstr[1].c_str())));
       for(int i=0; i<atoi(vecstr[3].c_str()); i++)
       {
           onegram_hyp.push_back(make_pair(atoi(vecstr[4].c_str()),function_word(functionWords, vechyp[ahyp+i])));
          //cout<<"test_2"<<endl;
       }
       for(int i=0; i<atoi(vecstr[1].c_str()); i++)
       {
           //cout<<aref<<" "<<vecref.size()<<" "<<ref<<endl; 
           onegram_ref.push_back(make_pair(atoi(vecstr[4].c_str()),function_word(functionWords, vecref[aref+i])));
           // cout<<"test_3"<<endl;
       }
      }
      vector<int> chunks=getchunks(vechypmatch,vecrefmatch);
      chunks_bak=getchunks(vecrefmatch,vechypmatch);
      return chunks;
}

float meteor_BP(vector<int> & chunks, int lenh, int lenr)
{
   ; 
}
float entropyscore(vector<int> & chunks, int lenh, int lenr){
    int i; 
    float score=0;
    float entry=0;
    int nummatch=0;
    for(int i=0; i<chunks.size(); i++)
        nummatch += chunks[i];
    if(chunks.size() == 0) return 0;
    else 
    {
      for(int i=0; i<chunks.size(); i++)
      { 
        if(DBG_bool) cout << chunks[i] << "  ";
        //float p=(float)chunks[i]/(float)lenh;
        float p=(float)chunks[i]/(float)nummatch;
        float tmp=-p*log(p);
        entry+=tmp;
      }
      float lp=fabs((float)lenh/(float)lenr-1);
      //float penalty=std::pow(weight[7],lp);
      float a=MET_weight[0];
      score = std::pow(a,-entry);
      //score = std::pow(a,-entry*penalty);
      if(DBG_bool) cout << endl;
      return score;
    }
}
int getlength(string line)
{
    istringstream is(line.c_str());
    string word;
    vector<string> vec;
    while(is>>word)
        vec.push_back(word);
    return vec.size();
}
#endif
