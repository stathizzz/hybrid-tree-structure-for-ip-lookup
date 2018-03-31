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
#include <stdio.h>          //for file manipulation
#include "splaytree.h"
#include "Nexthop.h"

#define MAX_ADDRESS 100000 
#define ADRSIZE 32         // the  number of bits in an address
#define SPLAYBITS  8       //we use the last (LSB) 8 bits of an Ip adr for the Splaytrees  
#define EMPTY_STRUCTURE  -9999  

typedef unsigned int word;

//Construction of a node of the multitrie
//an example of the structure of the 16-bit value stored in every node

//   <-----pointer to nexthop------> <-------the PE--------> <-flag->
//   ________________________________________________________________
//  |___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|____|

#define FLAG(nod)          ((nod) & 0x0003)
#define SETFLAG(nod,value) ((nod) = (((nod) & (~0x0003)) | short(value)) )
#define PE(nod)            (((nod) >> 2) & 0x003f)
#define SETPE(nod,value)   ((nod) = (((nod)& 0xff03) | ((short(value) << 2) & (~0xff03))))
#define ADR(nod)           (((nod) >> 8) & 0x00ff)
#define SETADR(nod,value)  ((nod) = ((nod) & 0x00ff) | ((short(value) << 8) & 0xff00))


struct trieNode 
{
	//The 16-bit value stored in each trieNode corresponds to 3 different values, represented
	//on the scheme above (pointer to nexthop, PE, flag).Small analysis of each follows:
	//flag:
    //flag showing the entry status of current node (there is an  
	//entry iff the path of the entry in the trie uses current node) 
	//0:no entry in current node  
    //1:Prefix Expanded entry exists    
	//2:Non-Prefix Expanded entry exists & the length of the entry matches node's stride
	//3:Non-Prefix Expanded entry exists & the length of the entry doesnt match node's stride
	//PE:
    //if entry is Prefix Expanded, PE -> length of the PE entry
	//if entry is NPE point -> PE == 0  
  	//Pointer to nexthop:
	//gives the location of the nexthop value stored in the Next-Hop Table.Because of its
	//8-bit value, it can store up to 255 different nexthop addresses, a number twice as
	//big as the number of Next Hop addresses in today's backbone routers

    unsigned short DATA;
	
    trieNode() {
	    
		DATA = 0;      
	}
};

//SplayTrie extends the Splaytree structure, holding the pointer to the trie node attached,
//the # of nodes in the Splaytree and the SplaytreeNode structure 
class SplayTrie : public SplayTree<int> 
{
public:
	
	//A splayTree node must compare the lengths of the ip_value   
	struct splaytrieNode {
		int length[SPLAYBITS];       //we dont care much about issuing pointers cause the 
		int nexthop[SPLAYBITS];      //arrays are very small
		int ip_value;
		
	};
	
	// max number of nodes in a splay tree is 2^8 - 1
	splaytrieNode Snode[(1 << SPLAYBITS) -1];  //pointer may be potentially used  
	unsigned int triePointer;    //pointer to the TrieNode the Splaytree is attached
	unsigned int num_nodes;
	
	SplayTrie(int empty) : SplayTree<int>(EMPTY_STRUCTURE)
	{	
		/*for (int k=0;k<8;k++)            //The already defined arrrays dont require 
			for (int l=0;l<8;l++){         //initialization 
			Snode[k].length[l] = 0;
			Snode[k].nexthop[l] = 0;
		    Snode[k].ip_value = 0;
		}*/
		num_nodes = 0;
	  	triePointer = empty;
        
	}
};

//main class
class MultiTrie
{
 public:
	int stride1;	// the stride of the root
	int stride2;    // the second-level stride
	unsigned int bf1;// # nodes in first-level,branching factor
	unsigned int bf2; //# nodes in second-level for each one-level node 
	
	trieNode  *nodeSet;		  //base of array of Nodes
	trieNode  *nodeSet2;      //second level array of nodes
	SplayTrie *spltree[];     
	
    
	MultiTrie(int k1 , int k2 )  //k1 1st level stride1, k2 2nd level stride 
	{		
		stride1 = k1;
		stride2 = k2;
		bf1 = 1<<stride1;
		bf2 = 1<<stride2;
		
		//initialization of arrays 
    	nodeSet = new trieNode[bf1];	  //array of 1st level trieNodes
	    nodeSet2 = new trieNode[bf1*bf2]; //array of 2nd level trieNodes			
	}


	~MultiTrie() 
	{ 
		delete []  nodeSet, nodeSet2; 
	}

	word iplookup( word );
    void insertEntries(char *, int);
    int lookupEntries(char *,char *, int);
	void insert( word , int, word );
};

//counter which shows how many Splaytrie structures have been created,
//regarding the whole structure
static unsigned int j = 0;  

//Insert the ip data of length len (prefix address) and nexthop address next_hop  
void MultiTrie::insert(word data, int len, word next_hop) 
{   
   
	register unsigned int p1,p2;  //used for the trie nodes 
	int new_len, prefchunk;
    unsigned int prefix, i;
     		
	//Insert the nexthop value to the nexthoptable and give back pointer to it. 
	word NHpointer = insertNHtable(nexthoptable, next_hop);
	//cout << nexthoptable[NHpointer] <<  "  " << data << endl;
	//word NHpointer = findpointer(next_hop, nexthoptable); 	
	
	// clear the 32-len last bits, this shouldn't be necessary
    // if the routtable data was consistent 
    data = data >> (32-len) << (32-len);
    prefix = data >> (32 - len);    //take first 'len' bits (MSB bits)
    
	p1 = 0, p2 = 0;
	if ( len > stride1 )
	{
		p1 = (prefix << (32 - len)) >> (32 - stride1) ;
		SETFLAG(nodeSet[p1].DATA,3);
		new_len = len - stride1;
		//cout <<  "1st level node: nodeSet[" << p1 << "]  ";
		if ( new_len >  stride2 ) 
		{
			int flag; //used to denote if a desired (ipdata,len) address is found in the splaytrees
			int temp1 = 0 ,temp2 = 0, z = 0;
            //prefchunk in each case shows the desired bitfield taken on account
			prefchunk = (prefix << (32 - new_len)) >> (32 - stride2); 
            p2 = bf2*p1 + prefchunk;
			SETFLAG(nodeSet2[p2].DATA,3);
		    //prefchunk in this case indicates the bitfield searched by a splaytree
			prefchunk = (prefix << (32 - new_len + stride2)) >> (32 - new_len + stride2);
			new_len = new_len - stride2;   //length of the new bitfield
			spltree[j] = new SplayTrie(EMPTY_STRUCTURE); 
			
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
						}
					}
					else   //there isn't any Splay node for this value, so create one 
					{
						z = spltree[i]->num_nodes;
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
		else if ( new_len == stride2) //ip address matches exactly to 2nd level trie node
		{
			prefchunk = (prefix << (32 - new_len )) >> (32 - stride2); 
            p2 = bf2*p2 + prefchunk;
			new_len = 0;

            //flag==3 bigger priority than flag==2
			if (FLAG(nodeSet2[p2].DATA) != 3)
				SETFLAG(nodeSet2[p2].DATA,2);
			if (PE(nodeSet2[p2].DATA) > 0)
				SETPE(nodeSet2[p2].DATA,0);
			
			SETADR(nodeSet2[p2].DATA,NHpointer); 
			
			//debugging stuff
			//cout << "The " << prefchunk << " node of nodeSet2" << 
			//" on the "<< (p2-prefchunk)/bf2 <<" node of nodeSet " << endl;
		}
		else if (new_len < stride2)  //Prefix expansion on the 2nd level
		{
			prefchunk = (prefix << (32 - new_len)) >> (32 - stride2);
			p2 = bf2*p1 + prefchunk;
			
			for (i = 0; i < unsigned(1 << (stride2 - new_len)); i++)
			{
				//check the entry.If there wasn't any entry or there was an expanded prefix 
				//one, check the length of that prefix dominating the node.If it's smaller,
				//the node gets dominated by the new expanded entry (LPM)
				if (FLAG(nodeSet2[p2 + i].DATA) == 0 
					|| (PE(nodeSet2[p2 + i].DATA) > 0 && PE(nodeSet2[p2 + i].DATA) < new_len))
				{
					SETPE(nodeSet2[p2 + i].DATA,new_len);
					SETFLAG(nodeSet2[p2 + i].DATA,1);
					SETADR(nodeSet2[p2 + i].DATA,NHpointer);
				}
                //if FLAG==3, the already inserted ip prefix address goes further down
				//to the trie.Besides, if the node doesnt have an ip value (that means
				//only flag = 0 or 3 dominated previously the node), we set the prefix
				//expansion parameters. Else check the PE.If smaller than the current
				//value, replace.
                if ((FLAG(nodeSet2[p2 + i].DATA) == 3 && ADR(nodeSet2[p2 + i].DATA) <= 0) 
				    || (FLAG(nodeSet2[p2 + i].DATA) == 3 && (PE(nodeSet2[p2 + i].DATA) > 0 
					&& PE(nodeSet2[p2 + i].DATA) < len )))
				{
					SETPE(nodeSet2[p2 + i].DATA,len);
					SETADR(nodeSet2[p2 + i].DATA,NHpointer);
				}
				//If flag == 2, the node already is dominated by a non expanded value,
				//so we dont change the node DATA)

			}
		    //debugging stuff
			//cout << "The " << prefchunk << " node of nodeSet2" <<
			//  " on the "<< (p2-prefchunk)/bf2 << " node in nodeSet" << endl;
		}
	}
    else if (len < stride1)   // Prefix expansion  on 1st level
	{  
		p1 = (prefix << ( stride1 - len ));
		
		for (i = 0; i < unsigned(1 << ( stride1 - len )); i++)
		{
			if (FLAG(nodeSet[p1 + i].DATA) == 0 
				|| (PE(nodeSet[p1 + i].DATA) > 0 && PE(nodeSet[p1 + i].DATA) < len) )
			{
				SETPE(nodeSet[p1 + i].DATA,len);
				SETFLAG(nodeSet[p1 + i].DATA,1);
				SETADR(nodeSet[p1 + i].DATA,NHpointer);
			}
			if ((FLAG(nodeSet[p1 + i].DATA) == 3 && ADR(nodeSet[p1 + i].DATA) <= 0)
				|| (FLAG(nodeSet[p1 + i].DATA) == 3 && (PE(nodeSet[p1 + i].DATA) > 0
				&& PE(nodeSet[p1 + i].DATA) < len )))
			{
				SETPE(nodeSet[p1 + i].DATA,len);
				SETADR(nodeSet[p1 + i].DATA,NHpointer);
			}
			
		}
        //debugging stuff
		//cout <<  " nodeSet[" << p1 << "] - nodeSet[" 
		//	<< p1+i << "]  :PE  " << endl;
		
	}

    //the ip prefix address matches exactly with 1st stride node
	else if ( len == stride1 )
	{   
		prefchunk = (prefix << (32 - len)) >> (32 - stride1) ;
		p1 = prefchunk;
		
		if (FLAG(nodeSet[p1].DATA) != 3) //flag==3 bigger priority than flag==2
			SETFLAG(nodeSet[p1].DATA,2);
		if (PE(nodeSet[p1].DATA) > 0)   //node is dominated by flag=2 address, so
			SETPE(nodeSet[p1].DATA,0);  //we (re)set PE=0

		SETADR(nodeSet[p1].DATA,NHpointer); 
		
		new_len = len - stride1;
		//debugging stuff
		//cout <<  "1st level node: nodeSet[" << p1 << "] " << endl;
	}
};

//The lookup routine, input is a 32-bit value. The routine searches
//through the prefixes inserted in the trie. If there are some matches,
//it returns the longest prefix one.   
word MultiTrie::iplookup(word address)
{
	register unsigned int p1,p2;
	int adrchunk;
	int  i, u, k, len;       // used for splay tree Nodes structure
	int flag = 0; //used for splaytree,if =1 denotes the lookup success in it 
	word nextadr = 0; 
	adrchunk = address >> (32 - stride1) ;
	p1 = adrchunk;
	
	if (FLAG(nodeSet[p1].DATA) == 3)
	{
		adrchunk = address << stride1 >> (32 - stride2);		
		p2 = bf2*p1 + adrchunk;
		if (ADR(nodeSet2[p2].DATA) > 0 && FLAG(nodeSet2[p2].DATA) != 3)
		{
			nextadr = ADR(nodeSet2[p2].DATA);
		}
		else if (ADR(nodeSet2[p2].DATA) <= 0 && FLAG(nodeSet2[p2].DATA) != 3)
		{
			if (ADR(nodeSet[p1].DATA) > 0)
				nextadr = ADR(nodeSet[p1].DATA);//there isnt any 2nd level entry
		    else 
				nextadr = 0;
		}
        
		else if (FLAG(nodeSet2[p2].DATA) == 3) //search beneath in the potential splaytrees
		{
			//search if there is a Splaytree structure beneath 
			//this trie node 
			for (unsigned int temp = 0; temp <= j; temp++)
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
			if (flag != 1) //no nextadr in splaytree, search backwards 
			{
				if (ADR(nodeSet2[p2].DATA) > 0)
					nextadr = ADR(nodeSet2[p2].DATA);
                else
				{
					//there isn't any 2 level entry, so we search the 1 level
					if (ADR(nodeSet[p1].DATA) > 0)
						nextadr = ADR(nodeSet[p1].DATA);
					else 
						nextadr = 0;  //no entry at all -> lookup failure
				}
			}		
		}
		
	}
    else if (FLAG(nodeSet[p1].DATA) == 2 || FLAG(nodeSet[p1].DATA) == 1)
		nextadr = ADR(nodeSet[p1].DATA);
    else
		nextadr = 0;         //no entry at all -> lookup failure
	
	//the splaytree structures hold the nexthop value itsself and not some pointer
	//to the nexthoptable.
	return (flag == 1) ? nextadr : nexthoptable[nextadr];
}


//Write the first n lines of the file input to file output, showing some attributes
//Returns the number of entries read. 
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
	    if (nentries > n - 1)
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
void MultiTrie::insertEntries(char *filename, int n)
{
    word ipdata, nexthop;
	int len, nentries = 0;
    FILE *infile;
    
    if (!(infile = fopen(filename, "r"))) 
        perror(filename);
        
	while (fscanf(infile,"%lu%i%lu", &ipdata, &len, &nexthop) != EOF) 
	{
		if (nentries > (n -1) || nentries > MAX_ADDRESS) 
			break;
		
		this->insert(ipdata, len, nexthop);

		nentries++;
	}
	fclose(infile);
};


//Lookup the first n entries (entry is the ip address - a 32bit value)  from the file 
//"traffic". Each line of the file should contain one ip address.Results are placed on 
//the output file "results" and declare for each entry if it gets routed (showing also
// the nexthop address related to the LPM) or not.
//Returns the number of entries prosecuted   
int MultiTrie::lookupEntries(char *traffic,char *results, int n)
{
    word ipdata, temp = -1;
	int nentries = 0;
    FILE *instream, *outstream;

   	if (!(instream = fopen(traffic, "rb"))) 
        perror(traffic);
    
	if (!(outstream = fopen(results, "wb"))) 
        perror(results);
    

	while(fscanf(instream,"%lu", &ipdata ) != EOF)  
	{
		if (nentries > (n - 1) || nentries > MAX_ADDRESS) 
			break;
		
		if ((temp = iplookup(ipdata)) > 0)
			fprintf(outstream, "Packet with ip address: %lu routed to %lu \n", ipdata, temp);
		else if (temp == 0)
			fprintf(outstream, "Packet with ip address: %lu routed to DEFAULT\n", ipdata);

		nentries++;
	}
	fclose(outstream);      fclose(instream);
	return nentries;
};

int main(/*int argc, char *argv[]*/)
{
	
	MultiTrie *trie;
	trie = new MultiTrie(16,8);
	cout << trie << endl;
	argv[0]= "multiTrie_beta.exe";
	argv[1] = "routing_file.txt";
    //int a =20;
	*(argv[2]) = 20;

	if (argc < 2 || argc > 5) 
	{
		cerr << "Usage: " << argv[0]  << " routing_file [n] [traffic_file] [n]" << endl;
	    return;
	}

	
	if (argc == 2) 
	{
		rhodian->insertEntries(argv[1], MAX_ADDRESS);
		printEntries(argv[1],"inserted_ips_format.txt", MAX_ADDRESS);
    } 
	else if (argc == 3)
    {
		if (isdigit(*(argv[2])))
		{
			rhodian->insertEntries(argv[1], *argv[2]);
		    printEntries(argv[1],"inserted_ips_format.txt", *argv[2]);
		}
		else if (istrstream(argv[2]))
		{
			rhodian->insertEntries(argv[1], MAX_ADDRESS);
            rhodian->lookupEntries(argv[2],"out_ip.txt",  MAX_ADDRESS);
			printEntries(argv[1],"inserted_ips_format.txt", MAX_ADDRESS);
		}
       
	}
	else if (argc == 4)
	{
		if (isdigit(*(argv[2])))
		{
			rhodian->insertEntries(argv[1], *argv[2]);
			rhodian->lookupEntries(argv[3],"out_ip.txt",  MAX_ADDRESS);
			printEntries(argv[1],"inserted_ips_format.txt", *argv[2]);
		}
		else if (istrstream(argv[2]))
		{
			rhodian->insertEntries(argv[1], MAX_ADDRESS);
            rhodian->lookupEntries(argv[2],"out_ip.txt", *argv[3]);
			printEntries(argv[1],"inserted_ips_format.txt", MAX_ADDRESS);
		}
       
    }
    else if (argc == 5)
	{
		rhodian->insertEntries(argv[1], *argv[2]);
		rhodian->lookupEntries(argv[3],"out_ip.txt", *argv[4]);
		printEntries(argv[1],"inserted_ips_format.txt",  *argv[2]);
	}
   

	trie->insertEntries("aads.C", MAX_ADDRESS);
	trie->lookupEntries("aadstraf2","out_ip.txt", MAX_ADDRESS);
	printEntries("aads.C","inserted_ips_format.txt", MAX_ADDRESS);
	
	return 0;

};
 
