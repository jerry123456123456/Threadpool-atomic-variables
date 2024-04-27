#include<iostream>
#include<queue>
#include<future>
#include<mutex>
#include<thread>
#include<functional>
#include<memory>
#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
using namespace std;

//////////////////////////////////////////////////////zero_thread.h
void getNow(timeval *tv);
int64_t getNowMs();
#define TNOW getNow()
#define TNOWMS getNowMs()

class ZERO_ThreadPool{
protected:
    struct TaskFunc{
        TaskFunc(int64_t expireTime) : _expireTime(expireTime){ }
        std::function<void()> _func;
        int64_t _expireTime = 0; //超时的绝对时间
    };
    typedef shared_ptr<TaskFunc> TaskFuncPtr;
public: 
    ZERO_ThreadPool();
    virtual ~ZERO_ThreadPool();
    bool init(size_t num);
    size_t getThreadNum()  //获取线程个数
    {
        std::unique_lock<std::mutex> lock(_mutex);
        return _threads.size();
    }
    size_t getJobNum(){    //获取任务个数
        std::unique_lock<std::mutex> lock(_mutex);
        return _tasks.size();
    }
    void stop(); //停止所有线程
    bool start(); //用于线程池启动任务
    //用线程池启动任务,返回任务的future对象, 可以通过这个对象来获取返回值
    template<class F,class... Args>
    auto exec(F&&f,Args&&... args) -> std::future<decltype(f(args...))>{//`F&& f`和`Args&&... args`使用了右值引用，可以实现对传入函数和参数的移动语义和完美转发。这样做可以提高性能，避免不必要的拷贝，并保持传入参数的原始类型和引用性质
        return exec(0,f,args...);
    }
    //用线程池启用任务(F是function, Args是参数),这里并没有开始执行任务，只是把任务封装并添加到任务队列中
    template<class F,class... Args>
    auto exec(int64_t timeoutMs,F&& f,Args&&... args) -> std::future<decltype(f(args...))>{
        int64_t expireTime = (timeoutMs == 0 ? 0 : TNOWMS + timeoutMs); // 获取现在时间
        //定义返回值类型
        using RetType = decltype(f(args...)); // 推导返回值,using的作用是将返回值类型取别名
        //封装任务
        //综合起来，通过使用`bind`函数对函数`f`和参数`args`进行绑定，能够实现对任务和参数的封装，并确保参数的类型和引用性质能够正确传递给`packaged_task`对象。这种方法可以保证在异步执行任务时，任务和参数能够正确地传递和执行。
        auto task=std::make_shared<std::packaged_task<RetType()>>(bind(std::forward<F>(f),std::forward<Args>(args)...));  //封装任务指针，设置过期时间
        TaskFuncPtr fptr=std::make_shared<TaskFunc>(expireTime);
        fptr->_func=[task](){  //具体的执行函数
            (*task)();
        };
        std::unique_lock<std::mutex> lock(_mutex);
        _tasks.push(fptr);   //插入任务
        _condition.notify_one();    //唤醒苏测得线程，可以考虑只有任务队列为空的时候再去noify
        return task->get_future();
    }
    bool waitForAllDone(int millsecond = -1);  //等待当前任务队列中, 所有工作全部结束(队列无任务).
protected:
    bool get(TaskFuncPtr&task); //获取任务
    bool isTerminate(){return _bTerminate;} //线程是否退出
    void run();  //线程运行态
protected:
    /**
    * 任务队列
    */
    queue<TaskFuncPtr> _tasks;
    /**
    * 工作线程
    */
    std::vector<thread*> _threads;
    std::mutex _mutex;
    std::condition_variable _condition;
    size_t _threadNum;
    bool _bTerminate;
    std::atomic<int> _atomic{ 0 };
};

///////////////////////////////////////////////////////zero_thread.cpp
ZERO_ThreadPool::ZERO_ThreadPool() : _threadNum(1),_bTerminate(false)
{
}

ZERO_ThreadPool::~ZERO_ThreadPool(){
    stop();
}

bool ZERO_ThreadPool::init(size_t num){
    std::unique_lock<std::mutex> lock(_mutex);
    if(!_threads.empty()){
        return false;
    }
    _threadNum=num;
    return true;
}

void ZERO_ThreadPool::stop(){
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _bTerminate=true;
        _condition.notify_all();
    }
    for(size_t i=0;i<_threads.size();i++){
        if(_threads[i]->joinable()){
            _threads[i]->join();
        }
        delete _threads[i];
        _threads[i]=NULL;
    }
    std::unique_lock<std::mutex> lock(_mutex);
    _threads.clear();
}

bool ZERO_ThreadPool::start(){
    std::unique_lock<std::mutex> lock(_mutex);
    if(!_threads.empty()){
        return false;
    }
    for(size_t i=0;i<_threadNum;i++){
        _threads.push_back(new thread(&ZERO_ThreadPool::run,this));
    }
    return true;
}

bool ZERO_ThreadPool::get(TaskFuncPtr& task){   //从任务队列里获取任务
    std::unique_lock<std::mutex> lock(_mutex);
    if(_tasks.empty()){
        _condition.wait(lock,[this]{return _bTerminate || !_tasks.empty();});
    }
    if(_bTerminate){
        return false;
    }
    if(!_tasks.empty()){
        task=std::move(_tasks.front());
        _tasks.pop();
        return true;
    }
    return false;
}

void ZERO_ThreadPool::run(){  //执行任务的线程
    //调用处理部分
    while(!isTerminate()){
        TaskFuncPtr task;
        bool ok=get(task);
        if(ok){
            ++_atomic;  //当前执行的任务量
            try{
                if(task->_expireTime!=0&&task->_expireTime<TNOWMS){
                    //超时任务，是否需要处理？
                }else{
                    task->_func();//执行任务
                }
            }
            catch(...){

            }
            --_atomic;
            //任务都执行完毕了
            std::unique_lock<std::mutex> lock(_mutex);
            if(_atomic==0&&_tasks.empty()){
                _condition.notify_all(); // 这里只是为了通知waitForAllDones
            }
        }
    }
}

bool ZERO_ThreadPool::waitForAllDone(int millsecond){
    std::unique_lock<std::mutex> lock(_mutex);
    if(_tasks.empty()){
        return true;
    }
    if(millsecond<0){
        _condition.wait(lock,[this]{return _tasks.empty();});
        return true;
    }else{
        return _condition.wait_for(lock,std::chrono::milliseconds(millsecond),[this]{return _tasks.empty();});
    }
}

int gettimeofday(struct timeval &tv)
{
    #if WIN32
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year = wtm.wYear - 1900;
    tm.tm_mon = wtm.wMonth - 1;
    tm.tm_mday = wtm.wDay;
    tm.tm_hour = wtm.wHour;
    tm.tm_min = wtm.wMinute;
    tm.tm_sec = wtm.wSecond;
    tm. tm_isdst = -1;
    clock = mktime(&tm);
    tv.tv_sec = clock;
    tv.tv_usec = wtm.wMilliseconds * 1000;
    return 0;
    #else
    return::gettimeofday(&tv,0);
    #endif
}

void getNow(timeval *tv)
{
    #if TARGET_PLATFORM_IOS || TARGET_PLATFORM_LINUX
    int idx = _buf_idx;
    *tv = _t[idx];
    if(fabs(_cpu_cycle - 0) < 0.0001 && _use_tsc)
    {
    addTimeOffset(*tv, idx);
    }
    else
    {
    TC_Common::gettimeofday(*tv);
    }
    #else
    gettimeofday(*tv);
    #endif
}
int64_t getNowMs()
{
    struct timeval tv;
    getNow(&tv);
    return tv.tv_sec * (int64_t)1000 + tv.tv_usec / 1000;
}

///////////////////////////////////////////////////////main.cpp
void func0()
{
    std::cout << "func0()" << endl;
}
    void func1(int a)
{
    std::cout << "func1() a=" << a << endl;
}
void func2(int a, string b)
{
    std::cout << "func1() a=" << a << ", b=" << b<< endl;
}

void test1() // 简单测试线程池
{
    ZERO_ThreadPool threadpool;
    threadpool.init(2);
    threadpool.start(); // 启动线程池
    // 假如要执行的任务
    threadpool.exec(1000,func0);
    threadpool.exec(func1, 10);
    threadpool.exec(func2, 20, "jerry");
    threadpool.waitForAllDone();
    threadpool.stop();
}

int func1_future(int a)
{
    std::cout << "func1() a=" << a << endl;
    return a;
}
string func2_future(int a, string b)
{
    std::cout << "func1() a=" << a << ", b=" << b<< endl;
    return b;
}

void test2() // 测试任务函数返回值
{
    ZERO_ThreadPool threadpool;
    threadpool.init(2);
    threadpool.start(); // 启动线程池
    // 假如要执行的任务
    std::future<decltype (func1_future(0))> result1 = threadpool.exec(func1_future, 10);
    std::future<string> result2 = threadpool.exec(func2_future, 20, "jerry");
    // auto result2 = threadpool.exec(func2_future, 20, "jerry");
    std::cout << "result1: " << result1.get() << std::endl;
    std::cout << "result2: " << result2.get() << std::endl;
    threadpool.waitForAllDone();
    threadpool.stop();
}

int main()
{
    test1(); // 简单测试线程池
    //test2(); // 测试任务函数返回值
    std::cout << "main end" << endl;
    return 0;
}