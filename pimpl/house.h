#pragma once

#include <memory>

class House {
public:
    // We need a constructor and destructor, even if they're empty,
    // in the .cpp file after we define the `Vars` struct.
    // The constructor will create the `vars`.
    House();
    ~House();
    
    void DoSomething();
    
private:
    // Forward declare the holder of our instance variables.
    struct Vars;
    std::unique_ptr< Vars > vars;
};
