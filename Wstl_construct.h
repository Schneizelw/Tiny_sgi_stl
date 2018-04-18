#ifndef WSTL_CONSTRUCT_H_INCLUDED
#define WSTL_CONSTRUCT_H_INCLUDED
#include<new>
#include "Wtype_traits.h"
#include "Wstl_iterator.h"
namespace W_STL
{
    template<typename T1,typename T2>
    inline void construct(T1* p , const T2& value)
    {
        new(p) T1(value); //!placement new
    }

    //!接受一个指针的destroy()版本
    template<typename T>
    inline void destroy(T* pointer)
    {
        pointer->~T(); //!调用T类型的析构函数
    }

    //!判断元素的数值型别(value type)是否有trival destructor
    template<typename ForwardIterator,typename T>
    inline void __destroy(ForwardIterator first,ForwardIterator last,T*)
    {
        typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor ;
        __destroy_aux(first,last,trivial_destructor());
    }

    //!接受两个迭代器的destroy()版本
    template<typename ForwardIterator>
    inline void destroy(ForwardIterator first,ForwardIterator last)
    {
        __destroy(first,last,value_type(first));
    }

    //!如果元素的数字型别(value type)有non-trival_destructor
    template<typename ForwardIterator>
    inline void __destroy_aux(ForwardIterator first,ForwardIterator last,__false_type)
    {
        for(;first<last;++first){
            destroy(&*first);//也就是调用析构函数
          //!^^^^^^^^^^^^^^  调用析构函数的版本
        }
    }

    //!如果元素的数值型别(value type)有non-trival destructor
    template<typename ForwardIterator>
    inline void __destroy_aux(ForwardIterator first,ForwardIterator last,__true_type){}

    //!特例化版本两个版本 char* wchar_t
    inline void destroy(char*,char*){}
    inline void destroy(wchar_t*,wchar_t*){}
    //执行更快 如果没有 会按照上面的destroy一步步执行 这样效率更高

}
#endif // WSTL_CONSTRUCT_H_INCLUDED








