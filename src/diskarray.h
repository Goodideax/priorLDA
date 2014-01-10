#ifndef _DISKARRAY_H
#define _DISKARRAY_H

#include <string>
using namespace std;
class diskArray
{
	//storage file name
	string filename;
	FILE * fp;

	//the row and column number
        int rows;
        int length;

	//in memory cache
	int * cached_row;

	//the current row number;
	int cur_row;

	//flush the cache to disk;
	void flush();

	//load a row from the disk
	void load(int rows);
public:
	//set the row and column number
        void init(string name);
	void set_length(int rownumber, int len);

	//visit an element
	int &visit(int i,int j);
	diskArray()
	{
		cur_row = -1;
	}	
};
#endif
