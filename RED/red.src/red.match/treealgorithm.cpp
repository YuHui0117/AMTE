/*
 * =====================================================================================
 *
 *       Filename:  treealgorithm.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  19/09/12 20:22:20
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <map>
#include <iostream>
#include<algorithm>
#include <string>
#include <math.h>
#include "treealgorithm.h"
#include "myalgorithm.h"
#include <assert.h>
#include "entropy.h"

using namespace std;
extern bool DBG_bool;
/*如果hwc和fixed-floating有重复的链，只保留hwc中的*/
/*对参考依存的链和机器翻译结果进行匹配*/
float getScore_No_Align(Config cfg, string &hypstring, SubLinkData & links,string refstring)
{//compute relative position as the score
   
    vector<string> reftmp = getVector(refstring);//the link string
    vector<string> hyp = getVector(hypstring);//the total hyp string
    vector<vector<int> > pos_hyp_conpress,pos_hyp;
    for(size_t indx1 = 0 ; indx1 < reftmp.size(); indx1++)
    {//loop of the path
        //find the word reftmp[indx1] in the position of hyp
        vector<int> tmp;
        for(size_t indx2 = 0; indx2 < hyp.size(); indx2++)
        {//loop of the hyp words
            if((hyp[indx2]).compare( reftmp[indx1]) == 0)
            {
                tmp.push_back(indx2);
            }
        }
        if(!tmp.empty()) pos_hyp_conpress.push_back(tmp);
    }
    if(pos_hyp_conpress.size()!= reftmp.size() || pos_hyp_conpress.size() == 0) return 0;//can not find the path in hyp

    if(pos_hyp_conpress.size() == 1){
        //return the minimum number of coocurrence,when the link length is 1
        return min(pos_hyp_conpress.begin()->size(),links.positions.size());
    }

    pos_hyp = decompress_path(pos_hyp_conpress);

    vector<vector<int> > pos_ref = links.positions;//there might be same link but appeared in different positon of the same tree.
    if(DBG_bool && ((links.positions.begin())->size() == 0 || links.positions.size() == 0))
    {
        cout<<"something wrong here"<<__LINE__<<" "<<(links.positions.begin())->size()<<" "<<links.positions.size()<<endl;
        exit(0);
    }

    int minsize = min(pos_ref.size(),pos_hyp.size());
    map<float,pair<size_t,size_t>  >  mp;
    for(size_t hyp_it = 0;hyp_it < pos_hyp.size(); hyp_it++)
    {
        for(size_t ref_it =0; ref_it < pos_ref.size(); ref_it++)
        {
            vectorDist<int> a(pos_hyp[hyp_it],pos_ref[ref_it]);
            mp.insert(make_pair(a.euclidDist(cfg),make_pair(hyp_it,ref_it)));
        }
    }

  float sum =0;
    for(map<float,pair<size_t,size_t> >::reverse_iterator rit =mp.rbegin();
            rit!=mp.rend() && minsize-- >0;
            rit++)
    {
        sum += rit->first;//why add them together?
    }
    return sum;
}
/*对一个链ref_pos_one与hypstring进行匹配*/
float numberTopologyMatch_relative_one(Config cfg, string &hypstring, vector<int> & ref_pos_one,string refstring,vector<int> vecfuc)
{//compute relative position as the score
  
    vector<int> matmodel(vecfuc.size(),0);
    vector<string> reftmp = getVector(refstring);//the link string
    vector<string> hyp = getVector(hypstring);//the total hyp string
    vector<vector<int> > pos_hyp_conpress,pos_hyp;
    for(size_t indx1 = 0 ; indx1 < reftmp.size(); indx1++)
    {//loop of the path
        //find the word reftmp[indx1] in the position of hyp
        vector<int> tmp;
        for(size_t indx2 = 0; indx2 < hyp.size(); indx2++)
        {//loop of the hyp words
            if((hyp[indx2]).compare( reftmp[indx1]) == 0)
            {
                tmp.push_back(indx2);
            }
        }
        if(!tmp.empty()) pos_hyp_conpress.push_back(tmp);
    }
    if(pos_hyp_conpress.size()!= reftmp.size() || pos_hyp_conpress.size() == 0) return 0;//can not find the path in hyp

    if(pos_hyp_conpress.size() == 1){
        //return the minimum number of coocurrence,when the link length is 1
        return min(pos_hyp_conpress.begin()->size(),(size_t)1);
    }

    pos_hyp = decompress_path(pos_hyp_conpress);

	float dist = 0;

    int minsize = min((size_t)1,pos_hyp.size());
    map<float,pair<size_t,size_t>  >  mp;
    for(size_t hyp_it = 0;hyp_it < pos_hyp.size(); hyp_it++)
    {
            vectorDist<int> a(pos_hyp[hyp_it],ref_pos_one, matmodel, vecfuc);
            mp.insert(make_pair(a.euclidDist(cfg),make_pair(hyp_it,0)));
    }

    
    map<float,pair<size_t,size_t> >::reverse_iterator rit =mp.rbegin();
    if(rit!=mp.rend())
        return rit->first;
    else 
        return 0;
}
void findalign(vector<map<int,pair<int,int> > > valign, int pos, vector<pair<int,int> > &matalgn)
{//pos 是否在meteor的对齐里面
    for(int i=0; i<valign.size(); i++){
            map<int,pair<int,int> >::iterator it;
            it=valign[i].find(pos);
            if(it!=valign[i].end())
            {
               pair<int,int> tmp=it->second;
               matalgn.push_back(tmp);
            }
    
    }
}
int getOrder(vector<int> hyp_pos)
{
    int tag=0;
    for(int j=0; j<hyp_pos.size()-1; j++)
    {
    
        if(hyp_pos[j]>hyp_pos[j+1])
        {
            tag=1; break;
        }
    }
    return tag;
}
void getMatModule(vector<map<int,pair<int,int> > > valign,vector<int> ref_pos_i, vector<int> &hyp_pos, vector<int> &matmodel)
{
    for(int j=0; j<ref_pos_i.size(); j++)
    {
         map<int,pair<int,int> >::iterator it;
         vector<pair<int,int> > mattmp;
         findalign(valign, ref_pos_i[j], mattmp);
         //多个ref时，由于ref的依存ngram都合到一块了，不能区别出来自哪个ref，在判断对齐先用第一个，但是可能不准确    
         if(mattmp.size()>0){
         int p=mattmp[0].first;
             hyp_pos.push_back(p);
             matmodel.push_back(mattmp[0].second);
         }
    }
}
void printMatchWordPosition(vector<int> ref_pos_i, vector<int> hyp_pos,int i)
{
     cout<< "ref_pos: ";
     for(int m=0; m<ref_pos_i.size(); m++)
         cout<< ref_pos_i[m]<<" ";
     cout<<"hyp_pos: ";
     for(int m=0; m<hyp_pos.size(); m++)
         cout<< hyp_pos[m]<<" ";
     cout<<endl;
}
vector<float> getMatchScore_oneLink(Config cfg,string type, vector<int> ref_pos_i, vector<int> hyp_pos, vector<int> vecfuc, vector<int> matmodel)
{
    vector<float> mp;
    if(hyp_pos.size() != ref_pos_i.size() ) 
        mp.push_back(0);
    else if(ref_pos_i.size()==1 && hyp_pos.size() == 1) 
    {
        float fw;
        if(vecfuc[0] == 0)//0代表不是功能词
            fw=1-cfg.wfun;
        else
            fw=cfg.wfun;
        if(matmodel[0]==0) 
             mp.push_back(1*cfg.wMatchModule[0]*fw);
        else if (matmodel[0]==1)
             mp.push_back(1*cfg.wMatchModule[1]*fw);
        else if(matmodel[0]==2)
             mp.push_back(1*cfg.wMatchModule[2]*fw);
        if(cfg.Train_bool)
            cout<<type<<"-"<<vecfuc[0]<<"-"<<matmodel[0]<<" ";
    }
    else 
    {
        //filt the link that donot satisfy the order   
        int tag= getOrder(hyp_pos);
        if(tag==0)
        { 
          vectorDist<int> a(hyp_pos,ref_pos_i,matmodel,vecfuc);
          if(cfg.Train_bool)
              cout<<type<<"-";
          mp.push_back(a.euclidDist(cfg));
        }
    }
    return mp;
}
/*加入对齐后的匹配*/
float getScore_Align(Config cfg, string type, SubLinkData & links, string refstring)
{//compute relative position as the score
    vector<vector<int> > ref_pos = links.positions;
    vector<int> vecfuc = links.fuctag;
    vector<float> mp;
    for(int i=0; i<ref_pos.size(); i++)
    {
      //用meteor对齐过滤
        vector<int> hyp_pos;
        vector<int> matmodel;
        getMatModule(cfg.vmalgn,ref_pos[i], hyp_pos, matmodel); 
        if(DBG_bool) printMatchWordPosition(ref_pos[i], hyp_pos, i);
        mp = getMatchScore_oneLink(cfg, type, ref_pos[i], hyp_pos, vecfuc, matmodel);
    }
    float sum =0;
    for(int i=0; i<mp.size(); i++)
    {
        sum += mp[i];
    }
   return sum;
}
/*计算系统级的分数*/
float getRedScore_sys(Config cfg)
{
    
	float syst2sts= 0;
	float score=0;
    vector<int> vecsys;
	for(int i =0; i < cfg.link_len; i ++)
	{
	    if(cfg.Fmean_bool)
        {
            float p=((float)(cfg.vec_sys_t2s_match[i])/(float)cfg.vec_sys_t2s_tgt[i]) * cfg.wNgrams[i];
            float r=((float)(cfg.vec_sys_t2s_match[i])/(float)cfg.vec_igram_c[i]) * cfg.wNgrams[i];
            float fmean;
	        if((p+r)==0) fmean=0;
	        else{ 
                float wp=cfg.wfmean*10;
                float wr=(1-cfg.wfmean)*10;
                fmean = 10*p*r/(wp*p+wr*r);
                }
                syst2sts += fmean;
            
        }
        else
            syst2sts += ((float)(cfg.vec_sys_t2s_match[i])/(float)cfg.vec_sys_t2s_tgt[i]) * cfg.wNgrams[i];
    }
    float entscore;
    if(cfg.Entropy_bool)
    {
        int chunks_num=0, unigram_match=0;
        for(int i=0; i<cfg.chunks_sys.size(); i++)
        {
            chunks_num += cfg.chunks_sys[i].size();
            for(int k=0; k<cfg.chunks_sys[i].size(); k++)
            {
                vecsys.push_back(cfg.chunks_sys[i][k]);
                unigram_match += cfg.chunks_sys[i][k];
            }
        }
 
        entscore=entropyscore(vecsys, (int)cfg.vec_sys_t2s_tgt[0] , cfg.vec_igram_c[0]);
        //float entscore=cfg.MET_weight[0]*pow((float)chunks_num/(float)unigram_match,cfg.MET_weight[1]);
    }
    float ariBP;
    if(cfg.BP_bool)
    {
        if(cfg.vec_sys_t2s_tgt[0] > cfg.vec_igram_c[0])
            ariBP=1;
        else 
            ariBP = exp(1-(float)cfg.vec_igram_c[0]/(float)cfg.vec_sys_t2s_tgt[0]);
    }
	if(!cfg.Entropy_bool && !cfg.BP_bool)  score = (syst2sts/(float)cfg.link_len); 
    if(cfg.Entropy_bool && !cfg.BP_bool) score = (syst2sts/(float)cfg.link_len)* entscore;
	if(!cfg.Entropy_bool && cfg.BP_bool)  score = (syst2sts/(float)cfg.link_len)* ariBP; 
	if(cfg.Entropy_bool && cfg.BP_bool)  score = (syst2sts/(float)cfg.link_len)* ariBP * entscore; 
    return score;
}
bool check(vector<vector<int> > &matpara, vector<int> tmp)
{//matpara中是否有和tmp重复的短语
    for(int i=0; i<matpara.size(); i++){
        if(matpara[i][2]==tmp[2] && matpara[i][3]==tmp[3])
            return true;
        else 
            return false;
    }
}
int findstring(string hyp, string refstr)
{//在hyp的句子中，找到几次refstr
    int s=0,matchingscore_test=0;
    while(s>-1 && s<hyp.size()){
        int t=hyp.find(refstr,s);
        if(t!=-1) {
            matchingscore_test++;
            s=t+refstr.size();
            }
        else break;
    }
    return matchingscore_test;
}
string vec2string(vector<string> vec,string contag)
{
    string str;
    for(int i=0; i<vec.size(); i++)
    {
        str += vec[i];
        str += contag;
    }
    return str;
}
vector<int> functionTag(map<string, int> functionWords, string sstr)
{
    vector<int> ft;
    vector<string> vec=rain::split(sstr," ");
    map<string,int>::iterator it;
    for(int i=0; i<vec.size(); i++)
    {
        it=functionWords.find(vec[i]);
        if(it==functionWords.end())
        { 
            ft.push_back(0);
        }
        else{
            ft.push_back(1);
        }
    }
    return ft;
}
int getFunctionWordCnt(vector<int> funtag)
{   
    int funnum=0;
    for(int i=0; i<funtag.size(); i++)
    {
        if(funtag[i]==1)
            funnum++;
    }
    return funnum;
}
void print1gram(vector<int> chunks, vector<int> chunks_bak, int hypfunnum, int hyplen, int reffunnum, int reflen, vector<pair<int,int> > onegram_hyp, vector<pair<int,int> > onegram_ref)
{
    int match_count=0;
    int match_count_bak=0;
    float entscore;
    for(int p=0; p<chunks.size(); p++)
        match_count += chunks[p];
    for(int p=0; p<chunks_bak.size(); p++)
        match_count_bak += chunks_bak[p];
    cout<<"chunk-"<<(float)(match_count + match_count_bak)/(float)2<<"-"<<chunks.size();
    for(int p=0; p<chunks.size(); p++)
    {
        cout<<"-"<<chunks[p];
    }
    cout<<" ";
    cout<<hyplen<<"-"<<hypfunnum<<"-"<<hyplen-hypfunnum<<" ";
    cout<<reflen<<"-"<<reffunnum<<"-"<<reflen-reffunnum<<" ";
    cout<<"hyp1gram";
    for(int i=0; i<onegram_hyp.size(); i++)
        cout<<"-"<<onegram_hyp[i].second<<"_"<<onegram_hyp[i].first;
    cout<<" ref1gram";
    for(int i=0; i<onegram_ref.size(); i++)
        cout<<"-"<<onegram_ref[i].second<<"_"<<onegram_ref[i].first;
    cout<<" ";
}
float get1gramFun(Config cfg, vector<pair<int,int> > onegram)
{ 
    float hyps;
    for(int i=0; i<onegram.size(); i++)
    {
        float func;
        if(onegram[i].second==1) func = cfg.wfun;
        else func = 1-cfg.wfun;
        hyps += cfg.wMatchModule[onegram[i].first]*func;
    }
    return hyps;
}
pair<float,float> calculate_meteor_score(Config &cfg, int id)
{
    string align_line = cfg.valigEssp[0];
    string hyp = cfg.hypOneSent;
    string ref = cfg.vecrefstr[id];
    vector<pair<int,int> > onegram_hyp, onegram_ref; 
    vector<int> chunks_bak;
    vector<int> chunks = matchhyp(cfg.func_word, align_line,hyp,ref,onegram_hyp,onegram_ref,chunks_bak);
    cfg.chunks_sys.push_back(chunks);
    vector<int> funtag_hyp = functionTag(cfg.func_word, hyp);
    vector<int> funtag_ref = functionTag(cfg.func_word, ref);
    float entscore;
    if(chunks.size()>0) 
    {
       entscore=entropyscore(chunks,funtag_hyp.size(),funtag_ref.size()); 
        //entscore = cfg.MET_weight[0]*pow((float)chunks.size()/(float)match_count,cfg.MET_weight[1]);
        //entscore = 1-entscore;
    }
    else entscore=0;
    int hypfunnum = getFunctionWordCnt(funtag_hyp);
    int hypConnum = funtag_hyp.size()-hypfunnum;
    int reffunnum = getFunctionWordCnt(funtag_ref);
    int refConnum = funtag_ref.size()-reffunnum;
    
    float hyps = get1gramFun(cfg, onegram_hyp);
    float refs = get1gramFun(cfg, onegram_ref);
    cfg.vec_sys_t2s_match[0] += hyps;
    cfg.vec_sys_t2s_tgt[0] += funtag_hyp.size();
    cfg.vec_igram_c[0] += funtag_ref.size();
    
    if(cfg.Train_bool) 
        print1gram(chunks, chunks_bak, hypfunnum, funtag_hyp.size(), reffunnum, funtag_ref.size(), onegram_hyp, onegram_ref);
    float precn = hyps/funtag_hyp.size();
    float recall = refs/funtag_ref.size();
    float fmean = precn * recall/(cfg.wfmean * precn + (1-cfg.wfmean) * recall); 
    return make_pair(fmean, entscore);   
}

vector<int> getParaScore(Config &cfg, int num_ref)
{
    vector<int> paramatch(cfg.link_len,0);
    vector<int> func_hyp,func_ref;
    if(cfg.Function_bool){
        func_hyp=functionTag(cfg.func_word, cfg.hypOneSent);
        func_ref=functionTag(cfg.func_word, (cfg.vecrefstr)[num_ref]);
    }
    for(int k=0; k<cfg.vparaphrase.size(); k++){
      for(int i=0; i<cfg.vparaphrase[k].size(); i++){
         // bool ptrue=check(matpara,vparaphrase[k][i]);
          if(!cfg.Function_bool){   
                int pgramLen;
                vector<int> ptmp=cfg.vparaphrase[k][i].first;
                if(ptmp[1]>ptmp[3])
                    pgramLen = ptmp[1];
                else pgramLen = ptmp[3];
                if(pgramLen<cfg.link_len+1)
                  paramatch[pgramLen-1] += 1*cfg.wMatchModule[3];
                else {
                  int tmp=pgramLen/cfg.link_len +1;
                  paramatch[cfg.link_len-1]+=tmp*cfg.wMatchModule[3];
                }
          }
           else{ 
               //用hyp的长度算分数
               float weightFun=0;
               string tag="hyp";
               vector<int> ptmp=(cfg.vparaphrase[k][i]).first;
               int pgramLen;
               vector<int> vecf;
               if(tag=="hyp"){
               pgramLen=ptmp[3];
               for(int p=0; p<pgramLen; p++)
               {
                   vecf.push_back(func_hyp[ptmp[2]+p]);
                   float tmp;
                    if(func_hyp[ptmp[2]+p]==1) 
                        tmp=cfg.wfun;
                    else tmp=1-cfg.wfun;
                    weightFun += tmp;
               }
               (cfg.vparaphrase[k][i]).second = vecf;
               weightFun = weightFun/(float)pgramLen;
               }
               //用ref的長度算分數
               if(tag=="ref"){
               pgramLen=ptmp[1];
               for(int p=0; p<pgramLen; p++)
               {
                   vecf.push_back(func_ref[ptmp[0]+p]);
                   float tmp;
                    if(func_ref[ptmp[0]+p]==1) 
                        tmp=cfg.wfun;
                    else tmp=1-cfg.wfun;
                    weightFun += tmp;
               }
               (cfg.vparaphrase[k][i]).second=vecf;
               weightFun = weightFun/(float)pgramLen;
               }
               if(pgramLen<cfg.link_len+1)
                  paramatch[pgramLen-1] += 1*cfg.wMatchModule[3]*weightFun;
                else {
                  int tmp=pgramLen/cfg.link_len +1;
                  paramatch[cfg.link_len-1]+=tmp*cfg.wMatchModule[3]*weightFun;
                }
           }
      }
    }
    return paramatch;
}
void printParaphrase(Config cfg)
{
    for(int k=0; k<cfg.vparaphrase.size(); k++)
    {
      for(int i=0; i<cfg.vparaphrase[k].size(); i++)
      {
        cout<<"p-";
        vector<int> tmp=cfg.vparaphrase[k][i].second;
        for(int j=0; j<tmp.size()-1; j++)
            cout<<tmp[j]<<"_";
        cout<<tmp[tmp.size()-1]<<"-3 ";
      }
    }
}
int getLenMultiRef(vector<string> vecrefstr, int len_hyp_sent)
{
    int len_diff_min=100;
    int c=0;
    int num_ref=0;
    for(c=0; c<vecrefstr.size(); c++){
        int ref_len_tmp = getTokenNumber(vecrefstr[c]);
        int len_diff = fabs(ref_len_tmp-len_hyp_sent);
        if(len_diff<len_diff_min) {len_diff_min=len_diff; num_ref=c;}
    }
    return num_ref;
}

map<string,SubLinkData> getLinks_i(map<string,int> functionWords, map<string,SubLinkData> ref_links, int &c_dep_ngram, int link_len_i)
{
    map<string,SubLinkData> len_i_ref;
    for(map<string,SubLinkData>::iterator it =ref_links.begin(); it != ref_links.end(); it++)
    {//get lenth i path
        if(getTokenNumber(it->first) == link_len_i)
        {
            vector<int> ft = functionTag(functionWords, it->first);
            (it->second).fuctag = ft;
            len_i_ref.insert(*it);
            c_dep_ngram += it->second.count;
        }
    }
    return len_i_ref;
}
pair<float,float> getMatchScore_hwc(Config cfg, map<string,SubLinkData> len_i_ref)
{//only fixed&float but keep dep-1-gram in headword chain
    float sum_ref_i=0;
    float sum_ref_i_clip=0;
    float matchingscore=0;
    for(map<string,SubLinkData>::iterator it = len_i_ref.begin(); it != len_i_ref.end(); it++)
    {
        if(cfg.Align_bool) matchingscore = getScore_Align(cfg, "h",it->second,it->first);
        else matchingscore = getScore_No_Align(cfg, cfg.hypOneSent, it->second, it->first);
        sum_ref_i += (it->second).count;
        sum_ref_i_clip += matchingscore;
        if(DBG_bool ) 
        {
           cout<<it->first<<"   HWCM count is: "<<(it->second).count<<" ";
           cout<<"matching score is: "<<matchingscore<<endl;
        }
    }
    return make_pair(sum_ref_i_clip,sum_ref_i);
}
float getffScore(Config cfg, string dep_ngram, SubLinkData links, string hyp)
{
    float matchingscore_test = findstring(hyp,dep_ngram);
    float matchingscore;
    if(links.count > (int)matchingscore_test) matchingscore=matchingscore_test;
    else matchingscore=links.count;
    //function word
    vector<int> ft = links.fuctag;
    float wfall=0;
    for(int t=0; t<ft.size(); t++)
       { 
           float wf;
           if(ft[t]==0)
               wf=1-cfg.wfun;
           else 
               wf=cfg.wfun;
           wfall += wf;
       }
    wfall = wfall/(float)ft.size();
    matchingscore = matchingscore * wfall;   
    return matchingscore;
}
void getMatchScore_ff(Config cfg, map<string,SubLinkData> len_i_ref_c, string hyp, float &sum_ref_i_clip, float &sum_ref_i)
{
    for(map<string,SubLinkData>::iterator it = len_i_ref_c.begin(); it != len_i_ref_c.end(); it++)
    {
    
        sum_ref_i += (it->second).count;
        float matchingscore=0;
        if(!cfg.Align_bool) matchingscore = getffScore(cfg, it->first, it->second ,hyp);
        if(cfg.Align_bool) matchingscore = getScore_Align(cfg, "f",it->second,it->first);
        if(DBG_bool) 
        {
           cout<<it->first<<"FF  count is: "<<(it->second).count<<" ";
           cout<<"matching score is: "<<matchingscore<<endl;
        }
        sum_ref_i_clip += matchingscore;
    }
}
float getFmean(Config cfg, int len_hyp_sent, float sum_ref_i_clip, float c_dep_ngram)
{
    float p, r, score;
    if(len_hyp_sent==0) {p=0; r=0;}
    else
    {
         p=max(sum_ref_i_clip,(float)0.001)/(float)len_hyp_sent;
         if(c_dep_ngram!=0) r=max(sum_ref_i_clip,(float)0.001)/(float)c_dep_ngram;
         else r=0;
    }
    if((r+p) != 0) 
    {
        float wp=cfg.wfmean*10;
        float wr=(1-cfg.wfmean)*10;
        score=10*p*r/(wp*p+wr*r);
    }
    else score=0;
    return score;
}
float getPrecision(int len_hyp_sent, float sum_ref_i_clip)
{
    float red_ngram=0;
    if(len_hyp_sent==0)  
        red_ngram=0;
    else
        red_ngram = max(sum_ref_i_clip,(float)0.001)/(float)len_hyp_sent;
    if(DBG_bool) cout << red_ngram<<endl;
    return red_ngram;
}
/*计算句子级的匹配分数*/
vector<pair<float,float> > getRedScore(Config &cfg)
{
    int D = cfg.link_len;
    string hyp = cfg.hypOneSent;
    float red = 0;
    vector<pair<float,float> > vecRed;
    pair<float,float> metScorePart; 
    int len_hyp_sent=getTokenNumber(hyp);
    int num_ref = 0; // getLenMultiRef(cfg.vecrefstr, len_hyp_sent);
    int len_ref_sent=getTokenNumber(cfg.vecrefstr[num_ref]);
    if(cfg.Entropy_bool)
    {
        metScorePart = calculate_meteor_score(cfg,num_ref);
        red = metScorePart.first*cfg.wNgrams[0];
        vecRed.push_back(make_pair(red, red));
    }
    //using paraphrase
    vector<int> paramatch(D,0);
    if(cfg.Para_bool) paramatch = getParaScore(cfg, num_ref);
    if(cfg.Train_bool) printParaphrase(cfg);
    if(DBG_bool) cout<<"red: "<<endl;
    if(DBG_bool) cout<<"hyp is :"<<hyp<<endl;
    int i;
    if(cfg.Entropy_bool) 
        i = 2;
    else
        i=1;
    for(; i <= D; i++)
    {
        float sum_ref_i = 0,sum_ref_i_clip = 0;
        int c_dep_ngram=0;//count of links if ref
        if(cfg.Para_bool) 
        {
            cfg.vec_sys_t2s_tgt[i-1] += paramatch[i-1];
            c_dep_ngram = paramatch[i-1];
            sum_ref_i_clip += paramatch[i-1]; 
        }
        if(i==1&&cfg.FF_bool || cfg.HWC_bool)
        { 
            map<string,SubLinkData> len_i_ref = getLinks_i(cfg.func_word, cfg.m_links_with_depth, c_dep_ngram, i); //get links with length of i
            pair<float,float> matchingscore = getMatchScore_hwc(cfg, len_i_ref);
            sum_ref_i_clip += matchingscore.first;//matched links with length of i
            sum_ref_i += matchingscore.second;//all links in ref with length of i
        }
        if(cfg.FF_bool)
        {
            map<string,SubLinkData> len_i_ref_c = getLinks_i(cfg.func_word, cfg.m_links_with_depth_c, c_dep_ngram, i);
            getMatchScore_ff(cfg, len_i_ref_c,hyp, sum_ref_i_clip, sum_ref_i);
        }
        if(DBG_bool) cout<<i<<" : ref count is: "<< sum_ref_i<<"  clip is: "<<sum_ref_i_clip<< "  hyp len is: "<<len_hyp_sent<<endl;
        float red_ngram;
        if(cfg.Fmean_bool) 
            red_ngram = getFmean(cfg, len_hyp_sent, sum_ref_i_clip, c_dep_ngram);
        else 
            red_ngram = getPrecision(len_hyp_sent, sum_ref_i_clip);
        
        red += red_ngram* cfg.wNgrams[i-1];
        vecRed.push_back(make_pair(red, sum_ref_i_clip));
        cfg.links_ref.push_back(c_dep_ngram);
        cfg.vec_sys_t2s_match[i-1] += max(sum_ref_i_clip,(float)0.001);
        cfg.vec_sys_t2s_tgt[i-1] += len_hyp_sent;
        cfg.vec_igram_c[i-1]=cfg.vec_igram_c[i-1]+c_dep_ngram;
    }
    if(cfg.Train_bool)//输出ref中每个dep-ngram的个数,不包含1gram
    {
        for(int k=0; k<cfg.links_ref.size()-1;k++)
            cout<<cfg.links_ref[k]<<"-";
        cout<<cfg.links_ref[cfg.links_ref.size()-1];
    }
    
    float BPexp;
    if(cfg.BP_bool)  //length penalty
    {
        if(len_hyp_sent > len_ref_sent)
            BPexp = 1;
        else BPexp= exp(1-(float)len_ref_sent/(float)len_hyp_sent);
    }
    if(!cfg.Entropy_bool && cfg.BP_bool) red = red/(float)D * BPexp;
    if(cfg.Entropy_bool && !cfg.BP_bool) red = red/(float)D * metScorePart.second;
    if(cfg.Entropy_bool && cfg.BP_bool)  red = red/(float)D *BPexp * metScorePart.second;
    if(!cfg.Entropy_bool && !cfg.BP_bool) red = red/(float)D;

    vecRed.push_back(make_pair(red,red));//the last element stores the the final red score.
    return vecRed;
}

void removeSameLink(Config &cfg)
{
    map<string,SubLinkData>::iterator it=(cfg.m_links_with_depth_c).begin();
    while(it!=(cfg.m_links_with_depth_c).end()){
       map<string,SubLinkData>::iterator itt=(cfg.m_links_with_depth).find(it->first);
       if(itt!=cfg.m_links_with_depth.end()){
           vector<vector<int> > vec=itt->second.positions;
           vector<vector<int> > vec_c=it->second.positions;
           vector<vector<int> > vec_new;
           vector<int> hight;
           int c=0;
           for(int i=0; i<vec_c.size(); i++){
               int t=0;
               for(int j=0; j<vec.size(); j++){
                   if(vec_c[i]==vec[j]) t=1;
               }
               if(t==0) {vec_new.push_back(vec_c[i]); c++; hight.push_back(it->second.hight[i]);}
           }
           if(c==0) {//remove this links
              map<string,int>::iterator iter =cfg. m_links_c.find(it->first);
              cfg.m_links_with_depth_c.erase(it++); 
              cfg.m_links_c.erase(iter);
           }
           else {
               it->second.positions=vec_new;
               it->second.hight=hight;
               it->second.count=c;
               map<string,int>::iterator iter = cfg.m_links_c.find(it->first);
               if(iter==cfg.m_links_c.end()) {cout<<"error when remove the same links!"; exit(0);}
               else iter->second=c;
               it++;
           }
       }
       else it++;
   }//end of remove the same links
            
}
void printLinks(Config cfg)
{
    //print the combined dep-ngrams of multiple references
    cout <<"the combined dep-ngrams of multiple references:" <<endl;
    cout <<".....................headword chains....................."<<endl;
    for(int i=1; i<=cfg.link_len; i++)
    for(map<string,SubLinkData>::iterator it=cfg.m_links_with_depth.begin(); it!=cfg.m_links_with_depth.end(); it++)
        if(getTokenNumber(it->first)==i) cout<<it->first <<"  "<<it->second.count<<endl;
    cout<<".........end........"<<endl;
    cout<<"...............floating and fixed structure................"<<endl;
    for(int i=1; i<=cfg.link_len; i++)
    for(map<string,SubLinkData>::iterator it=cfg.m_links_with_depth_c.begin(); it!=cfg.m_links_with_depth_c.end(); it++)
       if(getTokenNumber(it->first)==i) cout<<it->first <<"  "<<it->second.count<<endl;
    cout<<".........end......."<<endl;
}
void getRefTrees(Config &cfg,FILE *ifbest,int &itref)
{
    string Nline;
    for(int i=0; i<cfg.refnum; i++){
        int best_N;
        if(cfg.nbest_bool){
            Nline = mygetline(ifbest);
            best_N = atoi(Nline.c_str());
        }
        else 
            best_N = 1;
        for(int n=0; n<best_N; n++){
             Tree treeref(cfg.parseedstringRef[itref]);
             treeref.buildTree();
             cfg.vecrefstr.push_back(treeref.tree2string());
             if(cfg.print_dep_bool) {cout<<treeref.tree2string()<<endl;}
             treeref.printTree(cfg.print_dep_bool);
             treeref.getSubLinks(cfg,"red");
             itref++;
        }
    }
}
