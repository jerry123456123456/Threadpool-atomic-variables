#if 0  
//可变模版参数函数
#include<iostream>
using namespace std;

template <class... T>
void f(T... args){
    cout << sizeof...(args)<< endl; // 使用展开操作符展开参数包
}

int main(){
    f();
    f(1, 2);
    f(1, 2.5, ""); 
    return 0;
}


//递归打印参数包
#include<iostream>
using namespace std;

//递归终止函数
void print(){
    cout<<"empty"<<endl;
}

//展开函数
template<class T,class ...args>
void print(T head,args... rest){
    cout << "parameter " << head << endl;
    print(rest...);
}

int main(){
    print(1,2,3,4);
    return 0;
}

#endif

#if 0
//逗号表达式展开参数包
#include<iostream>
using namespace std;

template<class T>
void printarg(T t){
    cout<<t<<endl;
}

// 展开参数包并打印每个参数
template<class ...Args>
void expand(Args... args){
    (printarg(args), ...); // 使用折叠表达式(逗号表达式)展开参数包并调用printarg函数
    cout << endl;
}

int main(){
    expand(1,2,3,4);
    return 0;
}
#endif 

//最终版本
#include <iostream>
using namespace std;
template<class F, class... Args>void expand(const F& f, Args&&...args)
{
    //这里用到了完美转发
    (f(forward<Args>(args)), ...);
}
int main()
{
    expand([](int i){cout<<i<<endl;}, 1,2,3);
    return 0;
}

