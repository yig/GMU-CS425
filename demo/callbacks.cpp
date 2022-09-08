#include <iostream>
#include <functional>

// This doesn't work (reference isn't const):
// void repeat( int count, std::function<void()>& callback ) {
// This does work (const reference):
// void repeat( int count, const std::function<void()>& callback ) {
// This works (pass by value):
void repeat( int count, std::function<void()> callback ) {
    for( int i = 0; i < count; ++i ) {
        callback();
    }
}

void printhi() {
    std::cout << "Hi!\n";
}

int main(int argc, char *argv[]) {
    repeat( 3, printhi );
    
    repeat( 3, [&](){ std::cout << "Hi again!\n"; } );
    
    int value = 7;
    repeat( 3, [&](){
        value += 1;
    } );
    std::cout << "Value: " << value << '\n';
}
