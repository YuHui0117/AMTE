/*
 * =====================================================================================
 *
 *       Filename:  test.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/09/12 10:51:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "contree.h"
#include "treenode.h"
#include "treealgorithm.h"
#include "parsedepfile.h"
#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "rain_algorithm.h"
using namespace std;
using namespace rain;
bool DBG_bool = false;
vector<float> MET_weight(2,1);//parameters in meteor
string Score_Method = "red";
int main(int argc, char* argv[]) 
{
    if (argc < 6) 
    { 
        std::cout << "Usage is -r reffile -h hypfile <-D linklenth> <-refnum n> -Fmean -FF -HWC";
        exit(0);
    } 
    else 
    { 
        Config cfg;
        CfgInit(cfg); 
        parseArgs(argc, argv, cfg); //keep the args in cfg 
        
        if(cfg.Function_bool) // read function word file
        {
            FILE *ifunc=fopen(cfg.fucfile.c_str(),"r");
            readFile(ifunc,cfg.func_word);
        }
        FILE *ifcontree;  if(cfg.ConTree_bool) ifcontree = fopen(cfg.contreeFile.c_str(), "r"); //constituent tree file
        FILE *ifbest;  if(cfg.nbest_bool) ifbest = fopen(cfg.nbestFile.c_str(), "r");  // nbest of reference deptree
        FILE *ifalign; if(cfg.Align_bool) ifalign = fopen(cfg.alignfile.c_str(), "r"); //meteor alignment
        FILE *ifalign_p;  if( cfg.Para_bool) ifalign_p = fopen(cfg.alignfile_p.c_str(),"r"); //meteor alignment
        FILE *ifalign_mt; if(cfg.Entropy_bool) ifalign_mt = fopen(cfg.MeAlign.c_str(),"r"); //meteor alignment 
        FILE *ifs=fopen(cfg.hyp_file,"r");
        cfg.parseedstringRef = parseDepfile(cfg.ref_file);//read the dependency tree of reference
        cfg.numcount=cfg.parseedstringRef.size()/cfg.refnum;//the number of all sentences
        cfg.hypOneSent = mygetline(ifs);

        float RedScore;
        for(int itref=0; itref < cfg.parseedstringRef.size();)//for every sentence
        {
            //cout<<"hyp: "<<itref<<" "<<cfg.hypOneSent<<endl;
            myclear(cfg);
            if(cfg.ConTree_bool) contreeExtracNgram(cfg, ifcontree);//extract Ngrams in constituent tree
            if(cfg.Align_bool) readAlign(cfg,ifalign); //meteor alignment with exact-stem-synonym
            if(cfg.Para_bool) readParaphrase(cfg,ifalign_p); //meteor alignment with paraphrase
            if(cfg.Entropy_bool) readAlignEssp(cfg,ifalign_mt); //meteor alignment with exact-stem-synonym-paraphrase
            getRefTrees(cfg, ifbest, itref); //get dep-ngrams(links) in reference tree
            removeSameLink(cfg); //keep the links in hwc if hwc and f&f have the same link
            if(cfg.print_link_bool) printLinks(cfg); //print the links 
            
            vector<pair<float,float> > RedNgram = getRedScore(cfg);//get score for one sentence
            RedScore = RedNgram[cfg.link_len].first;
            if(RedScore < 100) cfg.scoreAllSents += RedScore;
            if(!cfg.Train_bool)
                cout<<RedScore<<endl;
            else cout<<endl;
            cfg.hypOneSent = mygetline(ifs); //read the next hyp sentence
         }

		//system level
        float sysscore = getRedScore_sys(cfg);
        if(!cfg.Train_bool)
        {
           //   if(Score_Method=="red") cout<<"sys level red "<< sysscore <<endl;
		   if(Score_Method=="red") cout<<"sys level t2sts "<< cfg.scoreAllSents/(float)cfg.numcount <<endl;
		}    
        return 0;
    }
}
