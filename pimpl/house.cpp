#include "house.h"

#include "dresser.h"
#include "table.h"
#include "kitchen.h"

class HouseImpl {
public:
    int age = 10;
    
    Dresser d;
    Table t;
    Kitchen k;
};

House::House() {
    // d.reset( new Dresser );
    
    pImpl = std::make_unique<HouseImpl>();
}

House::~House() {
    // delete d;
}
