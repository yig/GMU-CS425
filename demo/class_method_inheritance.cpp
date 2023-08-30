#include <iostream>
#include <string>

class Shape {
public:
    uint64_t NumberOfSides = 3;
    uint64_t Dimension = 2;
    
    Shape() {
        std::cout << "Shape constructor\n";
    }
    virtual ~Shape() {
        std::cout << "Shape destructor\n";
    }
    
    virtual std::string GetName() const = 0;
};

class Square : public Shape {
public:
    Square() {
        NumberOfSides = 4;
    }
    
    std::string GetName() const override {
        return "Square!";
    }
};

void PrintShapeName( Shape& s ) {
    std::cout << s.GetName() << std::endl;
}

int add( int a, int b = 1 ) {
    return a + b;
}

using namespace std;
int main(int argc, char *argv[]) {
    Square s;
    s.Dimension = 4;
    PrintShapeName(s);
    
    std::cout << add(5) << '\n';
    std::cout << add(5,1) << '\n';
    std::cout << add(5,2) << '\n';
}