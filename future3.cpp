#include<iostream>
#include<thread>
#include<future>
#include<string>
using namespace std;

void print(promise<string>& p) {
    p.set_value("There is the result whitch you want.");
}

void do_some_things() {
    cout << "Hello World!" << endl;
}

int main() {
    promise<string> promise;
    future<string> result = promise.get_future();
    thread t(print, ref(promise));
    do_some_things();
    cout << result.get() << endl;
    t.join();
    return 0;
}