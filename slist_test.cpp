#include<vector>
#include<iostream>
#include<algorithm>
#include "Wstl_slist.h"
#include "Wstl_algobase.h"
using namespace std;
using namespace W_STL;


int main(){
    int i;
    slist<int> islist;
    cout << "size=" << islist.size() <<endl;
    islist.push_front(9);
    islist.push_front(1);
    islist.push_front(2);
    islist.push_front(3);
    islist.push_front(4);
    cout << "size=" << islist.size() <<endl;

    slist<int>::iterator ite = islist.begin();
    slist<int>::iterator ite2 = islist.end();
    for(;ite!=ite2;++ite) cout << *ite <<" ";
    cout << endl;

    ite = W_STL::find(islist.begin(),islist.end(),1);
    if(ite!=0) islist.insert(ite,99);


    ite = islist.begin();
    ite2 = islist.end();
    for(;ite!=ite2;++ite) cout << *ite <<" ";
    cout << endl;

    ite = W_STL::find(islist.begin(),islist.end(),3);
    if(ite!=0)
        cout << *(islist.erase(ite))  << endl;

    ite = islist.begin();
    ite2 = islist.end();
    for(;ite!=ite2;++ite) cout << *ite <<" ";
    cout << endl;

}
