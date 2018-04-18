#ifndef WSTL_ALLOC_H_INCLUDED
#define WSTL_ALLOC_H_INCLUDED
#include<cstdlib>
#include<cstddef>
#if 0
#   include<new>
#   define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#   include<iostream>
#   define __THROW_BAD_ALLOC std::cerr<< "out of memory" << std::endl; exit(1)
#endif

//template<int inst> class __malloc_alloc_template;
//template<bool threads,int inst> class __default_alloc_template;
#define __NODE_ALLOCATOR_THREADS 0
#include <stdlib.h>
//�ռ�������

//!��һ��������
namespace W_STL
{
    template<int inst> //��ʾ���Դ���һ��ʵ����ȥ
    class __malloc_alloc_template{
    private:
        //���´����ڴ治������  oom=out-of-memory
        //
        static void *oom_malloc(size_t);
        static void *oom_realloc(void *,size_t);

        //�ú���ָ�� ��ʼΪ0 ��������� ���Ǵ������� ����malloc�����ڴ�ʧ�ܵ�ʱ�����
        //oom_malloc ѭ������(���ô�������) ֱ�������ڴ�ɹ�Ϊֹ ���ʧ�� �ӳ�
        //__THROW_BAD_ALLOC  �� reallocͬ��
        static void (* __malloc_alloc_oom_handler)(); //����ָ�� ָ�����Ϊ�� ��������Ϊ��

    public:
        static void * allocate(size_t n)
        {
            void *result = malloc(n); //!��һ������ֱ��ʹ��malloc
            //!һ���޷���������ʱ������ oom_malloc()
            if(0 == result) result = oom_malloc(n);
            return result;
        }
        //!�ڶ������� ����������ǵ��ڽӿڶ�Ӧ�� simple_alloc�еĵ��á��Լ���ʾ���ͷŵĴ�С
        static void deallocate(void *p,size_t /* n */)
        {
            free(p);//!��һ��������ֱ��ʹ��free()
        }

        static void reallocate(void *p,size_t /* old_sz*/,size_t new_sz)
        {
            void *result = realloc(p,new_sz); //!��һ��������ֱ��ʹ��realloc()
            if(0 == result) result = oom_realloc(p,new_sz);
            return result;
        }

        //!����ָ��ֱ�ӵ�out-of-memory handler
        static void (* set_malloc_handler(  void(*f)()  ))
        {
            void (*old)() = __malloc_alloc_oom_handler;
            __malloc_alloc_oom_handler = f;
            return (old);
        }
    }; // end   __malloc_alloc_template

    //!��ģ�徲̬��Ա������ʼ��
    template<int inst>
    void (* __malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

    template<int inst>
    void * __malloc_alloc_template<inst>::oom_malloc(size_t n)
    {
         void (* my_malloc_handler)();
         void *result;

         for(;;){ //!���ϳ����ͷţ����ã����ͷţ�������
            my_malloc_handler = __malloc_alloc_oom_handler;
            if( 0 == my_malloc_handler){ __THROW_BAD_ALLOC ;}
            //!������ڴ��Ѿ�ɽ��ˮ�� ���ǿ������ԿͶ���Ƶ�handlerȥ�ͷ�ĳЩ�ڴ�ﵽ�����ڴ��Ŀ��
            (*my_malloc_handler)(); //!���ô������̣���ͼ�ͷ��ڴ�
            result = malloc(n);
            if(result) return (result);
         }
    }

    template<int inst>
    void * __malloc_alloc_template<inst>::oom_realloc(void *p,size_t n)
    {
         void (* my_malloc_handler)();
         void *result;

         for(;;){
            my_malloc_handler = __malloc_alloc_oom_handler;
            if( 0 == my_malloc_handler){ __THROW_BAD_ALLOC ;}
            //!������ڴ��Ѿ�ɽ��ˮ�� ���ǿ������ԿͶ���Ƶ�handlerȥ�ͷ�ĳЩ�ڴ�ﵽ�����ڴ��Ŀ��
            (*my_malloc_handler)(); //!���ô������̣���ͼ�ͷ��ڴ�
            result = realloc(p,n);
            if(result) return (result);
         }
    }



    //!�ӿ���,����ӿ���ʹ�����������õ�λת��Ϊbytes ͨ��sizeofת��Ϊbytes ����int �ͻ�ת��Ϊ4
    //!SGI STL����ȫ���������simple_alloc�ӿ�
    typedef __malloc_alloc_template<0> malloc_alloc;

//!==================================================================================================
//!�ڶ���������
    enum {__ALIGN = 8};
    enum {__MAX_BYTES = 128 };
    enum {__NFREELISTS = __MAX_BYTES/__ALIGN}; //!free-lists ����

    template<bool threads,int inst>
    class __default_alloc_template{
    private:
        //!ROUND_UP() ��bytes�ϵ���8�ı���
        static size_t ROUND_UP(size_t bytes)
        {
            return ((bytes+__ALIGN-1) & ~(__ALIGN-1));
            //�� byte = 13 ��13+8-1 = 20 ������ 7�ķ��� Ҳ���Ǳ�������8�Ĳ���(��������11111000)
            //Ҳ���Ƕ�������8�ı����Ĳ���
        }

    private:
        //!֮������union ����union֮�� �����һ�ֶο���obj����Ϊһ��ָ�룬 ָ����ͬ��ʽ��һ��obj
        //!����ڶ����ֿ� obj����Ϊһ��ָ�룬ָ����ָ��ʵ��������ϸ���ͣ� ����free list�е�
        //!ʱ���ʹ�õ�һ���ֶ�Ҳ����free_list_link���ӵ���һ���ڴ�,����ȡ������ʱ��client_data
        //!����һ��ָ��(����client_data[0]) �������ָ�벻��sizeof(obj())�ķ�Χ��,�����ָ�� Ҳ��
        //!��client_data��ָ��client_data[0] (Ҳ����ָ����Ԫ��),������union������ ���client_data[0]
        //!������free_list_link��һ����(ǰ1/4 һ��ָ���СΪ4bytes char��Ϊ1)
        union obj{
            union obj *free_list_link;//!��free-list�ǿյ�ʱ������������һ����(block)
            char client_data[1]; //!client_data��һ�����ⲿ��ָ��ָ������ڴ���׸�char(ÿһ��С��Ԫ��char)
        };

    private:
        //!16��free lists
        static obj* volatile free_list[__NFREELISTS];
        //!һ�º�����������Ĵ�С������ʹ�õ�n��free-list��n��0��ʼ
        static size_t FREELIST_INDEX(size_t bytes)
        {
            return (((bytes)+ __ALIGN - 1)/__ALIGN - 1);
                                              //!  ^^^^ free_list�����±��0��ʼ ����Ҫ��ȥ1��
        }
        //!����һ����СΪn�Ķ��󣬲����ܼ����СΪn���������鵽free list�����Ӻ�free list�ĸ���С�鵥Ԫ
        //!�������ӵ���ص�free_list��
        static void *refill(size_t n);

        //!����һ���ռ䣬������nobjs����СΪ"size"������ ע��nobjs�� PASS BY REFERENCE �����п��ܻ�ı�
        static char *chunk_alloc(size_t size,int &nobjs);

        //!chunk allocation state
        static char *start_free; //!�ڴ����ʼλ�ã� ֻ��chunk_alloc()�б仯
        static char *end_free;  //!�ڴ�ؽ���λ�ã� ֻ��chunk_alloc()�б仯
        static size_t heap_size; //!��������Ŀռ���ܴ�С

    public:
        /*static void *allocate(size_t n);
        static void deallocate(void *p,size_t n);
        static void *reallocate(void *p,size_t old_sz,size_t new_sz);*/

    public:
        static void *allocate(size_t n)
        {
            obj *volatile *my_free_list;
            obj *result;
            //!����128�͵��õ�һ��������
            if(n > (size_t)__MAX_BYTES){
                return (malloc_alloc::allocate(n));
            }
            //!Ѱ��16��free lists���ʵ���һ��
            my_free_list = free_list + FREELIST_INDEX(n);
            result = *my_free_list;
            if(result == 0){
                //!û�ҵ����õ�free list,׼���������free list
                void *r = refill(ROUND_UP(n));
                return r;
            }
            //!����free list
            *my_free_list = result ->free_list_link;
            return (result);
        }

        //!p��������0
        static void deallocate(void *p,size_t n)
        {
            obj *q = (obj *) p ;
            obj * volatile *my_free_list;
            //!����128bytes�͵��õ�һ��������
            if( n>(size_t)__MAX_BYTES){
                malloc_alloc::deallocate(p,n);
                return ;
            }
            //!Ѱ�Ҷ�Ӧ��free list
            my_free_list = free_list + FREELIST_INDEX(n);
            q->free_list_link = *my_free_list;
            *my_free_list = q;
        }
    };//  end __default_alloc_template

    //!������static data member�Ķ������ֵ���趨
    //!ģ���ྲ̬��Ա��ʼ��
    template<bool threads,int inst>
    char * __default_alloc_template<threads,inst>::start_free = 0;

    template<bool threads,int inst>
    char * __default_alloc_template<threads,inst>::end_free = 0;

    template<bool threads,int inst>
    size_t __default_alloc_template<threads,inst>::heap_size = 0;

    template<bool threads,int inst>
    typename __default_alloc_template<threads,inst>::obj* volatile
    __default_alloc_template<threads,inst>::free_list[__NFREELISTS] =
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

    template<bool threads,int inst>
    char *
    __default_alloc_template<threads,inst>::chunk_alloc(size_t size,int &nobjs)
    {
        char *result;
        size_t total_bytes = size*nobjs; //!һ��free list���ڴ���
        size_t bytes_left = end_free - start_free; //!�ڴ��ʣ��ռ�

        if(bytes_left >= total_bytes){
            //!�ڴ�ص�ʣ��ռ���ȫ����������
            result = start_free;
            start_free += total_bytes;
            return (result);
        }else if(bytes_left >= size){
            //!�ڴ��ʣ��ռ䲻����ȫ���������������㹻��Ӧһ��(��)���ϵ�����
            nobjs = bytes_left/size;//!�������Է�����������
            total_bytes = size * nobjs; //!�ڴ�ط�������ֽ���
            result = start_free;
            start_free += total_bytes;
            return (result);
        }else{
            //!�ڴ����һ�����鶼�޷��ṩ
            size_t bytes_to_get = 2*total_bytes+ROUND_UP(heap_size >> 4);
            //!�����������ڴ���еĲ�����ͷ�������ü�ֵ
            if(bytes_left > 0){
                //!�ڴ�ػ���һЩ��ͷ��������ʵ���free list ����Ѱ�Һ��ʵ�free list
                obj *volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
                //!����free list ���ڴ�صĲ���ռ����
                ((obj *)start_free)->free_list_link = *my_free_list;
                *my_free_list = (obj *)start_free;
            }

            //!����heap�ռ�
            start_free = (char *)malloc(bytes_to_get);
            if( 0 == start_free){
                //!heap�ռ䲻�㣬 mallocʧ��
                int i;
                obj *volatile * my_free_list,*p;//����p����ָ���ָ�룬��һ��ָ��
                //!���ż�����������ӵ�еĶ������ⲻ������˺������ǲ����㳢�����ý�С������
                //!��Ϊ���ڶ���̵Ļ��������׵������ѣ�һ����Ѱ�ʵ���free list
                //!�ʵ�Ҳ���� ������δ�����飬�����鹻�� ֮free_list
                for(i=size ; i<=__MAX_BYTES ; i+=__ALIGN){
                    my_free_list = free_list + FREELIST_INDEX(i);
                    p = *my_free_list;
                    if( 0 != p){ //!free list ������δ������
                        //!����free list �ͷ�δ������
                        *my_free_list = p->free_list_link;
                        start_free = (char *)p;
                        end_free = start_free + i;
                        //!�ݹ�����Լ� Ϊ������nobjs
                        return(chunk_alloc(size,nobjs));
                        //!ע�� �ڴ���ǿ���ʣ��ռ��
                    }
                }
                end_free = 0; //!�����������(ɽ��ˮ����������û���ڴ��������)
                //!����һ��������������OUT-OF-MEMORY �����ܷ񾡵���
                start_free = (char *)malloc_alloc::allocate(bytes_to_get);
            }//if
            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            //!�ݹ�����Լ���Ϊ����nobjs
            return (chunk_alloc(size,nobjs));
        }//else
    } //end chunk_alloc


    template<bool threads,int inst>
    void *__default_alloc_template<threads,inst>::refill(size_t n)
    {                                                   //^^^^^^^ nΪһС��ռ�Ĵ�С
        int nobjs = 20 ;//!һ���趨ֵ
        //!����chunk_alloc(),����ȡ��nobjs��������Ϊfree list���½ڵ� ע�����nobjs�� PASS BY REFERENCE
        char *chunk = chunk_alloc(n,nobjs);
        obj* volatile *my_free_list;
        obj *result;
        obj *current_obj,*next_obj;
        int i;

        //!���ֻ��һ�����飬�������ͷ���������ߣ�free list���½ڵ�
        if(1==nobjs) return(chunk);
        //!����׼������free list �����½ڵ�
        my_free_list = free_list+FREELIST_INDEX(n);

        //!������chunk�ռ��ڽ��� free list
        result = (obj *)chunk; //!��һ��׼�����ظ��ͻ���
        //!���µ���free listָ�������õĿռ� (ȡ���ڴ��)
        *my_free_list = next_obj = (obj *)(chunk + n); //!���ú���ʵ����
        //!���½�free list�����ڵ㴮������
        for(i=1; ;i++){
            current_obj = next_obj;
            next_obj = (obj *)((char *)next_obj + n);
            if(nobjs - 1 == i){
                current_obj -> free_list_link = 0;
                break;
            }else{
                current_obj ->free_list_link = next_obj; //!��������
            }
        }
        return (result);
    } // end refill


    #ifdef __USE_MALLOC
    typedef __malloc_alloc_template<0> malloc_alloc;
    typedef malloc_alloc alloc;
    #else
    typedef __default_alloc_template<__NODE_ALLOCATOR_THREADS,0> alloc;
    #endif

    template<typename T,typename Alloc>
    class simple_alloc{
    public:
        static T *allocate(size_t n){ return 0==n? 0 :(T*)Alloc::allocate(n*sizeof(T)); }
        static T *allocate(void){ return (T*)Alloc::allocate(sizeof(T)); }
        static void deallocate(T *p,size_t n){ if (0!=n)Alloc::deallocate(p,n*sizeof(T)); }
        static void deallocate(T *p){ Alloc::deallocate(p,sizeof(T)); }
    };
}

#endif // WSTL_ALLOC_H_INCLUDED
