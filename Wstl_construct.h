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

    //!����һ��ָ���destroy()�汾
    template<typename T>
    inline void destroy(T* pointer)
    {
        pointer->~T(); //!����T���͵���������
    }

    //!�ж�Ԫ�ص���ֵ�ͱ�(value type)�Ƿ���trival destructor
    template<typename ForwardIterator,typename T>
    inline void __destroy(ForwardIterator first,ForwardIterator last,T*)
    {
        typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor ;
        __destroy_aux(first,last,trivial_destructor());
    }

    //!����������������destroy()�汾
    template<typename ForwardIterator>
    inline void destroy(ForwardIterator first,ForwardIterator last)
    {
        __destroy(first,last,value_type(first));
    }

    //!���Ԫ�ص������ͱ�(value type)��non-trival_destructor
    template<typename ForwardIterator>
    inline void __destroy_aux(ForwardIterator first,ForwardIterator last,__false_type)
    {
        for(;first<last;++first){
            destroy(&*first);//Ҳ���ǵ�����������
          //!^^^^^^^^^^^^^^  �������������İ汾
        }
    }

    //!���Ԫ�ص���ֵ�ͱ�(value type)��non-trival destructor
    template<typename ForwardIterator>
    inline void __destroy_aux(ForwardIterator first,ForwardIterator last,__true_type){}

    //!�������汾�����汾 char* wchar_t
    inline void destroy(char*,char*){}
    inline void destroy(wchar_t*,wchar_t*){}
    //ִ�и��� ���û�� �ᰴ�������destroyһ����ִ�� ����Ч�ʸ���

}
#endif // WSTL_CONSTRUCT_H_INCLUDED








