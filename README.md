# IllEngine

In this class you will make your own game engine from scratch.

## Platform setup

The first thing we need is a compiler and build system. In this class, we will use [`xmake`](https://xmake.io/) as our build system.
`xmake` is simple, modern, cross-platform, and comes with a package manager.
There are [installation instructions](https://xmake.io/#/guide/installation) for many platforms.
On macOS, you can use [Homebrew](https://brew.sh/): `brew install xmake`.
On Windows, you can use `winget` (or other options): `winget install xmake`.
On Linux, there are instructions for various flavors.

You also need a compiler.
On macOS, you should install `Xcode` with the `App Store`.
On Windows, install Visual Studio.
On Linux, install gcc or clang.

One you have a compiler and `xmake` installed, we are ready to begin.

## Making a project

Create a new directory. Since we're making a "little engine", I called mine `illengine` (🤷).
Create a file `xmake.lua` and put the following inside:

```
add_rules("mode.debug", "mode.release")

target("helloworld")
    set_kind("binary")
    set_languages("cxx17")
    
    add_files("demo/helloworld.cpp")
```

This declares that we want to support debug and release builds in general.
It also declares a target program named `helloworld`.
(It would have declared a library if instead of `binary` we had written `static` or `shared`.)
We want to use the C++17 standard.
This Hello, World! program will verify that your environment is working correctly.
Let's put the program in a `demo` directory. Create `demo/helloworld.cpp` and put the most basic C++ program inside:

```
#include <iostream>

int main( int argc, const char* argv[] ) {
    std::cout << "Hello, World!\n";
    return 0;
}
```

Then type `xmake` on the command line. Unless something has gone wrong, you should see something like:

```
[ 25%]: ccache compiling.release demo/helloworld.cpp
[ 50%]: linking.release helloworld
[100%]: build ok!
```

`xmake` puts the resulting program (`binary`) somewhere inside a new directory it creates (and manages) named `build`. You can run it directly (for me, it's `./build/macosx/arm64/release/helloworld`). It's easier to just ask `xmake` to run it for you with `xmake run helloworld`. This will print:

```
Hello, World!
```

Some useful `xmake` commands:

* `xmake run -d helloworld` launches the program in a debugger.
* `xmake f --menu` opens a command-line menu system you can use to switch between debug and release modes.
* `xmake f -m debug` and `xmake f -m release` directly switch between debug and release mode.

## Setting up version control

Before going any further, let's set up version control with git. You can use git on the command line or using your favorite git gui. Before we proceed with git, we need to create a `.gitignore` file at the top level of our project directory so we don't add build products or other extraneous files to our git repository. Put this inside:

```
# Xmake
/build
/.xmake

# macOS
.DS_Store
# Windows
Thumbs.db
```

Now you should initialize your git repository and make a first commit.

**You have reached the first checkpoint.** Upload your code. Run `xmake clean` and then zip your entire directory. Your directly tree should look like:

```
.git/
    ... lots of stuff
.gitignore
xmake.lua
demo/
    helloworld.cpp
```

## Starting your engine

It's time to start designing and building your game engine. We will work in the following order:

1. Create a window (the beginning of a `GraphicsManager`).
2. Create a game loop (the core of the `Engine`).
3. Detect input (`InputManager`).
4. Play sounds on keypress (`SoundManager` and `ResourceManager`).
5. Draw a textured square (most of the `GraphicsManager`).
6. Manage game objects (`EntityManager`).
7. Add scripting support (`ScriptManager`).

We'll put our engine source code in a new directory called `src` next to `demo`. Create a file named something like `Engine.h`. The `Engine.h` header will declare an `Engine` class that stores all of the various managers, starts them up, runs the game loop, and shuts the managers down. We will also need a new target in our `xmake.lua`, a library for our engine. Since I'm calling mine `illengine`, that's what I'm calling this target. Let's make it a `static` library, since those are usually less trouble. (Our operating system doesn't have to worry about finding them dynamically.)

```
target("illengine")
    set_kind("static")
    set_languages("cxx17")
    
    -- Declare our engine's header path.
    -- This allows targets that depend on the engine to #include them.
    add_includedirs("src", {public = true})
    
    -- Add all .cpp files in the `src` directory.
    add_files("src/*.cpp")
```

Let's make our `helloworld` target depend on `illengine` by adding the line `add_deps("illengine")` to it:

```
target("helloworld")
    set_kind("binary")
    set_languages("cxx17")
    
    add_deps("illengine")
    
    add_files("demo/helloworld.cpp")
```

## Digression on C++ Software Design

In this course you have a certain amount of architectural freedom. So long as you create the desired re-usable functionality, you can stray from the minor decisions I am recommending here.) In C++, the only difference between a `struct` and a `class` is that `struct` members are `public` by default, whereas `class` members are `private` by default. Which you choose is a matter of taste. Generally, I use `struct`s when I'm primarily grouping data together and classes when I want methods to execute close to some data. Be generous with `struct`s. You might organize the parameters to a function as a `struct`. The `struct` (and `class`) member variables can be declared with sensible defaults (e.g., `bool fullscreen = false;`).

Namespaces are another important organizing principle in modern C++. By wrapping your header declarations in a `namespace Foo { ... }`, you prevent accidental collisions. In my engine, all my headers have a `namespace illengine { ... }`. Inside a `.cpp` file or a function, you can write `using namespace Foo` to avoid having to prefix all your names with `Foo`. Where in C you might declare a static variable `static int global_thing;` in a `.c` file, in C++ you can declare an anonymous namespace `namespace { int global_thing; }` in a `.cpp` file for the same effect.

## Back to the Engine: Declaring Common Types

In any large software project, it's a good idea to make a header file containing common types. For example, you might create a file `Types.h` which declares what a real-number type and string type are throughout your project. For example:

```
#pragma once

#include <string>

namespace Foo {
    typedef double real;
    typedef std::string string;
    
    ...
}
```

You will need to `#include <string>` in order to have access to the C++ standard library's string type.

This header is also a good place for "forward declarations". Because C++ uses file-based compilation units rather than some kind of module system (that is, until the very recent C++20 standard), we need to be careful to avoid including the same header file multiple times. The easiest way to do this is by putting the line `#pragma once` at the top of every header file. While technically not standard, it's [supported by all the major C++ compilers](https://en.wikipedia.org/wiki/Pragma_once#Portability) and a lot less typing and redundancy than the [header guards](https://en.wikipedia.org/wiki/Include_guard) you may have seen in C or in C++ in the past. However, `#pragma once` won't solve circular dependency problems, where, for example, the `GraphicsManager` needs to know about the `ResourceManager` and vice versa. The solution is to partially declare a class in advance. Writing `class Engine;` in our `Types.h` file is enough to let the compiler know that there is an `Engine` class in the `Foo` namespace. The forward declaration satisfies the compiler enough to let us declare a pointer or reference to the `Engine`. Before we actually use the `Engine` in a `.cpp` file, we will have to include its header. (Another solution to circular dependencies is the so-called ["pimpl pattern"](https://www.fluentcpp.com/2017/09/22/make-pimpl-using-unique_ptr/).)

## What is the Engine?

The `Engine` class is actually quite small and simple. In fact, it could just be a nested namespace named `Engine`. Data-wise, it stores all the managers (they could be global variables or members of the `Engine` class). All the managers get a reference to the engine so they can access each other. It takes some basic parameters are input (size of the window, whether to go full-screen, etc.). `Engine::Startup()` calls `Startup()` on the managers in the right order. `Engine::Shutdown()` does the same. Finally, `Engine` needs a method to run the game loop. That function should take a callback so that the game can have a chance to run an update function each time through the loop. Here is pseudocode in class form for an engine with graphics and input managers:

```
class Engine:
    public:
        GraphicsManager graphics
        InputManager input
        
        def Startup( GameParameters ):
            graphics.Startup()
            input.Startup()
        
        def Shutdown():
            input.Shutdown()
            graphics.Shutdown()
        
        def RunGameLoop( UpdateCallback ):
            while( True ):
                input.Update()
                
                UpdateCallback()
                
                graphics.Draw()
                
                // Manage timestep
}
```

This is just pseudocode. In practice, the engine probably needs to pass a reference to itself to all the managers so that they can access each other. (Another possibility is to make an Engine global variable.) Eventually, we will have even more managers. For this next checkpoint, you don't need to create an input manager.

You can manage the timestep using C++'s `std::this_thread::sleep_for()` and passing it a C++ `std::chrono::duration<>`.
You can get a double-valued timer by calling `glfwGetTime()` (which you can subtract and create a `std::chrono::duration<double>` from). See below for how to include `GLFW`. You don't need GLFW for this. You can instead use the C++ standard library directly by subtracting two `std::chrono::time_point`s, which you can get via `std::chrono::steady_clock::now()`. For example, `const auto t1 = std::chrono::steady_clock::now()` stores the current time in a variable `t1`. You can create a 0.1 second duration via `const auto one_tenth_of_a_second = std::chrono::duration<real>( 1./10. )`. You will need to `#include <thread>` and `#include <chrono>` to access the C++ standard library's functionality.

## The `GraphicsManager`

At this point, all we want from the graphics manager is to create a window. We will use [GLFW](https://www.glfw.org/) for this. It's an extremely lightweight graphics and input handling library. Declare your need for it at the top of your `xmake.lua` (just below the `add_rules()` line:

```
add_requires("glfw")
```

Inside `target("illengine")`, add it as a package:

```
    add_packages("glfw")
```

In your graphics manager C++ file, you will need to include the GLFW headers. You can do this in the implementation file only (the `.cpp` file). Users of your engine don't need to know that it uses GLFW to create a window. The engine is abstracting this detail! Since we are using `sokol_gfx` as our GPU API, we don't want GLFW to include any OpenGL headers directly for us. This is how to do it:

```
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
```

Here is a magic bit of code for your graphics manager's startup method to create a window and lock its aspect ratio:

```
glfwInit();
// We'll use sokol_gfx's OpenGL backend
glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );
glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
GLFWwindow* window = glfwCreateWindow( window_width, window_height, window_name, window_fullscreen ? glfwGetPrimaryMonitor() : 0, 0 );
glfwSetWindowAspectRatio( window, window_width, window_height );
if( !window )
{
    std::cerr << "Failed to create a window." << std::endl;
    glfwTerminate();
}
glfwMakeContextCurrent( window );
glfwSwapInterval(1);
```

This code expects a few parameters: the `window_width` and `window_height`, a name for the window (`window_name`), and a boolean to specify whether the window should be fullscreen (`window_fullscreen`). You can hard-code some reasonable values or you can find a way to take parameters. You may notice that this code snippet is asking for an OpenGL-compatible window. That's because we will use `sokol_gfx`'s OpenGL backend for cross-platform simplicity.

Finally, this is a good time to mention that I recommend a proper logging library rather than outputting to `std::cout` and `std::cerr`. You have a lot of options. I used [spdlog](https://github.com/gabime/spdlog). That way I can do things like: `spdlog::error( "Failed to create a window." );` or `spdlog::info( "Calling UpdateCallback()." );`. To use spdlog, you have to include it (`#include "spdlog/spdlog.h"`) and add it to your `xmake.lua` (`add_requires("spdlog")` near the top and `add_packages("spdlog")` in your `illengine` target).

**You have reached the second checkpoint.** Commit your code to your git repository. Upload it for grading. Run `xmake clean` and then zip your entire directory. This checkpoint should contain:

* A graphics manager class that creates a window.
* A game engine class that starts up the graphics manager and runs a game loop 60 times per second.
* `demo/helloworld.cpp` modified to start up your engine, run its main loop, and shut it down when the main loop terminates.

---

## ChangeLog

* 2022-08-22: First week.
* 2022-08-23: First two checkpoints.
* 2022-08-23: `Types.h` example is more complete. It now has a `#pragma once` and an `#include`.
* 2022-08-23: Added links for pragma once and header guards.
* 2022-08-25: Added `xmake` commands to switch to debug mode and run with a debugger. Simplified Engine pseudocode. Added clearer checkpoint guidelines.
* 2022-08-28: Clarified the relationship of the Engine pseudocode to the checkpoints. Clarified checkpoint.
