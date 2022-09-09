#include "house.h"

#include "dresser.h"
#include "table.h"
#include "kitchen.h"

#include <iostream>

class House::Vars {
public:
    int age = 10;
    
    Dresser d;
    Table t;
    Kitchen k;
};

House::House() {
    // d.reset( new Dresser );
    
    vars = std::make_unique<Vars>();
}

House::~House() {
    // delete d;
}

void House::DoSomething() {
    std::cout << "My dresser has " << vars->d.num_drawers << " drawers.\n";
}
