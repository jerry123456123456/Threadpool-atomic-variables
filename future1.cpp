#include<iostream>
#include<future>
#include<thread>
using namespace std;

int find_result_to_add(){
    this_thread::sleep_for(chrono::seconds(2));  //用来测试异步延迟的影响
    cout<<"find_result_to_add"<<endl;
    return 1+1;
}

int find_result_to_add2(int a,int b){
    this_thread::sleep_for(chrono::seconds(5));  //用来测试异步延迟的影响
    return a+b;
}

void do_other_things(){
    cout<<"do_other_things"<<endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

int main(){
    //future<int> result=async(find_result_to_add);
    //future<decltype(find_result_to_add())> result=async(find_result_to_add);
    auto result=async(find_result_to_add);
    do_other_things();
    cout<<"result:"<<result.get()<<endl;
    //decltype用于推导函数的返回值类型
    future<decltype(find_result_to_add2(0,0))> result2=async(find_result_to_add2,10,20);
    cout<<"result2:"<<result2.get()<<endl;

    return 0;
}