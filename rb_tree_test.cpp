#include<vector>
#include<iostream>
#include<algorithm>
using namespace std;
#include "Wstl_functional.h"
#include "Wstl_RB_tree.h"

using namespace W_STL;

int main()
{
    W_STL::rb_tree<int,int,identity<int>,less<int>> itree;
    cout << itree.size() <<endl;
    itree.insert_unique(10);
    itree.insert_unique(7);
    itree.insert_unique(8);
    itree.insert_unique(15);
    itree.insert_unique(5);
    itree.insert_unique(6);
    itree.insert_unique(11);
    itree.insert_unique(13);
    itree.insert_unique(12);
    cout << itree.size() <<endl;
    W_STL::rb_tree<int,int,identity<int>,less<int>>::iterator ite1 = itree.begin();
    W_STL::rb_tree<int,int,identity<int>,less<int>>::iterator ite2 = itree.end();
    W_STL::__rb_tree_base_iterator rbtite;

    for(;ite1!=ite2;++ite1){
        rbtite = __rb_tree_base_iterator(ite1);
        cout << *ite1 << '(' << rbtite.node->color << ") ";
    }

}
