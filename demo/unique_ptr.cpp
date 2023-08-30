#include <iostream>
#include <memory>

struct Point {
    int x = 31337;
    int y = 100;
    
    virtual ~Point() = default;
};

using namespace std;
int main(int argc, char *argv[]) {
    auto p = std::make_unique<Point>();
    
    cout << p->x << '\n';
    
    auto q = p;
    q->x = -1;
    
    cout << p->x << '\n';
}
