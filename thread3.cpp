//使用条件变量实现一个同步队列，同步队列作为一个线程安全的数据共享区，经常用于线程之间的数据读取
#include<iostream>
#include<mutex>
#include<condition_variable>
#include<list>
#include<thread>

using namespace std;

template<typename T>
class SyncQueue{
private:
    bool IsFull()const{
        return _queue.size()==_maxSize;
    }
    bool IsEmpty()const{
        return _queue.empty();
    }
public:
    SyncQueue(int maxSize) : _maxSize(maxSize){}

    void Put(const T& x){          //数据的插入
        lock_guard<mutex> locker(_mutex);
        while(IsFull()){
            cout<<"full wait..."<<endl;
            _notFull.wait(_mutex);
        }
        _queue.push_back(x);
        _notFull.notify_one();
    } 
    void Take(T& x){               //数据的删除
        lock_guard<mutex> locker(_mutex);
        while(IsEmpty()){
            cout<<"empty wait..."<<endl;
            _notEmpty.wait(_mutex);
        }
        x=_queue.front();
        _queue.pop_front();
        _notFull.notify_one();
    }

    bool Empty(){
        lock_guard<mutex> locker(_mutex);
        return _queue.empty();
    }
    bool Full(){
        lock_guard<mutex> locker(_mutex);
        return _queue.size()==_maxSize;
    }
    size_t Size(){
        lock_guard<mutex> locker(_mutex);
        return _queue.size();
    }
    int Count(){
        return _queue.size();
    }

private:
    list<T> _queue;  //双向链表
    mutex _mutex;
    condition_variable_any _notEmpty;  //不为空的条件变量
    condition_variable_any _notFull;   //没有满的条件变量
    int _maxSize;                      //同步队列最大的size
};

///////////////////////////////////////////////////////////////
SyncQueue<int> syncQueue(5);

void PutDatas()
{
    for (int i = 0; i < 20; ++i){
        syncQueue.Put(888);
    }
    cout << "PutDatas finish\n";
}
void TakeDatas()
{
    int x = 0;
    for (int i = 0; i < 20; ++i){
        syncQueue.Take(x);
        cout << x << std::endl;
    }
    cout << "TakeDatas finish\n";
}

int main(){
    thread t1(PutDatas);
    thread t2(TakeDatas);
    t1.join();
    t2.join();
    cout<<"main finished\n";
    return 0;
}