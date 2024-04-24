//线程封装
#include<iostream>
#include<thread>
#include<sstream>
#include<exception>
using namespace std;

class ZERO_Thread{
public:
    ZERO_Thread();
    virtual ~ZERO_Thread();
    bool start();
    void stop();
    bool isAlive() const;
    thread::id id(){ return th_->get_id();}
    thread *getThread(){ return th_;}
    void join();     //等待当前线程结束，不能再当前线程调用
    void detach();   //能在当前线程调用
    static size_t  CURRENT_THREADID();
protected:
    void threadEntry();  
    virtual void run()=0;
protected:
    bool running_;  //是否在运行
    thread *th_;
};

ZERO_Thread::ZERO_Thread():
    running_(false),th_(NULL)    //不写在括号里面是提高性能
{

}

ZERO_Thread::~ZERO_Thread(){
    if(th_!=NULL){
        if(th_->joinable()){
            //在C++中，如果一个线程对象是可连接的（joinable），而在其析构函数中没有被显式join或detach，那么程序会调用`std::terminate()`来终止程序的执行，从而导致未定义行为。
            cout<<"~ZERO_Thread detach"<<endl;
            th_->detach();
        }
        delete th_;
        th_=NULL;
    }
    cout<<"~ZERO_Thread()"<<endl;
}

bool ZERO_Thread::start(){
    if(running_){         //如果已经创建，则返回false
        return false;
    }
    //`try`和`catch`是C++中异常处理的关键字，用于捕获和处理异常。在`try`块中放置可能抛出异常的代码，一旦异常抛出，程序会立即跳转到最近的匹配的`catch`块中进行异常处理
    try
    {
        th_=new thread(&ZERO_Thread::threadEntry,this);
    }
    catch(...)
    {
        throw "[ZERO_Thread::start] thread start error";
    }
    return true;
}

void ZERO_Thread::stop(){
    running_=false;
}

//将`ZERO_Thread::isAlive()` 声明为 `const` 是为了遵循良好的编程实践，确保这个函数只用于查询对象的状态，不会修改对象的成员变量。
bool ZERO_Thread::isAlive() const{
    return running_;
}

void ZERO_Thread::join(){
    if(th_->joinable()){
        th_->join();     //不是detach采取join 
    }
}

void ZERO_Thread::detach(){
    th_->detach();
}

//这段代码的作用是获取当前线程的唯一ID，并确保每个线程只获取一次自己的线程ID
size_t ZERO_Thread::CURRENT_THREADID(){
    // 声明为thread_local的本地变量在线程中是持续存在的，不同于普通临时变量的生命周期，
    // 它具有static变量一样的初始化特征和生命周期，即使它不被声明为static。
    static thread_local size_t threadId=0;
    //- `thread_local` 关键字用于指示变量是线程局部存储的，每个线程都有自己的变量副本，互不干扰。
    // - 在多线程环境中，不同线程访问 `threadId` 变量时会访问各自线程的副本，避免了线程间的竞争和冲突。
    if(threadId==0){
        //这段代码确保了每个线程只在第一次调用 `CURRENT_THREADID()` 函数时获取自己的线程ID，后续调用则直接返回之前保存的值，避免了重复获取线程ID的开销。    
        stringstream ss;
        ss<<this_thread::get_id();
        threadId=strtol(ss.str().c_str(),NULL,0);  //将流中的字符串转化为长整型，并赋值给threadId
    }
    return threadId;
}

#if 0
在C++异常处理中，`catch(exception& ex)` 和 `catch(...)` 是两种不同的异常处理方式，它们之间的区别如下：

1. `catch(exception& ex)`：

   - 这种形式的 `catch` 语句捕获特定类型的异常，即继承自 `std::exception` 的异常。在捕获到该类型的异常时，会执行对应的处理代码。

   - 在代码中，`running_` 被设置为 `false`，然后重新抛出捕获到的异常 `ex`，使得异常可以被上层代码继续处理。

   - 这种方式允许对特定类型的异常进行特殊处理，可以根据具体的异常类型执行不同的逻辑。

2. `catch(...)`：

   - 这种形式的 `catch` 语句是通用的异常处理方式，可以捕获任何类型的异常，包括不继承自 `std::exception` 的异常。

   - 在捕获到任何类型的异常时，会执行对应的处理代码。

   - 在代码中，同样将 `running_` 设置为 `false`，然后重新抛出捕获到的异常，使得异常可以被上层代码继续处理。

   - 这种方式适用于处理不特定类型的异常，可以在一个通用的 `catch` 块中捕获所有异常。

    总的来说，`catch(exception& ex)` 是针对特定类型的异常进行处理，而 `catch(...)` 是通用的异常处理方式，可以捕获任何类型的异常。
在具体使用时，根据需要选择合适的异常处理方式来处理不同类型的异常。
#endif

void ZERO_Thread::threadEntry(){
    running_=true;
    try{
        run();
    }
    catch(exception&ex){
        running_=false;
        throw ex;
    }
    catch(...){
        running_=false;
        throw;
    }
    running_=false;
}

///////////////////////////////////////////////////////////////////////
class A:public ZERO_Thread{
public:
    void run(){
        while(running_){
            cout<<"print A"<<endl;
            this_thread::sleep_for(chrono::seconds(5));
        }
        cout<<"-------leave A "<<endl;
    }
};

class B:public ZERO_Thread{
public:
    void run(){
        while(running_){
            cout<<" print B "<<endl;
            this_thread::sleep_for(chrono::seconds(2));
        }
        cout<<"--------leave B "<<endl;
    }
};

int main(){
    {
        A a;
        a.start();
        B b;
        b.start();
        this_thread::sleep_for(chrono::seconds(5));
        a.stop();
        a.join();
        b.stop();
        b.join();
    }
    cout<<"hello world!"<<endl;
    return 0;
}

#if 0
print A print B

 print B
 print B
print A
 print B
 print B
-------leave A
--------leave B
~ZERO_Thread()
~ZERO_Thread()
hello world!
#endif

