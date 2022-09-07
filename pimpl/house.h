#pragma once

#include <memory>

// class Dresser;

class HouseImpl;

class House {
public:
    // int age = 10;
    // std::unique_ptr< Dresser > d;
    // Dresser* d;
    
    House();
    ~House();
    
private:
    std::unique_ptr< HouseImpl > pImpl;
};
