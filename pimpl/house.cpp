#include "house.h"

#include "dresser.h"
#include "table.h"
#include "kitchen.h"

#include <iostream>

struct House::Vars {
    int age = 10;
    
    Dresser d;
    Table t;
    Kitchen k;
};

House::House() {
    vars = std::make_unique<Vars>();
}

House::~House() {}

void House::DoSomething() {
    std::cout << "My dresser has " << vars->d.num_drawers << " drawers.\n";
}
