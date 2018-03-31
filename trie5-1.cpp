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
#include  <iostream.h>
#include "splaytree.h"
#include <stdio.h>          //for file manipulation

#define NODE_EMPTY -3
#define MAX_ADDRESS 100000

typedef unsigned int word;

struct trieNode 
{
    //flag showing the entry status of current node (there is an  
	//entry iff the path of the entry in the trie uses current node) 
	//0:no entry in current node  
    //1:Non-Prefix Expanded entry exists & the length of the entry matches node's stride
	//2:Prefix Expanded entry exists    
	//3:Non-Prefix Expanded entry exists & the length of the entry doesnt match node's stride
	int flag;  
    
	//if entry is Prefix Expanded, PE -> length of the PE entry
	//if entry is NPE point -> PE == 0 
	//int point;  
	int PE;

	// the next-hop data
	unsigned int nexthop;    

    trieNode() {
	    flag = 0;
		PE = 0;
        nexthop = 0;
	}
};

//#include <stdlib.h>

class SplayTrie : public SplayTree<int> 
{
public:
	 struct splaytrieNode {
		int length[8];
		int nexthop[8];
		int ip_value;
		//splaytrieNode();
		
	};
	
	splaytrieNode Snode[128];
	unsigned int triePointer;
	unsigned int num_nodes;//
	//word nexthop;
	SplayTrie(int empty) : SplayTree<int>(-9999)
	{	
		for (int k=0;k<8;k++) 
		for (int l=0;l<8;l++){
			Snode[k].length[l] = 0;
			Snode[k].nexthop[l] = 0;
		    Snode[k].ip_value = 0;
		}
		//splaytrieNode *Snode; 
		//Snode = 0;
		//t = new Splaytree[];
		num_nodes = 0;
	  	triePointer = empty;
        //nexthop = 0;
	//*matrix = 0;
	}
	//friend class SplayTree;
};

 


class Trie
{
 public:
	int stride1;	// the stride of the root
	int stride2;    // the second-level stride
	unsigned int bf1;// # nodes in first-level,branching factor
	unsigned int bf2; //# nodes in second-level for each one-level node 
	
	trieNode  *nodeSet;		// base of array of Nodes
	trieNode  *nodeSet2;
	SplayTrie *spltree[];
	

	Trie(int k1 , int k2 ) // k the  stride1
	{		
		stride1 = k1;
		stride2 = k2;
		bf1 = 1<<stride1;
		bf2 = 1<<stride2;
    	nodeSet = new trieNode[bf1];		// base of array of Nodes
	    nodeSet2 = new trieNode[bf1*bf2];			
	}


	~Trie() 
	{ 
		delete []  nodeSet, nodeSet2; 
	}

	word iplookup( word );
    void insertEntries(char *, int);
    int lookupEntries(char *,char *, int);
 //private:
	void insert( word , int, word );
};
	static int j = 0;

void Trie::insert(word data, int len, word next_hop) 
{   
    cout << "ari8mos dentrou " << j << endl;
	register unsigned int p1,p2;
	int new_len, prefchunk;
    unsigned int prefix, i;
    		
	// clear the 32-len last bits, this shouldn't be necessary
    // if the routtable data was consistent 
    data = data >> (32-len) << (32-len);
    
	prefix = data >> (32 - len);    //take first 'len' bits (MSB bits)
    p1 = 0, p2 = 0;
	if ( len > stride1 )
	{
		p1 = (prefix << (32 - len)) >> (32 - stride1) ;
		nodeSet[p1].flag = 3;
		//nodeSet[p].PE = 0;
		new_len = len - stride1;
		//cout <<  "1st level node: nodeSet[" << p << "]  ";
		if ( new_len >  stride2 )
		{
		
			int flag;
			int temp1 = 0 ,temp2 = 0, z = 0;

			prefchunk = (prefix << (32 - new_len)) >> (32 - stride2); 
            p2 = bf2*p1 + prefchunk;
			nodeSet2[p2].flag = 3;
		    
			prefchunk = (prefix << (32 - new_len + stride2)) >> (32 - new_len + stride2);
			new_len = new_len - stride2;
			spltree[j] = new SplayTrie(-9999);
			
			for (i = 0; i <= j ; i++)
			{
				//if there already is a splaytree
				if (spltree[i]->triePointer == p2) 
				{
                   z = spltree[i]->num_nodes;
					//if there is already an ip_value like this one
					if (( spltree[i]->has(prefchunk)) )
					{
						//check for the length value
						for (temp1 = 0; temp1 < z; temp1++) 
						{
							for (temp2 = 0; temp2 < 8; temp2++)
								if(spltree[i]->Snode[temp1].length[temp2] == new_len)
								{
									flag = 1;
									spltree[i]->Snode[temp1].nexthop[temp2] = next_hop;
									break;
								} 
								//there is already the same length
								else if(spltree[i]->Snode[temp1].length[temp2] == 0 )
								{
									flag = 1;
									spltree[i]->Snode[temp1].length[temp2] = new_len;
									spltree[i]->Snode[temp1].nexthop[temp2] = next_hop;
									break;
								}
							if (flag == 1)  break;
						}//spltree[i]->Snod.length[k] = prefchunk;
					}
					else   //there isn't any Splay node for this value, so create one 
					{
						z = spltree[j]->num_nodes;
						spltree[i]->insert(prefchunk);
						spltree[i]->Snode[z].ip_value = prefchunk;
						spltree[i]->Snode[z].length[0] = new_len;
						spltree[i]->Snode[z].nexthop[0] = next_hop;
						flag = 1;
						break;
						
					}
				}
				if (flag == 1)  break;
			}
	
			if (flag != 1) //in the current Trie Leaf,no splaytree exists
			{
				z = spltree[j]->num_nodes;
				spltree[j]->triePointer = p2;
                spltree[j]->insert(prefchunk);
		        spltree[j]->Snode[z].ip_value = prefchunk;
		        spltree[j]->Snode[z].length[0] = new_len;
				spltree[j]->num_nodes++;
				j++;
			}

		}
		else if ( new_len == stride2)
		{
			prefchunk = (prefix << (32 - new_len )) >> (32 - stride2); 
            p2 = bf2*p2 + prefchunk;
			new_len = 0;
			if (nodeSet2[p2].flag != 3) //flag==3 bigger priority than flag==1
				nodeSet2[p2].flag = 1;
			if (nodeSet2[p2].PE > 0)
				nodeSet2[p2].PE = 0;
			
			nodeSet2[p2].nexthop = next_hop; 
			
			//cout << nodeSet2[p].nexthop << endl;
			//cout << "The " << prefchunk << " node of nodeSet2" << 
			//  " on the "<< (p-prefchunk)/bf2 <<" node of nodeSet " << endl;
		}
		else if (new_len < stride2)
		{
			//Prefix expansion
			prefchunk = (prefix << (32 - new_len)) >> (32 - stride2)  ;
			//cout << prefchunk << endl;
			p2 = bf2*p1 + prefchunk;
			//cout << p << endl;
			for (i = 0; i < (1 << (stride2 - new_len)); i++)
			{
				if (nodeSet2[p2 + i].flag == 0 
					|| (nodeSet2[p2 + i].PE > 0 && nodeSet2[p2 + i].PE < new_len))
				{
					nodeSet2[p2 + i].PE = new_len;
					nodeSet2[p2 + i].flag = 2;
					nodeSet2[p2 + i].nexthop = next_hop;
				}
                if ((nodeSet2[p2 + i].flag == 3 && nodeSet2[p2 + i].nexthop < 0)
				|| (nodeSet2[p2 + i].flag == 3 && (nodeSet2[p2 + i].PE > 0
				&& nodeSet2[p2 + i].PE < len )))
				{
					nodeSet2[p2 + i].PE = len;
					nodeSet2[p2 + i].nexthop = next_hop;
				}

			}
		    //cout << "The " << prefchunk << " node of nodeSet2" <<
			//  " on the "<< (p-prefchunk)/bf2 << " node in nodeSet" << endl;
		}
	}
    else if (len < stride1)
	{  
		// Prefix expansion
		p1 = (prefix << ( stride1 - len ));
		
		for (i = 0; i < (1 << ( stride1 - len )); i++)
		{
			if (nodeSet[p1 + i].flag == 0 || (nodeSet[p1 + i].PE > 0
				&& nodeSet[p1 + i].PE < len) )
			{
				nodeSet[p1 + i].PE = len;
				nodeSet[p1 + i].flag = 2;
				nodeSet[p1 + i].nexthop = next_hop;
			}
			if ((nodeSet[p1 + i].flag == 3 && nodeSet[p1 + i].nexthop < 0)
				|| (nodeSet[p1 + i].flag == 3 && (nodeSet[p1 + i].PE > 0
				&& nodeSet[p1 + i].PE < len )))
			{
				nodeSet[p1 + i].PE = len;
				nodeSet[p1 + i].nexthop = next_hop;
			}
			

		}
        //cout <<  " nodeSet[" << p1 << "] - nodeSet[" 
		//	<< p1+i << "]  :PE  " << endl;
		//return p1;
	}

	else if ( len == stride1 )
	{   
		prefchunk = (prefix << (32 - len)) >> (32 - stride1) ;
		p1 = prefchunk;
		
		if (nodeSet[p1].flag != 3) //flag==3 bigger priority than flag==1
			nodeSet[p1].flag = 1;
		if (nodeSet[p1].PE > 0)
			nodeSet[p1].PE = 0;

		nodeSet[p1].nexthop = next_hop; 
		
		new_len = len - stride1;
		//cout <<  "1st level node: nodeSet[" << p << "] " << endl;
	}
};

word Trie::iplookup(word address)
{
	register int p1,p2;
	int adrchunk;
	int  i, u, k, len;       // used for splay tree Nodes structure
	int flag = 0;        //used for splaytrees,if =1 denotes the lookup success 
	word nextadr = 0;
	adrchunk = address >> (32 - stride1) ;
	p1 = adrchunk;
	cout << p1 << endl;
	if (nodeSet[p1].flag == 3)
	{
		adrchunk = address << stride1 >> (32 - stride2);		
		p2 = bf2*p1 + adrchunk;
		if (nodeSet2[p2].nexthop > 0 && nodeSet2[p2].flag != 3)
		{
			nextadr = nodeSet2[p2].nexthop;
			//cout << nodeSet2[p2].nexthop << endl;
		}
		else if (nodeSet2[p2].nexthop <= 0 && nodeSet2[p2].flag != 3)
		{
			if (nodeSet[p1].nexthop > 0)
				nextadr = nodeSet[p1].nexthop;//den yparxei sto 2 level kapoio entry
		    else 
				nextadr = 0;
		}
        
		else if (nodeSet2[p2].flag == 3)
		{
			//search if there is a Splaytree structure beneath 
			//this trie node 
			for (int temp = 0; temp <= j; temp++)
				if (spltree[temp]->triePointer == p2)
					break;
			
			//search every instance of the adrchunk left beginning
			//from the longest length (thus the biggest value that can
			//be represented). If we find that value, we search the 
			//length array of that Splaytree Node.If it matches then
			//we have a match (whatsmore, a longest prefix match)
			//Else continue searching till the end  			
			for (i = 32 - stride1 - stride2; i > 0; i--)
			{
				adrchunk = address << (stride1 + stride2) >> (32 - i);
			    cout << adrchunk <<endl;
				len = i;
				  
				if (spltree[temp]->has(adrchunk))
				{
				   //search among the Snodes
					for (u = 0; u < spltree[temp]->num_nodes; u++)
					{
				       //if there's one with the appropiate value
						if (spltree[temp]->Snode[u].ip_value == adrchunk)
						{
					       //search the length array of each node
							for (k = 0; k < 8; k++)
							{
								if(spltree[temp]->Snode[u].length[k] == len)  
								{
								   nextadr = spltree[temp]->Snode[u].nexthop[k];
								   flag = 1;
								   break;
								}
							}
						}
					  //the ip_value field of a Snode is unique among the others 
                        if (flag == 1)    break;  
					}
				}
               //If we 've found a nextadr, then it surely is a LPM
				if (flag ==1)   break;
			}
			if (flag != 1)
			{
				if (nodeSet2[p2].nexthop > 0)
					nextadr = nodeSet2[p2].nexthop;
                else
				{
					//there isn't any 2 level entry, so we search the 1 level
					if (nodeSet[p1].nexthop > 0)
						nextadr = nodeSet[p1].nexthop;
					else 
						nextadr = 0;  //no entry at all -> lookup failure
				}
			}
			
		}
		
	}
    else if (nodeSet[p1].flag == 1 || nodeSet[p1].flag == 2)
		nextadr = nodeSet[p1].nexthop;
    else
		nextadr = 0;         //no entry at all -> lookup failure
	return nextadr;
}

/*
static int readentries(char *file_name,entry_t entry[], int maxsize)
{
   while (fscanf(in_file, "%lu%i%lu", &data, &len, &nexthop) != EOF) {
      if (nentries >= maxsize) return -1;
      entry[nentries] = (entry_t) malloc(sizeof(struct entryrec));
      // clear the 32-len last bits, this shouldn't be necessary
      //   if the routtable data was consistent 
      data = data >> (32-len) << (32-len);
      entry[nentries]->data = data;
      entry[nentries]->len = len;
      entry[nentries]->nexthop = nexthop;
      nentries++;
   }
   return nentries;
}

*/
// Write the first n lines of the array to stdout. 


int printEntries(char *input, char *output, int n) 
{
	int j ,len, nentries = 0;
    word ipdata, nexthop;
	FILE *instream, *outstream;
	
    if(!(instream = fopen(input, "rb"))) 
        perror(input);

	if(!(outstream = fopen(output, "wb"))) 
        perror(output);
     
    fprintf(outstream, "Ip Address(dec form, length, bin form --> nexthop)\n" );

	while(fscanf(instream, "%lu%i%lu", &ipdata, &len, &nexthop) != EOF)
    {
	    if (nentries > n)
		    break;

	    fprintf(outstream, "%lu %d   " , ipdata, len);
		
		for (j = 0; j < len; j++)
		{
			fprintf(outstream,"%d", ((ipdata<<j)>>31));
            
			if (j%8 == 7)  
				fprintf(outstream, " ");	
        }
        fprintf(outstream,"  --> %lu\n" ,nexthop);
        nentries++;
    } 
    fclose(outstream);    fclose( instream );
	return nentries;
}


//Insert entries from a file to the data structure. Each entry is 
//represented by three numbers: ipdata, len, and nexthop in decimal 
//notation, where ipdata is the ip address, len is the length of the 
//ip address to consider, and nexthop is the next-hop address
void Trie::insertEntries(char *filename, int n)
{
    word ipdata, nexthop;
	int len, nentries = 0;
    FILE *infile;

    if (!(infile = fopen(filename, "rb"))) 
        perror(filename);
        
	while (fscanf(infile,"%lu%i%lu", &ipdata, &len, &nexthop) != EOF) 
	{
		if (nentries > n || nentries > MAX_ADDRESS) 
			break;
		insert(ipdata, len, nexthop);
		//cout << prefix << " " << len << " " << next_hop << endl;
		nentries++;
	}
	fclose(infile);
}



int Trie::lookupEntries(char *traffic,char* results, int n)
{
    word ipdata, temp;
	int nentries = 0;
    FILE *instream,*outstream;

   	if (!(instream = fopen(traffic, "r"))) 
        perror(traffic);
    
	if (!(outstream = fopen(results, "w"))) 
        perror(results);
    

	while (fscanf(instream,"%lu", &ipdata ) != EOF) 
	{
		if (nentries > n || nentries > MAX_ADDRESS) 
			break;
		
		if ((temp = iplookup(ipdata)) > 0)
			fprintf(outstream, "Packet with ip address: %lu routed to %lu \n", ipdata, temp);
		else if (temp == 0)
			fprintf(outstream, "Packet with ip address: %lu  routed to DEFAULT\n", ipdata);

		nentries++;
	}
	fclose(outstream);      fclose(instream);
	return nentries;
}


void main(/*int argc, char * argv[]*/)
{
	
	Trie *m_root;
	m_root = new Trie(16,8);
	cout << m_root << endl;
	
	cout << " Elegxos (len > stride1+stride2) " << endl;
	
	//10010000 00000111 10100
	m_root->insert(2416418816,21,54);
	
	//10010000 00000111 0010100
	m_root->insert(2416388096,23,354564);
	
	//10010000 00000111 00101000 01111111
	cout << m_root->iplookup( 2416388223 ) << endl;	
    
	//10010000 00000111 10100001 11111100
	m_root->insert(2416419324, 23, 9999);	
    m_root->insert(2416419324, 21, 6666);	
	
	cout << m_root->iplookup( 2416419324 ) << endl;	
    
	m_root->insertEntries("aads.C", MAX_ADDRESS);
    m_root->lookupEntries( "aadstraf2","out_ip.txt",  MAX_ADDRESS);
	
	printEntries("aads.C","inserted_ips_format.txt", MAX_ADDRESS);
};
 