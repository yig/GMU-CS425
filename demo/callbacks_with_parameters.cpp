#include <iostream>
#include <functional>

class Engine {
public:
    int cylinders = 8;
};

void print_engine( Engine& e ) {
    std::cout << "Engine cylinders: " << e.cylinders << '\n';
}

void RunEngine( int cylinders, std::function<void( Engine& e )> callback ) {
    Engine e;
    e.cylinders = cylinders;
    
    callback( e );
}

int main(int argc, char *argv[]) {
    RunEngine( 8, print_engine );
    
    RunEngine( 1, print_engine );
}
