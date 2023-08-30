#include <iostream>
#include <unordered_map>
#include <string>

namespace cs425 {

struct Point {
    int x;
    int y;
    
    struct Pet {
        std::string name = "fluffy";
    };
    
    Pet mypet;
};

}

using namespace std;
int main(int argc, char *argv[]) {
    cs425::Point::Pet pet;
    
    std::unordered_map<int, std::string> MyMap;
    
    MyMap[ 1 ] = "cat";
    MyMap[ 11 ] = "dog";
    MyMap[ -31337 ] = "fish";
    
    std::cout << MyMap[1] << ". end of line" << std::endl;
    std::cout << MyMap[11] << ". end of line" << std::endl;
    std::cout << MyMap[-31337] << ". end of line" << std::endl;
    std::cout << MyMap[100] << ". end of line" << std::endl;
    
    for( const auto& key_and_value : MyMap ) {
        std::cout << "MyMap[" << key_and_value.first << "]: " << key_and_value.second << std::endl;
    }
    
    std::vector< int > MyInts;
    MyInts.resize(10);
    MyInts[5] = 500;
    
    MyInts.push_back( -31337 );
    
    for( const auto& i : MyInts ) {
        std::cout << i << std::endl;
    }
    
    Point p;// = {};
    std::cout << p.x << std::endl;
}
