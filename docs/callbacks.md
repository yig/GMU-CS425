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
