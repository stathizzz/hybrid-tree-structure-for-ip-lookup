/*
* Copyright (c) 2004-2018, Sfecas D. Efstathios <stathizzz@gmail.com>.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the IP Lookup Using Level-Compressed Dynamic Tries Project nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/
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

