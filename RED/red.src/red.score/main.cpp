/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/17/13 09:30:54
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <math.h>
#include <cstdlib>
#include <map>
#include <algorithm>
#include <time.h>
using namespace std;

typedef struct depunit{
    vector<int> vecfuc;
    vector<int> vecmodel;
    vector<int> vecdis;
}depunit;

typedef struct Matunit{
    //for chunk
    vector<pair<int,int> > vechyp1gram;//算meteor的precision   fun-match
    vector<pair<int,int> > vecref1gram;//算meteor的recall
    vector<int> vechypFunCon;//len(hyp)-num(function word)-num(contant word)
    vector<int> vecrefFunCon;
    int num_chunk;
    int num_match1gram;
    vector<int> vecchunk;//每个chunk中词的个数

    //parapharse
    vector<vector<int> > vecparaps;//存放每个paraphrase的function tag. hyp or ref
    vector<depunit> vechwc;
    vector<depunit> vecff;

    //每元depngram的总个数，算recall
    vector<int> vecnumngram; 
}Matunit;
Matunit unitSys;
void Matunitclear(Matunit &tmp)
{
    tmp.vechyp1gram.clear();
    tmp.vecref1gram.clear();
    tmp.vechypFunCon.clear();
    tmp.vecrefFunCon.clear();
    tmp.vecparaps.clear();
    tmp.vechwc.clear();
    tmp.vecff.clear();
}

map<string,map<string, vector<Matunit> > > lan_sys_matunit;
float wmatch[4]={1.0};
float params[2]={0.5};
float wngram[3]={1.0};
float wentry[1]={1.2};
int D ;
bool myfunction (double i,double j) { return (i>j); }
string year(string datatag)
{
    if(datatag=="wmt12")
        return "newstest2012";
    if(datatag=="wmt13")
        return "newstest2013";
    if(datatag=="wmt14")
        return "newstest2014";
    if(datatag=="wmt15")
        return "newstest2015";
    return "newstest";
    }
vector<string> split(const string& src, string delimit)
{
    vector<string> r;
    int f = 0;
    int p = -1;

    while ((p = (int)src.find(delimit, p + 1)) != (int)string::npos)
    {
        if (p > f)
            r.push_back(src.substr(f, p - f));				
        f = p + (int)delimit.size();
    }
    p = (int)src.size();
    if (p > f)
        r.push_back(src.substr(f, p - f));				
    return r;
}
pair<string,string> getSysname(string filename, string yeartag)
{
    vector<string> tmp=split(filename,".");
    string sysname="";
    string lan="";
    if(yeartag=="wmt12" || yeartag == "wmt13" || yeartag == "wmt10")
    {   
        lan=tmp[1];
    
        for(int i=2; i<tmp.size()-4; i++)
        {
            sysname += tmp[i];
            sysname += ".";
        }
        sysname += tmp[tmp.size()-4];
    
    }
    if(yeartag=="wmt14" || yeartag=="wmt15")
    {   
        lan=tmp[tmp.size()-4];
        for(int i=1; i<tmp.size()-5; i++)
        {
            sysname += tmp[i];
            sysname += ".";
        }
        sysname += tmp[tmp.size()-5];
        sysname = sysname;    
    }//cout<<"file: "<<filename<<" sysname: "<<sysname<<" lan: "<<lan<<" "<<endl;
    return make_pair(sysname,lan);
}
void loadSysMatOut_one(string sysfilename, string datatag)
{
    pair<string, string> tmp=getSysname(sysfilename, datatag);
    string sysname = tmp.first;
    string lan=tmp.second;
    //cout<<lan<<" "<<sysname<<endl;
    fstream f;
    f.open(sysfilename.c_str());
    if(!f.is_open())
    {
        cout<<"can not open "<<sysfilename.c_str()<<endl;
        exit(0);
    }
    vector<Matunit> sys_matunit;
    
    string line;
    int idcnt=1;
    while(getline(f,line))
    {
        Matunit mtmp;
       vector<string> vec=split(line," ");
       vector<string> vec0=split(vec[0],"-");
       mtmp.num_match1gram =atoi(vec0[1].c_str());
       mtmp.num_chunk = atoi(vec0[2].c_str());
       for(int i=2; i<vec0.size(); i++)
       {
            (mtmp.vecchunk).push_back(atoi(vec0[i].c_str()));
       }
       vector<string> vec1=split(vec[1],"-");
       mtmp.vechypFunCon.push_back(atoi(vec1[0].c_str()));//total length of hyp
       mtmp.vechypFunCon.push_back(atoi(vec1[1].c_str()));//function word count
       mtmp.vechypFunCon.push_back(atoi(vec1[2].c_str()));//contant word count
       vector<string> vec2=split(vec[2],"-");
       mtmp.vecrefFunCon.push_back(atoi(vec2[0].c_str()));
       mtmp.vecrefFunCon.push_back(atoi(vec2[1].c_str()));
       mtmp.vecrefFunCon.push_back(atoi(vec2[2].c_str()));
       vector<string> vec3=split(vec[3],"-");
       for(int i=1; i<vec3.size(); i++)
       {
            vector<string> tmph=split(vec3[i],"_");
            mtmp.vechyp1gram.push_back(make_pair(atoi(tmph[0].c_str()),atoi(tmph[1].c_str())));
       }
       vector<string> vec4=split(vec[4],"-");
       for(int i=1; i<vec4.size(); i++)
       {
            vector<string> tmpr=split(vec4[i],"_");
            mtmp.vecref1gram.push_back(make_pair(atoi(tmpr[0].c_str()),atoi(tmpr[1].c_str())));
       }
       for(int i=5; i<vec.size()-1; i++)
       {
           depunit depu;
           vector<string> tmp=split(vec[i],"-" );
           if(tmp[0]=="h" || tmp[0]=="f")
           {
               vector<string> v1=split(tmp[1],"_");
               for(int k=0; k<v1.size(); k++)
                    depu.vecfuc.push_back(atoi(v1[k].c_str()));
               vector<string> v2=split(tmp[2],"_");
               for(int k=0; k<v2.size(); k++)
                    depu.vecmodel.push_back(atoi(v2[k].c_str()));
               vector<string> v3=split(tmp[3],"_");
               for(int k=0; k<v3.size(); k++)
                    depu.vecdis.push_back(atoi(v3[k].c_str()));
               if(tmp[0] == "h")
                   mtmp.vechwc.push_back(depu);
               else
                   mtmp.vecff.push_back(depu);
                //v1.swap(vector<string>());
                //v2.swap(vector<string>());
                //v3.swap(vector<string>());
           }
           if(tmp[0]=="p")
           {
               vector<int> vp;
               vector<string> v1=split(tmp[1],"_");
               for(int k=0; k<v1.size(); k++)
                    vp.push_back(atoi(v1[k].c_str()));
               (mtmp.vecparaps).push_back(vp);
                //vp.swap(vector<int>());
           }
       }
       vector<string> veclast=split(vec[vec.size()-1],"-");
       mtmp.vecnumngram.push_back(0);//1gram不用，随便赋值
       for(int i=0; i<veclast.size(); i++)
       {
            mtmp.vecnumngram.push_back(atoi(veclast[i].c_str()));
       }
       sys_matunit.push_back(mtmp);
       // cout<<idcnt<<endl;
       // idcnt++;
    }

    map<string,map<string, vector<Matunit> > >::iterator it;
    it=lan_sys_matunit.find(lan);
    if(it!=lan_sys_matunit.end())
    {
        map<string,vector<Matunit> > tmp=it->second;
        tmp.insert(make_pair(sysname,sys_matunit));
        it->second = tmp;
    }else{
        map<string,vector<Matunit> > tmp;
        tmp.insert(make_pair(sysname,sys_matunit));
        sys_matunit.clear(); 
        lan_sys_matunit.insert(make_pair(lan,tmp));
        tmp.clear();
    }
    //cout<<lan<<"-"<<sysname<<endl;
}

void loadSysMatOut(string filename, string datatag)
{
    fstream f;
    f.open(filename.c_str());
    if(!f.is_open())
    {
        cout<<"can not open "<<filename.c_str()<<endl;
        exit(0);
    }
    string line;
    while(getline(f,line))
    {
        if(line.size()>10)
        {
//            cout<<line<<endl;
            loadSysMatOut_one(line, datatag);
        }
    }
}
double entropy(Matunit mtmp)
{
    double score=0;
    double entry=0;
    int m_1gram=0;
    if(mtmp.num_chunk ==0) return 0;
    else{
        for(int i=0; i<(mtmp.vecchunk).size(); i++)
        {
            double p=(double)(mtmp.vecchunk)[i]/(double)mtmp.num_match1gram;
            if(p>0)
            {
                double tmp=-p*log(p);
                entry += tmp;
                m_1gram += (mtmp.vecchunk)[i];
            }
        }
        score = pow(wentry[0],-1*entry);
        return score;
    }
}
pair<double,double> getMeteorScore(Matunit mtmp,vector<double> &sys_ngram, vector<double> &sys_ref_ngram, int& sys_hyp_len)
{
   double  pen_ent=entropy(mtmp);
   double precion_deno = (mtmp.vechypFunCon)[1] + (mtmp.vechypFunCon[2]);
   double precision_mole=0;
   vector<pair<int,int> > vechyp=mtmp.vechyp1gram;
   for(int i=0; i<vechyp.size(); i++)
   {
        double fun;
        if(vechyp[i].first == 1) fun=params[1];
        else fun=1-params[1];
        precision_mole += fun * wmatch[vechyp[i].second];
   }
   sys_ngram[0] += precision_mole;
   sys_hyp_len += precion_deno;
   sys_ref_ngram[0] +=  (mtmp.vecrefFunCon)[1] + (mtmp.vecrefFunCon)[2];
   double precsn;
   if(precion_deno==0) precsn=0;
   else precsn=precision_mole/precion_deno;

   double recall_deno = (mtmp.vecrefFunCon)[1] + (mtmp.vecrefFunCon[2]);
   double recall_mole=0;
   vector<pair<int,int> > vecref=mtmp.vecref1gram;
   for(int i=0; i<vecref.size(); i++)
   {
        double fun;
        if(vecref[i].first == 1) fun=params[1];
        else fun=1-params[1];
        recall_mole += fun * wmatch[vecref[i].second];
   }
   double recall;
   if(recall_deno==0) recall=0;
   else recall=recall_mole/recall_deno;
   double fmean;
   if((params[0]*precsn + (1-params[0])*recall)==0) fmean=0;
   else fmean = (precsn*recall)/(params[0]*precsn + (1-params[0])*recall);
  return make_pair(fmean,pen_ent);
}
double getfun(int tmp)
{
     double fun;
     if(tmp == 1) fun=params[1];
     else fun=1-params[1];
     return fun;
}
pair<double,double> getParaphraseScore(Matunit mtmp,int len)
{
    vector<vector<int> > vecp = mtmp.vecparaps;
    double score=0;
    double link_ref=0;
    int D=(mtmp.vecnumngram).size();
        //parapharse
        for(int k=0; k<vecp.size(); k++)
        {
            if(vecp[k].size()==len)
            {
                double funp;
                for(int p=0; p<vecp[k].size(); p++)
                {
                    double fun=getfun(vecp[k][p]);
                    funp += fun;
                }
                if((float)vecp[k].size() ==0)
                    funp=0;
                else funp = funp/(double)vecp[k].size();
                score += funp*wmatch[3];
                link_ref += funp;
                //link_ref++;//没有区分function word
            }
            if(len==D-1 && vecp[k].size()>len)
            {//近似算下

                double funp;
                for(int p=0; p<vecp[k].size(); p++)
                {
                    double fun=getfun(vecp[k][p]);
                    funp += fun;
                }
                if((float)vecp[k].size()==0)
                    funp=0;
                else funp = funp/(double)vecp[k].size();
                score += funp*wmatch[3];
                score += 1;//多加1
                link_ref += funp;
                link_ref += 1;
            }
        }
        return make_pair(score,link_ref);
}

pair<double,double> getHwcScore(vector<depunit> vec, int len)
{
    double score=0,link_ref=0;
    for(int i=0; i<vec.size(); i++)
    {
        if((vec[i].vecfuc).size()==len)
        {
             vector<int> vectmp=vec[i].vecfuc;
             double fun=0,wei=0,dis=0;
             for(int k=0; k<vectmp.size(); k++)
             {
                 double funtmp=getfun(vectmp[k]);
                 fun += funtmp;
             }
             fun = fun/(double)vectmp.size();
             vectmp = vec[i].vecmodel;
             for(int k=0; k<vectmp.size(); k++)
             {
                 wei += wmatch[vectmp[k]];
             }
             wei = wei/(double)vectmp.size();
             vectmp = vec[i].vecdis;
             for(int k=0; k<vectmp.size(); k++)
             {
                 dis+=vectmp[k];
             }
             dis=dis/(double)vectmp.size();
             //dis = dis*0.1;
             double stmp=exp(-1*dis)*fun*wei;
             score += stmp;
             link_ref += fun;
        }
    }
    return make_pair(score, link_ref);
}
void addtounitSys(Matunit mtmp)
{
    for(int i=0; i<mtmp.vechyp1gram.size(); i++)
        unitSys.vechyp1gram.push_back(mtmp.vechyp1gram[i]);
    for(int i=0; i<mtmp.vecref1gram.size(); i++)
        unitSys.vecref1gram.push_back(mtmp.vecref1gram[i]);
    for(int i=0; i<mtmp.vechypFunCon.size(); i++)
        unitSys.vechypFunCon.push_back(mtmp.vechypFunCon[i]);
    for(int i=0; i<mtmp.vecrefFunCon.size(); i++)
        unitSys.vecrefFunCon.push_back(mtmp.vecrefFunCon[i]);
    unitSys.num_chunk += mtmp.num_chunk;
    unitSys.num_match1gram += mtmp.num_match1gram;
    for(int i=0; i<mtmp.vecchunk.size(); i++)
        unitSys.vecchunk.push_back(mtmp.vecchunk[i]);
    for(int i=0; i<mtmp.vecnumngram.size(); i++)
        unitSys.vecnumngram.push_back(mtmp.vecnumngram[i]);

}
double calculatescore_oneSent(Matunit mtmp, vector<double> &sys_ngram, vector<double> &sys_ref_ngram, int& sys_hyp_len, vector<int> &sys_chunk)
{
    vector<int> chunk_tmp=mtmp.vecchunk;
    for(int i=0; i<chunk_tmp.size(); i++)
    {
        sys_chunk.push_back(chunk_tmp[i]);
    }

    vector<double> ngramFmean;
    pair<double,double> metScore= getMeteorScore(mtmp, sys_ngram, sys_ref_ngram, sys_hyp_len); //meteor score . calculating 1gram fmean
    addtounitSys(mtmp);
    ngramFmean.push_back(metScore.first); //1gram fmean
    vector<depunit> vech = mtmp.vechwc;
    vector<depunit> vecf = mtmp.vecff;
    for(int i=1; i<D; i++)  //for 2gram, 3gram
    {
        double score=0;
        pair<double,double> scorePara=getParaphraseScore(mtmp,i+1); //paraphrse with length of i+1
        pair<double,double> scorehwc=getHwcScore(vech,i+1);//hwc with length of i+1
        pair<double,double> scoreff=getHwcScore(vecf,i+1);//fix-floating with length of i+1
        score = scorePara.first + scorehwc.first + scoreff.first;
        if(score==0) score = 0.001;
        double presn=score/(double)(mtmp.vechypFunCon)[0];
        double recall=score/(double)(mtmp.vecnumngram)[i];
        double fmean;
        if((params[0]*presn + (1-params[0])*recall) ==0) fmean=0;
        else fmean = presn*recall/(params[0]*presn + (1-params[0])*recall);
        if(fmean<100) ; //inf
        else fmean=0;
        ngramFmean.push_back(fmean);
        //for sys 
        sys_ngram[i] += score;
        sys_hyp_len += (mtmp.vechypFunCon)[0];
        sys_ref_ngram[i] += (mtmp.vecnumngram)[i]; //1gram 在getMeteorScore（）里面赋值
    }
    double total=0;
    for(int i=0; i<ngramFmean.size(); i++)
    {
        total += ngramFmean[i]*wngram[i];
    }
    total = total/(double)(ngramFmean.size()) *metScore.second;
    
    return total; 
}


double calculate_sysscore_one(vector<double> sys_ngram_score,vector<double> sys_ref_ngram,int sys_hyp_len,vector<int> sys_chunk)
{
    double score=0,p=0,r=0;
    for(int i=0; i<sys_ngram_score.size(); i++)
   {
        r=(double)sys_ngram_score[i]/(double)sys_ref_ngram[i];
        p=(double)sys_ngram_score[i]/(double)sys_hyp_len;
        double wp=params[0]*10;
        double wr=(1-params[0])*10;
        double f=(p*r)/(wp*p+wr*r);
        score += wngram[i]*f;
   
   }
    int sys_unigram=0;
    for(int i=0; i<sys_chunk.size(); i++)
    {
        sys_unigram += sys_chunk[i];
    }
    double ent=0;
    for(int i=0; i<sys_chunk.size(); i++)
    {
        double pp=(double)sys_chunk[i]/(double)sys_unigram;
        ent += -1*pp*log(pp);
    }
    score=score;
    //score = pow(wentry[0], -1*ent);
    double pen_ent=entropy(unitSys); 
    //return metScore.first * metScore.second; 
    return score;
}


map<string,map<string,vector<double> > > calculatescore(string segname, string sysname, string datatag)
{
    ofstream foutseg;
    ofstream foutsys;
    foutseg.open(segname.c_str());
    foutsys.open(sysname.c_str());
    map<string,map<string,vector<double> > > sysscore_lan;
    map<string,map<string,vector<Matunit> > >::iterator itlan;
    for(itlan=lan_sys_matunit.begin(); itlan!=lan_sys_matunit.end(); itlan++)//for every language pair
    {
        map<string,vector<double> > scores_sys;
        map<string,vector<Matunit> > sys_matunit = itlan->second;    
        map<string,vector<Matunit> >::iterator itsys;
        
        for(itsys=sys_matunit.begin(); itsys!=sys_matunit.end(); itsys++)//for every system translation
        {
            string strsys=itsys->first;
            vector<string> strvec=split(strsys,".");
            string newsys="";
            newsys=strsys;
            vector<double> scores_tmp;
            vector<Matunit> vecMatunit= itsys->second;
            double score_system=0;
            
            vector<double> sys_ref_ngram(D,0), sys_ngram_score(D,0);
            int sys_hyp_len=0;
            vector<int> sys_chunk;
            unitSys.vechyp1gram.clear(); 
            unitSys.vecref1gram.clear();
            unitSys.vechypFunCon.clear();
            unitSys.vecrefFunCon.clear();
            unitSys.num_chunk = 0;
            unitSys.num_match1gram = 0;
            unitSys.vecchunk.clear();

            for(int i=0; i<vecMatunit.size(); i++)//for every senstence
            {
                double score = calculatescore_oneSent(vecMatunit[i],sys_ngram_score, sys_ref_ngram, sys_hyp_len, sys_chunk); 
                    foutseg<<"RED\t"<<itlan->first<<"\t"<<year(datatag)<<"\t"<<newsys<<"\t"<<i+1<<"\t"<<score<<endl;
                scores_tmp.push_back(score);
                if(score<100) 
                    score_system += score;
            }
            scores_tmp.push_back(score_system/(double)vecMatunit.size());
            foutsys<<"RED\t"<<itlan->first<<"\t"<<year(datatag)<<"\t"<<newsys<<"\t"<<score_system/(double)vecMatunit.size()<<endl;
            scores_sys.insert(make_pair(itsys->first,scores_tmp));
        }
        sysscore_lan.insert(make_pair(itlan->first,scores_sys));
    }
    return sysscore_lan;
}
string ftoa(float value)
{
    std::ostringstream o;
      if(!(o << value)) 
           return "";
      return o.str();
    }
string toString()
{
    string str;
    str = ftoa(wmatch[0])+"-"+ftoa(wmatch[1])+"-"+ftoa(wmatch[2])+"-"+ftoa(wmatch[3])+"-";
    str = str + ftoa(wngram[0])+"-"+ftoa(wngram[1])+"-"+ftoa(wngram[2])+"-";
    str = str + ftoa(params[0])+"-"+ftoa(params[1])+"-";
    str = str + ftoa(wentry[0]);
    return str;
}

int main(int argc, char* argv[])
{
    char* sysf;
    string datatag;
    if(argc<6)
    { 
        cout<<"-t:deprefoutfile(name) -D:dep-ngram -tag year(wmt12,wmt13,wmt14) -wmatch 4-value -wngram 3-value -params 2-value -wentry 1-value"<<endl;
        exit(0);
    }
    else 
    {
        for(int i=1; i<argc; i++){
        if(i != argc)
            if(string(argv[i]) == "-t"){//for train file, all systems match result
                sysf=argv[++i];
            }else if(string(argv[i]) == "-D"){
                D = atoi(argv[++i]);
            }else if(string(argv[i]) == "-tag"){
                datatag = argv[++i];
            }else if(string(argv[i]) == "-wmatch"){
                wmatch[0] = atof(argv[++i]);
                wmatch[1] = atof(argv[++i]);
                wmatch[2] = atof(argv[++i]);
                wmatch[3] = atof(argv[++i]);
            }else if(string(argv[i]) == "-wngram"){
                wngram[0] = atof(argv[++i]);
                wngram[1] = atof(argv[++i]);
                wngram[2] = atof(argv[++i]);
            }else if(string(argv[i]) == "-params"){
                params[0] = atof(argv[++i]);
                params[1] = atof(argv[++i]);
            }else if(string(argv[i]) == "-wentry"){
                wentry[0] = atof(argv[++i]);
            }
        }
    }
  string suf = toString();
  loadSysMatOut(sysf, datatag);
  //string segname="outputscore_seg."+datatag+"."+suf;
  //string sysname="outputscore_sys."+datatag+"."+suf;
  string segname="outputscore_seg."+datatag;
  string sysname="outputscore_sys."+datatag;
  map<string,map<string,vector<double> > > sysscore= calculatescore(segname, sysname, datatag); 
  return 1;
}
