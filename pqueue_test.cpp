#include<vector>
#include<iostream>
#include "Wstl_priority_queue.h"
using namespace std;
using namespace W_STL;

int main()
{
    std::vector<int> ivec = {0,1,2,3,4,8,9,3,5};
    W_STL::priority_queue<int> ipq(ivec.begin(),ivec.end());
    cout << "size=" << ipq.size() <<endl;
    for(int i=0;i<ipq.size();++i){
        cout << ipq.top();
    }
    cout <<endl;
    while(!ipq.empty()){
        cout << ipq.top() << ' ';
        ipq.pop();
    }
    cout << endl;
}
