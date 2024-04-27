#include<iostream>
#include<functional>
#include<string>
using namespace std;

//保存普通函数
void func1(int a){
    cout<<"a="<<a<<endl;
}

//保存成员函数
class A{
public:
    A(string name) : name_(name){}
    void func3(int i) const {cout<<name_<<", "<<i<<endl;}
private:
    string name_;
};

int main(){
    cout<<"main1----------------------"<<endl;
    //1.保存普通函数
    function<void(int a)> func1_;
    func1_=func1;
    func1_(3);

    cout<<"main2----------------------"<<endl;
    //2.保存lambada表达式
    function<void()> func2_=[](){cout<<"hello world!\n";};
    func2_();

    cout<<"main3----------------------"<<endl;
    //3.保存成员函数
    function<void(const A&,int)> func3_=&A::func3;
    A a("jerry");
    func3_(a,1);

    return 0;
}