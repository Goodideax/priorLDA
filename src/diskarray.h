#ifndef _DISKARRAY_H
#define _DISKARRAY_H
#include <cstdio>
#include <string>
#include "constants.h"
using namespace std;
class diskArray
{
	//storage file name
	string filename;
	FILE * fp;

	//the row and column number
        long long rows;
        int length;

	//in memory cache
	int * cached_row;

	//the current row number;
	long long cur_row;

	//flush the cache to disk;
	void flush();

	//load a row from the disk
	void load(long long rows);
public:
	//set the row and column number
        void init(string name);
	void set_length(long long rownumber, int len);

	//visit an element
	int &visit(int i,int j);
	diskArray()
	{
		cur_row = -1;
	}

	~diskArray()
	{
/*	  flush();
	  delete cached_row;
	  if(fp)
		fclose(fp);
*/
	}	
};
#endif
