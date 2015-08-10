/*
 * =====================================================================================
 *
 *       Filename:  treenode.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/09/12 10:19:53
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "treenode.h" 
#include "myalgorithm.h"
#include <algorithm>
#include <sstream>
extern bool DBG_bool;
#include "rain_algorithm.h"
using namespace rain;
using namespace std;
/*遍历树，并保留节点信息*/
void Tree::outputchild(TreeNode * node, map<int,wordpos>& mstu)
{
    if(node->getChilden()==NULL) 
        return;
    for(size_t i=0; i<node->getChilden()->size(); i++)
    {
        wordpos tmp;
        tmp.w=(*(node->getChilden()->begin()+i))->getData();
        tmp.p=getNodePosition(*(node->getChilden()->begin()+i));
        mstu.insert(make_pair(tmp.p,tmp));
        TreeNode* n1Child=*(node->getChilden()->begin()+i);
        outputchild(n1Child,mstu);
    }
}
/*获得fixed结构的链*/
void Tree::getfixed( TreeNode * n1)
{
    if(n1 == NULL)
        return;
    vector<TreeNode*>::iterator it;
    vector<wordpos> vectmp;
    map<int,wordpos> mtmp;
    if(n1->getChilden() == NULL) return;
    for(int i=0; i<n1->getChilden()->size(); i++)
    {
        
        for(int j=i; j<n1->getChilden()->size(); j++)
        {
            mtmp.clear();
            wordpos tmp;
            tmp.w=n1->getData();
            tmp.p=getNodePosition(n1);
            mtmp.insert(make_pair(tmp.p,tmp));
            for(int k=i; k<j+1; k++)
            {
            tmp.w=(*(n1->getChilden()->begin()+k))->getData();
            tmp.p=getNodePosition(*(n1->getChilden()->begin()+k));
            mtmp.insert(make_pair(tmp.p,tmp));
            TreeNode* n1Child=*(n1->getChilden()->begin()+k);
            outputchild(n1Child,mtmp);
            }
            //判断是否连续？
           map<int,wordpos>::iterator it;
            int tag=0;
            map<int,wordpos>::iterator itend;
            itend=mtmp.end();
            itend--;
            for(it=mtmp.begin(); it!=itend; it++)
            {
                int a=it->first;
                map<int,wordpos>::iterator it1;
                it1=it;
                it1++;
                int b=it1->first;
                
                if(b!=a+1) { tag=1;break;}
            }
            if(tag==0) vecfixed.push_back(mtmp);
        }
    }
    for(it = n1->getChilden()->begin(); it != n1->getChilden()->end() ; it++){ getfixed(*it);}
}
/*获得floating的链*/
void Tree::getfloating( TreeNode * n1)
{
    if(n1 == NULL)
        return;
    vector<TreeNode*>::iterator it;
    vector<wordpos> vectmp;
    map<int,wordpos> mtmp;
    if(n1->getChilden() == NULL) return;
    for(int i=0; i<n1->getChilden()->size(); i++)
    {
        for(int j=i+1; j<n1->getChilden()->size(); j++)
        {
            mtmp.clear();
            wordpos tmp;
            for(int k=i; k<j+1; k++)
            {
            tmp.w=(*(n1->getChilden()->begin()+k))->getData();
            tmp.p=getNodePosition(*(n1->getChilden()->begin()+k));
            mtmp.insert(make_pair(tmp.p,tmp));
            TreeNode* n1Child=*(n1->getChilden()->begin()+k);
            outputchild(n1Child,mtmp);
            }
            //判断是否连续？
            map<int,wordpos>::iterator it;
            int tag=0;
            map<int,wordpos>::iterator itend;
            itend=mtmp.end();
            itend--;
            for(it=mtmp.begin(); it!=itend; it++)
            {
                int a=it->first;
                map<int,wordpos>::iterator it1;
                it1=it;
                it1++;
                int b=it1->first;
                
                if(b!=a+1) { tag=1;break;}
            }
            if(tag==0) vecfloating.push_back(mtmp);
        }
    }
    
    for(it = n1->getChilden()->begin(); it != n1->getChilden()->end() ; it++)
    {
        getfloating(*it);
    }
}
/*把向量存储的链信息存入map结构*/
void Tree::tosubLink(vector<map<int, wordpos> > &vec)
{
    map<string,SubLinkData>::iterator iter;
    
    for(int i=0; i<vec.size(); i++)
    {
        map<int,wordpos>::iterator it;
        vector<int> vint;
        string strdata;
        for(it=vec[i].begin(); it!=vec[i].end(); it++){
            wordpos wp=it->second;
            vint.push_back(wp.p);
            strdata=strdata+wp.w+" ";
        }
        iter=m_links_with_depth_c_tmp.find(strdata);
        if(iter==m_links_with_depth_c_tmp.end()){
            SubLinkData slink(0,1,vint);
            m_links_with_depth_c_tmp.insert(make_pair(strdata,slink));
             m_links_c_tmp.insert(make_pair(strdata,1));
        }
        else{
            SubLinkData slink=iter->second;
            int tag=0;
            for(int j=0; j<slink.positions.size(); j++)
            {
                if(slink.positions[j]==vint)
                { tag=1;break;}
            }
            if(tag==0)
            {
                slink.count +=1;
                slink.positions.push_back(vint);
                iter->second=slink;
                m_links_c_tmp[strdata]+=1;
            }
        }

    }
    vec.clear();
}
/*合并存储链结构的两个map*/
void tosubLink_all(int refnum,map<string,SubLinkData>& m_links_with_depth_c, map<string,int>& m_links_c,  map<string,SubLinkData>& m_links_with_depth_c_tmp,map<string,int>& m_links_c_tmp)
{
    map<string,int>::iterator cit,citall;
    map<string,SubLinkData>::iterator it,ittmp;
    if(refnum==1)
    {
        m_links_c = m_links_c_tmp;
        m_links_with_depth_c = m_links_with_depth_c_tmp;
    }
    else{
        for(ittmp=m_links_with_depth_c_tmp.begin();ittmp!=m_links_with_depth_c_tmp.end(); ittmp++)
        {
            it=(m_links_with_depth_c).find(ittmp->first);
            cit = m_links_c_tmp.find(ittmp->first);
                if(it == m_links_with_depth_c.end()) {
                    m_links_with_depth_c.insert(make_pair(ittmp->first,ittmp->second));
                    m_links_c.insert(make_pair(cit->first,cit->second));
                }
                else {
                        citall = m_links_c.find(ittmp->first);
                        if(citall->second < cit->second) citall->second = cit->second;
                        //combine the dep-ngram of one reference to m_links_with_depth_c.
                        vector<vector<int> > vecposi = ittmp->second.positions;
                        vector<vector<int> > vecposi_all = it->second.positions;
                        for(int i=0; i<vecposi.size(); i++)
                        {
                            int tag=0;
                            for(int j=0; j<vecposi_all.size(); j++)
                           {
                                if(vecposi_all[j] == vecposi[i]) {
                                    tag=1;
                                    break;
                                }
                            }
                            if(tag==0){
                                it->second.positions.push_back(vecposi[i]);
                                it->second.hight.push_back(ittmp->second.hight[i]);
                                it->second.count = it->second.count + 1;
                            }
                        }
                        it->second.count = citall->second;
                }
        }
    }
    m_links_with_depth_c_tmp.clear();
    m_links_c_tmp.clear();
}
/*获得所有的子链,包含hwc和fixed-floating*/
void Tree::getSubLinks(Config &cfg, string metric)
{
    if(metric=="red")
    {
        getfloating(m_root);
        getfixed(m_root);
        tosubLink(vecfloating);
        tosubLink(vecfixed);
    }
    getSubLink(cfg, m_root,cfg.link_len,0,metric);
    tosubLink_all(cfg.refnum, cfg.m_links_with_depth_c, cfg.m_links_c, m_links_with_depth_c_tmp,m_links_c_tmp);
    tosubLink_all(cfg.refnum, cfg.m_links_with_depth, cfg.m_links, m_links_with_depth_tmp,m_links_tmp);

    if(cfg.print_link_bool) 
    {
        cout<<".......link ******list******head word chain...for one reference........."<<endl;
        for(int i = 1; i<=cfg.link_len; i++)
            for(map<string,SubLinkData>::iterator it = cfg.m_links_with_depth.begin();
                it != cfg.m_links_with_depth.end();
                it++)
            {
                if(getTokenNumber(it->first)==i)
                cout << it->first <<" "<< (it->second).count<<endl;
            }

        if(metric=="red")
        {
            cout<<"..........link ******list******floating and fixed...for one reference......."<<endl;
            for(int i = 1; i<=cfg.link_len; i++)
             for(map<string,SubLinkData>::iterator it = cfg.m_links_with_depth_c.begin();
                it != cfg.m_links_with_depth_c.end();
                it++)
             {
                if(getTokenNumber(it->first)==i)
                cout << it->first <<" "<< (it->second).count<<endl;
             }
        }
    }
}
/*把树转换为字符串*/
string Tree::tree2string(){
    if(m_vecTree == NULL) return "";
    stringstream ss;
    vector<TreeNode*>::iterator it = m_vecTree->begin();

    while(true)
    {
        ss<<(*it)->getData();
        if(++it != m_vecTree->end())
        {
            ss<<" ";
        }
        else
            break;
    }
    ss<<" ";
    return ss.str();
}
/*获得一个节点的位置*/
int Tree::getNodePosition(TreeNode * node)
{//get the position of a word in the original sentence. no matter how many times it occure in a sentence
    size_t pos = 0;
    for(; node != *(m_vecTree->begin()+pos); pos++);
    if(pos == m_vecTree->size()) return -1;
    return pos;
}
/*把链中词的位置按在原句子中的顺序排列*/
void Tree::reorder_withrightpostion(map<int,string> & path_wordposition_in_sent,vector<int> &pos, string & tmp) 
{
    stringstream words_with_right_position;
    for(map<int,string>::iterator it = path_wordposition_in_sent.begin();
            it!=path_wordposition_in_sent.end();
            it++)
    {
        words_with_right_position<<(*it).second<<" ";
        pos.push_back((*it).first);
    }
    tmp = words_with_right_position.str();
}
/*插入一个子链到总的链的存储结构中*/
void Tree::insert2links(vector<int> &pos_reordered,string str_reordered,int hight)
{// insert SubLinkData in to m_links and m_links_with_depth;
        if(m_links_tmp.find(str_reordered) != m_links_tmp.end()){
            m_links_tmp[str_reordered] += 1;
        }else
            m_links_tmp.insert(make_pair(str_reordered,1));

        if(m_links_with_depth_tmp.find(str_reordered) != m_links_with_depth_tmp.end()){
            vector<vector<int> > &posvec = m_links_with_depth_tmp[str_reordered].positions;
            for(size_t i = 0 ; i< posvec.size(); i++)
            {   
                size_t j;
                for(j = 0; j< posvec[i].size() && posvec[i][j] == pos_reordered[j]; j++);
                    //if(posvec[i][j] != pos_reordered[j]) a = true;
                if(j == posvec[i].size())return;//found identical: same string and same positons;
            }

            m_links_with_depth_tmp[str_reordered].count += 1;
            (m_links_with_depth_tmp[str_reordered].hight).push_back( hight);
            (m_links_with_depth_tmp[str_reordered].positions).push_back(pos_reordered);
        }else
            m_links_with_depth_tmp.insert(make_pair(str_reordered,SubLinkData(hight,1,pos_reordered)));
}
/*对树中的一个节点，获得子链*/
void Tree::getSubLink(Config &cfg, TreeNode * node, int leng, int currentdepth, string metric){
    if(node == NULL) return;
    m_path[currentdepth] = node;

    map<int,string> path_wordposition_in_sent;
    for(int j = 0; j < leng && j <= currentdepth; j++)
    {
        path_wordposition_in_sent.insert(make_pair(getNodePosition(m_path[currentdepth-j]),m_path[currentdepth-j]->getData()));

        vector<int> pos_reordered;//the position vector store each word's positions
        string str_reordered;

        reorder_withrightpostion(path_wordposition_in_sent,pos_reordered,str_reordered);
        insert2links(pos_reordered,str_reordered,currentdepth - j);
    }
    if(node->getChilden() == NULL) return;

    if(leng >= 3 && cfg.HORIZ_bool)// no use now
    {
        path_wordposition_in_sent.clear();
        vector<int> child;
        for(size_t i = 0; i <node->getChilden()->size(); i ++)
            child.push_back(i);
        vector<vector<int> > b = kSubsets(child,min((size_t)(leng -1),child.size()));
        for(size_t i = 0 ; i<b.size(); i++)
        {//iterate over ksubset
            map<int,string> path_wordposition_in_sent;
            for(size_t j = 0 ; j < b[i].size(); j++)
            {
                path_wordposition_in_sent.insert(make_pair(
                            getNodePosition(*(node->getChilden()->begin() + b[i][j])),
                            (*(node->getChilden()->begin()+b[i][j]))->getData()
                            )
                        );
            }
            path_wordposition_in_sent.insert(make_pair(getNodePosition(node),node->getData()));
            vector<int>pos_reordered;
            string str_reordered;
            reorder_withrightpostion(path_wordposition_in_sent,pos_reordered,str_reordered);
            insert2links(pos_reordered,str_reordered,currentdepth);
        }
    }

    vector<TreeNode*>::iterator it;
    for(it = node->getChilden()->begin() ; 
            it!= node->getChilden()->end();
            it++)//i < (node->getChilden())->size();i++)
                getSubLink(cfg, *it, leng, currentdepth +1, metric);
}
/*打印树*/
void Tree::printiteratively(TreeNode * root,string prefix, bool istail){
    string token = istail ? "└── " : "├── ";
    cout<< prefix<< token<<root->getData()<<endl;
    if (root->getChilden()!= NULL) {
        vector<TreeNode*>::iterator it;
        for( it= root->getChilden()->begin();
                it != root->getChilden()->end()-1;
                it++)
        {
            printiteratively(*it,prefix + (istail ? "    " : "│   "), false);
        }
        if (root->getChilden()->size() >= 1) {
            printiteratively(*it, prefix + (istail ?"    " : "│   "), true);
        }
    }
}
/*构建树*/
void Tree::buildTree(){
    vector<vector<int> > linklist;//the tree, represented by vector
    // nodei, nodes... that dependent on nodei
    char *s = new char[100];
    int nodeindx = 0;
    int root;
    for(m_datablock.getline(s,100);strlen(s) > 2 && !m_datablock.eof();m_datablock.getline(s,100))
    {
        string data;int parent;
        string line = s;
        int lineindx = 0;

        while(line.find('\t',0) != string::npos){
            size_t pos = line.find('\t',0);
            string tmp = line.substr(0,pos);
            line.erase(0,pos+1);
            if(lineindx == 0) data = tmp; 
            if(lineindx == 2) {
                parent = atoi(tmp.c_str())-1;
                break;
            }
            lineindx++;
        }
        m_dep.push_back(Dep(data,parent));
        if(parent == -1) {
            //this is the root node
            root = nodeindx;
        }else
        {
            if((size_t)parent >= linklist.size())
                for(size_t i = linklist.size(); i <= (size_t)parent;i++)
                {
                    vector<int> a;
                    linklist.push_back(a);
                }
            linklist[parent].push_back(nodeindx);
        }
        nodeindx++;
    }
    for(size_t i = linklist.size(); i < (size_t)nodeindx ;i++)
    {
        vector<int> a;
        linklist.push_back(a);
    }
 
    m_root =getTree(linklist,root,nodeindx); 
    m_path = new TreeNode*[linklist.size()];
    delete []s;
}
/*获得树结构*/
TreeNode* Tree::getTree(vector<vector<int> > & ll,int root,int nodesize)
{
    m_vecTree = new vector<TreeNode*>;
    for(size_t indx = 0; indx < (unsigned int)nodesize; indx++)
    {
        m_vecTree->push_back(new TreeNode());
        (*(m_vecTree->begin() + indx))->setData(m_dep[indx].data);
    }

    for(size_t indx = 0; indx < m_vecTree->size(); indx++)
    {
        for(vector<int>::iterator  i = ll[indx].begin(); i != ll[indx].end(); i++)
        {
            (*(m_vecTree->begin() + indx))->addChild(*(m_vecTree->begin() + *i));
        }
    }
    return *(m_vecTree->begin() + root);
}
/* 递归遍历*/
void Tree::Traverse( TreeNode * n1)
{
    if(n1 == NULL)
        return;
    vector<TreeNode*>::iterator it;

    cout<<n1->getData()<<endl;
    if(n1->getChilden() == NULL) return;

    for(it = n1->getChilden()->begin(); it != n1->getChilden()->end() ; it++)
    {
        Traverse(*it);
    }
}
    void CfgInit(Config &cfg)
    {
        cfg.print_dep_bool=false;
        cfg.print_link_bool=false;
        cfg.HORIZ_bool = false;
        cfg.LOG_bool = false;
        cfg.Align_bool = false;
        cfg.Fmean_bool = false;
        cfg.FF_bool = false;
        cfg.HWC_bool = false;
        cfg.BP_bool = false;
        cfg.refnum = 1;
        cfg.Para_bool = false;
        cfg.nbest_bool = false;
        cfg.ConTree_bool = false;
        cfg.Train_bool = false;
        cfg.Map_bool = false;
        cfg.Nw_bool = false;
        cfg.Bleu_bool = false;
        cfg.Entropy_bool = false;
        cfg.Function_bool = false;
        cfg.link_len = 2;
        cfg.wfmean=0.75;
        cfg.wfun=0.3;
        cfg.numcount=0; //count of sentences
        
        (cfg.wNgrams).push_back(1);
        (cfg.wNgrams).push_back(1);
        (cfg.wNgrams).push_back(1);
        (cfg.wMatchModule).push_back(1);
        (cfg.wMatchModule).push_back(1);
        (cfg.wMatchModule).push_back(1);
        (cfg.wMatchModule).push_back(1);
        (cfg.wEnt).push_back(1);
        (cfg.wEnt).push_back(1);
        cfg.vec_igram_c.clear();
        cfg.vec_sys_t2s_match.clear();
        cfg.vec_sys_t2s_tgt.clear();
        cfg.scoreAllSents=0;
    }
    void myclear(Config &cfg)
    {
        cfg.m_links_with_depth.clear();
        cfg.m_links.clear();
        cfg.m_links_c.clear();
        cfg.m_links_with_depth_c.clear();
        cfg.contreeNgram.clear();
        cfg.vmalgn.clear();
        cfg.vparaphrase.clear();
        cfg.valigEssp.clear();
        cfg.vecrefstr.clear();
        cfg.links_ref.clear();
}
    int parseArgs(int argc,char* argv[], Config &cfg)
    {
        for (int i = 1; i < argc; i++) { /*  We will iterate over argv[] to get the parameters stored inside.
                                          * Note that we're starting on 1 because we don't need to know the 
                                          * path of the program, which is stored in argv[0] */
            if (i != argc) // Check that we haven't finished parsing already
                if (string(argv[i]) == "-r") {
                    // We know the next argument *should* be the filename:
                    cfg.ref_file = argv[++i ];
                } else if (string(argv[i]) == "-h") {
                    cfg.hyp_file = argv[++i ];
                } else if (string(argv[i]) == "-D") {
                    cfg.link_len = atoi(argv[++i ]);
                } else if (string(argv[i]) == "-FF") {
                    cfg.FF_bool = true;
                } else if(string(argv[i])=="-HWC"){
                    cfg.HWC_bool = true;
                } else if (string(argv[i])== "-BP"){
                    cfg.BP_bool = true;
                } else if (string(argv[i]) == "-refnum") {
                    cfg.refnum = atoi(argv[++i]);
                } else if (string(argv[i]) == "-pdep") {//print the dependent tree?
                    cfg.print_dep_bool = true;
                } else if (string(argv[i]) == "-plink") {//print the links 
                    cfg.print_link_bool = true;
                } else if(string(argv[i]) == "-dbg"){
                    DBG_bool = true;
                }  else if(string(argv[i]) == "-HORI"){
                    cfg.HORIZ_bool = true;
                }   else if(string(argv[i]) == "-LOG"){
                    cfg.LOG_bool = true;
                }else if(string(argv[i]) =="-Align"){
                    cfg.Align_bool=true;
                    cfg.alignfile=argv[++i];
                }else if(string(argv[i]) == "-Fmean"){
                    cfg.Fmean_bool=true;
                }else if(string(argv[i]) == "-Para"){
                    cfg.Para_bool = true;
                    cfg.alignfile_p=argv[++i];
                }else if(string(argv[i]) == "-nbest"){
                    cfg.nbest_bool = true;
                    cfg.nbestFile = argv[++i];//每个句子nbest的数量，可能有的短句子句法树的个数小于n.
                }else if(string(argv[i]) == "-ConTree"){
                    cfg.ConTree_bool = true;
                    cfg.contreeFile = argv[++i];
                }else if(string(argv[i]) == "-Train"){
                    cfg.Train_bool = true;
                }else if(string(argv[i]) == "-BLEU"){
                    cfg.Bleu_bool = true;
                }else if(string(argv[i]) == "-Align"){
                    cfg.Align_bool = true;
                    cfg.alignfile = string(argv[++i]);
                    
                }else if(string(argv[i]) == "-Para"){
                    cfg.Para_bool = true;
                    cfg.alignfile_p = string(argv[++i]);
                    
                }else if(string(argv[i]) == "-Entropy"){
                    cfg.Entropy_bool = true;
                    cfg.MeAlign = string(argv[++i]);
                    
                }else if(string(argv[i]) == "-Function"){
                    cfg.Function_bool = true;
                    cfg.fucfile = string(argv[++i]);
                }else {
                    std::cout << argv[i]<<"invalid arguments, please try again.\n";
                    exit(0);
                }
            //std::cout << argv[i] << " ";
        }
        //std::cout<<endl;
    
        for(int i=0; i<cfg.link_len; i++)
        {
            (cfg.vec_sys_t2s_match).push_back(0);
            (cfg.vec_sys_t2s_tgt).push_back(0);
            (cfg.vec_igram_c).push_back(0);
        
        }
    
    }
void openfile(string filename, FILE *ifRead)
{
    if((ifRead = fopen(filename.c_str(),"r"))==NULL)
    {
        cout<<"can't open "<<filename<<endl;
        exit(0);
    }
}
void readFile(FILE *ifRead, map<string,int> &words)
{
    char line[100000];
    string linenew;
    int read;
    while(fgets(line,100000, ifRead) )
    {
        linenew = rain::strip(line,"\n"); 
        words.insert(make_pair(linenew,1));
    }
}
void readAlign(Config &cfg, FILE *ifalign)
{
    string align_line;
    for(int i=0; i<cfg.refnum; i++){
        map<int,pair<int,int> > malgn;
        align_line=mygetline(ifalign);
        istringstream is(align_line);
        string word;
        while(is>>word){
            vector<string> vec=rain::split(word,"-");
            vector<int> veci;
            for(int i=0; i<vec.size(); i++){
                veci.push_back(atoi(vec[i].c_str()));
            }
                malgn.insert(make_pair(veci[0],make_pair(veci[2],veci[4])));
        }
        (cfg.vmalgn).push_back(malgn);
    }
}
void readParaphrase(Config &cfg, FILE *ifalign_p)
{
    string align_line,word;
    for(int i=0; i<cfg.refnum; i++)
    {
        vector<pair<vector<int>,vector<int> > > paraphrase; 
        vector<int> tmp;
        align_line = mygetline(ifalign_p);
        istringstream is(align_line);
        while(is>>word)
        {
            vector<string> vec=rain::split(word,"-");
            vector<int> veci;
            for(int i=0; i<vec.size(); i++){
                veci.push_back(atoi(vec[i].c_str()));
            }
            if(veci[1]==1 && veci[3]==1)
                ;
            else
                paraphrase.push_back(make_pair(veci,tmp));
        }
        cfg.vparaphrase.push_back(paraphrase);
    }
}
void readAlignEssp(Config &cfg,FILE *ifalign_essp)
{
    string line;
    for(int i=0; i<cfg.refnum; i++){
        line = mygetline(ifalign_essp);
        (cfg.valigEssp).push_back(line);
    }
}
string mygetline(FILE *ifRead)
{
    char line[100000];
    fgets(line,100000, ifRead);

    return rain::strip(line,"\n");
 }
