#include <iostream>

void repeat( int count, const std::function<void()>& callback ) {
    for( int i = 0; i < count; ++i ) {
        callback();
    }
}

void printhi() {
    std::cout << "Hi!\n";
}

int main(int argc, char *argv[]) {
    repeat( 3, printhi );
}
