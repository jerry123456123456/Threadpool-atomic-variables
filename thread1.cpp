#include<iostream>
#include<thread>
#include<string.h>
using namespace std;

//传入0个值
void func1(){
    cout<<"func1 into"<<endl;
}

//传入2个值
void func2(int a,int b){
    cout<<"func2 a+b="<<a+b<<endl;
}

//传入引用
void func3(int &c){
    cout<<"func3 c="<<&c<<endl;
    c+=10;
}

//
class A{
public:
    void func4(int a){
        cout<<"thread:"<<name_<<",func4 a="<<a<<endl;
    }
    void setName(string name){
        name_=name;
    }
    void displayName(){
        cout<<"this:"<<this<<",name:"<<name_<<endl;
    }
    void play(){
        cout<<"play call!"<<endl;
    }
private:
    string name_;
};

//
void func5(){
    cout<<"func5 into sleep"<<endl;
    this_thread::sleep_for(chrono::seconds(1));  //这个代码的作用只是让线程休眠一秒
    cout<<"func5 leave "<<endl;
}

//move
void func6(){
    cout<<"This is func6"<<endl;
}

int main(){
    //1.
    cout<<"\n\nmain1---------------------------------\n";
    thread t1(func1);
    t1.join();

    //2.
    cout<<"\n\nmain2---------------------------------\n";
    int a=10;
    int b=20;
    thread t2(func2,a,b);
    t2.join();

    //3.
    cout<<"\n\nmain3---------------------------------\n";
    int c=10;
    thread t3(func3,ref(c));   //`std::ref()` 是一个函数模板，位于 `<functional>` 头文件中。它用于将一个对象包装成一个 `std::reference_wrapper` 对象，从而允许将其作为引用传递给函数或函数对象。
    t3.join();
    cout<<"main3 c="<<&c<<", "<<c<<endl;

#if 0
    //4.
    cout<<"\n\nmain4----------------------------------\n";
    A *a4_ptr=new A();
    a4_ptr->setName("jerry");
    //thread t4(A::func4,a4_ptr,10);
    thread t4([a4_ptr](){ a4_ptr->func4(10); });
    delete a4_ptr;    //这个删除是在主线程删除的，可能在t4线程结束前调用
    //这段代码有问题：
    //问题出在线程 `t4` 的函数对象中捕获了指向已删除的对象的指针。
    //在主线程中创建了一个对象 `a4_ptr`，然后通过 Lambda 函数捕获了这个指针，但在 Lambda 函数执行时，可能会出现指针已经被删除的情况，从而导致未定义的行为。
    //为了避免这种情况，应该确保在线程执行完毕前，不要删除被捕获的对象指针。一种解决方法是等待线程执行完毕后再删除对象指针。
#endif

    //4.
    cout << "\n\nmain4----------------------------------\n";
    A *a4_ptr = new A();
    a4_ptr->setName("jerry");
    thread t4([a4_ptr]() { 
        a4_ptr->func4(10); 
        delete a4_ptr; // 在线程执行完毕后删除对象指针
    });
    t4.join(); // 等待线程执行完毕

    //5.
    cout<<"\n\nmain5----------------------------------\n";
    thread t5(func5);
    cout << "pid: " << t5.get_id() << endl;
    cout << "joinable: " << t5.joinable() << endl;
    t5.detach();
    // cout<<"pid: "<<t5.get_id()<<endl;     当线程分离后，这两个函数无法管理线程
    // cout<<"joinable: "<<t5.joinable()<<endl;
    this_thread::sleep_for(chrono::seconds(2));
    cout<<"\n  main5 end \n";

    //6.move
    cout<<"\n\nmain6----------------------------------\n";
    int x=10;
    thread t6_1(func6);
    thread t6_2(move(t6_1));
    //t6_1.join();     抛出异常,因为move的本质是剪切，t6_1的所有权被移到了t6_2
    t6_2.join();

    return 0;
}