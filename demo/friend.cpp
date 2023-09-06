#include <iostream>

// Forward declaration isn't necessary, but allows us to declare in any order.
class A;
class B;

class A {
private:
    int m_private = 31337;
    
    // Without this `friend` line, B can't access A's private members.
    friend class B;
};

class B {
public:
    void WhatAreFriendsFor( const A& a ) {
        std::cout << "B can access A! a.m_private: " << a.m_private << '\n';
    }
};

int main( int argc, char* argv[] ) {
    A a;
    B b;
    b.WhatAreFriendsFor( a );
}
