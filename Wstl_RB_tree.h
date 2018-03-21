#ifndef WSTL_RB_TREE_H_INCLUDED
#define WSTL_RB_TREE_H_INCLUDED
#include "Wstl_iterator.h"
#include "Wstl_alloc.h"
#include "Wstl_construct.h"
#include<utility>
#include<iostream>

namespace W_STL
{
    typedef bool __rb_tree_color_type;
    const __rb_tree_color_type __rb_tree_red = false;
    const __rb_tree_color_type __rb_tree_black = true;

    struct __rb_tree_node_base
    {
        typedef __rb_tree_color_type color_type;
        typedef __rb_tree_node_base* base_ptr;

        color_type color;   //节点颜色非黑即红
        base_ptr parent;    //rb树的许多操作，必须知道父节点
        base_ptr left;      //指向左结点
        base_ptr right;     //指向右结点

        static base_ptr minimum(base_ptr x)
        {
            while(x->left!=0)x=x->right; //一直向左会找到最小值
            return x;
        }
        static base_ptr maximum(base_ptr x)
        {
            while(x->right!=0) x=x->right; //一直向右会找到最大值
            return x;
        }

    };

    template<class Value>
    struct __rb_tree_node : public __rb_tree_node_base
    {
        typedef __rb_tree_node<Value>* link_type;
        Value value_field; //结点值
    };

    struct __rb_tree_base_iterator
    {
        typedef __rb_tree_node_base::base_ptr base_ptr;
        typedef ptrdiff_t difference_type;
        typedef bidirectional_iterator_tag iterator_category;

        base_ptr node;

        void increment()
        {
            if(node->right != 0) {  //如果有右子节点 情况1
                node = node->right;
                while(node->left!=0)
                    node = node->left;
            }else{                  //情况2 没有右子节点
                base_ptr y = node->parent;   //找出父节点
                while(node==y->right){      //如果现行行本身是个右子节点
                    node = y;               //一直上溯
                    y = y->parent;
                }
                if(node->right != y){
                    node = y;   //情况3找到了一个合适的值
                }else {
                    //情况4 此时node为答案
                }

            }
        }


        void decrement()
        {
            //node为header时 情况1
            if(node->color == __rb_tree_red && node->parent->parent == node)
                node = node->right;//直接指向最大值
            else if(node->left != 0){ //情况2
                base_ptr y = node->left;
                while(y->right !=0)
                    y = y->right;
                node = y;
            }else{  //情况3
                base_ptr y = node->parent;
                while( node == y->left){
                    node = y;
                    y = y->parent;
                }
                node = y;
            }
        }
        inline bool operator==(const __rb_tree_base_iterator& right) const {
            return node==right.node;
        }
        inline bool operator!=(const __rb_tree_base_iterator& right) const {
            return node!=right.node;
        }


    };

    template<class Value,class Ref,class Ptr>
    struct __rb_tree_iterator: public __rb_tree_base_iterator
    {
        typedef Value value_type;
        typedef Ref reference;
        typedef Ptr pointer;
        typedef __rb_tree_iterator<Value,Value*,Value&> iterator;
        typedef __rb_tree_iterator<Value,const Value*,const Value&> const_iterator;
        typedef __rb_tree_iterator<Value,Ref,Ptr> self;
        typedef __rb_tree_node<Value>* link_type;

        __rb_tree_iterator() {}
        __rb_tree_iterator(link_type x) {node = x; }
        __rb_tree_iterator(const iterator& it){ node = it.node; }

        reference operator*() const { return link_type(node)->value_field; }
        reference operator->() const {return &(operator*());}

        self& operator++() {
            increment();
            return *this;
        }
        self operator++(int){
            self tmp = *this;
            increment();
            return tmp;
        }

        self& operator--(){
            decrement();
            return *this;
        }
        self operator--(int){
            self tmp = *this;
            decrement();
            return *this;
        }


    };

    template<class Key,class Value,class KeyOfValue,class Compare,class Alloc = alloc>
    class rb_tree{
    protected:
        typedef void* void_pointer;
        typedef __rb_tree_node_base* base_ptr;
        typedef __rb_tree_node<Value> rb_tree_node;
        typedef simple_alloc<rb_tree_node,Alloc> rb_tree_node_allocator;
        typedef __rb_tree_color_type color_type;

    public:
        typedef Key key_value;
        typedef Value value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef rb_tree_node* link_type;

    protected:
        link_type get_node() { return rb_tree_node_allocator::allocate(); }
        void put_node(link_type p) { rb_tree_node_allocator::deallocate(p);}

        link_type create_node(const value_type& x){
            link_type tmp = get_node();
            construct(&tmp->value_field,x);
            return tmp;
        }

        void destroy_node(link_type p){
            destroy(&p->value_field);
            put_node(p);
        }

        link_type clone_node(link_type x){
            link_type tmp= create_node(x->value_field);
            tmp->color = x->color;
            tmp->left = 0;
            tmp->right = 0;
            return tmp;
        }



    protected:
        //RB_tree只以三笔数据表现
        size_type node_count;
        link_type header;
        Compare key_compare; //节点间的键值大小比较准则。
        //以下3个成员以便获得header的成员
        link_type& root() const { return (link_type&)header->parent; }
        link_type& leftmost() const { return (link_type&)header->left; }
        link_type& rightmost() const { return (link_type&)header->right; }

        static link_type& left(link_type x){ return (link_type&)(x->left); }
        static link_type& right(link_type x){ return (link_type&)(x->right); }
        static link_type& parent(link_type x){ return (link_type&)(x->parent); }
        static reference value(link_type x){ return x->value_field; }
        static const Key& key(link_type x){ return KeyOfValue()(value(x));}
        static color_type& color(link_type x){ return (color_type&)(x->color); }

        static link_type& left(base_ptr x){ return (link_type&)(x->left); }
        static link_type& right(base_ptr x){ return (link_type&)(x->right); }
        static link_type& parent(base_ptr x){ return (link_type&)(x->parent); }
        static reference value(base_ptr x){ return ((link_type)x)->value_field; }
        static const Key& key(base_ptr x){ return KeyOfValue()(value((link_type)x)); }
        static color_type& color(base_ptr x){ return (color_type&)(link_type(x)->color); }

        static link_type maximum(link_type x){
            return (link_type) __rb_tree_node_base::maximum(x);
        }
        static link_type minimum(link_type x){
            return (link_type) __rb_tree_node_base::minimum(x);
        }

    public:
        typedef __rb_tree_iterator<value_type,reference,pointer> iterator;

    private:
        iterator __insert(base_ptr x,base_ptr y,const value_type& v);
        link_type copy(link_type x, link_type p);
        void __erase(link_type x);
        void init(){
            header = get_node();    //产生一个空间节点，令header指向它
            color(header) = __rb_tree_red; //令header为红色 用来区分
            root() = 0;
            //初始化左右节点指向自己
            leftmost() = header;
            rightmost() = header;
        }

    public:
        rb_tree(const Compare& comp = Compare())
        :node_count(0),key_compare(comp) { init(); }

        ~rb_tree(){
            // clear();
            put_node(header);
        }
        rb_tree<Key,Value,KeyOfValue,Compare,Alloc>&
        operator=(const rb_tree<Key,Value,KeyOfValue,Compare,Alloc>& x);

    public:
        Compare key_comp() const { return key_compare; }
        iterator begin() { return leftmost();}
        iterator end() { return header; }
        bool empty() const { return node_count==0; }
        size_type size() const { return node_count; }
        size_type max_size() const { return size_type(-1); }
    public:
        std::pair<iterator,bool> insert_unique(const value_type& x);
        iterator insert_equal(const value_type& x);

    };

    inline void
    __rb_tree_rotate_left(__rb_tree_node_base* x,__rb_tree_node_base*& root){
        //x为旋转点
        cout << "__rb_tree_rotate_left " <<endl;
        __rb_tree_node_base* y = x->right; //y为旋转点的右子结点
        x->right = y->left;
        if(y->left!=0) y->left->parent = x;
        y->parent = x->parent;
        if(x==root) root = y;//x为根节点
        else if(x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

     inline void
    __rb_tree_rotate_right(__rb_tree_node_base* x,__rb_tree_node_base*& root){
        cout << "__rb_tree_rotate_right" <<endl;
        __rb_tree_node_base* y = x->left;
        x->left = y->right;
        if(y->right!=0) y->right->parent = x;
        y->parent = x->parent;
        if(x==root) root = y;
        else if(x->parent->right == x) x->parent->right = y;
        else x->parent->left = y;
        y->right = x;
        x->parent = y;

    }

    inline void
    __rb_tree_rebalance(__rb_tree_node_base* x,__rb_tree_node_base*& root)
    {
        cout << " __rb_tree_rebalance " <<endl;
        x->color = __rb_tree_red;
        //插入点不为root且父节点为红
        while(x!=root && x->parent->color == __rb_tree_red){
            //父节点为祖父结点的左子结点
            if(x->parent == x->parent->parent->left){
                __rb_tree_node_base* y = x->parent->parent->right;//y为伯父结点
                //伯父结点存在且伯父节点颜色为红
                if(y && y->color==__rb_tree_red){
                    x->parent->color = __rb_tree_black;
                    y->color = __rb_tree_black;
                    x->parent->parent->color = __rb_tree_red;
                    x = x->parent->parent; //继续往上准备检查
                }else{ //伯父节点不存在或者伯父节点为黑
                    if(x == x->parent->right){
                        //内侧插入的第一步left旋转
                        x = x->parent;
                        __rb_tree_rotate_left(x,root);
                    }
                    //内侧插入的第二步 或者 外侧插入的第一步right旋转
                    x->parent->color = __rb_tree_black;
                    x->parent->parent->color = __rb_tree_red;
                    __rb_tree_rotate_right(x->parent->parent,root);
                }
            }//if
            else{ //父节点为祖父结点的右子结点
                __rb_tree_node_base* y=x->parent->parent->left;//y为伯父结点
                 //伯父结点存在且伯父节点颜色为红
                if(y && y->color==__rb_tree_red){
                    x->parent->color = __rb_tree_black;
                    y->color = __rb_tree_black;
                    x->parent->parent->color = __rb_tree_red;
                    x= x->parent->parent; //继续往上准备检查
                }else{

                    if(x = x->parent->left){
                        //内侧插入//内侧插入的第二步 或者 外侧插入的第一步右旋转的第一步right旋转
                        x = x->parent;
                        __rb_tree_rotate_right(x,root);
                    }
                    //内侧插入的第二步 或者 外侧插入的第一步left旋转
                    x->parent->color = __rb_tree_black;
                    x->parent->parent->color = __rb_tree_red;
                    __rb_tree_rotate_left(x->parent->parent,root);
                }

            }
        }//while
        root->color = __rb_tree_black;
    }

    template<class Key,class Value,class KeyOfValue,class Compare,class Alloc>
    typename rb_tree<Key,Value,KeyOfValue,Compare,Alloc>::iterator
    rb_tree<Key,Value,KeyOfValue,Compare,Alloc>::
    __insert(base_ptr x_,base_ptr y_,const Value& v)
    {
        link_type x = (link_type)x_;
        link_type y = (link_type)y_;
        link_type z;
        //cout << KeyOfValue()(v) << "|| "<<key(y) <<endl;
        if(y==header || x!=0 || key_compare(KeyOfValue()(v),key(y))){
            cout << "insert left" <<endl;
            z = create_node(v);
            left(y) = z;
            if(y==header){
                root() = z;
                rightmost() = z;
            }
            else if(y == leftmost())
                leftmost() = z;
        }else{
            z = create_node(v);
            right(y) = z;
            if(y == rightmost())
                rightmost() = z;
        }
        //设置新结点的指针
        parent(z) = y;
        left(z) = 0;
        right(z) = 0;
        __rb_tree_rebalance(z,header->parent);
        ++node_count;
        return iterator(z);
    }


    template<class Key,class Value,class KeyOfValue,class Compare,class Alloc>
    typename rb_tree<Key,Value,KeyOfValue,Compare,Alloc>::iterator
    rb_tree<Key,Value,KeyOfValue,Compare,Alloc>::insert_equal(const Value& v)
    {
        link_type y = header;
        link_type x = root();
        while(x != 0){
            y = x;
            x = key_compare(KeyOfValue()(v),key(x)) ? left(x): right(x);
        }
        return __insert(x,y,v);

    }

    template<class Key,class Value,class KeyOfValue,class Compare,class Alloc>
    std::pair<typename rb_tree<Key,Value,KeyOfValue,Compare,Alloc>::iterator,bool>
    rb_tree<Key,Value,KeyOfValue,Compare,Alloc>::insert_unique(const Value& v)
    {
        link_type y = header;
        link_type x = root();
        bool comp = true;
        while(x!=0){
            y = x;
            //默认情况下 KeyOfValue()(v) < key(x) 成立返回true
            comp = key_compare(KeyOfValue()(v),key(x));
            x = comp? left(x):right(x);
        }
        //此时离开while循环后，y所指即插入点的父节点
        //调用了构造函数__rb_tree_iterator(link_type x) {node = x; }
        iterator j = iterator(y);
        //如果离开while时，节点是插在左侧
        if(comp){
            //j==begin() 意味着一直left 不会出现false的情况，即肯定不会相等。
            if(j == begin()) return std::pair<iterator,bool>(__insert(x,y,v),true);
            else --j;  //当前插入点为左侧，不可能相等，返回最后一次走right的结点。
        }
        //注意和上一个key_compare的参数调换了，是一种技巧，如果a<b为true,同时b<a为true 那么a!=b.
        //相反 如果a<b为false 而b<a也为false那么 a==b
        if(key_compare(key(j.node),KeyOfValue()(v)))
            return std::pair<iterator,bool>(__insert(x,y,v),true);

        //到这表示重复key值了
        return std::pair<iterator,bool>(j,false);

    }





}


#endif // WSTL_RB_TREE_H_INCLUDED
