/*  main.cpp
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
#include "util.h"
#include "btree.h"
#include "assert.h"

using namespace std;
#define MAX (1<<15)

static void 
shuffle_array( int *a, int ub )
{
    int i,j;
    int temp;

    for( i=ub-1; i>0; i-- ){
        j = random_in_range(0, i+1);
        temp = a[i];
        a[i]=a[j];
        a[j]=temp;
    }

}

static void
create_array( int *a, int n, int max ){
    
    int in, im;

    im = 0;

    for (in = 0; in < max && im < n; ++in) {
        int rn = max - in;
        int rm = n - im;
        if ( random_in_range( 1,max ) % rn < rm)    
            /* Take it */
            a[im++] = in + 1; /* +1 since your range begins from 1 */
    }

    assert(im == n);

    shuffle_array( a, n );

}

static void
reset_array( int *a, int len )
{
    int i;

    for( i=0; i<len; i++ )
        a[i]=-1;
}

static int
tc_0( BTreeClass *bt ){

#define TC_0_TRIAL (8192)

    int i, n;
    int result;
    BTreeObjectClass obj;
    int keys[TC_0_TRIAL];

    result = 0;

    reset_array( &keys[0], TC_0_TRIAL );

    for( n=1; n<=TC_0_TRIAL; n++ ){
    
        create_array( &keys[0], n, MAX );

        for( i=0; i<n; i++ )
            bt->insert(keys[i]);
    
        cout<< n << " keys inserted"<<endl;
        cout<< "Max key = "<< bt->max_key()<< " \tMin key = "<< bt->min_key()<<endl;
        cout<< "Tree height = "<< bt->height()<<endl;
        
        //search test
        for( i=0; i<TC_0_TRIAL; i++ ){
            obj = bt->search(keys[i]);
            if( i<n && obj.read_node() )
                continue;
            if( i>=n && (!obj.read_node()) )
                continue;
            result = -1;
            break;
        }
       
        //deletion test 
        for( i=0; i<n; i++ )
            bt->remove(keys[i]);
        
        cout<< n << " keys removed"<<endl;

        reset_array( &keys[0], TC_0_TRIAL );

        if( result !=0 )
            break;
    }

    if( result != 0 )
        cout<<__func__<<" fails at the "<<n<<"-th trail"<<endl;

    return result;
}

/*
 * Test case 1: traverse the tree
 * */
static bool
tc_1( BTreeClass *bt ){

    #define TC_1_TRIAL (1024)

    int i;
    int result;
    int keys[TC_1_TRIAL];
    BTreeObjectClass obj;

    reset_array( &keys[0], TC_1_TRIAL );

    result = 0;

    create_array( &keys[0], TC_1_TRIAL, MAX );

    for( i=0; i<TC_1_TRIAL; i++ )
        bt->insert(keys[i]);
    
    cout<< TC_1_TRIAL << " keys inserted"<<endl;
    cout<< "Max key = "<< bt->max_key()<< " \tMin key = "<< bt->min_key()<<endl;
    cout<< "Tree height = "<< bt->height()<<endl;
    
    bt->traverse();

    //deletion all keys
    for( i=0; i<TC_1_TRIAL; i++ )
        bt->remove(keys[i]);
    
    cout<< TC_1_TRIAL << " keys removed"<<endl;
    bt->traverse();
    
    reset_array( &keys[0], TC_1_TRIAL );

    if( result != 0 )
        cout<<__func__<<" fails"<<endl;

    return result;
   
}

int main(void){
   
    int result;

    BTreeClass *bt = new BTreeClass(3);

    result = tc_0(bt);

    if( result == 0 )
        result = tc_1(bt);

    if( result!=0 )
        cout<<"Unit test fails"<<endl;
    
    delete bt;

}
