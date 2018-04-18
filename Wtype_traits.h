/*
    type_traits ������ __type_trait������ȡ�ͱ����ԣ��������ǹ�ע��������ָ��
�Ƿ�߱�trivial copy ctor���Ƿ�߱� trivial assignment operator��
�Ƿ�߱�trivial dtor���Ƿ����trivial default ctor����������С�
��ô�ڶ�����ͱ���п��� ���� ���� ��ֵ �Ȳ�����ʱ�� �Ϳ��Բ���
����Ч�ʵĵĴ�ʩ ���������������߾�λ��constructor destructor
���ǲ����ڴ�ֱ�Ӵ������malloc memcpy�� ��ȡ���Ч�ʡ�
1���򵥵Ķ�Ӧ��ϵ trivial =  ����Ч��   non-trivial = ����Ҫ����
*/

#ifndef WTYPE_TRAITS_H_INCLUDED
#define WTYPE_TRAITS_H_INCLUDED
#include "Wstl_config.h"
namespace W_STL{

    //!������ֻ�����class object��ʽ�Ĳ����Ż��������Ƶ���
    struct __true_type {};
    struct __false_type {};

    template<typename type>
    struct __type_traits{
        typedef __true_type this_dummy_member_must_be_first;
        /*
            �������ǲ����Ƴ��ģ���ͳ�� ���������Զ���__type_trait�ػ����ı�����˵������
        ��������������� __type_trait template������ġ�����Ϊ��ȷ����һ������Ҳʹ��
        һ����Ϊ __type_trait����ʵ��˴��Ķ��岢���κι�����templateʱ���������鶼��
        ��˳��������
        */

        //!ȫ���趨Ϊ__false_type��ʵ��һ�ֱȽϱ��صķ�����
        typedef __false_type has_trivial_default_constructor;
        typedef __false_type has_trivial_copy_constructor;
        typedef __false_type has_trivial_assignment_operator;
        typedef __false_type has_trivial_destructor;
        typedef __false_type is_POD_type;

    };

        /*
            ���C++�Ļ����ͱ�char,signed char,unsigned char ,short,unsigned short,int,
        unsigned int,long,unsigned long,float,double,long double�ṩ�������汾��
        ÿһ����Ա����__true_type,��ʾ��Щ�ͱ𶼿��Բ�������ٵķ�ʽ(����memcpy)
        �����п���(copy)��ֵ(assign)����
        */
        //!ֱ�Ӿ��廯Ϊĳһ�������� ����templateҲ��û���� ����û��ģ���� ��Щ������
        //!��Ҫ�ֶ�д��template<> ����Щ����Ҫ ������config����һ���жϡ�
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

        //!ԭ��ָ����������汾
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
