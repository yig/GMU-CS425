#pragma once

#include <memory>

// class Dresser;

class House {
public:
    // int age = 10;
    // std::unique_ptr< Dresser > d;
    // Dresser* d;
    
    House();
    ~House();
    
    void DoSomething();
    
private:
    class Vars;
    std::unique_ptr< Vars > vars;
};
