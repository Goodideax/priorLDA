/*
 * Copyright (C) 2007 by
 * 
 * 	Xuan-Hieu Phan
 *	hieuxuan@ecei.tohoku.ac.jp or pxhieu@gmail.com
 * 	Graduate School of Information Sciences
 * 	Tohoku University
 *
 * GibbsLDA++ is a free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * GibbsLDA++ is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GibbsLDA++; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

/* 
 * References:
 * + The Java code of Gregor Heinrich (gregor@arbylon.net)
 *   http://www.arbylon.net/projects/LdaGibbsSampler.java
 * + "Parameter estimation for text analysis" by Gregor Heinrich
 *   http://www.arbylon.net/publications/text-est.pdf
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cstring>
#include "constants.h"
#include "strtokenizer.h"
#include "utils.h"
#include "dataset.h"
#include "model.h"
using namespace std;
static void swap(int &a, int &b)
{
	int t;
	t = a;
	a = b;
	b = t;
	return;
}

void model::inc(int topic, int word, int *nw)
{
	if(topic>=K)
		cout<<"problem found in inc: "<<topic<<' '<<word<<endl;
	int i;
//	if(word==74)
//		cin>>i>>i;
//        if(wordCount.find(word)==wordCount.end()) 
//	    cout<<word<<" not exist!"<<endl;
//	else
//	    cout<<word<<" exists"<<endl;
        int len=wordCount[word];
	for(i=0;i<min(len,K);i++){
		if((nw[i]&0x3ff)==topic){
			nw[i]+=(1<<10);
			break;
		}
		else if(nw[i]==0){
			nw[i]+=(1<<10);
			nw[i]|=topic;
			break;
		}
	}
	for(;i>0;i--)
		if(nw[i]>nw[i-1]) swap(nw[i],nw[i-1]);
}

void model::dec(int topic, int word, int *nw)
{
	if(topic>=K)
		cout<<"problem found in inc: "<<topic<<' '<<word<<endl;

	int i;
	int len = wordCount[word];
	for(i=0;i<min(K,len);i++){
		if((nw[i]&0x3ff)==topic){
			nw[i]-=(1<<10);
			if((nw[i]>>10) == 0) nw[i]=0;
			break;
		}
	}

	for(;i<min(len,K)-1 && nw[i+1]!=0 ;i++)
		if(nw[i]<nw[i+1]) swap(nw[i],nw[i+1]);

}

model::~model() {
    if (p) {
	delete p;
    }

    if (ptrndata) {
	delete ptrndata;
    }
    
    if (pnewdata) {
	delete pnewdata;
    }

/*
    if (z) {
	for (int m = 0; m < M; m++) {
	    if (z[m]) {
		delete z[m];
	    }
	}
    }
*/  
    if (nw) {
	for (int w = 0; w < V; w++) {
	    if (nw[w]) {
		delete nw[w];
	    }
	}
    }

    if (nd) {
	for (int m = 0; m < M; m++) {
	    if (nd[m]) {
		delete nd[m];
	    }
	}
    } 
    
    if (nwsum) {
	delete nwsum;
    }   
    
    if (ndsum) {
	delete ndsum;
    }
    
    if (theta) {
	for (int m = 0; m < M; m++) {
	    if (theta[m]) {
		delete theta[m];
	    }
	}
    }
    
    if (phi) {
	for (int k = 0; k < K; k++) {
	    if (phi[k]) {
		delete phi[k];
	    }
	}
    }

    // only for inference
    if (newz) {
	for (int m = 0; m < newM; m++) {
	    if (newz[m]) {
		delete newz[m];
	    }
	}
    }
    
    if (newnw) {
	for (int w = 0; w < newV; w++) {
	    if (newnw[w]) {
		delete newnw[w];
	    }
	}
    }

    if (newnd) {
	for (int m = 0; m < newM; m++) {
	    if (newnd[m]) {
		delete newnd[m];
	    }
	}
    } 
    
    if (newnwsum) {
	delete newnwsum;
    }   
    
    if (newndsum) {
	delete newndsum;
    }
    
    if (newtheta) {
	for (int m = 0; m < newM; m++) {
	    if (newtheta[m]) {
		delete newtheta[m];
	    }
	}
    }
    
    if (newphi) {
	for (int k = 0; k < K; k++) {
	    if (newphi[k]) {
		delete newphi[k];
	    }
	}
    }
}

void model::set_default_values() {
    wordmapfile = "wordmap.txt";
    trainlogfile = "trainlog.txt";
    tassign_suffix = ".tassign";
    theta_suffix = ".theta";
    phi_suffix = ".phi";
    others_suffix = ".others";
    twords_suffix = ".twords";
    
    dir = "./";
    dfile = "trn.dat";
    model_name = "model-final";    
    model_status = MODEL_STATUS_UNKNOWN;
    
    ptrndata = NULL;
    pnewdata = NULL;
    
    M = 0;
    V = 0;
    K = 200;
    alpha = 50.0 / K;
    beta = 0.1;
    niters = 2000;
    liter = 0;
    savestep = 200000;    
    twords = 0;
    withrawstrs = 0;
    
    p = NULL;
//    z = NULL;
    nw = NULL;
    nd = NULL;
    nwsum = NULL;
    ndsum = NULL;
    theta = NULL;
    phi = NULL;
    
    newM = 0;
    newV = 0;
    newz = NULL;
    newnw = NULL;
    newnd = NULL;
    newnwsum = NULL;
    newndsum = NULL;
    newtheta = NULL;
    newphi = NULL;
}

int model::parse_args(int argc, char ** argv) {
    return utils::parse_args(argc, argv, this);
}

int model::init(int argc, char ** argv) {
    // call parse_args
    if (parse_args(argc, argv)) {
	return 1;
    }
    cout<<"model_status is "<<model_status<<endl; 
    if (model_status == MODEL_STATUS_EST) {
	// estimating the model from scratch
	if (init_est()) {
	    return 1;
	}
	
    } else if (model_status == MODEL_STATUS_ESTC) {
	// estimating the model from a previously estimated one
	if (init_estc()) {
	    return 1;
	}
	
    } else if (model_status == MODEL_STATUS_INF) {
	// do inference
	if (init_inf()) {
	    return 1;
	}
    } else if (model_status == MODEL_STATUS_EST_DISK) {
	if(init_est_disk()) {
		return 1;
	}
    } else if (model_status ==MODEL_STATUS_ESTC_DISK) {
	if(init_estc_disk()){
		return 1;
	}
    } else if (model_status ==MODEL_STATUS_INF) {
	if(init_inf_disk()) {
		return 1;
	}
    } else if(model_status == MODEL_STATUS_EST_DISK_SAMPLE) {
	cout<<"new world!"<<endl;
	if(init_est_disk_sample()) {
		return 1;
	}
    }
    return 0;
}

int model::load_model(string model_name) {
    int i, j;
    
    string filename = dir + model_name + tassign_suffix;
    FILE * fin = fopen(filename.c_str(), "r");
    if (!fin) {
	printf("Cannot open file %d to load model!\n", filename.c_str());
	return 1;
    }
    
    char buff[BUFF_SIZE_LONG];
    string line;

    // allocate memory for z and ptrndata
//    z = new int*[M];
    ptrndata = new dataset(M);
    ptrndata->set_dfile(dfile);
    ptrndata->V = V;

    for (i = 0; i < M; i++) {
	char * pointer = fgets(buff, BUFF_SIZE_LONG, fin);
	if (!pointer) {
	    printf("Invalid word-topic assignment file, check the number of !\n");
	    return 1;
	}
	
	line = buff;
	strtokenizer strtok(line, " \t\r\n");
	int length = strtok.count_tokens();
	
	vector<int> words;
	vector<int> topics;
	for (j = 0; j < length; j++) {
	    string token = strtok.token(j);
    
	    strtokenizer tok(token, ":");
	    if (tok.count_tokens() != 2) {
		printf("Invalid word-topic assignment line!\n");
		return 1;
	    }
	    
	    words.push_back(atoi(tok.token(0).c_str()));
	    topics.push_back(atoi(tok.token(1).c_str()));
	}
	
	// allocate and add new document to the corpus
	document * pdoc = new document(words);
	pdoc->add_topic(topics);
	ptrndata->add_doc(pdoc, i);
	
/*
	// assign values for z
	z[i] = new int[topics.size()];
	for (j = 0; j < topics.size(); j++) {
	    z[i][j] = topics[j];
	}
*/
    }   
    
    fclose(fin);
    
    return 0;
}

int model::save_model(string model_name) {
    if (save_model_tassign(dir + model_name + tassign_suffix)) {
	return 1;
    }
    
    if (save_model_others(dir + model_name + others_suffix)) {
	return 1;
    }
    
    if (save_model_theta(dir + model_name + theta_suffix)) {
	return 1;
    }
    
    if (save_model_phi(dir + model_name + phi_suffix)) {
	return 1;
    }
    
    if (twords > 0) {
	if (save_model_twords(dir + model_name + twords_suffix)) {
	    return 1;
	}
    }
    
    return 0;
}

int model::save_model_tassign(string filename) {
    int i, j;
    
    FILE * fout = fopen(filename.c_str(), "w");
    if (!fout) {
	printf("Cannot open file %s to save!\n", filename.c_str());
	return 1;
    }

    // wirte  with topic assignments for words
    for (i = 0; i < ptrndata->M; i++) {    
	for (j = 0; j < ptrndata->doc(i)->length; j++) {
	    fprintf(fout, "%d:%d ", ptrndata->doc(i)->words[j], ptrndata->doc(i)->tags[j]);
//		fprintf(fout, "%d:%d ", ptrndata->doc(i)->words[j], z[i][j]);
	}
	fprintf(fout, "\n");
    }

    fclose(fout);
    
    return 0;
}

int model::save_model_theta(string filename) {
    FILE * fout = fopen(filename.c_str(), "w");
    if (!fout) {
	printf("Cannot open file %s to save!\n", filename.c_str());
	return 1;
    }
    
    for (int i = 0; i < M; i++) {
	for (int j = 0; j < K; j++) {
	    fprintf(fout, "%f ", theta[i][j]);
	}
	fprintf(fout, "\n");
    }
    
    fclose(fout);
    
    return 0;
}

int model::save_model_phi(string filename) {
    FILE * fout = fopen(filename.c_str(), "w");
    if (!fout) {
	printf("Cannot open file %s to save!\n", filename.c_str());
	return 1;
    }
    
    for (int i = 0; i < K; i++) {
	for (int j = 0; j < V; j++) {
	    fprintf(fout, "%f ", phi[i][j]);
	}
	fprintf(fout, "\n");
    }
    
    fclose(fout);    
    
    return 0;
}

int model::save_model_others(string filename) {
    FILE * fout = fopen(filename.c_str(), "w");
    if (!fout) {
	printf("Cannot open file %s to save!\n", filename.c_str());
	return 1;
    }

    fprintf(fout, "alpha=%f\n", alpha);
    fprintf(fout, "beta=%f\n", beta);
    fprintf(fout, "ntopics=%d\n", K);
    fprintf(fout, "n=%d\n", M);
    fprintf(fout, "nwords=%d\n", V);
    fprintf(fout, "liter=%d\n", liter);
    
    fclose(fout);    
    
    return 0;
}

int model::save_model_twords(string filename) {
    FILE * fout = fopen(filename.c_str(), "w");
    if (!fout) {
	printf("Cannot open file %s to save!\n", filename.c_str());
	return 1;
    }
    
    if (twords > V) {
	twords = V;
    }
    mapid2word::iterator it;
    
    for (int k = 0; k < K; k++) {
	vector<pair<int, double> > words_probs;
	pair<int, double> word_prob;
	for (int w = 0; w < V; w++) {
	    word_prob.first = w;
	    word_prob.second = phi[k][w];
	    words_probs.push_back(word_prob);
	}
    
        // quick sort to sort word-topic probability
	utils::quicksort(words_probs, 0, words_probs.size() - 1);
	
	fprintf(fout, "Topic %dth:\n", k);
	for (int i = 0; i < twords; i++) {
	    it = id2word.find(words_probs[i].first);
	    if (it != id2word.end()) {
		fprintf(fout, "\t%s   %f\n", (it->second).c_str(), words_probs[i].second);
	    }
	}
    }
    
    fclose(fout);    
    
    return 0;    
}

int model::save_inf_model(string model_name) {
    if (save_inf_model_tassign(dir + model_name + tassign_suffix)) {
	return 1;
    }
    
    if (save_inf_model_others(dir + model_name + others_suffix)) {
	return 1;
    }
    
    if (save_inf_model_newtheta(dir + model_name + theta_suffix)) {
	return 1;
    }
    
    if (save_inf_model_newphi(dir + model_name + phi_suffix)) {
	return 1;
    }

    if (twords > 0) {
	if (save_inf_model_twords(dir + model_name + twords_suffix)) {
	    return 1;
	}
    }
    
    return 0;
}

int model::save_inf_model_tassign(string filename) {
    int i, j;
    
    FILE * fout = fopen(filename.c_str(), "w");
    if (!fout) {
	printf("Cannot open file %s to save!\n", filename.c_str());
	return 1;
    }

    // wirte  with topic assignments for words
    for (i = 0; i < pnewdata->M; i++) {    
	for (j = 0; j < pnewdata->doc(i)->length; j++) {
	    fprintf(fout, "%d:%d ", pnewdata->doc(i)->words[j], newz[i][j]);
	}
	fprintf(fout, "\n");
    }

    fclose(fout);
    
    return 0;
}

int model::save_inf_model_newtheta(string filename) {
    int i, j;

    FILE * fout = fopen(filename.c_str(), "w");
    if (!fout) {
	printf("Cannot open file %s to save!\n", filename.c_str());
	return 1;
    }
    
    for (i = 0; i < newM; i++) {
	for (j = 0; j < K; j++) {
	    fprintf(fout, "%f ", newtheta[i][j]);
	}
	fprintf(fout, "\n");
    }
    
    fclose(fout);
    
    return 0;
}

int model::save_inf_model_newphi(string filename) {
    FILE * fout = fopen(filename.c_str(), "w");
    if (!fout) {
	printf("Cannot open file %s to save!\n", filename.c_str());
	return 1;
    }
    
    for (int i = 0; i < K; i++) {
	for (int j = 0; j < newV; j++) {
	    fprintf(fout, "%f ", newphi[i][j]);
	}
	fprintf(fout, "\n");
    }
    
    fclose(fout);    
    
    return 0;
}

int model::save_inf_model_others(string filename) {
    FILE * fout = fopen(filename.c_str(), "w");
    if (!fout) {
	printf("Cannot open file %s to save!\n", filename.c_str());
	return 1;
    }

    fprintf(fout, "alpha=%f\n", alpha);
    fprintf(fout, "beta=%f\n", beta);
    fprintf(fout, "ntopics=%d\n", K);
    fprintf(fout, "n=%d\n", newM);
    fprintf(fout, "nwords=%d\n", newV);
    fprintf(fout, "liter=%d\n", inf_liter);
    
    fclose(fout);    
    
    return 0;
}

int model::save_inf_model_twords(string filename) {
    FILE * fout = fopen(filename.c_str(), "w");
    if (!fout) {
	printf("Cannot open file %s to save!\n", filename.c_str());
	return 1;
    }
    
    if (twords > newV) {
	twords = newV;
    }
    mapid2word::iterator it;
    map<int, int>::iterator _it;
    
    for (int k = 0; k < K; k++) {
	vector<pair<int, double> > words_probs;
	pair<int, double> word_prob;
	for (int w = 0; w < newV; w++) {
	    word_prob.first = w;
	    word_prob.second = newphi[k][w];
	    words_probs.push_back(word_prob);
	}
    
        // quick sort to sort word-topic probability
	utils::quicksort(words_probs, 0, words_probs.size() - 1);
	
	fprintf(fout, "Topic %dth:\n", k);
	for (int i = 0; i < twords; i++) {
	    _it = pnewdata->_id2id.find(words_probs[i].first);
	    if (_it == pnewdata->_id2id.end()) {
		continue;
	    }
	    it = id2word.find(_it->second);
	    if (it != id2word.end()) {
		fprintf(fout, "\t%s   %f\n", (it->second).c_str(), words_probs[i].second);
	    }
	}
    }
    
    fclose(fout);    
    
    return 0;    
}


int model::init_est() {
    int m, n, w, k;

    p = new double[K];

    // + read training data
    ptrndata = new dataset;
    ptrndata->set_dfile(dfile);
    if (ptrndata->read_trndata(dir + dfile, dir + wordmapfile)) {
        printf("Fail to read training data!\n");
        return 1;
    }
		
    // + allocate memory and assign values for variables
    M = ptrndata->M;
    V = ptrndata->V;
    // K: from command line or default value
    // alpha, beta: from command line or default values
    // niters, savestep: from command line or default values

    nw = new int*[V];
    for (w = 0; w < V; w++) {
        nw[w] = new int[K];
        for (k = 0; k < K; k++) {
    	    nw[w][k] = 0;
        }
    }
	
    nd = new int*[M];
    for (m = 0; m < M; m++) {
        nd[m] = new int[K];
        for (k = 0; k < K; k++) {
    	    nd[m][k] = 0;
        }
    }
	
    nwsum = new int[K];
    for (k = 0; k < K; k++) {
	nwsum[k] = 0;
    }
    
    ndsum = new int[M];
    for (m = 0; m < M; m++) {
	ndsum[m] = 0;
    }

    srandom(time(0)); // initialize for random number generation
//    z = new int*[M];
    for (m = 0; m < ptrndata->M; m++) {
	int N = ptrndata->doc(m)->length;
//	z[m] = new int[N];
	
        // initialize for z
        for (n = 0; n < N; n++) {
    	    int topic = (int)(((double)random() / RAND_MAX) * K);
	    ptrndata->doc(m)->tags[n] = topic;  
//  	    z[m][n] = topic;
    	    
    	    // number of instances of word i assigned to topic j
    	    nw[ptrndata->doc(m)->words[n]][topic] += 1;
    	    // number of words in document i assigned to topic j
    	    nd[m][topic] += 1;
    	    // total number of words assigned to topic j
    	    nwsum[topic] += 1;
        } 
        // total number of words in document i
        ndsum[m] = N;      
    }
    
    theta = new double*[M];
    for (m = 0; m < M; m++) {
        theta[m] = new double[K];
    }
	
    phi = new double*[K];
    for (k = 0; k < K; k++) {
        phi[k] = new double[V];
    }    
    
    return 0;
}

int model::init_estc() {
    // estimating the model from a previously estimated one
    int m, n, w, k;

    p = new double[K];

    // load moel, i.e., read z and ptrndata
    if (load_model(model_name)) {
	printf("Fail to load word-topic assignmetn file of the model!\n");
	return 1;
    }

    nw = new int*[V];
    for (w = 0; w < V; w++) {
        nw[w] = new int[K];
        for (k = 0; k < K; k++) {
    	    nw[w][k] = 0;
        }
    }
	
    nd = new int*[M];
    for (m = 0; m < M; m++) {
        nd[m] = new int[K];
        for (k = 0; k < K; k++) {
    	    nd[m][k] = 0;
        }
    }
	
    nwsum = new int[K];
    for (k = 0; k < K; k++) {
	nwsum[k] = 0;
    }
    
    ndsum = new int[M];
    for (m = 0; m < M; m++) {
	ndsum[m] = 0;
    }

    for (m = 0; m < ptrndata->M; m++) {
	int N = ptrndata->doc(m)->length;

	// assign values for nw, nd, nwsum, and ndsum	
        for (n = 0; n < N; n++) {
    	    int w = ptrndata->doc(m)->words[n];
//    	    int topic = z[m][n];
    	    int topic = ptrndata->doc(m)->tags[n];
    	    

    	    // number of instances of word i assigned to topic j
    	    nw[w][topic] += 1;
    	    // number of words in document i assigned to topic j
    	    nd[m][topic] += 1;
    	    // total number of words assigned to topic j
    	    nwsum[topic] += 1;
        } 
        // total number of words in document i
        ndsum[m] = N;      
    }
	
    theta = new double*[M];
    for (m = 0; m < M; m++) {
        theta[m] = new double[K];
    }
	
    phi = new double*[K];
    for (k = 0; k < K; k++) {
        phi[k] = new double[V];
    }    

    return 0;        
}

void model::estimate() {
    if (twords > 0) {
	// print out top words per topic
	dataset::read_wordmap(dir + wordmapfile, &id2word);
    }

    printf("Sampling %d iterations!\n", niters);

    int last_iter = liter;
    for (liter = last_iter + 1; liter <= niters + last_iter; liter++) {
	printf("Iteration %d ...\n", liter);
	
	// for all z_i
	for (int m = 0; m < M; m++) {
	    for (int n = 0; n < ptrndata->doc(m)->length; n++) {
		// (z_i = z[m][n])
		// sample from p(z_i|z_-i, w)
		int topic = sampling(m, n);
		ptrndata->doc(m)->tags[n] = topic;
	    }
	}
	
	if (savestep > 0) {
	    if (liter % savestep == 0) {
		// saving the model
		printf("Saving the model at iteration %d ...\n", liter);
		compute_theta();
		compute_phi();
		save_model(utils::generate_model_name(liter));
	    }
	}
    }
    
    printf("Gibbs sampling completed!\n");
    printf("Saving the final model!\n");
    compute_theta();
    compute_phi();
    liter--;
    save_model(utils::generate_model_name(-1));
    int i,j;
    int count = 0;
    for(i=0;i<V;i++)
	for(j=0;j<K;j++)
	    if(nw[i][j]==0) count++;
    printf("count is %d,total is %d\n",count,V*K);
}

int model::sampling(int m, int n) {
    // remove z_i from the count variables
    int topic = ptrndata->doc(m)->tags[n];
    int w = ptrndata->doc(m)->words[n];
    nw[w][topic] -= 1;
    nd[m][topic] -= 1;
    nwsum[topic] -= 1;
    ndsum[m] -= 1;

    double Vbeta = V * beta;
    double Kalpha = K * alpha;    
    // do multinomial sampling via cumulative method
    for (int k = 0; k < K; k++) {
	p[k] = (nw[w][k] + beta) / (nwsum[k] + Vbeta) *
		    (nd[m][k] + alpha) / (ndsum[m] + Kalpha);
    }
    // cumulate multinomial parameters
    for (int k = 1; k < K; k++) {
	p[k] += p[k - 1];
    }
    // scaled sample because of unnormalized p[]
    double u = ((double)random() / RAND_MAX) * p[K - 1];
    
    for (topic = 0; topic < K; topic++) {
	if (p[topic] > u) {
	    break;
	}
    }
    
    // add newly estimated z_i to count variables
    nw[w][topic] += 1;
    nd[m][topic] += 1;
    nwsum[topic] += 1;
    ndsum[m] += 1;    
    
    return topic;
}

void model::compute_theta() {
    for (int m = 0; m < M; m++) {
	for (int k = 0; k < K; k++) {
	    theta[m][k] = (nd[m][k] + alpha) / (ndsum[m] + K * alpha);
	}
    }
}

void model::compute_phi() {
    for (int k = 0; k < K; k++) {
	for (int w = 0; w < V; w++) {
	    phi[k][w] = (nw[w][k] + beta) / (nwsum[k] + V * beta);
	}
    }
}
static int min(int a, int b)
{
    if(a < b) return a; else return b;
}

int model::init_est_disk() {
    int m, n, w, k;

    if(TEST)
	printf("Entering: init_est_disk()\n");

    p = new double[K];

    ptrndata = new dataset;
    ptrndata->set_dfile(dfile);
    if(TEST) cout<<"dfile is "<<dfile<<endl;
    if(TEST)
    {
	printf("Starting to print compressed file\n");
  	if(ptrndata->read_trndata_to_compress(dir + dfile, dir + wordmapfile)){
		printf("Fail to read training data!\n");	
	}
    }
    printf("finish compress!\n");	
    // + allocate memory and assign values for variables
    M = ptrndata->M;//Number of documents
    V = ptrndata->V;//Number of words
    // K: from command line or default value
    // alpha, beta: from command line or default values
    // niters, savestep: from command line or default values

    // K is the topic number.
    cout<<V<<" "<<K<<' '<<M<<endl;
    cout<<"about to begin initial"<<endl;
    int a;
    nw = new int*[V];
    wordCount = ptrndata->idCount;
    for (w = 0; w < V; w++) {
        if(wordCount[w] <= K)
        	nw[w] = new int[wordCount[w]];
	else
		nw[w] = new int[K];
        for (k = 0; k < min(wordCount[w],K); k++) {
    	    nw[w][k] = 0;
        }
    }
//    int tmp = ptrndata->doc(m)->length;
    string name = "nd.tmp";
    ND.init(name);
    ND.set_length(M,K);
    for(m=0; m<M; m++){
	for( k=0; k<K; k++)
	   ND.visit(m,k)=0;
    }
    cout<<"finish initial"<<endl;
    nwsum = new int[K];
    for (k = 0; k < K; k++) {
	nwsum[k] = 0;
    }
    
    ndsum = new int[M];
    for (m = 0; m < M; m++) {
	ndsum[m] = 0;
    }
    srandom(time(0)); // initialize for random number generation
    for (m = 0; m < ptrndata->M; m++) {
	//ptrndata->fp = fopen((ptrndata->dfile+".cmps").c_str(),"r+b");
	int N = ptrndata->doc(m)->length;
        // initialize for z
	if(m%100000==0) cout<<m<<endl;
        for (n = 0; n < N; n++) {
    	    int topic = (int)(((double)random() / RAND_MAX) * K)+1;
    	    ptrndata->doc(m)->tags[n] = topic;
//	    cout<<"m,n,topic:"<<m<<' '<<n<<' '<<topic<<endl;
    	    w = ptrndata->doc(m)->words[n]; 
    	    // number of instances of word i assigned to topic j
    	    inc(topic-1, w, nw[w]);
//	    for(int i=0;i<min(K,wordCount[w]);i++)
//		cout<<(nw[w][i]&0x3ff)<<' '<<(nw[w][i]>>10)<<endl;
    	    // number of words in document i assigned to topic j
    	    ND.visit(m,topic-1) += 1;
    	    // total number of words assigned to topic j
    	    nwsum[topic-1] += 1;
        } 
        // total number of words in document i
        ndsum[m] = N;      
    }
/*    
    theta = new double*[M];
    for (m = 0; m < M; m++) {
        theta[m] = new double[K];
    }
	
    phi = new double*[K];
    for (k = 0; k < K; k++) {
        phi[k] = new double[V];
    }    
*/  
    cout<<"finish whold initial"<<endl;  
    return 0;
}

int model::init_est_disk_sample() {
    int m, n, w, k;
    tcount=0;
    if(TEST)
	printf("Entering: init_est_disk_sample()\n");

    p = new double[K];

    ptrndata = new dataset;
    ptrndata->set_dfile(dfile);
    if(TEST) cout<<"dfile is "<<dfile<<endl;
    if(TEST)
    {
	printf("Starting to print compressed file\n");
  	if(ptrndata->read_trndata_to_compress(dir + dfile, dir + wordmapfile)){
		printf("Fail to read training data!\n");	
	}
    }
    printf("finish compress!\n");	
    // + allocate memory and assign values for variables
    M = ptrndata->M;//Number of documents
    V = ptrndata->V;//Number of words
    // K: from command line or default value
    // alpha, beta: from command line or default values
    // niters, savestep: from command line or default values

    // K is the topic number.
    cout<<V<<" "<<K<<' '<<M<<endl;
    cout<<"about to begin initial"<<endl;
    int a;
    nw = new int*[V];
    wordCount = ptrndata->idCount;
    for (w = 0; w < V; w++) {
        if(wordCount[w] <= K)
        	nw[w] = new int[wordCount[w]];
	else
		nw[w] = new int[K];
        for (k = 0; k < min(wordCount[w],K); k++) {
    	    nw[w][k] = 0;
        }
    }
//    int tmp = ptrndata->doc(m)->length;
    string name = "nd.tmp";
    ND.init(name);
    ND.set_length(M,K);
    for(m=0; m<M; m++){
	for( k=0; k<K; k++)
	   ND.visit(m,k)=0;
    }
    cout<<"finish initial"<<endl;
    nwsum = new int[K];
    for (k = 0; k < K; k++) {
	nwsum[k] = 0;
    }
    
    ndsum = new int[M];
    for (m = 0; m < M; m++) {
	ndsum[m] = 0;
    }
    srandom(time(0)); // initialize for random number generation
    for (m = 0; m < ptrndata->M; m++) {
        if(((double)random()/RAND_MAX) < sample_rate) {
	    sdoc.push_back(m);
	    cout<<m<<' '<<endl;
	}
	else
	    continue;
	//ptrndata->fp = fopen((ptrndata->dfile+".cmps").c_str(),"r+b");
	int N = ptrndata->doc(m)->length;
        // initialize for z
	if(m%100000==0) cout<<m<<endl;
        for (n = 0; n < N; n++) {
    	    int topic = (int)(((double)random() / RAND_MAX) * K)+1;
    	    ptrndata->doc(m)->tags[n] = topic;
	    if(topic>K+1)
		cout<<"bad topic:"<<topic<<endl;
    	    w = ptrndata->doc(m)->words[n]; 
    	    // number of instances of word i assigned to topic j
    	    inc(topic-1, w, nw[w]);
    	    // number of words in document i assigned to topic j
    	    ND.visit(m,topic-1) += 1;
    	    // total number of words assigned to topic j
    	    nwsum[topic-1] += 1;
        } 
        // total number of words in document i
        ndsum[m] = N;
    }
    estimate_disk_sample();
    cout<<"finish sample estimate"<<endl;
    for(m=0; m< ptrndata->M; m++) {
	if(ndsum[m]!=0) continue;
	ssum = 0;
	rsum = 0;
	int tmpnd[K];
	int tmpsum,tmpcur;
	memset(tmpnd,0,sizeof(tmpnd));
	for( int i=0;i<ptrndata->doc(m)->length;i++)
	{
		int w = ptrndata->doc(m)->words[i];
		int len = wordCount[i];
		int j;
		tmpsum = 0;
		for(j=0;j<min(len,K);j++)
		{
			if(nw[w][j]==0) break;
			tmpsum+= (nw[w][j]>>10);
//			tmpnd[nw[w][j]&0x3ff]+= (nw[w][j]>>10);
		}
		double rand = ((double) random()/ RAND_MAX)*tmpsum;
		for(j=0;j<min(len,K);j++)
		{
			tmpcur+=(nw[w][j]>>10);
			if(tmpcur>=rand) break;
		}
		if(tmpcur<rand) j = min(len,K)-1;
		tmpnd[nw[w][j]&0x3ff]++;
	}
	for( int i=0; i<K; i++)
		ND.visit(m,i) = tmpnd[i];
	double tmp;
	for( int t = 0; t < K; t++)
        {
            tmp = alpha * beta / (beta*V + nwsum[t]);
	    ssum +=tmp;
            rsum += ND.visit(m,t)*beta / (beta*V + nwsum[t]);
	    q1[t] = (alpha + ND.visit(m,t)) / (beta*V + nwsum[t]);
//	    cout<<"q1[t], ND: "<<q1[t]<<' '<<ND.visit(m,t)<<endl;
        }
//	cout<<"initial ssum: "<<ssum<<endl;
        for (int n = 0; n < ptrndata->doc(m)->length; n++) {
	    // (z_i = z[m][n])
	    // sample from p(z_i|z_-i, w)
	    
	    int topic = sampling_disk(m, n);
 	    int oldtopic = ptrndata->doc(m)->tags[n];
	    ptrndata->doc(m)->tags[n] = topic;
        }
	memset(tmpnd,0,sizeof(tmpnd));
	for( int i=0;i<ptrndata->doc(m)->length; i++)
		tmpnd[ptrndata->doc(m)->tags[i]-1]++;
	for( int i=0;i<K;i++)
		ND.visit(m,i) = tmpnd[i];
    }
    cout<<"finish initial"<<endl;  
    return 0;
}

void model::estimate_disk_sample() {
    int m;
    if(TEST)
	printf("Entering: estimate_disk_sample\n");
    if (twords > 0) {
	// print out top words per topic
	dataset::read_wordmap(dir + wordmapfile, &id2word);
    }

    printf("Sampling %d iterations!\n", niters);
    q1 = new double[K];
    int last_iter = liter;
    for (liter = last_iter + 1; liter <= niters + last_iter; liter++) {
	printf("Iteration %d ...\n", liter);
	cout<<"sdoc size is "<<sdoc.size()<<endl;	
	// for all z_i
//	for (int i=0;i<ptrndata->M; i++)
//		cout<<i<<' '<<ndsum[i]<<endl;
	for (int i = 0; i < sdoc.size(); i++) {
	    m = sdoc[i];
	    ssum = rsum = qsum = 0;
            for( int t = 0; t < K; t++)
            {
                ssum += alpha * beta / (beta*V + nwsum[t]);
                rsum += ND.visit(m,t)*beta / (beta*V + nwsum[t]);
		q1[t] = (alpha + ND.visit(m,t)) / (beta*V + nwsum[t]);
//	    	cout<<"q1[t], ND: "<<q1[t]<<' '<<ND.visit(m,t)<<endl;
            }
	    for (int n = 0; n < ptrndata->doc(m)->length; n++) {
		// (z_i = z[m][n])
		// sample from p(z_i|z_-i, w)
		int topic = sampling_disk(m, n);
 		int oldtopic = ptrndata->doc(m)->tags[n];
		ptrndata->doc(m)->tags[n] = topic;
		if(topic>K+1 || topic <=0)
			cout<<"invalid topic: "<<topic<<" m,n:"<<m<<' '<<n<<endl;

	    }
	
	}

	if (savestep > 0) {
	    if (liter % savestep == 0) {
		// saving the model
		printf("Saving the model at iteration %d ...\n", liter);
		compute_theta_disk();
		compute_phi_disk();
		save_model(utils::generate_model_name(liter));
	    }
	}
    }
    
    printf("Gibbs sampling completed!\n");
    printf("Saving the final model!\n");
//    compute_theta_disk();
//    compute_phi_disk();
    liter--;
    if(TEST)
      	printf("saving the modelING....");
//    save_model(utils::generate_model_name(-1));
}

int model::init_estc_disk() {
    // estimating the model from a previously estimated one
    int m, n, w, k;

    p = new double[K];

    // load moel, i.e., read z and ptrndata
    if (load_model(model_name)) {
	printf("Fail to load word-topic assignmetn file of the model!\n");
	return 1;
    }

    nw = new int*[V];
    for (w = 0; w < V; w++) {
        nw[w] = new int[K];
        for (k = 0; k < K; k++) {
    	    nw[w][k] = 0;
        }
    }
/*	
    nd = new int*[M];
    for (m = 0; m < M; m++) {
        nd[m] = new int[K];
        for (k = 0; k < K; k++) {
    	    nd[m][k] = 0;
        }
    }
*/
    string name = "nd.tmp";	
    ND.init(name);
    ND.set_length(M,K);
    for(m=0;m<M;m++){
	for(k=0;k<K;k++)
	  ND.visit(m,k)=0;
    }

    nwsum = new int[K];
    for (k = 0; k < K; k++) {
	nwsum[k] = 0;
    }
    
    ndsum = new int[M];
    for (m = 0; m < M; m++) {
	ndsum[m] = 0;
    }

    for (m = 0; m < ptrndata->M; m++) {
	int N = ptrndata->doc(m)->length;

	// assign values for nw, nd, nwsum, and ndsum	
        for (n = 0; n < N; n++) {
    	    int w = ptrndata->doc(m)->words[n];
    	    int topic = ptrndata->doc(m)->tags[n];
    	    
    	    // number of instances of word i assigned to topic j
    	    nw[w][topic] += 1;
    	    // number of words in document i assigned to topic j
    	    ND.visit(m,topic) += 1;
    	    // total number of words assigned to topic j
    	    nwsum[topic] += 1;
        } 
        // total number of words in document i
        ndsum[m] = N;      
    }
	
    theta = new double*[M];
    for (m = 0; m < M; m++) {
        theta[m] = new double[K];
    }
	
    phi = new double*[K];
    for (k = 0; k < K; k++) {
        phi[k] = new double[V];
    }    

    return 0;        
}

void model::estimate_disk() {
    if(TEST)
	printf("Entering: estimate_disk\n");
    if (twords > 0) {
	// print out top words per topic
	dataset::read_wordmap(dir + wordmapfile, &id2word);
    }

    printf("Sampling %d iterations!\n", niters);
    q1 = new double[K];
    int last_iter = liter;
    for (liter = last_iter + 1; liter <= niters + last_iter; liter++) {
	printf("Iteration %d ...\n", liter);
	int diff = 0;
	// for all z_i
	int oldND[K];
	for (int m = 0; m < M; m++) {
	    ssum = rsum = qsum = 0;
            for( int t = 0; t < K; t++)
            {
                ssum += alpha * beta / (beta*V + nwsum[t]);
                rsum += ND.visit(m,t)*beta / (beta*V + nwsum[t]);
		q1[t] = (alpha + ND.visit(m,t)) / (beta*V + nwsum[t]);
	        oldND[t] = ND.visit(m,t);
            }
	    if(rsum<0) cout<<"rsum is invalid: "<<rsum<<endl;
	    for (int n = 0; n < ptrndata->doc(m)->length; n++) {
		// (z_i = z[m][n])
		// sample from p(z_i|z_-i, w)
//		cout<<"m,n,topic: "<<m<<' '<<n<<' '<<ptrndata->doc(m)->tags[n]<<' '<<ptrndata->doc(m)->words[n]<<endl;
		int topic = sampling_disk(m, n);
		if(topic>K+1 || topic <=0)
			cout<<"invalid topic: "<<topic<<" m,n:"<<m<<' '<<n<<endl;
 		int oldtopic = ptrndata->doc(m)->tags[n];
		ptrndata->doc(m)->tags[n] = topic;
	    }
	    for( int t=0;t<K;t++)
		diff += abs(oldND[t]-ND.visit(m,t));

	}
	cout<<"diff in iteration "<<liter<<" is "<<diff<<endl;
	if (savestep > 0) {
	    if (liter % savestep == 0) {
		// saving the model
		printf("Saving the model at iteration %d ...\n", liter);
		compute_theta_disk();
		compute_phi_disk();
		save_model(utils::generate_model_name(liter));
	    }
	}
    }
//    for( int m = 0; m<M; m++) {
//	for( int n = 0; n< ptrndata->doc(m)->length; n++)
//		cout<<"m,n,topic: "<<m<<' '<<n<<' '<<ptrndata->doc(m)->tags[n]<<endl;
//    } 
    printf("Gibbs sampling completed!\n");
    printf("Saving the final model!\n");
    cout<<"tcount is "<<tcount<<endl;
//    compute_theta_disk();
//    compute_phi_disk();
    liter--;
    if(TEST)
      	printf("saving the modelING....");
//    save_model(utils::generate_model_name(-1));
}

int model::sampling_disk(int m, int n) {
    // remove z_i from the count variables
//    if(TEST)
//	printf("sampleing_disk: m:%d n:%d\n",m,n);
    int topic = ptrndata->doc(m)->tags[n];
//    cout<<"m,n: "<<m<<' '<<n<<" topic: "<<topic<<endl;
    int ret;
    int w = ptrndata->doc(m)->words[n];
    double Vbeta = V * beta;
    double Kalpha = K * alpha;    
    int t;
    int len;
    int k;
//    cout<<rsum<<endl;   
    if(topic!=0){
	ssum -= alpha * beta / ( Vbeta + nwsum[topic-1]);
        rsum -= ND.visit(m,topic-1) * beta / (Vbeta + nwsum[topic-1]);
//	cout<<rsum<<endl;
	dec(topic-1,w,nw[w]); 
	ND.visit(m,topic-1) -= 1;
	nwsum[topic-1] -= 1;
	ndsum[m] -= 1;
	if(ndsum[m]<0)
		cout<<"wrong ndsum m,n,topic: "<<ndsum[m]<<' '<<m<<' '<<n<<' '<<topic<<endl;

	ssum += alpha * beta / (Vbeta + nwsum[topic-1]);
	rsum += ND.visit(m,topic-1) * beta / (Vbeta + nwsum[topic-1]);
	q1[topic-1]= (alpha + ND.visit(m,topic-1))/(Vbeta + nwsum[topic-1]);
//        cout<<rsum<<endl;
    }
    double tsum = 0;
    for(int r=0;r<K;r++)
	tsum += ND.visit(m,r) * beta / (Vbeta + nwsum[r]);
//    cout<<"real rsum: "<<tsum<<endl;
    qsum=0;
    len = wordCount[w];
//    cout<<len<<' '<<K<<endl;
//    for(int i=0;i<min(K,wordCount[w]);i++)
//	cout<<(nw[w][i]&0x3ff)<<' '<<(nw[w][i]>>10)<<endl;
    for(k=0; k<min(len,K); k++)
    {
        if(nw[w][k]==0) break;
        t = nw[w][k]&0x3ff;
        p[k]=q1[t]*(nw[w][k]>>10);
	qsum+=p[k];
	if(t>=K) 
        	cout<<k<<' '<<p[k]<<' '<<t<<' '<<(nw[w][k]>>10)<<' '<<q1[t]<<' '<<nwsum[t]<<' '<<ND.visit(m,t)<<endl;
    }
    len = k;    
    // scaled sample because of unnormalized p[]
    double u = ((double)random() / RAND_MAX) * (ssum+rsum+qsum);
//    cout<<"ssum,rsum,qsum,u: "<<ssum<<' '<<rsum<<' '<<qsum<<' '<<u<<endl;
    if(rsum<0) cout<<"ND "<<ND.visit(m,topic-1)<<endl;
    if(u<=qsum) {
        for(k=1; k<len; k++)
            p[k]+=p[k-1];
        for(ret = 0; ret < len; ret++)
            if (p[ret] >= u){ 
		ret = nw[w][ret]&0x3ff;
		break;
	    }
        if(ret==len)
	    ret = len-1;
        tcount+=len;
    }
    else {
	tcount+=K;
        for(k=0; k<K; k++)
            p[k]=(ND.visit(m,k)+alpha)*beta / (Vbeta + nwsum[k]);
        for(k=1; k<K; k++)
            p[k] += p[k-1];
        for(ret = 0; ret<K; ret++)
            if(p[ret] > u- qsum) 
		break;
        if(ret==K) ret = K -1;
//	cout<<p[K-1]<<' '<<ssum+rsum<<endl;
    }
    ret++;
     
    ssum -= alpha * beta / (Vbeta + nwsum[ret-1]);
    rsum -= ND.visit(m, ret-1) * beta / (Vbeta + nwsum[ret-1]);
    inc(ret-1,w,nw[w]);
    ND.visit(m,ret-1) += 1;
    nwsum[ret-1] += 1;
    ndsum[m] += 1;    
    ssum += alpha * beta / (Vbeta + nwsum[ret-1]);
    rsum += ND.visit(m,ret-1) * beta / (Vbeta + nwsum[ret-1]);
    q1[ret-1] = (alpha + ND.visit(m,ret-1)) / (Vbeta + nwsum[ret-1]);
//    cout<<rsum<<endl;
    if(ret==0)
      cout<<"ssum,rsum,qsum,u,len: "<<ssum<<' '<<rsum<<' '<<qsum<<' '<<u<<' '<<len<<endl;
//    cout<<"old, new topic: "<<topic<<' '<<ret<<endl;
    return ret;
}

void model::compute_theta_disk() {
    if(TEST)
	printf("Enter the compute_theta_disk\n");
    for (int m = 0; m < M; m++) {
	for (int k = 0; k < K; k++) {
	    theta[m][k] = (ND.visit(m,k) + alpha) / (ndsum[m] + K * alpha);
	}
    }
}

void model::compute_phi_disk() {
    if(TEST)
	printf("Enter the compute_phi_disk\n");
    for (int k = 0; k < K; k++) {
	for (int w = 0; w < V; w++) {
	    phi[k][w] = (nw[w][k] + beta) / (nwsum[k] + V * beta);
	}
    }
}



int model::init_inf_disk() {
    // estimating the model from a previously estimated one
    int m, n, w, k;

    p = new double[K];

    // load moel, i.e., read z and ptrndata
    if (load_model(model_name)) {
	printf("Fail to load word-topic assignmetn file of the model!\n");
	return 1;
    }

    nw = new int*[V];
    for (w = 0; w < V; w++) {
        nw[w] = new int[K];
        for (k = 0; k < K; k++) {
    	    nw[w][k] = 0;
        }
    }
	
    nd = new int*[M];
    for (m = 0; m < M; m++) {
        nd[m] = new int[K];
        for (k = 0; k < K; k++) {
    	    nd[m][k] = 0;
        }
    }
	
    nwsum = new int[K];
    for (k = 0; k < K; k++) {
	nwsum[k] = 0;
    }
    
    ndsum = new int[M];
    for (m = 0; m < M; m++) {
	ndsum[m] = 0;
    }

    for (m = 0; m < ptrndata->M; m++) {
	int N = ptrndata->doc(m)->length;

	// assign values for nw, nd, nwsum, and ndsum	
        for (n = 0; n < N; n++) {
    	    int w = ptrndata->doc(m)->words[n];
    	    int topic = ptrndata->doc(m)->tags[n];
    	    
    	    // number of instances of word i assigned to topic j
    	    nw[w][topic] += 1;
    	    // number of words in document i assigned to topic j
    	    nd[m][topic] += 1;
    	    // total number of words assigned to topic j
    	    nwsum[topic] += 1;
        } 
        // total number of words in document i
        ndsum[m] = N;      
    }
    
    // read new data for inference
    pnewdata = new dataset;
    pnewdata->set_dfile(dfile);
    if (withrawstrs) {
	if (pnewdata->read_newdata_withrawstrs(dir + dfile, dir + wordmapfile)) {
    	    printf("Fail to read new data!\n");
    	    return 1;
	}    
    } else {
	if (pnewdata->read_newdata(dir + dfile, dir + wordmapfile)) {
    	    printf("Fail to read new data!\n");
    	    return 1;
	}    
    }
    
    newM = pnewdata->M;
    newV = pnewdata->V;
    
    newnw = new int*[newV];
    for (w = 0; w < newV; w++) {
        newnw[w] = new int[K];
        for (k = 0; k < K; k++) {
    	    newnw[w][k] = 0;
        }
    }
	
    newnd = new int*[newM];
    for (m = 0; m < newM; m++) {
        newnd[m] = new int[K];
        for (k = 0; k < K; k++) {
    	    newnd[m][k] = 0;
        }
    }
	
    newnwsum = new int[K];
    for (k = 0; k < K; k++) {
	newnwsum[k] = 0;
    }
    
    newndsum = new int[newM];
    for (m = 0; m < newM; m++) {
	newndsum[m] = 0;
    }

    srandom(time(0)); // initialize for random number generation
    newz = new int*[newM];
    for (m = 0; m < pnewdata->M; m++) {
	int N = pnewdata->doc(m)->length;
	newz[m] = new int[N];

	// assign values for nw, nd, nwsum, and ndsum	
        for (n = 0; n < N; n++) {
    	    int w = pnewdata->doc(m)->words[n];
    	    int _w = pnewdata->_docs[m]->words[n];
    	    int topic = (int)(((double)random() / RAND_MAX) * K);
    	    newz[m][n] = topic;
    	    
    	    // number of instances of word i assigned to topic j
    	    newnw[_w][topic] += 1;
    	    // number of words in document i assigned to topic j
    	    newnd[m][topic] += 1;
    	    // total number of words assigned to topic j
    	    newnwsum[topic] += 1;
        } 
        // total number of words in document i
        newndsum[m] = N;      
    }    
    
    newtheta = new double*[newM];
    for (m = 0; m < newM; m++) {
        newtheta[m] = new double[K];
    }
	
    newphi = new double*[K];
    for (k = 0; k < K; k++) {
        newphi[k] = new double[newV];
    }    
    
    return 0;        
}



int model::init_inf() {
    // estimating the model from a previously estimated one
    int m, n, w, k;

    p = new double[K];

    // load moel, i.e., read z and ptrndata
    if (load_model(model_name)) {
	printf("Fail to load word-topic assignmetn file of the model!\n");
	return 1;
    }

    nw = new int*[V];
    for (w = 0; w < V; w++) {
        nw[w] = new int[K];
        for (k = 0; k < K; k++) {
    	    nw[w][k] = 0;
        }
    }
	
    nd = new int*[M];
    for (m = 0; m < M; m++) {
        nd[m] = new int[K];
        for (k = 0; k < K; k++) {
    	    nd[m][k] = 0;
        }
    }
	
    nwsum = new int[K];
    for (k = 0; k < K; k++) {
	nwsum[k] = 0;
    }
    
    ndsum = new int[M];
    for (m = 0; m < M; m++) {
	ndsum[m] = 0;
    }

    for (m = 0; m < ptrndata->M; m++) {
	int N = ptrndata->doc(m)->length;

	// assign values for nw, nd, nwsum, and ndsum	
        for (n = 0; n < N; n++) {
    	    int w = ptrndata->doc(m)->words[n];
    	    int topic = ptrndata->doc(m)->tags[n];
    	    
    	    // number of instances of word i assigned to topic j
    	    nw[w][topic] += 1;
    	    // number of words in document i assigned to topic j
    	    nd[m][topic] += 1;
    	    // total number of words assigned to topic j
    	    nwsum[topic] += 1;
        } 
        // total number of words in document i
        ndsum[m] = N;      
    }
    
    // read new data for inference
    pnewdata = new dataset(M);
    pnewdata->set_dfile(dfile);
    if (withrawstrs) {
	if (pnewdata->read_newdata_withrawstrs(dir + dfile, dir + wordmapfile)) {
    	    printf("Fail to read new data!\n");
    	    return 1;
	}    
    } else {
	if (pnewdata->read_newdata(dir + dfile, dir + wordmapfile)) {
    	    printf("Fail to read new data!\n");
    	    return 1;
	}    
    }
    
    newM = pnewdata->M;
    newV = pnewdata->V;
    
    newnw = new int*[newV];
    for (w = 0; w < newV; w++) {
        newnw[w] = new int[K];
        for (k = 0; k < K; k++) {
    	    newnw[w][k] = 0;
        }
    }
	
    newnd = new int*[newM];
    for (m = 0; m < newM; m++) {
        newnd[m] = new int[K];
        for (k = 0; k < K; k++) {
    	    newnd[m][k] = 0;
        }
    }
	
    newnwsum = new int[K];
    for (k = 0; k < K; k++) {
	newnwsum[k] = 0;
    }
    
    newndsum = new int[newM];
    for (m = 0; m < newM; m++) {
	newndsum[m] = 0;
    }

    srandom(time(0)); // initialize for random number generation
    newz = new int*[newM];
    for (m = 0; m < pnewdata->M; m++) {
	int N = pnewdata->doc(m)->length;
	newz[m] = new int[N];

	// assign values for nw, nd, nwsum, and ndsum	
        for (n = 0; n < N; n++) {
    	    int w = pnewdata->doc(m)->words[n];
    	    int _w = pnewdata->_docs[m]->words[n];
    	    int topic = (int)(((double)random() / RAND_MAX) * K);
    	    newz[m][n] = topic;
    	    
    	    // number of instances of word i assigned to topic j
    	    newnw[_w][topic] += 1;
    	    // number of words in document i assigned to topic j
    	    newnd[m][topic] += 1;
    	    // total number of words assigned to topic j
    	    newnwsum[topic] += 1;
        } 
        // total number of words in document i
        newndsum[m] = N;      
    }    
    
    newtheta = new double*[newM];
    for (m = 0; m < newM; m++) {
        newtheta[m] = new double[K];
    }
	
    newphi = new double*[K];
    for (k = 0; k < K; k++) {
        newphi[k] = new double[newV];
    }    
    
    return 0;        
}

void model::inference() {
    if (twords > 0) {
	// print out top words per topic
	dataset::read_wordmap(dir + wordmapfile, &id2word);
    }

    printf("Sampling %d iterations for inference!\n", niters);
    
    for (inf_liter = 1; inf_liter <= niters; inf_liter++) {
	printf("Iteration %d ...\n", inf_liter);
	
	// for all newz_i
	for (int m = 0; m < newM; m++) {
	    for (int n = 0; n < pnewdata->doc(m)->length; n++) {
		// (newz_i = newz[m][n])
		// sample from p(z_i|z_-i, w)
		int topic = inf_sampling(m, n);
		newz[m][n] = topic;
	    }
	}
    }
    
    printf("Gibbs sampling for inference completed!\n");
    printf("Saving the inference outputs!\n");
    compute_newtheta();
    compute_newphi();
    inf_liter--;
    save_inf_model(dfile);
}

int model::inf_sampling(int m, int n) {
    // remove z_i from the count variables
    int topic = newz[m][n];
    int w = pnewdata->doc(m)->words[n];
    int _w = pnewdata->_docs[m]->words[n];
    newnw[_w][topic] -= 1;
    newnd[m][topic] -= 1;
    newnwsum[topic] -= 1;
    newndsum[m] -= 1;
    
    double Vbeta = V * beta;
    double Kalpha = K * alpha;
    // do multinomial sampling via cumulative method
    for (int k = 0; k < K; k++) {
	p[k] = (nw[w][k] + newnw[_w][k] + beta) / (nwsum[k] + newnwsum[k] + Vbeta) *
		    (newnd[m][k] + alpha) / (newndsum[m] + Kalpha);
    }
    // cumulate multinomial parameters
    for (int k = 1; k < K; k++) {
	p[k] += p[k - 1];
    }
    // scaled sample because of unnormalized p[]
    double u = ((double)random() / RAND_MAX) * p[K - 1];
    
    for (topic = 0; topic < K; topic++) {
	if (p[topic] > u) {
	    break;
	}
    }
    
    // add newly estimated z_i to count variables
    newnw[_w][topic] += 1;
    newnd[m][topic] += 1;
    newnwsum[topic] += 1;
    newndsum[m] += 1;    
    
    return topic;
}

void model::compute_newtheta() {
    for (int m = 0; m < newM; m++) {
	for (int k = 0; k < K; k++) {
	    newtheta[m][k] = (newnd[m][k] + alpha) / (newndsum[m] + K * alpha);
	}
    }
}

void model::compute_newphi() {
    map<int, int>::iterator it;
    for (int k = 0; k < K; k++) {
	for (int w = 0; w < newV; w++) {
	    it = pnewdata->_id2id.find(w);
	    if (it != pnewdata->_id2id.end()) {
		newphi[k][w] = (nw[it->second][k] + newnw[w][k] + beta) / (nwsum[k] + newnwsum[k] + V * beta);
	    }
	}
    }
}

