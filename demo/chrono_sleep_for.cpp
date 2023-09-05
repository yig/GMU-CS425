#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
int main(int argc, char *argv[]) {
    const auto t1 = std::chrono::steady_clock::now();
    // Do some work
    // ...
    const auto t2 = std::chrono::steady_clock::now();
    
    // Our desired total time.
    double h = .5;
    
    // Sleep for the difference.
    cout << "About to sleep\n";
    std::this_thread::sleep_for( std::chrono::duration<double>(h) - (t2-t1) );
    cout << "Done\n";
}
