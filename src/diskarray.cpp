#define _FILE_OFFSET_BITS 64
#include "diskarray.h"
#include <iostream>
#include <string>
#include <cstdio>
#include <errno.h>
using namespace std;
extern int errno;
void diskArray::flush()
{
	int ret;
	fseeko(fp,cur_row*length*sizeof(int),SEEK_SET);
	ret =fwrite(cached_row,sizeof(int),length,fp);
	if(ret!=length)
		cerr<<"error in flush"<<endl;
}

void diskArray::load(long long rows)
{
	int ret;
	fseeko(fp,rows*length*sizeof(int),SEEK_SET);
	ret = fread(cached_row,sizeof(int),length,fp);
	if(ret!=length)
	{
		cerr<<"error in load "<<ret<<' '<<length<<endl;	
		printf("%d\n", errno);
		cin>>ret;
	}
}

void diskArray::init(string name)
{
	filename = name;
	fp = fopen(filename.c_str(),"w+b");
	if(!fp)
	{
		cerr<<"error in open ND array FILE"<<endl;
	}
}

void diskArray::set_length(long long rownumber, int len)
{
	rows = rownumber;
	length = len;
	long long offset;
	offset = (rows+1)*length*sizeof(int);
	cout<<"offset is "<<offset<<endl;
	int ret;
	ret = fseeko(fp, offset,SEEK_SET);
	if(ret!=0)
	{
		printf("%d/",errno);
		cout<<"initialization error"<<' '<<ret<<endl;
	}
	//cout<<ftello(fp)<<endl;
	ret = fputc('\n',fp);
	//cout<<ftello(fp)<<endl;
	if(ret==EOF)
		cout<<"ND init error"<<endl;
//	fclose(fp);
//	int i;
//	cin>>i;
//	fp = fopen(filename.c_str(), "r+b");
	if(!fp)
	{
		cout<<"reopen error in diskarray:set_length"<<endl;
	}
	cached_row = new int[length];
	
}

int &diskArray::visit(int i,int j)
{
	if(i>=0&&i<rows&&j>=0&&j<length)
	{
		if(cur_row!=i)
		{
			flush();
			load(i);
			cur_row = i;
		}
	}
	else
	{
		cerr<<"error in visit "<<i<<' '<<j<<' '<<rows<<' '<<length<<endl;
		return cached_row[0];
	}
	return cached_row[j];
}
