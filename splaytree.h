#include <iostream.h>
#define ITEM_NOT_FOUND = -9999;
		
template <class T>
class BinaryNode
{
 public:

    BinaryNode( T new_data )  //constructor  
	{
        ip_data = new_data;
		parentnode = 0;
        leftnode   = 0;
        rightnode  = 0;
    };


    ~BinaryNode()           //destructor (recursively deletes children)
    {
        if( leftnode != 0 ) 
		{
            delete leftnode;
		}
		if( rightnode != 0 )
		{
            delete rightnode;			
		}
    }
    

    typedef BinaryNode<T> node;
	T ip_data;
    node* parentnode;
    node* leftnode;
    node* rightnode;
  
};


template <class T>
class SplayTree
{
 public:

	typedef BinaryNode<T> node;
	node* m_root;
    //int numberOfNodes;
    //int (*m_compare)(T& left, T& right); 
 

   
    SplayTree(T  notFound)  // ITEM_NOT_FOUND( notFound )
	{   
        m_root = new node(notFound);
        //numberOfNodes = 0;
        //m_compare = compare;
    }


    ~SplayTree()
    {
        if( m_root != 0 )
            delete m_root;
    }
 
    
  

    inline bool has( T new_data )
    {
        if( find( new_data ) == 0 )
            return false;
        return true;
    }

	void printTree ()
	{
		if ( m_root == 0 )
			cout << "Empty tree " << endl;
		else 
			printTree( m_root );
	}
	
	//inline int count()
	//{
    //    return numberOfNodes;
    //}


    inline int depth()
    {
        if( m_root == 0 )
            return -1;
        else depth(m_root);
	}

	
	inline bool removeWithoutSplay( T new_data )
    {
        node* x = findWithoutSplay( new_data );
        if( x == 0 )
            return 0;

        if( x->leftnode == 0 || x->rightnode == 0 )
            RemoveOneChild( x );
        else
            RemoveTwoChild( x );
       // numberOfNodes--;
        return 1;
    }
    

    inline bool insert ( const T new_data )
	{
		node* subTree = new node(new_data);
        
		//if ( m_root == 0 || m_root == new node(0))
		if ( m_root->ip_data == -9999
			|| m_root == new node(-9999))
		{
			m_root = new node( new_data ); 
            return 1;
		}
		splay( new_data );
		if ( m_root->ip_data == new_data )
			return 0;
		else if ( m_root->ip_data >  new_data )
		{
			subTree->leftnode = m_root->leftnode;
			m_root->leftnode = 0;
			subTree->rightnode = m_root;
			m_root = subTree;
		    return 1;
		}
		else 
		{
			subTree->rightnode = m_root->rightnode;
			m_root->rightnode = 0;
			subTree->leftnode = m_root;
			m_root = subTree;
		    return 1;
		}
	}

    inline bool remove( const T x ) 
	{   
		node* subTree = new node(0);
		
		if ( m_root == 0 )
			return 0;
		splay( x );
		if ( x != m_root->ip_data )
			return 0;
			
		if ( m_root->rightnode == 0 && m_root->leftnode == 0 )
		{
			m_root=0;
		}
		else if ( m_root->rightnode == 0 )
		{   
			m_root = m_root->leftnode;
		}
        else if ( m_root->leftnode == 0 )
		{
			m_root = m_root->rightnode;
		}
		else 
		{
		subTree = m_root->leftnode;
		m_root = m_root->rightnode;
		splay( x );
		m_root->leftnode = subTree; 
		}
		//delete subTree;
        return 1;
	}
	   

 public:
    
	node* find(const T  x )
        {
            if( m_root==0 )
                return 0;
            splay( x );
            if( m_root->ip_data != x )
                return 0;

            return m_root;
        }


	inline int depth( node* x )
	{
		int left_depth = -1;
		int right_depth = -1;
		if( x -> leftnode != 0 )
			left_depth = depth( x->leftnode );
		if( x -> rightnode != 0 )
			right_depth = depth( x->rightnode );
		if( left_depth > right_depth )
			return left_depth + 1;
		return right_depth + 1;		
	}

    
    inline void printTree( node*  x ) 
	{   
        
		cout << x->ip_data;
		cout << " [ " ;
		
		if ( x->leftnode != 0 ) 
			printTree(x->leftnode);

		if ( x->rightnode != 0 )
			printTree ( x->rightnode );
		
		cout << " ] ";
		
	}
	

	node* findWithoutSplay( T new_data )
    {
        node* current = m_root;
            
        while( current != 0 )
        {
            if( new_data == current->ip_data)
                return current;
            else if( new_data < current->ip_data )
                current = current->leftnode;
            else
                current = current->rightnode;
        }
        return current;
    }


    inline bool insertWithoutSplay( T new_data )
    {
        node* current = m_root;

        if( m_root == 0 )
		{
            m_root = new node( new_data );
            return 1;
		}
		else
        {
            while( current != 0 )
            {
                if( new_data < current->ip_data )
                {
                    if( current->leftnode == 0 )
                    {
                        current->leftnode = new node( new_data );
                        current->leftnode->parentnode = current;
                        current = 0;
                    }
                    else
                        current = current->leftnode;
                }
                else if(  new_data > current->ip_data )
                {
                    if( current->rightnode == 0 )
                    {
                        current->rightnode = new node( new_data );
                        current->rightnode->parentnode = current;
                        current = 0;
                    }
                    else
                        current = current->rightnode;
				}
				else 
					return 0;
            }
        return 1;
		}
    }

    inline void RemoveOneChild( node* x )
    {
        // briskei to paidi poy yfistatai,efoson yfistatai
        node* child = 0;
		const node* nullNode = new node(0);
        if( x->leftnode != 0 )
            child = x->leftnode;
        if( x->rightnode != 0 )
            child = x->rightnode;

        if( x->parentnode == 0 || x->parentnode == nullNode )
        {
            m_root = child;
        }
        else
        {
            if( x->parentnode != 0  &&  x->leftnode != 0 )
                x->parentnode->leftnode = child;
            else
                x->parentnode->rightnode = child;
        }

        // an to paidi tou parentnode yparxei kane reset.
        if( child != 0 )
            child->parentnode = x->parentnode;

        x->leftnode = 0;
        x->rightnode = 0;
        delete x;
		//delete nullNode;
    }

    inline void RemoveTwoChild( node* x )
    {   
		node* l;
		l = x->leftnode;
		while ( l->rightnode != 0 )
			l = l->rightnode;
        const node* nullNode = new node(0);
        // Yparxoun dyo periptwseis
        if( x->leftnode == l )  //to aristero paidi einai  to
        {                       //megalytero sto aristero ypodentro      
             l->rightnode = x->rightnode;
             l->rightnode->parentnode = l;
        }
        else     //to aristero paidi toy x exei deksi paidi
        {
            l->parentnode->rightnode = l->leftnode;
            if( l->leftnode != 0 )
                l->leftnode->parentnode = l->parentnode;

            // antikatastash toy x me to l
            l->leftnode = x->leftnode;
            l->leftnode->parentnode = l;
            l->rightnode = x->rightnode;
            l->rightnode->parentnode = l;
        }

        if( x->parentnode == 0 || x->parentnode == nullNode ) 
            m_root = l;
        else
        {
            if( x->parentnode != 0 &&  x->leftnode != 0 ) //isLeft(x) )
                x->parentnode->leftnode = l;
            else
                x->parentnode->rightnode = l;
        }

        l->parentnode = x->parentnode;
        x->leftnode = 0;
        x->rightnode = 0;
        delete x;
		//delete nullNode;
    }

 private:

    inline void splay(T new_data) 
	{
		node*  y;
		node* current;
		node* leftTreeMax = new node(0);
		node* rightTreeMin = new node(0);
		static node* header = new node(0);
    	leftTreeMax = header; 
		rightTreeMin = header;
        current = m_root;
		
		while (1) 
		{
			if ( new_data <  current->ip_data  ) 
			{
				if (current->leftnode == 0) 
					break;
				if ( new_data < current->leftnode->ip_data ) 
				{
					y = current->leftnode;              // rotate right
					current->leftnode = y->rightnode;
					y->rightnode = current;
					current = y;
					if (current->leftnode == 0) 
						break;
				}
				rightTreeMin->leftnode = current;       // link right
				rightTreeMin = current;
				current = current->leftnode;
			} 
			else if ( current->ip_data < new_data ) 
			{
				if (current->rightnode == 0) 
					break;
				if ( current->rightnode->ip_data < new_data ) 
				{
					y = current->rightnode;           // rotate left
					current->rightnode = y->leftnode;
					y->leftnode = current;
					current = y;
					if (current->rightnode == 0) 
						break;
				}
				leftTreeMax->rightnode = current;     // link left
				leftTreeMax = current;
				current = current->rightnode;
			}
			else if(  new_data == current->ip_data ) 				
				break;
				
		}
       	leftTreeMax->rightnode = current->leftnode;    // assemble 
		rightTreeMin->leftnode = current->rightnode;
		m_root=current;
		current->leftnode = header->rightnode;
		current->rightnode = header->leftnode;
	  	//delete leftTreeMax, rightTreeMin , header;
	}
}; 

