#include "diskarray.h"
#include <iostream>
#include <string>
#include <cstdio>
using namespace std;

void diskArray::flush()
{
	int ret;
	fseek(fp,cur_row*length*sizeof(int),SEEK_SET);
	ret =fwrite(cached_row,sizeof(int),length,fp);
	if(ret!=length)
		cerr<<"error in flush"<<endl;
}

void diskArray::load(int rows)
{
	int ret;
	fseek(fp,rows*length*sizeof(int),SEEK_SET);
	ret = fread(cached_row,sizeof(int),length,fp);
	if(ret!=length)
		cerr<<"error in load"<<endl;
}

void diskArray::init(string name)
{
	filename = name;
	fp = fopen(filename.c_str(),"rwb");
}

void diskArray::set_length(int rownumber, int len)
{
	rows = rownumber;
	length = len;
	fseek(fp,(rows+1)*length*sizeof(int),SEEK_SET);
	fputc('\n',fp);
	
}

int &diskArray::visit(int i,int j)
{
	if(i>=0&&i<rows&&j>=0&&j<length)
	{
		if(cur_row!=i)
		{
			flush();
			load(i);
		}
	}
	else
	{
		cerr<<"error in visit "<<i<<' '<<j<<endl;
		return cached_row[0];
	}
	return cached_row[j];
}
