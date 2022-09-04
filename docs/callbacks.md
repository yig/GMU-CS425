Callback functions let you write the following:

```
void repeat( int count, const std::function<void()>& callback ) {
    for( int i = 0; i < count; ++i ) {
        callback();
    }
}
```

What is that? It's a function that calls a given function (`callback`) a bunch of times. We can use it like this:

```
#include <iostream>
void printhi() {
    std::cout << "Hi!\n";
}

int main(int argc, char *argv[]) {
    repeat( 3, printhi );
}
```

Compiling and running this, we will see:

```
Hi!
Hi!
Hi!
```

Lambdas (inline, anonymous functions) are a great way to write callbacks. The following also works:

```
repeat( 3, [&](){ std::cout << "Hi again!\n"; } );
```

Compare `[&](){ std::cout << "Hi!\n"; }` to: `printhi(){ std::cout << "Hi!\n"; }`. The square brackets with the `&` let you use variables visible from main by reference. Our callback can affect state outside. For example:

```
int value = 7;
repeat( 3, [&](){
    value += 1;
} );
std::cout << "Value: " << value << '\n';
```
