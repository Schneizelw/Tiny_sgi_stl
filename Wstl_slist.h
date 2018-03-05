#ifndef WSTL_SLIST_H_INCLUDED
#define WSTL_SLIST_H_INCLUDED
#include "Wstl_iterator.h"
#include "Wstl_alloc.h"
#include "Wstl_construct.h"
namespace W_STL{
    struct __slist_node_base{
        __slist_node_base* next;
    };
    template<class T>
    struct __slist_node : public __slist_node_base{
        T data;
    };

    //已知某一节点,插入新节点于其后
    inline __slist_node_base* __slist_make_link(__slist_node_base* prev_node,
                                                __slist_node_base* new_node)
    {
        new_node->next = prev_node->next;
        prev_node->next = new_node;
        return new_node;
    }

    //返回链表的长度
    inline size_t __slist_size(__slist_node_base* node)
    {
        size_t result = 0;
        for(;node!=0;node=node->next) ++result;
        return result;
    }

    //单向链表的迭代器的基本结构
    struct __slist_iterator_base{
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef forward_iterator_tag iterator_category;

        __slist_node_base* node;//指向节点的基本结构

        __slist_iterator_base(__slist_node_base* x):node(x){}

        void incr() { node = node->next; }

        bool operator==(const __slist_iterator_base& x) const {
            return node == x.node;
        }

        bool operator!=(const __slist_iterator_base& x) const {
            return node!= x.node;
        }
    };

    //单向链表的迭代器结构
    template<class T,class Ref,class Ptr>
    struct __slist_iterator : public __slist_iterator_base
    {
        typedef __slist_iterator<T,T&,T*> iterator;
        typedef __slist_iterator<T,const T&,const T*> const_iterator;
        typedef __slist_iterator<T,Ref,Ptr> self;

        typedef T value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef __slist_node<T> list_node;

        __slist_iterator(list_node* x): __slist_iterator_base(x) {}
        __slist_iterator(const iterator& x) : __slist_iterator_base(x.node) {}
        __slist_iterator(): __slist_iterator_base(0) {}

        reference operator*() const { return ((list_node*)node)->data;}
        pointer operator->() const { return &(operator*());}

        self& operator++()
        {
            incr();
            return *this;
        }
        self operator++(int)
        {
            self tmp = *this;
            incr();
            return tmp;
        }
    };

    template<class T,class Alloc=alloc>
    class slist
    {
    public:
        typedef T value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        typedef __slist_iterator<T,T&,T*> iterator;
        typedef __slist_iterator<T,const T&,const T*> const_iterator;

    private:
        typedef __slist_node<T> list_node;
        typedef __slist_node_base list_node_base;
        typedef __slist_iterator_base iterator_base;
        typedef simple_alloc<list_node,Alloc> list_node_allocator;

        static list_node* create_node(const value_type& x){
            list_node* node = list_node_allocator::allocate(); //配置空间
            construct(&node->data,x);  //构造元素
            node->next = 0;
            return node;
        }

        static void destroy_node(list_node* node){
            destroy(&node->data);  //析构元素
            list_node_allocator::deallocate(node);  //释放空间
        }

    private:
        list_node_base head;//实体，不是指针
    public:
        slist() { head.next = 0;}
        ~slist() {  }

    public:
        iterator begin() { return iterator((list_node*)head.next);}
        iterator end() {return iterator(0);}
        size_type size() const { return __slist_size(head.next);}
        bool empty() const {return head.next==0;}

        void swap(slist& L){
            list_node_base* tmp = head.next;
            head.next = L.head.next;
            L.head.next = tmp;
        }

    public:
        reference front() { return ((list_node*)head.next)->data;}
        void push_front(const value_type& x){
            __slist_make_link(&head,create_node(x));
        }
        //!注意没有push_back()
        void pop_front(){
            list_node* node = (list_node*) head.next;
            head.next = node->next;
            destroy_node(node);
        }

        void insert(const iterator& iter,value_type x){
            if(iter==0) return;
            list_node_base* tmp = &head;
            while(tmp->next!=iter.node) tmp = tmp->next;
            __slist_make_link(tmp,create_node(x));
        }

        iterator erase(const iterator& iter){
            list_node_base* tmp = &head;
            while(tmp->next!=iter.node) tmp = tmp->next;
            tmp->next = (iter.node)->next;
            destroy_node((list_node*)iter.node);
            return (list_node*)(tmp->next);
        }
    };

}


#endif // WSTL_SLIST_H_INCLUDED
