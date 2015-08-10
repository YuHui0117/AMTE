#include <map>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <string>
#include <math.h>
#include <assert.h>
#include <vector>
#include "rain_algorithm.h"
using namespace rain;
using namespace std;
extern bool DBG_bool, BP_bool, ngramLength_bool;
extern vector<float> weight,params;
extern vector<vector<int> > chunks_sys;
class Match{
    public:
        int start;
        int len;
        int matchStart;
        int matchLen;
        double prob;
        int module;
    };
map<string,int> getFunList(string funFile)
{
    map<string,int> mFunList;
    ifstream infun;
    infun.open(funFile.c_str());
    if(!infun.is_open()) { cout<<"no file: "<<funFile<<endl; exit(0); }
    string line;
    while(getline(infun,line)){
        mFunList.insert(make_pair(line,1));
    }
    return mFunList;
}
vector<int> tagFunction(map<string,int> &mFunList, string line)
{
    vector<int> vfun;
    vector<string> vec = rain::split(line," ");
    map<string,int>::iterator it;
    for(int i=0; i<vec.size(); i++){
        it = mFunList.find(vec[i]);
        if(it != mFunList.end())
            vfun.push_back(1);
        else
            vfun.push_back(0);
    }
    return vfun;
}
vector<int> matchhyp(vector<Match> vMatchWords)
{
    vector<int> chunkLength;
    int matches1 = 0;
    int matches2 = 0;
    int chunkLength1 = 0;
    int chunkLength2 = 0;
    int idx = 0;
    int lastMatchEnd = -1;
    while (idx < vMatchWords.size()){
        Match m = vMatchWords[idx];
        if (m.start == -1){
            if (lastMatchEnd != -1) {
                chunkLength.push_back(chunkLength1);
                chunkLength1 = 0;
                chunkLength2 = 0;
                lastMatchEnd = -1;
            }
            idx++;
        } else {
            matches1 += m.matchLen;
            matches2 += m.len;
            if(lastMatchEnd != -1 && m.matchStart != lastMatchEnd){
                chunkLength.push_back(chunkLength1);
                chunkLength1 = m.matchLen;
                chunkLength2 = m.len;
            } else {
                chunkLength1 += m.matchLen;
                chunkLength2 += m.len;
            }
            idx = m.start + m.len;
            lastMatchEnd = m.matchStart + m.matchLen;
        }
    }
    if (lastMatchEnd != -1){
        chunkLength.push_back(chunkLength1);
        chunkLength1 = 0;
        chunkLength2 = 0;
    }
    return chunkLength;
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
float entropyscore(vector<int> & chunks, int lenh, int lenr){
    float score=0;
    float entry=0;
    int nummatch=0;
    for(int i=0; i<chunks.size(); i++){
        nummatch += chunks[i];
    }
    if(chunks.size() == 0) return 0;
    else {
      for(int i=0; i<chunks.size(); i++){ 
        float p=(float)chunks[i]/(float)nummatch;
        float tmp=-p*log(p);
        entry+=tmp;
        }
      //float lp=fabs((float)lenh/(float)lenr-1);
      //float penalty=std::pow(params[2],lp);
      //score = std::pow(params[3],-entry*penalty);
      score = std::pow(params[3], -entry);
      return score;
    }
}
float getFmean(vector<Match> &vMatchWords, vector<int> &vFunHyp, vector<int> &vFunRef)
{
    float hypLenFun=0;
    float refLenFun=0;
    float weightMatch=0;
    float weightMatchRef=0;
    for(int i=0; i<vFunHyp.size(); i++){
        if(vFunHyp[i] == 1)
            hypLenFun += 1*params[1];
        else
            hypLenFun += 1*(1-params[1]);
    }
    for(int i=0; i<vFunRef.size(); i++){
        if(vFunRef[i] == 1)
            refLenFun += 1*params[1];
        else
            refLenFun += 1*(1-params[1]);
    }
    for(int i=0; i<vMatchWords.size(); i++){
       if(vMatchWords[i].start != -1){
           for(int j=0; j<vMatchWords[i].matchLen; j++){
               if(vFunHyp[vMatchWords[i].matchStart + j] ==1)
                   weightMatch += weight[vMatchWords[i].module] * params[1];
               else
                   weightMatch += weight[vMatchWords[i].module] * (1-params[1]);
           }
           for(int j=0; j<vMatchWords[i].len; j++){
               if(vFunRef[vMatchWords[i].start + j] ==1)
                   weightMatchRef += weight[vMatchWords[i].module] * params[1];
               else
                   weightMatchRef += weight[vMatchWords[i].module] * (1-params[1]);
           }
           }
    }
    float precision=weightMatch/hypLenFun;
    float recall = weightMatchRef/refLenFun;
    float fmean;
    if(hypLenFun !=0 && refLenFun !=0)
        fmean = precision * recall / (params[0] * precision + (1-params[0]) * recall);
    else
        fmean = 0;
    if(DBG_bool) cout<<"weightMatch: "<<weightMatch<<" hypLenFun: "<<hypLenFun<<"refLenFun: "<<refLenFun<<endl;
    return fmean;
}
float getEntScore(int &ngramLen, vector<Match> &vMatchWords, int hypLen, int refLen)
{
    vector<int> chunks = matchhyp(vMatchWords);
    if(ngramLength_bool)
        for(int i=0; i<chunks.size(); i++)
            if(chunks[i] > 4)
                ngramLen++;

    if(DBG_bool){
        cout<<"chunks: "<<endl;
        for(int i=0; i<chunks.size(); i++)
            cout<<chunks[i]<<" ";
        cout<<"end."<<endl;
    }
    chunks_sys.push_back(chunks);
    int match_count=0;
    for(int p=0; p<chunks.size(); p++)
        match_count += chunks[p]; // cout<<chunks[p]<<"-";
    float entscore;
    if(match_count>0) 
        entscore=entropyscore(chunks,hypLen, refLen); 
    else 
        entscore=0;
    return entscore;   
}
void metricScore(string alignFile,string funFile)
{
    string line,hyp,ref;
    ifstream inalign;
    inalign.open(alignFile.c_str()); 
    if(!inalign.is_open()) { cout<<"no file: "<<alignFile<<endl; exit(0); }

    map<string,int> mFunList = getFunList(funFile);
    vector<Match> vMatchWords;
    vector<int> vFunRef, vFunHyp;
    float scoreSysEnt=0;
    float scoreSysFmean=0;
    float scoreSys=0;
    int ngramLen=0;
    int sentCnt=0;
    while(getline(inalign, line))
    {
        if(line.find("Alignment\t")==0){
            getline(inalign,hyp);
            getline(inalign,ref);
            vFunRef = tagFunction(mFunList,ref);
            vFunHyp = tagFunction(mFunList,hyp);
            int refLength = getlength(ref); 
            Match mtmp; mtmp.start = -1;
            vMatchWords.clear();
            for(int i=0; i<refLength; i++)
                vMatchWords.push_back(mtmp);
            getline(inalign,line);
            while(getline(inalign,line)){
                if(line.size()>1){
                    vector<string> vtmp = rain::split(line,"\t");
                    vector<string> vtmpr = rain::split(vtmp[0],":");
                    vector<string> vtmph = rain::split(vtmp[1],":");
                    Match m;
                    m.start = atoi(vtmpr[0].c_str());
                    m.len = atoi(vtmpr[1].c_str());
                    m.matchStart = atoi(vtmph[0].c_str());
                    m.matchLen = atoi(vtmph[1].c_str());
                    m.prob = atof(vtmp[3].c_str());
                    m.module = atoi(vtmp[2].c_str());
                    vMatchWords[m.start] = m;
                }
                else
                    break;
            }
      
        float entScore = getEntScore(ngramLen, vMatchWords, vFunHyp.size(), vFunRef.size());
        float fmean = getFmean(vMatchWords, vFunHyp, vFunRef);
        float finalscore = entScore * fmean; 
        //float finalscore = entScore; 
        sentCnt++;
        if(finalscore < 100) {
            scoreSysEnt += entScore;
            scoreSysFmean += fmean;
            scoreSys += finalscore;
        }
        if(finalscore <100) cout<<finalscore<<endl; 
        else cout<<0<<endl;
        }
    }    
	//system level
    cout<<"sys level t2sts "<< scoreSys <<endl;
    if(ngramLength_bool)
        cout<<(float)ngramLen/(float)sentCnt<<endl;
}
