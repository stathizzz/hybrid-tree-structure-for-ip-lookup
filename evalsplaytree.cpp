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
#include "splaytree.h"

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>

void main()
{   
    SplayTree<int> tree(-9999);
    
    int option;
    int temp;
    bool isElement;

	while(1)
    {  
	   cout << "Ylopoihsh ybridikhs domis twn splaytrees, kata thn opoia mporoume na kanoume " << endl;
	   cout << "inserts kai deletes  sto dentro xwris na efarmozoume epanazygiseis (ta finds " << endl;
	   cout	<< "ginontai ypoxrewtika  me epanazygiseis). Epilekste ari8mo apo 0 - 8 me : " << endl;    
	   cout << "   0   :   eksodo apo to programma " << endl;
	   cout << "   1   :   insert (me epazygiseis) " << endl;
	   cout << "   2   :   remove (me epanazygiseis) " << endl;
	   cout << "   3   :   find " << endl;
	   cout << "   4   :   emfanish domis dentrou " << endl;
	   cout << "   5   :   ari8mos kombwn sto dentro " << endl;
	   cout << "   6   :   ypsos dentrou " << endl;
	   cout << "   7   :   insert xwris epazygiseis (opws sto BST) " << endl;
	   cout << "   8   :   remove xwris epanazygiseis (opws sto BST)" << endl;
	   cin >> option;
       if( option == 0 )
            break;
    
	   switch(option)
	   {
	   case 1:
	       cout << "Enter number to insert "<< endl;
		   cin >> temp;
		   isElement = tree.insert( temp );
		   tree.insert( temp );
		   if ( isElement == 0 )
			   cout << "Number " << temp << " already belongs to tree " << endl << endl;
		   else cout << "Number inserted successfully " << endl << endl;
		   break;
       case 2:
		   cout << "enter number to delete " << endl;
		   cin >> temp;
		   if ( tree.remove(temp) != 0 )
		   {
		       cout << temp << " deleted from tree " << endl << endl;
		   }
		   else 
			   cout << "Tree does not have "<< temp  << endl << endl;
		   break;
	   case 3:
		   cout << "Enter number to search " << endl;
		   cin >> temp;
		   if( tree.has(temp) )
		      cout << "The tree has " << temp << "." << endl << endl;
	       else
              cout << "The tree does not have " << temp << "." << endl << endl;
	       break;
       case 4:    
		   tree.printTree();
		   cout << endl << endl;
		   break;
	   case 5:
		 //  cout << " Number of nodes in the tree : " << tree.count() << endl << endl;		   
		   break;
	   case 6:
		   cout << " Tree depth : " << tree.depth() << endl << endl;		   
		   break;
	   case 7:
	       cout << "Enter number to insert "<< endl;
		   cin >> temp;
		   isElement = tree.insertWithoutSplay( temp );
		   if ( isElement == 0 )
               cout << "The tree already has " << temp << endl;
		   else 
		       cout << "Insert of " << temp << " completed successfully" << endl;
	       break;
       case 8:
		   cout << "enter number to delete " << endl;
		   cin >> temp;
		   tree.removeWithoutSplay(temp);
		   //{
		   //    cout << temp << " deleted from tree " << endl << endl;
		   //}
		   //else 
		//	   cout << "Tree does not have "<< temp  << endl << endl;
		   break;
	   default:    
           break;
	   }
   }


    
};
