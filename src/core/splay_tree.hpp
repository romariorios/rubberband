// Rubberband language
// Copyright (C) 2013  Luiz Romário Santana Rios <luizromario at gmail dot com>
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef SPLAY_TREE
#define SPLAY_TREE

namespace rbb
{

template <class Key, class T>
class splay_tree
{
public:
    splay_tree();
    virtual ~splay_tree();
    
    T at(const Key &k) const;
    void insert(const Key &k, const T &item);
    
    T insert_if_not_found(const Key &k, const T& item);
    
protected:
    class node
    {
    public:
        node(const T item) :
            item(item),
            left(0),
            right(0),
            parent(0)
        {}
        
        virtual ~node()
        {
            delete left;
            delete right;
        }
        
        T item;
        node *left, *right, *parent;
    } *p_root;
    
    virtual Key key_from_node(node *n) const = 0;
    
private:
    struct prev_and_cur { node *prev, *cur; };
    
    splay_tree<Key, T>::prev_and_cur find(const Key& k) const;
    void ins(const Key& k, prev_and_cur p, const T& item);
    void rotate_left(node *x);
    void rotate_right(node *x);
    void splay(node *x);
};

template <class Key, class T>
splay_tree<Key, T>::splay_tree() :
    p_root(0)
{}

template <class Key, class T>
splay_tree<Key, T>::~splay_tree()
{
    delete p_root;
}

template <class Key, class T>
T splay_tree<Key, T>::at(const Key& k) const
{
    splay_tree::node *n = find(k).cur;
    return n? n->item : T();
}

template <class Key, class T>
void splay_tree<Key, T>::insert(const Key& k, const T& item)
{
    splay_tree::prev_and_cur f = find(k);
    ins(k, f, item);
}

// Returns the found object if it finds something or item if it doesn't
template <class Key, class T>
T splay_tree<Key, T>::insert_if_not_found(const Key& k, const T& item)
{
    splay_tree::prev_and_cur p = find(k);
    if (p.cur)
        return p.cur->item;
    
    ins(k, p, item);
    return item;
}

template <class Key, class T>
typename splay_tree<Key, T>::prev_and_cur splay_tree<Key, T>::find(const Key& k) const
{
    splay_tree::node *prev = 0;
    splay_tree::node *cur = p_root;
    for (;; cur = k < key_from_node(cur)? cur->left : cur->right) {
        if (!cur || k == key_from_node(cur))
            break;
        
        prev = cur;
    }
    
    splay_tree<Key, T>::prev_and_cur p;
    p.prev = prev;
    p.cur = cur;
    return p;
}

template <class Key, class T>
void splay_tree<Key, T>::ins(const Key &k, prev_and_cur p, const T& item)
{
    if (p.cur) {
        p.cur->item = item;
        return;
    }
    
    if (!p.prev) {
        p_root = new splay_tree::node(item);
        return;
    }
    
    splay_tree::node *n = new splay_tree::node(item);
    n->parent = p.prev;
    n->item = item;
    
    if (k < key_from_node(p.prev))
        p.prev->left = n;
    else
        p.prev->right = n;
    
    splay(n);
}

template <class Key, class T>
void splay_tree<Key, T>::rotate_left(splay_tree<Key, T>::node *x)
{
    splay_tree::node *y = x->right;
    x->right = y->left;
    if (y->left)
        y->left->parent = x;
    y->parent = x->parent;
    if (!x->parent) {
        p_root = y;
    } else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

template <class Key, class T>
void splay_tree<Key, T>::rotate_right(splay_tree<Key, T>::node *x)
{
    splay_tree::node *y = x->left;
    x->left = y->right;
    if (y->right)
        y->right->parent = x;
    y->parent = x->parent;
    if (!x->parent) {
        p_root = y;
    } else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;
    y->right = x;
    x->parent = y;
}

template <class Key, class T>
void splay_tree<Key, T>::splay(splay_tree<Key, T>::node* x)
{
    while (x->parent) {
        if (!x->parent->parent) {
            if (x->parent->left == x)
                rotate_right(x->parent);
            else
                rotate_left(x->parent);
        } else if (x->parent->left == x && x->parent->parent->left == x->parent) {
            rotate_right(x->parent->parent);
            rotate_right(x->parent);
        } else if (x->parent->right == x && x->parent->parent->right == x->parent) {
            rotate_left(x->parent->parent);
            rotate_left(x->parent);
        } else if (x->parent->left == x && x->parent->parent->right == x->parent) {
            rotate_right(x->parent);
            rotate_left(x->parent);
        } else {
            rotate_left(x->parent);
            rotate_right(x->parent);
        }
    }
}

}

#endif
