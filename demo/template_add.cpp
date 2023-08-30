#include <iostream>

namespace cs425 {

template< typename T, typename U >
T add( T left, U right );

}

int main(int argc, char *argv[]) {
    using std::cout, std::endl;
    using namespace cs425;
    
    int a = 6;
    float b = 4.5;
    float c = add( a, b );
    
    cout << c << endl;
}
