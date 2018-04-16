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
        int a = q.front();
        cout << "pulling " << q.front() << endl;
        q.pop();
        if (a == 5) break;

    }
    cout << "done" << endl;
}

void producer() {
    srand((unsigned) time(nullptr));
    while (true) {
        this_thread::sleep_for(chrono::seconds(1));
        std::mt19937 rng;
        rng.seed(std::random_device()());
        std::uniform_int_distribution<std::mt19937::result_type> dist6(1, 10); // distribution in range [1, 6]
        auto num = dist6(rng);

        unique_lock<mutex> locker(mu);
        cout << "pushing " << num << endl;
        q.push(num);
        locker.unlock();
        cv.notify_one();

        if (num == 5) break;
    }
}

int main() {

    thread t2(producer);
    thread t1(consumer);
    t2.join();
    t1.join();
    return 0;
}