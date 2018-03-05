#include<vector>
#include<iostream>
#include "Wstl_heap.h"
#include "Wstl_algobase.h"
using namespace std;
using namespace W_STL;

int main(){
    vector<int> ivec = {0,1,2,3,4,8,9,3,5};
    make_heap(ivec.begin(),ivec.end());
    for(auto &e : ivec) cout << e << " ";
    cout << endl;

    ivec.push_back(7);
    push_heap(ivec.begin(),ivec.end());
    for(auto &e : ivec) cout << e << " ";
    cout << endl;

    pop_heap(ivec.begin(),ivec.end());
    cout << ivec.back() <<endl;
    ivec.pop_back();

    for(auto &e : ivec) cout << e << " ";
    cout <<endl;

    sort_heap(ivec.begin(),ivec.end());
     for(auto &e : ivec) cout << e << " ";
    cout <<endl;


}
