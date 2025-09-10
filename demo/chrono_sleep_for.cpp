#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
int main(int argc, char *argv[]) {
    while( true ) {
        const auto t1 = std::chrono::steady_clock::now();
        // Do some work
        // ...
        std::cout << "It's now!\n";
        const auto t2 = std::chrono::steady_clock::now();
        
        // Our desired total time.
        double h = 2.0;
        
        // Sleep for the difference.
        cout << "About to sleep\n";
        std::this_thread::sleep_for( std::chrono::duration<double>(h) - (t2-t1) );
        cout << "Done\n";
    }
}
