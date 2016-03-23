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
#define _FILE_OFFSET_BITS   64
#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "strtokenizer.h"
#include "dataset.h"
#include <errno.h>
#include <string.h>

using namespace std;
extern int errno;
document * dataset::doc(int id)
    {
        int ret;
        if(id!=current)
        {
           if(docs==NULL) docs = new document;
           string str = "./";
           if(fp==NULL) fp = fopen((str+dfile+".cmps").c_str(),"r+b");
           if(fp==NULL){
                printf("%d\n",errno);
                cout<<"open file failed"<<endl;
           }
           if(current != -1) docs->flush(current,fp,meta);
           if(id==0){
                fseek(fp,0,SEEK_SET);
                pos = 0;
           }
	   if(fmeta==NULL) fmeta = fopen((str+dfile+".meta").c_str(),"r+b");
	   if(fmeta == NULL) {
		printf("%d\n", errno);
		cout<<"open meta file failed"<<endl;
	   }
	   ret = fseeko(fmeta,(id*2)*sizeof(long long), SEEK_SET);
	   ret = fread(meta,sizeof(long long),2,fmeta);
           docs->load(id,fp,meta);
           current = id;
        }
        return docs;
    }

void document::load(int id,FILE *fp, metafile *meta)
{
        int ret;
        long long count,length;
        count = meta->count;
        length = meta->length;
	this->length = length;
        //cout<<this->length<<endl;
        words = new int[length];
        tags = new int[length];
        ret = fseeko(fp,(count-length*2)*sizeof(int),SEEK_SET);
        if(ret!=0){
                cerr<<"error in fseek"<<endl;
                while(1);
        }

	long long tmp = ftello(fp);	
        ret = fread(words,sizeof(int),length,fp);
        if(ret!=length)
	{
		cerr<<(count-length*2)*sizeof(int)<<endl;
		cerr<<ret<<endl;
		cerr<<tmp<<endl;
                cerr<<"error in load "<<id<<endl;
	}
        ret = fread(tags,sizeof(int),length,fp);
        if(ret!=length)
                cerr<<"error in load "<<id<<endl;
}

void document::flush(int id,FILE *fp, metafile *meta)
{
        int ret;
        long long count,length;
        count = meta->count;
        length = meta->length;
        ret = fseeko(fp,(count-length*2)*sizeof(int),SEEK_SET);
        if(ret!=0)
                cerr<<"error in fseek:flush"<<endl;
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



int dataset::write_wordmap(string wordmapfile, mapword2id * pword2id) {
    FILE * fout = fopen(wordmapfile.c_str(), "w");
    if (!fout) {
	printf("Cannot open file %s to write!\n", wordmapfile.c_str());
	return 1;
    }    
    
    mapword2id::iterator it;
    fprintf(fout, "%d\n", pword2id->size());
    for (it = pword2id->begin(); it != pword2id->end(); it++) {
	fprintf(fout, "%s %d\n", (it->first).c_str(), it->second);
    }
    
    fclose(fout);
    
    return 0;
}

int dataset::read_wordmap(string wordmapfile, mapword2id * pword2id) {
    pword2id->clear();
    
    FILE * fin = fopen(wordmapfile.c_str(), "r");
    if (!fin) {
	printf("Cannot open file %s to read!\n", wordmapfile.c_str());
	return 1;
    }    
    
    char buff[BUFF_SIZE_SHORT];
    string line;
    
    fgets(buff, BUFF_SIZE_SHORT - 1, fin);
    int nwords = atoi(buff);
    
    for (int i = 0; i < nwords; i++) {
	fgets(buff, BUFF_SIZE_SHORT - 1, fin);
	line = buff;
	
	strtokenizer strtok(line, " \t\r\n");
	if (strtok.count_tokens() != 2) {
	    continue;
	}
	
	pword2id->insert(pair<string, int>(strtok.token(0), atoi(strtok.token(1).c_str())));
    }
    
    fclose(fin);
    
    return 0;
}

int dataset::read_wordmap(string wordmapfile, mapid2word * pid2word) {
    pid2word->clear();
    
    FILE * fin = fopen(wordmapfile.c_str(), "r");
    if (!fin) {
	printf("Cannot open file %s to read!\n", wordmapfile.c_str());
	return 1;
    }    
    
    char buff[BUFF_SIZE_SHORT];
    string line;
    
    fgets(buff, BUFF_SIZE_SHORT - 1, fin);
    int nwords = atoi(buff);
    
    for (int i = 0; i < nwords; i++) {
	fgets(buff, BUFF_SIZE_SHORT - 1, fin);
	line = buff;
	
	strtokenizer strtok(line, " \t\r\n");
	if (strtok.count_tokens() != 2) {
	    continue;
	}
	
	pid2word->insert(pair<int, string>(atoi(strtok.token(1).c_str()), strtok.token(0)));
    }
    
    fclose(fin);
    
    return 0;
}

int dataset::read_trndata(string dfile, string wordmapfile) {
    mapword2id word2id;
    
    FILE * fin = fopen(dfile.c_str(), "r");
    if (!fin) {
	printf("Cannot open file %s to read!\n", dfile.c_str());
	return 1;
    }   
    
    mapword2id::iterator it;    
    char buff[BUFF_SIZE_LONG];
    string line;
    
    // get the number of documents
    fgets(buff, BUFF_SIZE_LONG - 1, fin);
    M = atoi(buff);
    if (M <= 0) {
	printf("No document available!\n");
	return 1;
    }
    
    // allocate memory for corpus
    if (docs) {
	deallocate();
    } else {
	docs = new document;
    }
    
    // set number of words to zero
    V = 0;
    
    for (int i = 0; i < M; i++) {
	fgets(buff, BUFF_SIZE_LONG - 1, fin);
	line = buff;
	strtokenizer strtok(line, " \t\r\n");
	int length = strtok.count_tokens();

	if (length <= 0) {
	    printf("Invalid (empty) document!\n");
	    deallocate();
	    M = V = 0;
	    return 1;
	}
	
	// allocate new document
	document * pdoc = new document(length);
	
	for (int j = 0; j < length; j++) {
	    it = word2id.find(strtok.token(j));
	    if (it == word2id.end()) {
		// word not found, i.e., new word
		pdoc->words[j] = word2id.size();
		word2id.insert(pair<string, int>(strtok.token(j), word2id.size()));
	    } else {
		pdoc->words[j] = it->second;
	    }
	}
	
	// add new doc to the corpus
	add_doc(pdoc, i);
    }
    
    fclose(fin);
    
    // write word map to file
    if (write_wordmap(wordmapfile, &word2id)) {
	return 1;
    }
    
    // update number of words
    V = word2id.size();
    
    return 0;
}
int dataset::read_trndata_to_compress(string dfile, string wordmapfile) {
    mapword2id word2id;
//    idCounter idCount;
    FILE * fin = fopen(dfile.c_str(), "r");
    if (!fin) {
	printf("Cannot open file %s to read!\n", dfile.c_str());
	printf("%d\n", errno);
	return 1;
    }   
    mapword2id::iterator it;    
    char buff[BUFF_SIZE_LONG];
    string line;
    
    // get the number of documents
    fgets(buff, BUFF_SIZE_LONG - 1, fin);
//    FILE * ftmp;
//    ftmp = fopen("lens.tmp","r");
//    fgets(buff, BUFF_SIZE_LONG - 1, ftmp);
    M = atoi(buff);
//    fclose(ftmp);
    if (M <= 0) {
	printf("No document available!\n");
	return 1;
    }
//    FILE * fmap = fopen(wordmapfile.c_str(),"r");
//    fgets(buff,BUFF_SIZE_LONG - 1, fmap);
//    V = atoi(buff);
//    fclose(fin);
//    fclose(fmap);
//    return 0;
    // allocate memory for corpus
//    if (docs) {
//	delete docs;
//    }
    docs = new document;

//    if (meta) {
//	delete meta;
//    }
//    meta = new metafile*[M];
	    
    // set number of words to zero
    V = 0;
//add back
    FILE * fin_file = fopen((dfile+".cmps").c_str(),"wb");
    FILE * fin_file_meta = fopen((dfile+".meta").c_str(),"w");
    long long count=0, length;
    for (int i = 0; i < M; i++) {
	if(i%100000==0) cout<<i<<' '<<count<<endl;
	fgets(buff, BUFF_SIZE_LONG - 1, fin);
	line = buff;
	strtokenizer strtok(line, " \t\r\n");
	length = strtok.count_tokens();

	if (length <= 0) {
	    printf("Invalid (empty) document!\n");
	    deallocate();
	    M = V = 0;
	    return 1;
	}
	
	// allocate new document
//	document * pdoc = new document(length);
	metafile * pmeta = new  metafile;
	
	int * pdoc = new int[length];	
	for (int j = 0; j < length; j++) {
	    it = word2id.find(strtok.token(j));
	    if (it == word2id.end()) {
		// word not found, i.e., new word
//		pdoc->words[j] = word2id.size();
		pdoc[j] = word2id.size();
        	idCount.insert(pair<int, int>(word2id.size(),1));
		word2id.insert(pair<string, int>(strtok.token(j), word2id.size()));
	    } 
            else {
		pdoc[j] = it->second;
		idCount[it->second]+=1;
//		pdoc->words[j] = it->second;
	    }
	}
	// add new doc to the corpus
        count+=2*length;
	pmeta->count = count;
	pmeta->length = length;
//        cout<<i<<' '<<count<<' '<<length<<endl;
//add back
//	fprintf(fin_file_meta,"%d %d %d\n",i,count,length);
//	count+=(1+2*length)*sizeof(int);
//	fwrite(&length, sizeof(int), 1, fin_file);
	fwrite(pdoc,sizeof(int),length,fin_file);
	memset(pdoc,0,length*sizeof(int));
	fwrite(pdoc,sizeof(int),length,fin_file);
	fwrite(pmeta,sizeof(long long),2,fin_file_meta);
	delete pdoc;
    }
    if(TEST)
	printf("Already print all data!\n"); 
    fclose(fin);
    fclose(fin_file_meta);
    fclose(fin_file);
    // write word map to file
    if (write_wordmap(wordmapfile, &word2id)) {
	return 1;
    }
    cout<<count<<endl;
    // update number of words
    V = word2id.size();
    if(TEST)
	printf("I am about to return!\n");
    return 0;
}
int dataset::read_newdata(string dfile, string wordmapfile) {
    mapword2id word2id;
    map<int, int> id2_id;
    
    read_wordmap(wordmapfile, &word2id);
    if (word2id.size() <= 0) {
	printf("No word map available!\n");
	return 1;
    }

    FILE * fin = fopen(dfile.c_str(), "r");
    if (!fin) {
	printf("Cannot open file %s to read!\n", dfile.c_str());
	return 1;
    }   

    mapword2id::iterator it;
    map<int, int>::iterator _it;
    char buff[BUFF_SIZE_LONG];
    string line;
    
    // get number of new documents
    fgets(buff, BUFF_SIZE_LONG - 1, fin);
    M = atoi(buff);
    if (M <= 0) {
	printf("No document available!\n");
	return 1;
    }
    
    // allocate memory for corpus
    if (docs) {
	deallocate();
    } else {
	docs = new document;
    }
    _docs = new document*[M];
    
    // set number of words to zero
    V = 0;
    
    for (int i = 0; i < M; i++) {
	fgets(buff, BUFF_SIZE_LONG - 1, fin);
	line = buff;
	strtokenizer strtok(line, " \t\r\n");
	int length = strtok.count_tokens();
	
	vector<int> doc;
	vector<int> _doc;
	for (int j = 0; j < length; j++) {
	    it = word2id.find(strtok.token(j));
	    if (it == word2id.end()) {
		// word not found, i.e., word unseen in training data
		// do anything? (future decision)
	    } else {
		int _id;
		_it = id2_id.find(it->second);
		if (_it == id2_id.end()) {
		    _id = id2_id.size();
		    id2_id.insert(pair<int, int>(it->second, _id));
		    _id2id.insert(pair<int, int>(_id, it->second));
		} else {
		    _id = _it->second;
		}
		
		doc.push_back(it->second);
		_doc.push_back(_id);
	    }
	}
	
	// allocate memory for new doc
	document * pdoc = new document(doc);
	document * _pdoc = new document(_doc);
	
	// add new doc
	add_doc(pdoc, i);
	_add_doc(_pdoc, i);
    }
    
    fclose(fin);
    
    // update number of new words
    V = id2_id.size();
    
    return 0;
}

int dataset::read_newdata_withrawstrs(string dfile, string wordmapfile) {
    mapword2id word2id;
    map<int, int> id2_id;
    
    read_wordmap(wordmapfile, &word2id);
    if (word2id.size() <= 0) {
	printf("No word map available!\n");
	return 1;
    }

    FILE * fin = fopen(dfile.c_str(), "r");
    if (!fin) {
	printf("Cannot open file %s to read!\n", dfile.c_str());
	return 1;
    }   

    mapword2id::iterator it;
    map<int, int>::iterator _it;
    char buff[BUFF_SIZE_LONG];
    string line;
    
    // get number of new documents
    fgets(buff, BUFF_SIZE_LONG - 1, fin);
    M = atoi(buff);
    if (M <= 0) {
	printf("No document available!\n");
	return 1;
    }
    
    // allocate memory for corpus
    if (docs) {
	deallocate();
    } else {
	docs = new document;
    }
    _docs = new document*[M];
    
    // set number of words to zero
    V = 0;
    
    for (int i = 0; i < M; i++) {
	fgets(buff, BUFF_SIZE_LONG - 1, fin);
	line = buff;
	strtokenizer strtok(line, " \t\r\n");
	int length = strtok.count_tokens();
	
	vector<int> doc;
	vector<int> _doc;
	for (int j = 0; j < length - 1; j++) {
	    it = word2id.find(strtok.token(j));
	    if (it == word2id.end()) {
		// word not found, i.e., word unseen in training data
		// do anything? (future decision)
	    } else {
		int _id;
		_it = id2_id.find(it->second);
		if (_it == id2_id.end()) {
		    _id = id2_id.size();
		    id2_id.insert(pair<int, int>(it->second, _id));
		    _id2id.insert(pair<int, int>(_id, it->second));
		} else {
		    _id = _it->second;
		}
		
		doc.push_back(it->second);
		_doc.push_back(_id);
	    }
	}
	
	// allocate memory for new doc
	document * pdoc = new document(doc, line);
	document * _pdoc = new document(_doc, line);
	
	// add new doc
	add_doc(pdoc, i);
	_add_doc(_pdoc, i);
    }
    
    fclose(fin);
    
    // update number of new words
    V = id2_id.size();
    
    return 0;
}

