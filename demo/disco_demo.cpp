#include "ballroom.h"

#include <iostream>

int main( int argc, const char* argv[] ) {
    using namespace disco;
    
    Party p;
    p.AddDancer( Dancer( "cat" ) );
    p.AddDancer( Dancer( "dog" ) );
    p.AddDancer( Dancer( "fish" ) );
    
    auto who = p.WhoIsAtThisParty();
    for( auto s : who ) {
        std::cout << s << '\n';
    }
    
    return 0;
}
