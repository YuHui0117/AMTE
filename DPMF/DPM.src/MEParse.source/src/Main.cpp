#include <cstdlib>
#include <fstream>
#include <string>
#include <iostream>
#include <cmath>
#include "../include/Decoder.h"

using namespace std;
int refnum=1;
int main(int argc, char *argv[])
{
	if (argc != 8)
	{
		cerr << "Usage: model beam input outDepTree outTreeScore reffile refnum" << endl;
		cerr << "one sentence one model" << endl;
        exit(1);
	}
    refnum = atoi(argv[7]);
	ifstream fin(argv[3]);

	if (!fin)
	{
		cerr << "Can not open file: " << argv[3] << endl;
		exit(1);
	}

	ofstream fout(argv[4]);
	if (!fout)
	{
		cerr << "Can not open file: " << argv[4] << endl;
		exit(1);
	}
    ofstream fscore(argv[5]);
    if(!fscore)
    {
        cerr << "Can not open file: " << argv[5] << endl;
    }
    ifstream refin(argv[6]);
    if(!refin)
    {
        cerr << "Can not open reference file: " << argv[6] <<endl;
        exit(1);
    }

	maxent me;
//	int tag = me.load_maxent_model(argv[1]);
//    if(tag==0)
//    {
//        fscore << -100<<endl;
//        cout<<"no reference model."<<endl;
//        return 0;
//        }

	int tag = me.load_maxent_model(argv[1]);
	int beam = atoi(argv[2]);

	beam = (beam < 1) ? 1 : beam;

	CDecoder decoder;
	string line,liner;
	int cnt = 0;

	while (getline(fin, line))
	{
		if (line.empty())
		{
			continue;
		}
        vector<string> vecref;
        for(int i=0; i<refnum; i++){
            getline(refin,liner);
            vecref.push_back(liner);
        }
		string result;
        float maxscore;
        int lenh, lenr;
		decoder.Decode(me, beam, line, result, maxscore, lenh, vecref, lenr);
//        fscore << maxscore <<endl;
        maxscore = maxscore/(float)(2*lenh-1);
        //fscore << maxscore+1 <<endl;
        //maxscore += 1;//(0,1)
        //if(maxscore>0)  ;
        //else maxscore=0;
       
        float sbp= exp(1-(float)lenr/(float)min(lenh,lenr));
        float srp= exp(1-(float)max(lenh,lenr)/(float)lenr);
        
//        fscore << sbp << endl;
//        fscore << srp << endl;
        fscore << exp(maxscore) <<endl;
//        fscore << (maxscore)*(sbp+srp)/(float)2 << endl;
//        if(BPexp<0.00001) BPexp=0;
        //fscore << maxscore << endl;
//        fscore << (maxscore)*BPexp << endl;

		++cnt;

		if (cnt % 1 == 0)
		{
			cerr << cnt << endl;
		}
	}

	cerr << "Succeed." << endl;
	fin.close();
	fout.close();
	return 0;
}
