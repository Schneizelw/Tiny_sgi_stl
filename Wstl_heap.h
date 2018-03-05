#ifndef WSTL_HEAP_H_INCLUDED
#define WSTL_HEAP_H_INCLUDED
#include "Wstl_iterator.h"

namespace W_STL{
    //!push_heap=============================================================================

    /*
    summary：维护两个索引：holeIndex和parent分别指向孩子结点和父结点，使用插入的值value和父
    结点的值比较大小，由于最大堆的性质，如果value的值大于父结点的值，则将父结点的值放到holeIndex
    处，然后更新这两个索引，一直循环。最后在holeIndex出插入value.
    */
    template<class RandomAccessIterator,class Distance,class T>
    void __push_heap(RandomAccessIterator first,Distance holeIndex,Distance topIndex,
                     T value){
        Distance parent = (holeIndex - 1)/2; //找出父结点
        while(holeIndex > topIndex && *(first+parent) < value){
                                           //!^^^^^^^^^^^^^^^ <决定了是最大堆
            *(first+holeIndex) = *(first+parent);
            holeIndex = parent;
            parent = (holeIndex-1)/2;
        }
        *(first+holeIndex) = value;
    }

    template<class RandomAccessIterator,class Distance,class T>
    inline void __push_heap_aux(RandomAccessIterator first,RandomAccessIterator last,
                                Distance*,T*){
        __push_heap(first,Distance((last-first)-1),Distance(0),T(*(last-1)));
    }

    template<class RandomAccessIterator>
    inline void push_heap(RandomAccessIterator first,RandomAccessIterator last){
        __push_heap_aux(first,last,distance_type(first),value_type(first));
    }
    //!pop_heap===========================================================================

    template<class RandomAccessIterator,class T,class Distance>
    void __adjust_heap(RandomAccessIterator first,Distance holeIndex,Distance len,T value){
        Distance topIndex = holeIndex;
        Distance secondChild = 2*holeIndex+2;  //secondChild此时指向右孩子结点。
        while(secondChild < len){
            if(*(first+(secondChild-1)) > *(first+secondChild))
                secondChild--;//此时的secondChild表示的是孩子结点中较大的一个。
            *(first+holeIndex) = *(first+secondChild);
            holeIndex = secondChild;
            //找出新洞结点的右子结点
            secondChild = 2*(secondChild+1);
        }
        if(len==secondChild){
            //最后一步
            *(first+holeIndex) = *(first+(secondChild-1));
            holeIndex = secondChild -1 ;
        }
        __push_heap(first,holeIndex,topIndex,value);
    }

    /*
    summary：将最后一个值取出为value，将根的值放在最后一个位置.然后循环的把空位孩子结点较大的值
    放到空位。特殊情况：只剩最后一个最孩子结点了，将左孩子结点放到空位，然后percolate up操作.
    */
    template<class RandomAccessIterator,class T,class Distance>
    inline void __pop_heap(RandomAccessIterator first,RandomAccessIterator last,
                           RandomAccessIterator result,T value,Distance*){
        *result = *first;
        __adjust_heap(first,Distance(0),Distance(last-first),value);
    }

    template<class RandomAccessIterator,class T>
    inline void __pop_heap_aux(RandomAccessIterator first,RandomAccessIterator last,T*){
        __pop_heap(first,last-1,last-1,T(*(last-1)),distance_type(first));
    }

    template<class RandomAccessIterator>
    inline void pop_heap(RandomAccessIterator first,RandomAccessIterator last){
        __pop_heap_aux(first,last,value_type(first));
    }

    //!sort_heap==========================================================================
    /*
    summary：每次执行一次pop_heap(),极值即被放在了尾端，扣除尾端再执行一次pop_heap(),次极值又
    被放到在新尾端，一直下去最后即的排序结果。
    */
    template<class RandomAccessIterator>
    inline void sort_heap(RandomAccessIterator first,RandomAccessIterator last){
        //由于左闭右开当last-first==1的时候，其实已经排好。
        while(last-first > 1) pop_heap(first,last--);
    }


    //!make_heap==========================================================================

    template<class RandomAccessIterator,class T,class Distance>
    inline void __make_heap(RandomAccessIterator first,RandomAccessIterator last,T*,Distance*){
        if(last-first<2) return;
        Distance len = last-first;
        Distance parent = (len-2)/2;
        while(true){
            __adjust_heap(first,parent,len,T(*(first + parent)));

            if(parent==0) return;//走完根节点return;
            parent--;
        }
    }

    template<class RandomAccessIterator>
    inline void make_heap(RandomAccessIterator first,RandomAccessIterator last){
        __make_heap(first,last,value_type(first),distance_type(first));
    }
}

#endif // WSTL_HEAP_H_INCLUDED
