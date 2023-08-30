#include "templates.h"

#include <iostream>

int main( int argc, char* argv[] ) {
    int a = 6;
    float b = 4.5;
    float c = add( a, b );
    
    std::cout << c << std::endl;
}
