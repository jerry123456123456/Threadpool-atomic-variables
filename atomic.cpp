#include<iostream>
#include<thread>
#include<mutex>
#include<atomic>
using namespace std;

//atomic<int> count=0;  错误的初始化
atomic<int> count_(0);   //准确初始化，atomic是线程安全的

void set_count(int x){
    cout<<"set_count:"<<x<<endl;
    count_.store(x,memory_order_relaxed);   //将x的值存到count_中，以松散可以被优化的方式存储
}

void print_count(){
    int x;
    do{
        x=count_.load(memory_order_relaxed);  //将count_中的值加载到x中
    }while(x==0);
    cout<<"count:"<<x<<endl;
}

int main(){
    thread t1(print_count);
    thread t2(set_count,10);
    t1.join();
    t2.join();
    cout<<"main end"<<endl;
    return 0;
}