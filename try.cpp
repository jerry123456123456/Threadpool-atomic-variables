#if 0

#include<iostream>
#include<limits>
#include<stdexcept>
using namespace std;

void MyFunc(int c){
    if(c>numeric_limits<char>::max()){  //char类型的最大值
        throw invalid_argument("throw MyFunc argument too large.");
    }
}

int main(){
    try{
        MyFunc(256);
    }
    catch (invalid_argument& e){
        cerr<<"catch "<<e.what()<<endl;
        return -1;
    }
    return 0;
}

#endif

#include<iostream>
#include<limits>
#include<stdexcept>
using namespace std;
struct Test
{
    Test(const char* s, int i, double d)
    : s(s)
    , i(i)
    , d(d) {};
    const char* s;
    int i;
    double d;
    void print() const
    {
        printf("%s %d %.2f\n", s, i, d);
    }
};
int main()
{
    try
    {
        throw Test("LLF", 520, 13.14);
    }
    catch (const Test& e)
    {
        e.print();
    }
    return 0;
}
