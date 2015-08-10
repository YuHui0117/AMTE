#ifndef ME_PARSER_H_
#define ME_PARSER_H_

#include "maxent.h"

class me_parser
{
public:
	me_parser(){}
	~me_parser(){}

	//extract features, for training classification model
	void extract_feats(char* fn_trn);

	//decode a file
	void decode(char* fn_tst, char* fn_out, maxent & me);

private:
	void extract_feat_of_a_inst(instance & inst, strVecVec & v_feat);
	
	void init_config(const instance & inst, vector<configuration>& v_cfg);

	void get_feat(const instance & inst, configuration & cfg, vector<string> & v_feat);

	void gen_feat(vector<string> & feat, const string & op){feat.insert(feat.begin(),op);}

	void print_feats(strVecVec & v_feat, ostream & out=cout);

	void decode(instance & inst, maxent &me, vector<configuration>& v_cfg);

	void normalize(vector<double> & v_score);

	void get_cls_vals(vector<string> & v_feat, 
		vector<map<string,double> > & v_cls_val,
		maxent &me);
	
	void generate_new_cfgs(instance & inst,
		maxent & me,
		configuration & cfg, 
		vector<configuration> & v_cfg);

	double compute_score(vector<map<string,double> > & v_cls_val, const string & op);
	
	vector<string> feasible_ops(const configuration & cfg, const instance & inst);
};


#endif
