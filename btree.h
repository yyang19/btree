/*  btree.h
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
#ifndef __BTREE_BTREE_HEADER__
#define __BTREE_BTREE_HEADER__
#include "local.h"
#include "btreenode.h"
#include "btreeobject.h"

#define INVALID_KEY (~0)

class BTreeClass
{
    int t;
private:    
    BTreeNodeClass *root;
public:
    BTreeClass( int _t );
    ~BTreeClass();
    void traverse( void );
    void insert( int _key );
    BTreeObjectClass search( int key );
    void remove( int key );
    bool empty();
    int min_key();
    int max_key();
    int height();
private:
    static void _insert_nonfull( BTreeNodeClass *_x, int _key );
    static void _split_child( BTreeNodeClass *_x, int c );
    static BTreeObjectClass _search_subtree( BTreeNodeClass *r, int key );
    static BTreeNodeClass * _pre_descend_child( BTreeNodeClass *parent, int idx, void *ptr );
    static void _descend( BTreeNodeClass *r, int key, void *ptr );
    static int  _descend_right( BTreeNodeClass *node, void *ptr );
    static int  _descend_left( BTreeNodeClass *node, void *ptr );
    static void  _remove_from_leaf( BTreeNodeClass *node, int idx );
    static void  _remove_from_non_leaf( BTreeNodeClass *node, int idx, void *ptr );
    static void _move_key( int key, BTreeNodeClass *src, BTreeNodeClass *dest, int src_ptr_shift, int dest_ptr_shift );
    static void _merge_node( BTreeNodeClass *x, BTreeNodeClass *y);
    static void _node_key_shift_left( BTreeNodeClass *x, int index, int ptr_shift);
    static void _node_key_shift_right( BTreeNodeClass *x, int index, int ptr_shift );
    static int _minimum( BTreeNodeClass *r );
    static int _maximum( BTreeNodeClass *r );

    //static BTreeNodeClass * _predecessor( BTreeNodeClass *node, int key );
    //static BTreeNodeClass * _successor( BTreeNodeClass *node, int key );
};

#endif
