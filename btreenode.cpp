/*  btreenode.cpp
 *  Author: Yue Yang ( yueyang2010@gmail.com )
 *
 *
* Copyright (c) 2015, Yue Yang ( yueyang2010@gmail.com )
*  * All rights reserved.
*  *
*  - Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*  Redistributions of source code must retain the above copyright notice,
*  this list of conditions and the following disclaimer.
*
*  - Redistributions in binary form must reproduce the above copyright
*  notice, this list of conditions and the following disclaimer in the
*  documentation and/or other materials provided with the distribution.
*
*  - Neither the name of Redis nor the names of its contributors may be used
*  to endorse or promote products derived from this software without
*  specific prior written permission.
*  
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
*  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*                          
*/
#include "local.h"
#include "btreenode.h"

//constructor
BTreeNodeClass::BTreeNodeClass( int _t )
{
    int i;

    t = _t;
    leaf = 0;

    // Allocate memory for maximum number of possible keys
    // and child pointers
    keys = new int[2*t-1];
    C = new BTreeNodeClass *[2*t];

    i=0;
    while( i<2*t-1 ){
        keys[i]=-1;
        C[i++]=NULL;
    }

    C[i]=NULL;

    n=0;    
}
BTreeNodeClass::BTreeNodeClass( int _t, int _leaf )
{
    int i;
    t = _t;
    leaf = _leaf;

    // Allocate memory for maximum number of possible keys
    // and child pointers
    keys = new int[2*t-1];
    C = new BTreeNodeClass *[2*t];
    
    i=0;
    while( i<2*t-1 ){
        keys[i]=-1;
        C[i++]=NULL;
    }

    C[i]=NULL;

    n=0;    
}

//destructor
BTreeNodeClass::~BTreeNodeClass()
{
    delete [] keys;
    delete [] C;
}

//Function to traverse all nodes in a subtree rooted with this node
void BTreeNodeClass :: traverse( void )
{
    int i;
    
    for( i=0; i<n; i++ ){
        if( !leaf )
            C[i]->traverse();
        cout<< " " << keys[i];
        if( leaf )
            cout<< "(leaf)";
        cout<<endl;

    }

    if( !leaf )
        C[i]->traverse();
}

/*  Function: BTreeNodeClass::search_in_node
 *  Description: A utility function that finds the index of the first key that is no less than the key
 *
 * */
int 
BTreeNodeClass::search_in_node( int key )
{
    int i=0;

    //find the first key no less than key
    while( i<n && keys[i]<key )
        ++i;

    return i;
}

#ifdef DEBUG
void
BTreeNodeClass::_dump( void ){

    int i = 0;
    cout<< n << " nodes : ";
    
    while(i<=n-1){
        cout<< keys[i++] <<" ";
    };

    cout<<endl;
}
#endif

/* backup code
// copy constructor
BTreeNodeClass::BTreeNodeClass( const BTreeNodeClass& node )
{

    t = node.t;
    leaf = node.leaf;
    n = node.n;

    // Allocate memory for maximum number of possible keys
    // and child pointers
    keys = new int[2*t-1];
    C = new BTreeNodeClass *[2*t-1];

    *keys = *node.keys;
    *C = *node.C;
    
}
backup code end */
