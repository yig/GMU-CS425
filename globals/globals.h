#pragma once

namespace globals {

// Declare a class.
class World {
public:
    void Print();
};

// Create a global variable of that class.
inline World globalWorld;
// The old way was to write `extern World globalWorld` in a header and
// `World globalWorld` in one `.cpp` file. That also works, and has the benefit
// that the `extern` declaration can be written without the definition of `World`.
// In other words, you don't have to #include the class header first.

}
