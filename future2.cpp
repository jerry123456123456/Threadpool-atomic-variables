#include<iostream>
#include<thread>
#include<future>
using namespace std;

int add(int a,int b,int c){
    cout<<"call add\n";
    return a+b+c;
}

void do_other_things(){
    cout<<"do_other_things"<<endl;
}

int main(){
    packaged_task<int(int,int,int)> task(add);   //封装任务
    do_other_things();
    future<int> result=task.get_future();
    task(1,1,2);  //必须要让任务执行，否则在get（）获取future的值时会一直阻塞
    cout<<"result: "<<result.get()<<endl;
    return 0;
}