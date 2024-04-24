#include<iostream>
#include<thread>
#include<string.h>
using namespace std;

//����0��ֵ
void func1(){
    cout<<"func1 into"<<endl;
}

//����2��ֵ
void func2(int a,int b){
    cout<<"func2 a+b="<<a+b<<endl;
}

//��������
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
    this_thread::sleep_for(chrono::seconds(1));  //������������ֻ�����߳�����һ��
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
    thread t3(func3,ref(c));   //`std::ref()` ��һ������ģ�壬λ�� `<functional>` ͷ�ļ��С������ڽ�һ�������װ��һ�� `std::reference_wrapper` ���󣬴Ӷ���������Ϊ���ô��ݸ�������������
    t3.join();
    cout<<"main3 c="<<&c<<", "<<c<<endl;

#if 0
    //4.
    cout<<"\n\nmain4----------------------------------\n";
    A *a4_ptr=new A();
    a4_ptr->setName("jerry");
    //thread t4(A::func4,a4_ptr,10);
    thread t4([a4_ptr](){ a4_ptr->func4(10); });
    delete a4_ptr;    //���ɾ���������߳�ɾ���ģ�������t4�߳̽���ǰ����
    //��δ��������⣺
    //��������߳� `t4` �ĺ��������в�����ָ����ɾ���Ķ����ָ�롣
    //�����߳��д�����һ������ `a4_ptr`��Ȼ��ͨ�� Lambda �������������ָ�룬���� Lambda ����ִ��ʱ�����ܻ����ָ���Ѿ���ɾ����������Ӷ�����δ�������Ϊ��
    //Ϊ�˱������������Ӧ��ȷ�����߳�ִ�����ǰ����Ҫɾ��������Ķ���ָ�롣һ�ֽ�������ǵȴ��߳�ִ����Ϻ���ɾ������ָ�롣
#endif

    //4.
    cout << "\n\nmain4----------------------------------\n";
    A *a4_ptr = new A();
    a4_ptr->setName("jerry");
    thread t4([a4_ptr]() { 
        a4_ptr->func4(10); 
        delete a4_ptr; // ���߳�ִ����Ϻ�ɾ������ָ��
    });
    t4.join(); // �ȴ��߳�ִ�����

    //5.
    cout<<"\n\nmain5----------------------------------\n";
    thread t5(func5);
    cout << "pid: " << t5.get_id() << endl;
    cout << "joinable: " << t5.joinable() << endl;
    t5.detach();
    // cout<<"pid: "<<t5.get_id()<<endl;     ���̷߳���������������޷������߳�
    // cout<<"joinable: "<<t5.joinable()<<endl;
    this_thread::sleep_for(chrono::seconds(2));
    cout<<"\n  main5 end \n";

    //6.move
    cout<<"\n\nmain6----------------------------------\n";
    int x=10;
    thread t6_1(func6);
    thread t6_2(move(t6_1));
    //t6_1.join();     �׳��쳣,��Ϊmove�ı����Ǽ��У�t6_1������Ȩ���Ƶ���t6_2
    t6_2.join();

    return 0;
}