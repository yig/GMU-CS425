#include <iostream>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

int main(int argc, char *argv[]) {
    sol::state lua;
    lua.open_libraries(sol::lib::base);
    
    auto f = lua.load( R"(
        local a, b, c = ...
        
        print( a, b, c )
    )" );
    if( !f.valid() ) {
        std::cerr << "Failed to load script: " << sol::error(f).what() << std::endl;
        return -1;
	}
    
    f( 10, "foo", -1.5 );
    
    return 0;
}
