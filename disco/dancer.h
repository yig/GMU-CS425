#pragma once

#include <string>

namespace disco {

struct Dancer {
    std::string name;
    
    Dancer() = default;
    Dancer( const std::string& n ) { name = n; }
};

}
