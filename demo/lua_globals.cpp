#include <iostream>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

int main(int argc, char *argv[]) {
    sol::state lua;
    lua.open_libraries(sol::lib::base);
    
    lua.script( R"(
        x = "cats"
        y = "dogs"
        local z = "parrots"
    )" );
    
    lua.script( R"(
        print( x )
        print( y )
        print( z ) -- nil
    )" );
    
    return 0;
}
