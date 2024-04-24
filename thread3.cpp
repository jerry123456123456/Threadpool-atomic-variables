//ʹ����������ʵ��һ��ͬ�����У�ͬ��������Ϊһ���̰߳�ȫ�����ݹ����������������߳�֮������ݶ�ȡ
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

    void Put(const T& x){          //���ݵĲ���
        lock_guard<mutex> locker(_mutex);
        while(IsFull()){
            cout<<"full wait..."<<endl;
            _notFull.wait(_mutex);
        }
        _queue.push_back(x);
        _notFull.notify_one();
    } 
    void Take(T& x){               //���ݵ�ɾ��
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
    list<T> _queue;  //˫������
    mutex _mutex;
    condition_variable_any _notEmpty;  //��Ϊ�յ���������
    condition_variable_any _notFull;   //û��������������
    int _maxSize;                      //ͬ����������size
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