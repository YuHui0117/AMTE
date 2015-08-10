/*
 * =====================================================================================
 *
 *       Filename:  treenode.h
 *
 *    Description:  reading the Penn2Mat output and build the tree it needed
 *
 *        Version:  1.0
 *        Created:  03/09/12 14:16:05
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (xfwu), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __TREENODE_H_
#define __TREENODE_H_
#include "rain_algorithm.h"
#include <stdio.h>
#include <stdlib.h>
//#include "myalgorithm.h"
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string.h>
using namespace std;

typedef struct{
    string w;
    int p;
}wordpos;
/*链的存储结构*/
struct SubLinkData{
    vector<int> hight;
    vector<int> fuctag;
    int count;
    vector<vector<int> > positions;
    SubLinkData(int a,int b,vector<int>& p):count(b){hight.push_back(a);positions.push_back(p);}
    SubLinkData():hight(0),count(0),fuctag(0){}
};

class Config{
    public:
    bool print_dep_bool;
    bool print_link_bool;
    bool HORIZ_bool ;
    bool LOG_bool ;
    bool Align_bool ;
    bool Fmean_bool ;
    bool FF_bool ;
    bool HWC_bool;
    bool BP_bool ;
    int refnum ;
    bool Para_bool ;
    bool nbest_bool;
    bool ConTree_bool ;
    bool Train_bool ;
    bool Map_bool ;
    bool Nw_bool ;
    bool Bleu_bool ;
    bool Entropy_bool;
    bool Function_bool ;
    int link_len ; //length of maximum links
    float wfmean; //parameter for fmean
    float wfun;  //parameter for function word
    int numcount; //number of all sentences
    vector<float> wNgrams; //parameters for 1gram, 2gram, ...
    vector<float> wMatchModule;// parameters for match modules: exact  stem synonym paraphrase
    vector<float> wEnt; //parameters for entropy
    map<string,int> func_word; //keep all the functions words
    string alignfile, alignfile_p, nbestFile, contreeFile, Nwf, MeAlign, fucfile;
    char* ref_file, *hyp_file;

    //for every sentence
    map<string,int> m_links,m_links_c; //number of links for hwc and fix-floating
    map<string,SubLinkData> m_links_with_depth,m_links_with_depth_c; //links of hwc and fix-floating
    string hypOneSent;
    vector<string> parseedstringRef;
    vector<vector<string> > contreeNgram; //ngrams extracted in constituent tree
    vector<map<int,pair<int,int> > >  vmalgn; // alignment results with match module exact-stem-synonym. format:<refPos,<hypPos,matchmodul> > 
    vector<vector<pair<vector<int>,vector<int> > > > vparaphrase;//paraphrase extracted from meteor alignment (ref-num-hyp-num)
    vector<string> valigEssp; //alignment using meteor  
    vector<int> links_ref; //number of all links in refs for every ngram
    vector<string> vecrefstr; // reference string 

    //for system score
    vector<vector<int> > chunks_sys; 
    vector<int> vec_igram_c; //number of all links in refs for every ngram
    vector<float> vec_sys_t2s_match;//number of links matched with hyp for every ngram
    vector<float> vec_sys_t2s_tgt; //number of links in hyps for every ngram
    float scoreAllSents; //score of sum for every sentence score
    public:
};


template<class T>
class myVector:public vector<T>
{
    
    public:
        inline T & operator [](size_t indx)
        {
            if( indx >= this->size())
            {
                for(size_t i = this->size(); i <= indx ; i++ )
                {
                    T *obj = new T();
                    this->push_back(*obj);
                }
            }
            return *(this->begin()+indx);
        }
};

class TreeNode{
    private:
        string m_data;
        vector<TreeNode *> * m_children;
    public:
        TreeNode(){m_data="";m_children = NULL;}
        inline void addChild(TreeNode* tn){ 
            if(m_children == NULL) m_children = new vector<TreeNode*>; 
            m_children->push_back(tn);
        }
        void removeChild(TreeNode*);
        inline string& getData(){return m_data;}
        inline vector<TreeNode*> * getChilden(){return m_children;}
        inline void setData(string s){m_data = s;}
        ~TreeNode(){
            if(m_children != NULL)
                delete m_children;
        }
};

class Tree{
    struct Dep{
        string data;
        int parent;
        Dep(string ss,int p):data(ss),parent(p){}; 
    };
    private:
    string m_filename;
    stringstream m_datablock;
    TreeNode * m_root;
    TreeNode * getTree(vector<vector<int> > &,int,int ); 
    void printiteratively(TreeNode*,string,bool);
    vector<Dep> m_dep;
    TreeNode* *m_path;
    vector<TreeNode *> *m_vecTree;
    void getSubLink(Config &cfg, TreeNode * node, int leng, int currentdepth, string metric);
    void getSubTree(TreeNode * node, int leng, int currentdepth);
    map<string,int> m_links_tmp;//for one reference
    map<string,SubLinkData> m_links_with_depth_tmp;
    map<string,int> m_links_c_tmp; // the dep-gram for one reference.
    map<string,SubLinkData> m_links_with_depth_c_tmp;//存放连续的gram（floating and fixed）  for one reference.

    
    vector<map<int,wordpos> > vecfloating;
    vector<map<int,wordpos> > vecfixed;
    int getNodePosition(TreeNode * node);
    void reorder_withrightpostion(map<int,string> & path_wordposition_in_sent,vector<int> &pos, string & tmp) ;
    void insert2links(vector<int> &pos_reordered,string str_reordered,int hight);
    public:
    void Traverse(TreeNode * n1);
    vector<TreeNode*> * getAllTreeNodes(){return m_vecTree;}
    Tree(string s ){ Tree(); m_datablock.clear();m_datablock << s;}
    string tree2string();
    Tree(){m_root = NULL;m_datablock<<"";m_path=NULL;m_vecTree=NULL;}
    TreeNode* getRoot(){return m_root;}
    void buildTree();
    inline void printTree(bool print_bool ){ if(!print_bool) return;
        cout<<".......... dep tree............."<<endl;
        printiteratively(m_root,"",true);
    }
    inline void setFilename(string s){m_filename = s;}
    void getSubLinks(Config &cfg, string metric);
    virtual ~Tree(){
        if(m_path) delete []m_path;  
        if(m_vecTree)
        {
            for(vector<TreeNode*>::iterator it = m_vecTree->begin();
                    it != m_vecTree->end(); it++)
                delete *it;
            delete m_vecTree;
        }
    }
    inline map<string,int> & getLinks(){return m_links_tmp;}
    inline map<string,int> & getLinks_c(){return m_links_c_tmp;}

    inline map<string,SubLinkData> & getLinksWithDepth(){return m_links_with_depth_tmp;}
    inline map<string,SubLinkData> & getLinksWithDepth_c(){return m_links_with_depth_c_tmp;}

     void getfloating(TreeNode* node);
     void getfixed(TreeNode* node);
     void outputchild(TreeNode* node, map<int,wordpos>& mtmp);
     void tosubLink(vector<map<int, wordpos> > &vec);
};

void CfgInit(Config &cfg);
void myclear(Config &cfg);
int parseArgs(int argc,char* argv[], Config &cfg);
void openfile(string filename, FILE *ifRead);
void readFile(FILE *ifRead, map<string,int> &words);
void readAlign(Config &cfg, FILE *ifalign);
void readParaphrase(Config &cfg, FILE *ifalign_p);
void readAlignEssp(Config &cfg, FILE *ifalign_essp);

string mygetline(FILE *ifRead);

#endif
