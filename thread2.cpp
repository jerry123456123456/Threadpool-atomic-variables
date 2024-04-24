//�̷߳�װ
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
    void join();     //�ȴ���ǰ�߳̽����������ٵ�ǰ�̵߳���
    void detach();   //���ڵ�ǰ�̵߳���
    static size_t  CURRENT_THREADID();
protected:
    void threadEntry();  
    virtual void run()=0;
protected:
    bool running_;  //�Ƿ�������
    thread *th_;
};

ZERO_Thread::ZERO_Thread():
    running_(false),th_(NULL)    //��д�������������������
{

}

ZERO_Thread::~ZERO_Thread(){
    if(th_!=NULL){
        if(th_->joinable()){
            //��C++�У����һ���̶߳����ǿ����ӵģ�joinable��������������������û�б���ʽjoin��detach����ô��������`std::terminate()`����ֹ�����ִ�У��Ӷ�����δ������Ϊ��
            cout<<"~ZERO_Thread detach"<<endl;
            th_->detach();
        }
        delete th_;
        th_=NULL;
    }
    cout<<"~ZERO_Thread()"<<endl;
}

bool ZERO_Thread::start(){
    if(running_){         //����Ѿ��������򷵻�false
        return false;
    }
    //`try`��`catch`��C++���쳣����Ĺؼ��֣����ڲ���ʹ����쳣����`try`���з��ÿ����׳��쳣�Ĵ��룬һ���쳣�׳��������������ת�������ƥ���`catch`���н����쳣����
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

//��`ZERO_Thread::isAlive()` ����Ϊ `const` ��Ϊ����ѭ���õı��ʵ����ȷ���������ֻ���ڲ�ѯ�����״̬�������޸Ķ���ĳ�Ա������
bool ZERO_Thread::isAlive() const{
    return running_;
}

void ZERO_Thread::join(){
    if(th_->joinable()){
        th_->join();     //����detach��ȡjoin 
    }
}

void ZERO_Thread::detach(){
    th_->detach();
}

//��δ���������ǻ�ȡ��ǰ�̵߳�ΨһID����ȷ��ÿ���߳�ֻ��ȡһ���Լ����߳�ID
size_t ZERO_Thread::CURRENT_THREADID(){
    // ����Ϊthread_local�ı��ر������߳����ǳ������ڵģ���ͬ����ͨ��ʱ�������������ڣ�
    // ������static����һ���ĳ�ʼ���������������ڣ���ʹ����������Ϊstatic��
    static thread_local size_t threadId=0;
    //- `thread_local` �ؼ�������ָʾ�������ֲ߳̾��洢�ģ�ÿ���̶߳����Լ��ı����������������š�
    // - �ڶ��̻߳����У���ͬ�̷߳��� `threadId` ����ʱ����ʸ����̵߳ĸ������������̼߳�ľ����ͳ�ͻ��
    if(threadId==0){
        //��δ���ȷ����ÿ���߳�ֻ�ڵ�һ�ε��� `CURRENT_THREADID()` ����ʱ��ȡ�Լ����߳�ID������������ֱ�ӷ���֮ǰ�����ֵ���������ظ���ȡ�߳�ID�Ŀ�����    
        stringstream ss;
        ss<<this_thread::get_id();
        threadId=strtol(ss.str().c_str(),NULL,0);  //�����е��ַ���ת��Ϊ�����ͣ�����ֵ��threadId
    }
    return threadId;
}

#if 0
��C++�쳣�����У�`catch(exception& ex)` �� `catch(...)` �����ֲ�ͬ���쳣����ʽ������֮����������£�

1. `catch(exception& ex)`��

   - ������ʽ�� `catch` ��䲶���ض����͵��쳣�����̳��� `std::exception` ���쳣���ڲ��񵽸����͵��쳣ʱ����ִ�ж�Ӧ�Ĵ�����롣

   - �ڴ����У�`running_` ������Ϊ `false`��Ȼ�������׳����񵽵��쳣 `ex`��ʹ���쳣���Ա��ϲ�����������

   - ���ַ�ʽ������ض����͵��쳣�������⴦�����Ը��ݾ�����쳣����ִ�в�ͬ���߼���

2. `catch(...)`��

   - ������ʽ�� `catch` �����ͨ�õ��쳣����ʽ�����Բ����κ����͵��쳣���������̳��� `std::exception` ���쳣��

   - �ڲ����κ����͵��쳣ʱ����ִ�ж�Ӧ�Ĵ�����롣

   - �ڴ����У�ͬ���� `running_` ����Ϊ `false`��Ȼ�������׳����񵽵��쳣��ʹ���쳣���Ա��ϲ�����������

   - ���ַ�ʽ�����ڴ����ض����͵��쳣��������һ��ͨ�õ� `catch` ���в��������쳣��

    �ܵ���˵��`catch(exception& ex)` ������ض����͵��쳣���д����� `catch(...)` ��ͨ�õ��쳣����ʽ�����Բ����κ����͵��쳣��
�ھ���ʹ��ʱ��������Ҫѡ����ʵ��쳣����ʽ������ͬ���͵��쳣��
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

