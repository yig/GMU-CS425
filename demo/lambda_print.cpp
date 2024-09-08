#include <iostream>
#include <functional>

void PrintSomething( std::function<std::string(void)> f ) {
    std::cout << f() << std::endl;
}

std::string Moo() {
    return "Moo";
}

using namespace std;
int main(int argc, char *argv[]) {
    PrintSomething(Moo);
    
    std::string utterance("Woof");
    
    auto f = [&]() -> std::string {
        return utterance;
    };
    
    PrintSomething(f);
    
    utterance = "Meow";
    
    PrintSomething(f);
}
