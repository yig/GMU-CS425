#include <iostream>
#include <cstdint> // For int64_t

// If you want to use the `EntityID` struct directly as the key for a data structure
// like an `std::unordered_map` or `std::unordered_set`, use its `.id`.
// Otherwise, you will have to write additional code to tell the data structure
// how to hash and compare `EntityID`s. See `docs/entity_get_in_an_unordered_map.txt`
// for details.

// An EntityID struct that stores the actual ID and supports `.Get<Component>()`
struct EntityID {
    // The id is a 64-bit integer.
    typedef int64_t IDType;
    // Instance variable holding the actual ID, initialized to -1.
    IDType id{-1};
    // Construct this object with the actual ID.
    EntityID( IDType val ) : id(val) {}
    // We still want the default constructor.
    EntityID() = default;
    
    // This EntityID can convert itself to an int64_t on demand.
    operator IDType() { return id; }
    
    // This EntityID supports `.Get<Component>()`
    // The definition for this function declaration is below, since it uses the global ECS that hasn't been declared yet.
    template <typename T>
    T& Get();
};

// A fake Entity Component System
// It always returns the same (static) component.
struct ECS {
    template <typename T>
    T& Get( EntityID ) { static T val{}; return val; }
};

// A global variable holding the ECS. This technique also works if the ECS is a member of another global variable.
inline ECS ecs;

// The implementation of EntityID::Get<Component>() calls the global ECS.
template <typename T> T& EntityID::Get() { return ecs.Get<T>(*this); }

int main(int argc, char *argv[]) {
    using namespace std;
    
    // Make two entities.
    EntityID a(1);
    EntityID b(2);
    
    // This works (because of the int constructor, not because of the `operator int()`).
    b = 3;
    
    // We can print them out and compare them, since they convert to integers on demand.
    cout << "a: " << a << '\n';
    cout << "b: " << b << '\n';
    cout << "a == b: " << (a == b) << '\n';
    cout << "a == a: " << (a == a) << '\n';
    
    // Let's use this struct as a Component.
    struct Foo {
        int value{};
        
        // Let's make this Struct convert to and from ints for convenience, too.
        Foo( int v ) : value( v ) {}
        operator int() { return value; }
        // Just because we have an int constructor doesn't mean we don't still want an empty constructor.
        Foo() = default;
    };
    
    // Let's modify a's Foo.
    a.Get<Foo>() = 7;
    cout << "a.Get<Foo>(): " << a.Get<Foo>() << '\n';
    
    return 0;
}
