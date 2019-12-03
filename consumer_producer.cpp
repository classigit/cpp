#include <random>
#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <condition_variable>

using namespace std;

queue<int> q;
condition_variable cv;
mutex mu;

void consumer() {
    while (true) {
        unique_lock<mutex> locker(mu);
        cv.wait(locker, []() { return !q.empty(); });
        cout << "pulling " << q.front() << endl;
        q.pop();
    }
}

void producer() {
    default_random_engine generator;
    uniform_int_distribution<int> distribution(1, 50);
    int num;

    while (true) {
        num = distribution(generator);
        lock_guard<mutex> lock(mu);
        cout << "pushing " << num << endl;
        q.push(num);
        cv.notify_one();
    }
}

int main() {

    thread t1(producer);
    thread t2(producer);
    thread t3(consumer);
    t1.join();
    t2.join();
    t3.join();
    return 0;
}
