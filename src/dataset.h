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
#define _FILE_OFFSET_BITS    64
#ifndef	_DATASET_H
#define	_DATASET_H
#define _FILE_OFFSET_BITS   64
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <errno.h>
using namespace std;
extern int errno;
// map of words/terms [string => int]
typedef map<string, int> mapword2id;
// map of words/terms [int => string]
typedef map<int, string> mapid2word;
// counter of each id
//typedef map<int, int> idCounter;


class metafile
{
public:
	long long count;
	long long length;
};

class document {
public:
    int * words;
    int * tags;
    string rawstr;
    long long length;

    void load(int id,FILE *fp, metafile *meta);
    void flush(int id,FILE *fp, metafile *meta);

    document() {
	words = NULL;
	tags = NULL;
	rawstr = "";
	length = 0;
	
    }
 
    document(int length) {
	this->length = length;
	rawstr = "";
	words = new int[length];
	tags  = new int[length];	
    }
    
    document(int length, int * in_words) {
	this->length = length;
	rawstr = "";
	this->words = new int[length];
	this->tags  = new int[length];
	for (int i = 0; i < length; i++) {
	    this->words[i] = in_words[i];
	}
    }

    document(int length, int * in_words, string rawstr) {
	this->length = length;
	this->rawstr = rawstr;
	this->words = new int[length];
	this->tags  = new int[length];
	for (int i = 0; i < length; i++) {
	    this->words[i] = in_words[i];
	}
    }
    
    document(vector<int> & doc) {
	this->length = doc.size();
	rawstr = "";
	this->words = new int[length];
        this->tags  = new int[length];
	for (int i = 0; i < length; i++) {
	    this->words[i] = doc[i];
	}
    }

    void add_topic(vector<int> &topic){
	this->length = topic.size();
	this->tags = new int[length];
	for(int i=0; i< length; i++) {
	    this->tags[i] = topic[i];
	}
    }
    document(vector<int> & doc, string rawstr) {
	this->length = doc.size();
	this->rawstr = rawstr;
	this->words = new int[length];
	this->tags  = new int[length];
	for (int i = 0; i < length; i++) {
	    this->words[i] = doc[i];
	}
    }
    
    ~document() {
	//TODO flush them into the disk
	if (words) {
	    delete words;
	}
	if (tags) {
	    delete tags;
	}
    }
};



class dataset {
private:
    document * docs;
public:
    metafile *meta;
    int current;
    FILE * fp;
    FILE * fmeta;
    document ** _docs; // used only for inference
    map<int, int> _id2id; // also used only for inference
    int M; // number of documents
    int V; // number of words   
    string dfile="";
    long long pos;
    int length;
    vector<int> idCount;

    void set_dfile(string df)
    {
	this->dfile = df;
	if(TEST) cout<<"dfile is "<<dfile<<endl;
    }

    document * doc(int id);

    dataset()
    {
	this->M = 0;
	this->V = 0;
	fp = NULL;
	dfile = "";
	current = -1;
	_docs = NULL;
	docs = NULL;
	pos = 0;
	meta = new metafile;
    }

    dataset(int M) {
	this->M = M;
	this->V = 0;
	docs = new document;
	meta = new metafile;
//	int i;
//	for(i=0;i<M;i++)
//		docs[i] = new document;	
	_docs = NULL;
	fp = NULL;
	current = -1;
	pos = 0;
    }   

    ~dataset() {
	if (docs){
	    if(current!=-1)
		docs->flush(current,fp,meta); 
//	    for (int i = 0; i < M; i++) {
//		delete docs[i];
//	    }
	}
//	delete docs;
	
	if (_docs) {
	    for (int i = 0; i < M; i++) {
		delete _docs[i];		
	    }
	}
	delete _docs;	
    }
    
    void deallocate() {
//	if (docs) {
//	    for (int i = 0; i < M; i++) {
//		delete docs[i];
//	    }
//	}
	delete docs;
	docs = NULL;

	if (_docs) {
	    for (int i = 0; i < M; i++) {
		delete _docs[i];
	    }
	}
	delete _docs;
	_docs = NULL;
    }
  
  
    void add_doc(document * doc, int idx) {
	if (0 <= idx && idx < M) {
	    if(idx!=current){
		if(current!=-1) 
			docs->flush(current,fp,meta);
		current = idx;
	    }
	    docs = doc;
	}
    }

    
    void _add_doc(document * doc, int idx) {
	if (0 <= idx && idx < M) {
	    _docs[idx] = doc;
	}
    }       

    static int write_wordmap(string wordmapfile, mapword2id * pword2id);
    static int read_wordmap(string wordmapfile, mapword2id * pword2id);
    static int read_wordmap(string wordmapfile, mapid2word * pid2word);
    
    int read_trndata(string dfile, string wordmapfile);
    int read_trndata_to_compress(string dfile, string wordmapfile);
    int read_newdata(string dfile, string wordmapfile);
    int read_newdata_withrawstrs(string dfile, string wordmapfile);
};

#endif

