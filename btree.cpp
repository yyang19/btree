/*  btree.cpp
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
#include "btree.h"
#include <assert.h>

BTreeClass::BTreeClass( int _t )
{
    t = _t;
    root = NULL;
}

BTreeClass::~BTreeClass( )
{
    cout<<"BTree object destroyed"<<endl;
}

void 
BTreeClass :: traverse(void)
{
    cout<< "Traverse the BTree (t=" << t << ")"<<endl;

    if( root )
        root->traverse();
    else
        cout<<" The tree is empty"<<endl;
}

bool
BTreeClass :: empty(void)
{
    return root==NULL;
}

int 
BTreeClass::min_key()
{
    return empty() ? -1 : _minimum( root );
}

int
BTreeClass::max_key()
{
    return empty() ? -1 : _maximum( root );
}

int
BTreeClass::height()
{
    BTreeNodeClass *x = root;
    int h = 0;

    while( x ){
        h++;
        x = x->C[0];
    }

    return h;
}

void BTreeClass::insert( int key )
{
    BTreeNodeClass *r;

    //cout<< "Insert key "<< key << endl;

    if( ! root )
        root = new BTreeNodeClass(t, 1);

    r = root;

    if( r->n==2*t-1 ){
        BTreeNodeClass *s = new BTreeNodeClass(t,0);
        root = s;
        s->C[0]=r;
        _split_child(s,0);
        _insert_nonfull(s,key);
    }
    else
        _insert_nonfull(r,key);
}

BTreeObjectClass
BTreeClass::search( int key )
{
    //may have dest check the key value 
    BTreeObjectClass obj;

    obj = _search_subtree(root, key);

    if( obj.node )
        obj.assign(key);

    return obj;
}

void 
BTreeClass :: remove( int key )
{
    //cout<< "Remove key "<< key << endl;
    return _descend( root, key, this );
}

BTreeObjectClass
BTreeClass::_search_subtree( BTreeNodeClass *node, int key )
{
    int i=0;
    BTreeObjectClass obj;

    while( i<node->n && key > node->keys[i] )
        i++;

    if( i<node->n && key == node->keys[i] ){
        obj.node = node;
        obj.index = i;
    }
    else if( node->leaf ){
        obj.node = NULL;
        obj.index = 0;
    }
    else{
        //DISK_READ(node->C[i]) 
        obj = _search_subtree( node->C[i], key );
    }

    return obj;
}

void 
BTreeClass::_split_child( BTreeNodeClass *node, int i ){
    
    int j;
    int t = node->t;

    BTreeNodeClass *y;
    BTreeNodeClass *z = new BTreeNodeClass(t);

    y = node->C[i];
    z->leaf = y->leaf;
    z->n = t-1;

    //move the largest t-1 kyes and t children of y into z
    for( j=0; j<z->n; j++ ){
        z->keys[j] = y->keys[t+j];
    }

    if( !y->leaf )
        for( j=0; j<=z->n; j++ )
            z->C[j] = y->C[t+j];

    y->n = t-1;

    //shift child pointers in node dest the right dest create a roon for z
    for( j=node->n+1; j>i; j-- )
        node->C[j] = node->C[j-1];
    
    node->C[i+1] = z;

    for( j=node->n; j>i; j-- )
        node->keys[j] = node->keys[j-1];

    node->keys[i] = y->keys[t-1];
    node->n ++;

    //DISK_WRITE(y);
    //DISK_WRITE(z);
    //DISK_WRITE(node);
}

void
BTreeClass :: _insert_nonfull( BTreeNodeClass *node, int _key )
{
    int i = node->n;
    int t = node->t;

    if( node->leaf ){
        while( i>=1 && _key<node->keys[i-1] ){
            node->keys[i] = node->keys[i-1];
            i--;
        }
        node->keys[i] = _key;
        node->n++;
        // DISK-WRITE(x);
    }
    else{
        while( i>=1 && _key<node->keys[i-1] )
            i--;
        i = i + 1;
        // DISK-READ(node.C[i]);
        if( node->C[i-1]->n == 2*t-1 ){
            _split_child( node, i-1 );
            if( _key>node->keys[i-1])
                i = i+1;
        }

        _insert_nonfull( node->C[i-1], _key );
    
    }

}

/* Function : BTreeClass::_pre_descend_child
 * Description : pre-process a minimal child node before descend into it. `
 * PARAM @parent : parent node
 * PARAM @idx : child pointer position in the parent node
 * PARAM @ptr : a pointer carrying tree scalars
 * return : child pointer
 */
BTreeNodeClass *
BTreeClass :: _pre_descend_child( BTreeNodeClass *parent, int idx, void *ptr )
{
    BTreeNodeClass *child = NULL;
    BTreeNodeClass *lsibling = NULL;
    BTreeNodeClass *rsibling = NULL;
    int parent_key, predecessor_key, successor_key;

    int t = ((BTreeClass *)ptr)->t;

    child = parent->C[idx]; 

    if( child->n>t-1 ) // not a minimal node
        return child;
    
    if ( idx == parent->n ) {   
        lsibling = parent->C[idx-1];
        rsibling = NULL; 
    }else if ( idx == 0 ) {
        lsibling = NULL;
        rsibling = parent->C[1];
    }else {
        lsibling = parent->C[idx - 1]; 
        rsibling = parent->C[idx + 1];
    }
    
    if( lsibling && lsibling->n > t-1 ){ 
        assert(idx>0);

        parent_key = parent->keys[idx-1];
        _move_key( parent_key, parent, child, 0, 1 ); //move 1-level down
        predecessor_key = lsibling->keys[lsibling->n-1];
        child->C[0] = lsibling->C[lsibling->n];
        _move_key( predecessor_key, lsibling, parent, 0, 0 );
    }
    else if( rsibling && rsibling->n > t-1 ){
        parent_key = parent->keys[idx];
        _move_key( parent_key, parent, child, 0, 1 );//move 1-level down
        successor_key = rsibling->keys[0];
        child->C[child->n] = rsibling->C[0];
        _move_key( successor_key, rsibling, parent, 1, 0 );
    }
    else if( lsibling && (lsibling->n == t-1) ){
        assert(idx>0);
        
        parent_key = parent->keys[idx-1];
        _move_key( parent_key, parent, lsibling, 1, 0 );//move 1-level down
        _merge_node( lsibling, child );
        parent->C[idx-1] = lsibling;
    
        child = lsibling;
     }
    else if( rsibling && (rsibling->n == t-1) ){
        
        parent_key = parent->keys[idx];
        _move_key( parent_key, parent, child, 1, 0 );//move 1-level down
        _merge_node( child, rsibling );
        parent->C[idx] = child;
    }
    else
        assert(0);

    return child;
}

/* Function : BTreeClass::_descend_right
 * Description : recursion to delete the right-most key in a subtree
 * PARAM @node : root of the subtree
 * PARAM @ptr : a pointer carrying tree scalars
 * return : deleted key value
 */

int 
BTreeClass :: _descend_right( BTreeNodeClass *node, void *ptr )
{
    int idx;
    int pred_key;

    BTreeNodeClass *child = NULL;

    int t = ((BTreeClass *)ptr)->t;

    if( node->leaf ){
        assert( node->n >= t ); //at least t keys
        idx = node->n-1;
        pred_key = node->keys[idx];
        _remove_from_leaf( node, idx );
    }
    else{
        child = _pre_descend_child( node, node->n, ptr );        
        pred_key = _descend_right( child, ptr );
    }

    return pred_key;
}

/* Function : BTreeClass::_descend_left
 * Description : recursion to delete the left-most key in a subtree
 * PARAM @node : root of the subtree
 * PARAM @ptr : a pointer carrying tree scalars
 * return : deleted key value
 */
int 
BTreeClass :: _descend_left( BTreeNodeClass *node, void *ptr )
{
    int idx;
    int succ_key;

    BTreeNodeClass *child = NULL;

    int t = ((BTreeClass *)ptr)->t;

    if( node->leaf ){
        assert( node->n >= t ); //at least t keys
        idx = 0;
        succ_key = node->keys[idx];
        _remove_from_leaf( node, idx );
    }
    else{
        child = _pre_descend_child( node, 0, ptr );        
        succ_key = _descend_left( child, ptr );
    }

    return succ_key;
}

/* Function : BTreeClass::_descend 
 * Description : The deletion procedure is designed to guarantee one downward pass through the tree. 
 *               Therefore, this function is ensured that we never descend into a minimal node. 
 * PARAM @node : current node
 * PARAM @key  : key to delete
 * PARAM @ptr : a pointer carrying tree scalars
 * */
void
BTreeClass :: _descend( BTreeNodeClass *node, int key, void *ptr ){

    int idx;
    BTreeNodeClass **r = &((BTreeClass *)ptr)->root;
    BTreeNodeClass *child = NULL;
    
    assert( node );

    idx = node->search_in_node( key );
    
    if( idx<node->n && node->keys[idx]==key ){ //key found in the node
        if( node->leaf ){
            _remove_from_leaf( node, idx );
            if( node->n==0 && node==*r ){
                delete *r;
                *r=NULL;
            }
        }
        else
            _remove_from_non_leaf( node, idx, ptr );
        
        return;
    }

    //key not found in the node
    if( node->leaf ){
        cout<<" The key "<<key<< " does not exist in the tree\n";
        return;
    }

    child = _pre_descend_child( node, idx, ptr );

    _descend( child, key, ptr );

    if( node->n==0 && node==*r ){
        delete node;
        *r = child;
    }

    return;
}
/* Function: BTReeClass::_remove_from_leaf
 * Description: delete the idx-th key from the leaf node
 * Param @node : target node
 * Param @index: key idx
 * return: void
 * */
void
BTreeClass :: _remove_from_leaf( BTreeNodeClass *node, int idx )
{
    int i = idx;

    assert( node->leaf );
    assert( idx<node->n );

    while( i<node->n-1 ){
        node->keys[i] = node->keys[i+1];
        i++;
    }

    node->n--;

    return;
}

/* Function: BTReeClass::_remove_from_non_leaf
 * Description: delete the idx-th key from a non-leaf node
 * Param @node : target node
 * Param @index: key idx
 * PARAM @ptr : a pointer carrying tree scalars
 * return: void
 * */
void 
BTreeClass::_remove_from_non_leaf( BTreeNodeClass *node, int idx, void *ptr )
{    
    int key;
    int t = ((BTreeClass *)ptr)->t;

    BTreeNodeClass *pred_child, *succ_child;
    BTreeNodeClass **r = &((BTreeClass *)ptr)->root;
    
    pred_child = node->C[idx];
    succ_child = node->C[idx+1];

    if( pred_child->n > t-1 )
       node->keys[idx]= _descend_right( pred_child, ptr );
    else if( succ_child->n > t-1 )
       node->keys[idx]= _descend_left( succ_child, ptr );
    else{ 
       key = node->keys[idx];
       _move_key( key, node, pred_child, 1, 0 ); //move 1-level down
       _merge_node( pred_child, succ_child );
       node->C[idx] = pred_child;
       _descend( pred_child, key, ptr );
    }

    if( node->n==0 && node==*r ){
        *r = pred_child;
        delete node;
    }

    return;   
}

/*
 * Function used dest move a key src one node dest the other
 * @param node : pointer of the node
 * @param index : index of corresponding key separating the two children
 * @param src_ptr_shift : indicating if children pointers are shifted in the source node
 * @param dest_ptr_shift : indicating if children pointers are shifted in the destination node
 * @return void
 */

void 
BTreeClass::_move_key( int key, BTreeNodeClass *src, BTreeNodeClass *dest, int src_ptr_shift, int dest_ptr_shift ){

    int src_pos = 0;
    int dest_pos = 0;

    while( src_pos<src->n && key>src->keys[src_pos] )
        src_pos++;

    while( dest_pos<dest->n && key>dest->keys[dest_pos] )
        dest_pos++;

    _node_key_shift_right( dest, dest_pos, dest_ptr_shift );

    dest->keys[ dest_pos ] = src->keys[src_pos]; 

    _node_key_shift_left( src, src_pos, src_ptr_shift );

    //DISK_WRITE(src);
    //DISK_WRITE(dest);
}

/*
 * Function: BTreeClass::_merge_node
 * Description : merge sibling nodes. The right sibling node is freed up after merging.
 * @param left : left sibling node
 * @param right : right sibling node
 * @return void
 */

void 
BTreeClass::_merge_node( BTreeNodeClass *left, BTreeNodeClass *right ){

    int k;

    for( k=0; k<right->n; k++ ){
        left->keys[left->n+k] = right->keys[k];
        left->C[left->n+k] = right->C[k];
    }
    
    left->n += right->n;

    left->C[left->n] = right->C[right->n];
    
    delete right;

    //DISK_WRITE(left);
    //DISK_WRITE(right);
}

/* Function: BTReeClass::_node_key_shift_left
 * Description: left shift keys in a node
 * Param @node : node
 * Param @index: starting index
 * Param @ptr_shift : indicate if node pointers are shifted 
 * return: void
 * */
void 
BTreeClass::_node_key_shift_left( BTreeNodeClass *node, int index, int ptr_shift){
    
    int idx;

    for( idx=index; idx<node->n-1; idx++ ){
        node->keys[idx] = node->keys[idx+1];
        if( ptr_shift )
            node->C[idx]=node->C[idx+1];
    }

    if( ptr_shift )
        node->C[node->n-1]=node->C[node->n];
    node->n--;
}

/* Function: BTReeClass::_node_key_shift_right
 * Description: right shift keys in a node
 * Param @node : node
 * Param @index: starting index
 * Param @ptr_shift : indicate if node pointers are shifted 
 * return: void
 * */
void 
BTreeClass::_node_key_shift_right( BTreeNodeClass *node, int index, int ptr_shift ){
    
    int idx;

    assert( index<=node->n );

    for( idx=node->n; idx>index; idx-- ){
        node->keys[idx] = node->keys[idx-1];
        if( ptr_shift )
            node->C[idx+1]=node->C[idx]; 
    }

    node->keys[index] = INVALID_KEY;

    if( ptr_shift )
        node->C[idx+1]=node->C[idx]; 

    node->n++;
}

/* Function: BTreeClass::_minimum
 * Description: Find the minimum key value in a given subtree
 * Param @r: root of subtree
 * return: minimum key in subtree
 * */
int
BTreeClass::_minimum( BTreeNodeClass *r )
{
    BTreeNodeClass *node = r;

    while( !node->leaf )
        node = node->C[0];

    return node->keys[0];

}

/* Function: BTreeClass::_maximum
 * Description: Find the maximum key value in a given subtree
 * Param @r: root of subtree
 * return: maximum key in subtree
 * */
int
BTreeClass::_maximum( BTreeNodeClass *r )
{
    BTreeNodeClass *node = r;

    while( !node->leaf )
        node = node->C[node->n];

    return node->keys[node->n-1];

}

/* backup code
// find the predecessor of key {node,i}
BTreeNodeClass *
BTreeClass :: _predecessor( BTreeNodeClass *node, int key_pos )
{
    BTreeNodeClass *pred = NULL;

    if( !node )
        return pred;

    if( node->leaf )
        return pred;

    pred = node->C[key_pos];

    while( !pred->leaf )
        pred = pred->C[pred->n];

    return pred;
}

BTreeNodeClass *
BTreeClass :: _successor( BTreeNodeClass *node, int key_pos )
{
    BTreeNodeClass *succ = NULL;

    if( !node )
        return succ;

    if( node->leaf )
        return succ;

    succ = node->C[key_pos+1];

    while( !succ->leaf )
        succ = succ->C[0];

    return succ;
}

end of backup code */
