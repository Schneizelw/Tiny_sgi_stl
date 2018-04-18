/*
    type_traits 技法： __type_trait负责萃取型别特性，这里我们关注的特性是指：
是否具备trivial copy ctor？是否具备 trivial assignment operator？
是否具备trivial dtor？是否具有trivial default ctor，如果不具有。
那么在对这个型别进行拷贝 构造 析构 赋值 等操作的时候 就可以采用
最有效率的的措施 例如根本不采用身高居位的constructor destructor
而是采用内存直接处理操作malloc memcpy等 获取最高效率。
1个简单的对应关系 trivial =  降低效率   non-trivial = 必须要操作
*/

#ifndef WTYPE_TRAITS_H_INCLUDED
#define WTYPE_TRAITS_H_INCLUDED
#include "Wstl_config.h"
namespace W_STL{

    //!编译器只有面对class object形式的参数才会做类型推导。
    struct __true_type {};
    struct __false_type {};

    template<typename type>
    struct __type_traits{
        typedef __true_type this_dummy_member_must_be_first;
        /*
            该类型是不能移除的，它统治 “有能力自动将__type_trait特化”的编译器说，我们
        现在所看到的这个 __type_trait template是特殊的。这是为了确保万一编译器也使用
        一个名为 __type_trait而其实与此处的定义并无任何关联的template时，所有事情都仍
        将顺利运作。
        */

        //!全部设定为__false_type其实是一种比较保守的方法。
        typedef __false_type has_trivial_default_constructor;
        typedef __false_type has_trivial_copy_constructor;
        typedef __false_type has_trivial_assignment_operator;
        typedef __false_type has_trivial_destructor;
        typedef __false_type is_POD_type;

    };

        /*
            针对C++的基本型别char,signed char,unsigned char ,short,unsigned short,int,
        unsigned int,long,unsigned long,float,double,long double提供特例化版本。
        每一个成员都是__true_type,表示这些型别都可以采用最快速的方式(例如memcpy)
        来进行拷贝(copy)或赋值(assign)操作
        */
        //!直接具体化为某一个类型了 所以template也就没有了 就是没有模板了 有些编译器
        //!需要手动写上template<> 而有些则不需要 这里在config加了一个判断。
        __STL_TEMPLATE_NULL struct __type_traits<char>{
            typedef __true_type has_trivial_default_constructor;
            typedef __true_type has_trivial_copy_constructor;
            typedef __true_type has_trivial_assignment_operator;
            typedef __true_type has_trivial_destructor;
            typedef __true_type is_POD_type;
        };

         __STL_TEMPLATE_NULL struct __type_traits<signed char>{
            typedef __true_type has_trivial_default_constructor;
            typedef __true_type has_trivial_copy_constructor;
            typedef __true_type has_trivial_assignment_operator;
            typedef __true_type has_trivial_destructor;
            typedef __true_type is_POD_type;
        };

         __STL_TEMPLATE_NULL struct __type_traits<unsigned char>{
            typedef __true_type has_trivial_default_constructor;
            typedef __true_type has_trivial_copy_constructor;
            typedef __true_type has_trivial_assignment_operator;
            typedef __true_type has_trivial_destructor;
            typedef __true_type is_POD_type;
        };

         __STL_TEMPLATE_NULL struct __type_traits<short>{
            typedef __true_type has_trivial_default_constructor;
            typedef __true_type has_trivial_copy_constructor;
            typedef __true_type has_trivial_assignment_operator;
            typedef __true_type has_trivial_destructor;
            typedef __true_type is_POD_type;
        };

         __STL_TEMPLATE_NULL struct __type_traits<unsigned short>{
            typedef __true_type has_trivial_default_constructor;
            typedef __true_type has_trivial_copy_constructor;
            typedef __true_type has_trivial_assignment_operator;
            typedef __true_type has_trivial_destructor;
            typedef __true_type is_POD_type;
        };

         __STL_TEMPLATE_NULL struct __type_traits<int>{
            typedef __true_type has_trivial_default_constructor;
            typedef __true_type has_trivial_copy_constructor;
            typedef __true_type has_trivial_assignment_operator;
            typedef __true_type has_trivial_destructor;
            typedef __true_type is_POD_type;
        };

         __STL_TEMPLATE_NULL struct __type_traits<unsigned int>{
            typedef __true_type has_trivial_default_constructor;
            typedef __true_type has_trivial_copy_constructor;
            typedef __true_type has_trivial_assignment_operator;
            typedef __true_type has_trivial_destructor;
            typedef __true_type is_POD_type;
        };

         __STL_TEMPLATE_NULL struct __type_traits<long>{
            typedef __true_type has_trivial_default_constructor;
            typedef __true_type has_trivial_copy_constructor;
            typedef __true_type has_trivial_assignment_operator;
            typedef __true_type has_trivial_destructor;
            typedef __true_type is_POD_type;
        };

         __STL_TEMPLATE_NULL struct __type_traits<unsigned long>{
            typedef __true_type has_trivial_default_constructor;
            typedef __true_type has_trivial_copy_constructor;
            typedef __true_type has_trivial_assignment_operator;
            typedef __true_type has_trivial_destructor;
            typedef __true_type is_POD_type;
        };

         __STL_TEMPLATE_NULL struct __type_traits<float>{
            typedef __true_type has_trivial_default_constructor;
            typedef __true_type has_trivial_copy_constructor;
            typedef __true_type has_trivial_assignment_operator;
            typedef __true_type has_trivial_destructor;
            typedef __true_type is_POD_type;
        };

         __STL_TEMPLATE_NULL struct __type_traits<double>{
            typedef __true_type has_trivial_default_constructor;
            typedef __true_type has_trivial_copy_constructor;
            typedef __true_type has_trivial_assignment_operator;
            typedef __true_type has_trivial_destructor;
            typedef __true_type is_POD_type;
        };

         __STL_TEMPLATE_NULL struct __type_traits<long double>{
            typedef __true_type has_trivial_default_constructor;
            typedef __true_type has_trivial_copy_constructor;
            typedef __true_type has_trivial_assignment_operator;
            typedef __true_type has_trivial_destructor;
            typedef __true_type is_POD_type;
        };

        //!原生指针的特例化版本
        template<typename T>
        struct __type_traits<T*>{
            typedef __true_type has_trivial_default_constructor;
            typedef __true_type has_trivial_copy_constructor;
            typedef __true_type has_trivial_assignment_operator;
            typedef __true_type has_trivial_destructor;
            typedef __true_type is_POD_type;
        };

}


#endif // WTYPE_TRAITS_H_INCLUDED
