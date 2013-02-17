#include <iostream.h>

typedef unsigned int word;	  

word nexthoptable[255];
int TABLESIZE = sizeof(nexthoptable)/sizeof(nexthoptable[0]);

word insertNHtable(word NHtable[], word next_hop)
{
	int c;
	for (c = 1; c <= TABLESIZE ; c++)
	{
		//we leave NHtable[0] so as not to collide with the value 
		//returned to indicate error (or table full)
		if (next_hop == NHtable[c])   
			return c;                 
                                       
		if (NHtable[c] == 0) 
		{
			NHtable[c] = next_hop;
	        return c;
		}
	}
	return 0;

};

 
word findpointer( word next_hop, word nexthoptable[])
{
	int c;
	for (c = 1 ; c < TABLESIZE; c++)
	{
		if (next_hop == nexthoptable[c]) 
			return c;
	}
	return 0;

}

