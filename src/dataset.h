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

#ifndef	_DATASET_H
#define	_DATASET_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cstdio>
#include <cstdlib>
using namespace std;

// map of words/terms [string => int]
typedef map<string, int> mapword2id;
// map of words/terms [int => string]
typedef map<int, string> mapid2word;

class metafile
{
public:
	int count;
	int length;
};

class document {
public:
    int * words;
    int * tags;
    string rawstr;
    int length;
    

    void load(int id,FILE *fp, metafile **meta)
    {
	int ret;
	int count;
	count = meta[id]->count;
	length = meta[id]->length;
	words = new int[length];
	tags = new int[length];
	ret = fseek(fp,(count-length*2)*sizeof(int),SEEK_SET);
	if(ret!=0)
		cerr<<"error in fseek"<<endl;
	ret = fread(words,sizeof(int),length,fp);
	if(ret!=length)
		cerr<<"error in load "<<id<<endl;
	ret = fread(tags,sizeof(int),length,fp);
	if(ret!=length)
		cerr<<"error in load "<<id<<endl;
    }

    void flush(int id,FILE *fp, metafile **meta)
    {
	int ret;
	int count,length;
	count = meta[id]->count;
	length = meta[id]->length;
	fseek(fp,(count-length*2)*sizeof(int),SEEK_SET);

	ret = fwrite(words,sizeof(int),length,fp);
	if(ret!=length)
		cerr<<"error in flush doc_words "<<id<<endl;
	ret = fwrite(tags,sizeof(int),length,fp);
	if(ret!=length)
		cerr<<"error in flush doc_tags "<<id<<endl;
	delete words;
	delete tags;
	words = NULL;
	tags = NULL;
    }

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
    document ** docs;
public:
    metafile **meta;
    int current;
    FILE * fp;
    document ** _docs; // used only for inference
    map<int, int> _id2id; // also used only for inference
    int M; // number of documents
    int V; // number of words   
    string dfile;


    void set_dfile(string df)
    {
	this->dfile = df;
	if(TEST) cout<<"dfile is "<<dfile<<endl;
    }

    document * doc(int id)
    {
	if(id!=current)
	{
	   
	   if(docs[id]==NULL) docs[id] = new document;
	   if(fp==NULL) fp = fopen((dfile+".cmps").c_str(),"r+b");
	   if(current != -1) docs[current]->flush(current,fp,meta);
	   docs[id]->load(id,fp,meta);
	   current = id;
	}
	return docs[id];
    }

    dataset()
    {
	this->M = 0;
	this->V = 0;
	fp = NULL;
	dfile = "";
	current = -1;
	_docs = NULL;
	docs = NULL;
	meta = NULL;
    }

    dataset(int M) {
	this->M = M;
	this->V = 0;
	docs = new document*[M];
	meta = new metafile*[M];

	printf("META IS %p\n",(void *)meta);
	int i;
	for(i=0;i<M;i++)
		docs[i] = new document;	
	_docs = NULL;
	fp = NULL;
	current = -1;
    }   

    ~dataset() {
	if (docs){
	    if(current!=-1)
		docs[current]->flush(current,fp,meta); 
	    for (int i = 0; i < M; i++) {
		delete docs[i];
	    }
	}
	delete docs;
	
	if (_docs) {
	    for (int i = 0; i < M; i++) {
		delete _docs[i];		
	    }
	}
	delete _docs;	
    }
    
    void deallocate() {
	if (docs) {
	    for (int i = 0; i < M; i++) {
		delete docs[i];
	    }
	}
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
  
    void add_meta(struct metafile * mfile, int idx){
        if(meta==NULL) meta = new metafile*[M];
	if(0<=idx && idx <M) {
	   meta[idx] = mfile;
	}
    }

  
    void add_doc(document * doc, int idx) {
	if (0 <= idx && idx < M) {
	    if(idx!=current){
		if(current!=-1) 
			docs[current]->flush(current,fp,meta);
		current = idx;
	    }
	    docs[idx] = doc;
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

