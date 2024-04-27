#include<iostream>
#include<functional>
using namespace std;
class A{
public:
    void fun_3(int k,int m){
        cout<<"fun_3 a="<<a<<endl;
        cout<<"print :k="<<k<<",m="<<m<<endl;
    }
    void fun_3(string str){
        cout<<"print: str="<<str<<endl;
    }
    int a;
};

void fun_1(int x,int y,int z){
    cout<<"fun_1 print:x="<<x<<",y="<<y<<",z="<<z<<endl;
}

void fun_2(int &a,int &b){
    a++;
    b++;
    cout<<"print: a="<<a<<",b="<<b<<endl;
}

int main(){
    // //1.
    // auto f1=bind(fun_1,1,2,3);
    // f1();

    // auto f11 =bind(fun_1, 10, 20, 30);
    // f11();

    // auto f2=bind(fun_1,placeholders::_1,placeholders::_2,3);
    // f2(1,2);
    // f2(10,20,30);

    //2.
    A a;
    a.a=10;
    //auto f5 = bind(&A::fun_3, &a, placeholders::_1, placeholders::_2);
    //f5(10,20);
    auto f6=bind((void(A::*)(int,int))&A::fun_3,a,placeholders::_1,placeholders::_2);
    f6(10,20);
    cout<<endl;

    auto f_str=bind((void(A::*)(string))&A::fun_3,a,placeholders::_1);
    f_str("jerry");

    return 0;
}