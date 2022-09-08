#include <iostream>
#include <fstream>
#include <filesystem>

int main(int argc, char *argv[]) {
    typedef std::filesystem::path path;
    
    const path root{ "data" };
    // Doesn't work, because / requires one of the two sides to be an std::filesystem::path
    // path hello = root / ("subdir" / "hello.txt");
    // Does work, because / works left-to-right.
    path hello = root / "subdir" / "hello.txt";
    
    std::cout << "Is '" << hello << "' a regular file? " << std::filesystem::is_regular_file( hello ) << '\n';
    
    std::ifstream in( hello );
    std::cout << in.rdbuf();
}
