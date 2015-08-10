#ifndef __CONTREE_H_
#define __CONTREE_H_
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <math.h>
#include <cstdlib>
#include "treenode.h"
#include "rain_algorithm.h"
using namespace rain;
using namespace std;
typedef struct{
    string pos;
    int ltag;
    int rtag;
    int lcnt;
    int rcnt;
}Unit;
struct Cnode{
         string pos;
         vector<struct Cnode *> vch;
         int leavtag;
};
vector<Unit> tovec(string line)
{
    string word;
    vector<Unit> vline;
    istringstream is(line);
    while(is>>word)
    {
        Unit tmp;
        tmp.ltag=0;
        tmp.rtag=0;
        tmp.rcnt=0;
        tmp.lcnt=0;
        if(word.find('(')==0)
        {
            tmp.ltag=1;
            tmp.lcnt++;
            tmp.pos=word.substr(1,word.size()-1);
        }
        if(word[word.size()-1]==')')
        {
            tmp.rtag=1;
            int p;
            for(p=word.size()-1; p>0; p--)
            {
                if(word[p]==')')
                    tmp.rcnt++;
                else
                    break;
            }
            tmp.pos=word.substr(0,p+1);
        }
        vline.push_back(tmp);
    }
    return vline;
}
void findchild(vector<Unit> vline, Cnode* root)
{
       vector<vector<Unit> > vecch;
       string str=vline[0].pos;
       Cnode  *tmp=new Cnode();
       tmp->pos=str;
       tmp->leavtag=0;
       (root->vch).push_back(tmp);
       //cout<<(root->vch)[0]->pos<<endl;
       int ltag=0;
           if(vline[1].ltag==1)
           {
               ltag=0;
               int s=1;
                for(int i=1; i<vline.size(); i++)
                {
                    if(vline[i].ltag)
                        ltag=ltag+vline[i].lcnt;
                    if(vline[i].rtag)
                    {
                        ltag=ltag-vline[i].rcnt;
                    }
                    if(ltag<=0)
                    {
                        vector<Unit> child;
                        for(int p=s; p<i+1; p++)
                        {
                            child.push_back(vline[p]);
                        }
                        string last=vline[i].pos;
                        if(ltag<0)  child[child.size()-1].rcnt--;
                        vecch.push_back(child);
                        s=i+1;
                        ltag=0;
                    }
                }
                for(int i=0; i<vecch.size(); i++)
                {
                    //for(int p=0; p<vecch[i].size(); p++)
                    //    cout<<"i:"<<i<<" "<<vecch[i][p].lcnt<<vecch[i][p].pos<<vecch[i][p].rcnt<<" ";
                    //cout<<endl; 
                    findchild(vecch[i],tmp);
                }
           }
           else//叶节点
           {
               Cnode *leave=new Cnode();;
               leave->pos=vline[1].pos;
               leave->leavtag=1;
               (tmp->vch).push_back(leave);
           }
}
void printtree(Cnode *root, string prefix, bool istail)
{
    string token=istail ? "└── " : "├── ";
    cout<<prefix<<token<<root->pos<<endl;
    if(root->vch.size()>0){
        for(int i=0; i<root->vch.size()-1; i++)
        {
            printtree(root->vch[i], prefix + (istail ? "    " : "│   "), false);
        }
        if(root->vch.size()>=1)
            printtree(root->vch[root->vch.size()-1], prefix + (istail ?"    " : "│   "), true);
    }
}
void children(Cnode *root, vector<Cnode *> &vnode)
{
    vector<Cnode *> vtmp=root->vch;
    for(int i=0; i<vtmp.size(); i++){
            if(vtmp[i]->leavtag==0)
            { 
                //if((vtmp[i]->vch).size()>1)
                    children(vtmp[i],vnode);
                vnode.push_back(vtmp[i]);
            }
    }
}
void node2string(Cnode *node,vector<string> &str)
{
    if(node->leavtag==1)
        str.push_back(node->pos);
    else{
        vector<Cnode *> vec=node->vch;
        for(int i=0; i<vec.size(); i++){
            node2string(vec[i],str);
        }
    }
   
}

string printstr(vector<string> vec)
{
    string str;
    for(int i=0; i<vec.size(); i++)
    {
        str += vec[i];
        str += " ";
    }
    return str;
}
vector<vector<string> > extracNgram(Cnode *root)
{
    vector<Cnode *> vnode;
    children(root,vnode);
    vector<vector<string> > Cngrams;
    for(int i=0; i<vnode.size(); i++)
    {
        vector<string> vecstr;
        node2string(vnode[i],vecstr);
        Cngrams.push_back(vecstr);
        //cout<<"string: "<<printstr(vecstr)<<endl;; 
    }
    
    return Cngrams;
}                
    
void contreeExtracNgram(Config &cfg, FILE *ifcontree)
{
    string line;
    for(int i=0; i<cfg.refnum; i++){
        line = mygetline(ifcontree);
        line=line.substr(1,line.size()-3);
        vector<Unit> vline=tovec(line);
        Cnode *root = new Cnode();
        root->pos="NULL";
        findchild(vline,root);
        if(cfg.print_dep_bool) 
            printtree(root,"",true);
        cfg.contreeNgram=extracNgram(root);
    }
}

#endif
