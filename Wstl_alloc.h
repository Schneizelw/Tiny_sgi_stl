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
//空间适配器

//!第一级配置器
namespace W_STL
{
    template<int inst> //表示可以传递一个实例进去
    class __malloc_alloc_template{
    private:
        //以下处理内存不足的情况  oom=out-of-memory
        //
        static void *oom_malloc(size_t);
        static void *oom_realloc(void *,size_t);

        //该函数指针 初始为0 如果设置有 就是错误处理函数 当用malloc分配内存失败的时候调用
        //oom_malloc 循环处理(调用错误处理函数) 直到分配内存成功为止 如果失败 扔出
        //__THROW_BAD_ALLOC  。 realloc同理
        static void (* __malloc_alloc_oom_handler)(); //函数指针 指向参数为空 返回类型为空

    public:
        static void * allocate(size_t n)
        {
            void *result = malloc(n); //!第一级配置直接使用malloc
            //!一下无法满足需求时，改用 oom_malloc()
            if(0 == result) result = oom_malloc(n);
            return result;
        }
        //!第二个参数 更多的意义是调节接口对应于 simple_alloc中的调用。以及提示所释放的大小
        static void deallocate(void *p,size_t /* n */)
        {
            free(p);//!第一级适配器直接使用free()
        }

        static void reallocate(void *p,size_t /* old_sz*/,size_t new_sz)
        {
            void *result = realloc(p,new_sz); //!第一级适配器直接使用realloc()
            if(0 == result) result = oom_realloc(p,new_sz);
            return result;
        }

        //!设置指定直接的out-of-memory handler
        static void (* set_malloc_handler(  void(*f)()  ))
        {
            void (*old)() = __malloc_alloc_oom_handler;
            __malloc_alloc_oom_handler = f;
            return (old);
        }
    }; // end   __malloc_alloc_template

    //!类模板静态成员变量初始化
    template<int inst>
    void (* __malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

    template<int inst>
    void * __malloc_alloc_template<inst>::oom_malloc(size_t n)
    {
         void (* my_malloc_handler)();
         void *result;

         for(;;){ //!不断尝试释放，配置，再释放，再配置
            my_malloc_handler = __malloc_alloc_oom_handler;
            if( 0 == my_malloc_handler){ __THROW_BAD_ALLOC ;}
            //!及其的内存已经山穷水尽 但是可以试试客端设计的handler去释放某些内存达到分配内存的目的
            (*my_malloc_handler)(); //!调用处理例程，企图释放内存
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
            //!及其的内存已经山穷水尽 但是可以试试客端设计的handler去释放某些内存达到分配内存的目的
            (*my_malloc_handler)(); //!调用处理例程，企图释放内存
            result = realloc(p,n);
            if(result) return (result);
         }
    }



    //!接口类,这个接口类使配置器的配置单位转化为bytes 通过sizeof转化为bytes 比如int 就会转换为4
    //!SGI STL容器全都是用这个simple_alloc接口
    typedef __malloc_alloc_template<0> malloc_alloc;

//!==================================================================================================
//!第二级配置器
    enum {__ALIGN = 8};
    enum {__MAX_BYTES = 128 };
    enum {__NFREELISTS = __MAX_BYTES/__ALIGN}; //!free-lists 个数

    template<bool threads,int inst>
    class __default_alloc_template{
    private:
        //!ROUND_UP() 将bytes上调至8的倍数
        static size_t ROUND_UP(size_t bytes)
        {
            return ((bytes+__ALIGN-1) & ~(__ALIGN-1));
            //如 byte = 13 则13+8-1 = 20 再与上 7的反码 也就是保留大于8的部分(二进制是11111000)
            //也就是丢掉不是8的倍数的部分
        }

    private:
        //!之所以用union 由于union之故 从其第一字段看，obj可视为一个指针， 指向相同形式的一个obj
        //!从其第二个字看 obj可视为一个指针，指向所指的实际区域。详细解释： 当在free list中的
        //!时候会使用第一个字段也就是free_list_link连接到下一块内存,当被取出来的时候client_data
        //!就是一个指针(不是client_data[0]) 并且这个指针不在sizeof(obj())的范围内,而这个指针 也就
        //!是client_data会指向client_data[0] (也就是指向首元素),又由于union的性质 这个client_data[0]
        //!区域是free_list_link的一部分(前1/4 一个指针大小为4bytes char则为1)
        union obj{
            union obj *free_list_link;//!当free-list是空的时候用于连接下一个块(block)
            char client_data[1]; //!client_data是一个“外部”指针指向分配内存的首个char(每一个小单元是char)
        };

    private:
        //!16个free lists
        static obj* volatile free_list[__NFREELISTS];
        //!一下函数根据区块的大小，决定使用第n号free-list。n从0开始
        static size_t FREELIST_INDEX(size_t bytes)
        {
            return (((bytes)+ __ALIGN - 1)/__ALIGN - 1);
                                              //!  ^^^^ free_list数组下标从0开始 所以要减去1。
        }
        //!返回一个大小为n的对象，并可能加入大小为n的其它区块到free list，连接好free list的各个小块单元
        //!并且连接到相关的free_list中
        static void *refill(size_t n);

        //!配置一大块空间，可容纳nobjs个大小为"size"的区块 注意nobjs是 PASS BY REFERENCE 所以有可能会改变
        static char *chunk_alloc(size_t size,int &nobjs);

        //!chunk allocation state
        static char *start_free; //!内存池起始位置， 只在chunk_alloc()中变化
        static char *end_free;  //!内存池结束位置， 只在chunk_alloc()中变化
        static size_t heap_size; //!分配出来的空间的总大小

    public:
        /*static void *allocate(size_t n);
        static void deallocate(void *p,size_t n);
        static void *reallocate(void *p,size_t old_sz,size_t new_sz);*/

    public:
        static void *allocate(size_t n)
        {
            obj *volatile *my_free_list;
            obj *result;
            //!大于128就调用第一级配置器
            if(n > (size_t)__MAX_BYTES){
                return (malloc_alloc::allocate(n));
            }
            //!寻找16个free lists中适当的一个
            my_free_list = free_list + FREELIST_INDEX(n);
            result = *my_free_list;
            if(result == 0){
                //!没找到可用的free list,准备重新填充free list
                void *r = refill(ROUND_UP(n));
                return r;
            }
            //!调整free list
            *my_free_list = result ->free_list_link;
            return (result);
        }

        //!p不可以是0
        static void deallocate(void *p,size_t n)
        {
            obj *q = (obj *) p ;
            obj * volatile *my_free_list;
            //!大于128bytes就调用第一级配置器
            if( n>(size_t)__MAX_BYTES){
                malloc_alloc::deallocate(p,n);
                return ;
            }
            //!寻找对应的free list
            my_free_list = free_list + FREELIST_INDEX(n);
            q->free_list_link = *my_free_list;
            *my_free_list = q;
        }
    };//  end __default_alloc_template

    //!以下是static data member的定义与初值的设定
    //!模板类静态成员初始化
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
        size_t total_bytes = size*nobjs; //!一个free list的内存量
        size_t bytes_left = end_free - start_free; //!内存池剩余空间

        if(bytes_left >= total_bytes){
            //!内存池的剩余空间完全满足需求量
            result = start_free;
            start_free += total_bytes;
            return (result);
        }else if(bytes_left >= size){
            //!内存池剩余空间不能完全满足需求量，但足够供应一个(含)以上的区块
            nobjs = bytes_left/size;//!调整可以分配的区块个数
            total_bytes = size * nobjs; //!内存池分配的总字节数
            result = start_free;
            start_free += total_bytes;
            return (result);
        }else{
            //!内存池连一个区块都无法提供
            size_t bytes_to_get = 2*total_bytes+ROUND_UP(heap_size >> 4);
            //!以下试着让内存池中的残余零头还有利用价值
            if(bytes_left > 0){
                //!内存池还有一些零头，先配给适当的free list 首先寻找合适的free list
                obj *volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
                //!调整free list 将内存池的残余空间编入
                ((obj *)start_free)->free_list_link = *my_free_list;
                *my_free_list = (obj *)start_free;
            }

            //!配置heap空间
            start_free = (char *)malloc(bytes_to_get);
            if( 0 == start_free){
                //!heap空间不足， malloc失败
                int i;
                obj *volatile * my_free_list,*p;//这里p不是指针的指针，是一级指针
                //!试着检视我们手上拥有的东西。这不会造成伤害。我们不打算尝试配置较小的区块
                //!因为那在多进程的机器上容易导致灾难，一下搜寻适当的free list
                //!适当也就是 “尚有未用区块，且区块够大” 之free_list
                for(i=size ; i<=__MAX_BYTES ; i+=__ALIGN){
                    my_free_list = free_list + FREELIST_INDEX(i);
                    p = *my_free_list;
                    if( 0 != p){ //!free list 内尚有未用区块
                        //!调整free list 释放未用区块
                        *my_free_list = p->free_list_link;
                        start_free = (char *)p;
                        end_free = start_free + i;
                        //!递归调用自己 为了修正nobjs
                        return(chunk_alloc(size,nobjs));
                        //!注意 内存池是可以剩余空间的
                    }
                }
                end_free = 0; //!如果出现意外(山穷水尽，到处都没有内存可以用了)
                //!调用一级配置器，看看OUT-OF-MEMORY 机制能否尽点力
                start_free = (char *)malloc_alloc::allocate(bytes_to_get);
            }//if
            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            //!递归调用自己，为修正nobjs
            return (chunk_alloc(size,nobjs));
        }//else
    } //end chunk_alloc


    template<bool threads,int inst>
    void *__default_alloc_template<threads,inst>::refill(size_t n)
    {                                                   //^^^^^^^ n为一小块空间的大小
        int nobjs = 20 ;//!一个设定值
        //!调用chunk_alloc(),尝试取得nobjs个区块作为free list的新节点 注意参数nobjs是 PASS BY REFERENCE
        char *chunk = chunk_alloc(n,nobjs);
        obj* volatile *my_free_list;
        obj *result;
        obj *current_obj,*next_obj;
        int i;

        //!如果只有一个区块，这个区块就分配给调用者，free list无新节点
        if(1==nobjs) return(chunk);
        //!否则准备调整free list 纳入新节点
        my_free_list = free_list+FREELIST_INDEX(n);

        //!以下在chunk空间内建立 free list
        result = (obj *)chunk; //!这一块准备返回给客户端
        //!以下导引free list指向新配置的空间 (取自内存池)
        *my_free_list = next_obj = (obj *)(chunk + n); //!设置好其实条件
        //!以下将free list各个节点串接起来
        for(i=1; ;i++){
            current_obj = next_obj;
            next_obj = (obj *)((char *)next_obj + n);
            if(nobjs - 1 == i){
                current_obj -> free_list_link = 0;
                break;
            }else{
                current_obj ->free_list_link = next_obj; //!连接起来
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
