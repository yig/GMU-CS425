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

set_policy("build.warning", true) -- show warnings
set_warnings("all") -- warn about many things

target("helloworld")
    set_kind("binary")
    set_languages("cxx17")
    
    add_files("demo/helloworld.cpp")
```

This declares that we want to support debug and release builds in general, and that we want to see lots of compiler warnings.
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

* `xmake` compiles all targets.
* `xmake run <target>` runs the given target.
* `xmake run -d helloworld` launches the program in a debugger. If you are using a debugger, you will want to compile in debug mode.
* `xmake config --menu` opens a command-line menu system you can use to switch between debug and release modes.
* `xmake config -m debug` and `xmake f -m release` directly switch between debug and release mode.
* `xmake project -k xcode -m debug` or `xmake project -k vsxmake -m debug` to generate an Xcode project on macOS or a Visual Studio solution on Windows. You make prefer debugging directly in the IDE. [[docs](https://xmake.io/#/plugin/builtin_plugins)]
* `xmake watch` will re-run `xmake` automatically when any code changes. `xmake watch -r -t helloworld` will do the same and then run the `helloworld` target.
* If you are having trouble with `xmake`, run with the flags `-vwD` to print a lot of diagnostic output.
* If you install or remove a compiler, run `xmake g -c` to clear the global compiler detection cache.
* `xmake run -w <path> <target>` runs the target with path as the working directory.

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
4. Draw a textured square (most of the `GraphicsManager`).
5. Manage game objects (`EntityManager`).
6. Add scripting support (`ScriptManager`).

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

### Digression on C++ Software Design

In this course you have a certain amount of architectural freedom. So long as you create the desired re-usable functionality, you can stray from the minor decisions I am recommending here.) In C++, the only difference between a `struct` and a `class` is that `struct` members are `public` by default, whereas `class` members are `private` by default. Which you choose is a matter of taste. Generally, I use `struct`s when I'm primarily grouping data together and classes when I want methods to execute close to some data. Be generous with `struct`s. You might organize the parameters to a function as a `struct`. The `struct` (and `class`) member variables can be declared with sensible defaults (e.g., `bool fullscreen = false;`).

Namespaces are another important organizing principle in modern C++. By wrapping your header declarations in a `namespace Foo { ... }`, you prevent accidental collisions. In my engine, all my headers have a `namespace illengine { ... }`. Inside a `.cpp` file or a function, you can write `using namespace Foo` to avoid having to prefix all your names with `Foo`. Where in C you might declare a static variable `static int global_thing;` in a `.c` file, in C++ you can declare an anonymous namespace `namespace { int global_thing; }` in a `.cpp` file for the same effect.

### Back to the Engine: Declaring Common Types

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

This header is also a good place for "forward declarations". Because C++ uses file-based compilation units rather than some kind of module system (that is, until the very recent C++20 standard), we need to be careful to avoid including the same header file multiple times. The easiest way to do this is by putting the line `#pragma once` at the top of every header file. While technically not standard, it's [supported by all the major C++ compilers](https://en.wikipedia.org/wiki/Pragma_once#Portability) and a lot less typing and redundancy than the [header guards](https://en.wikipedia.org/wiki/Include_guard) you may have seen in C or in C++ in the past. However, `#pragma once` won't solve circular dependency problems, where, for example, the `GraphicsManager` needs to know about the `ResourceManager` and vice versa. The solution is to partially declare a class in advance. Writing `class Engine;` in our `Types.h` file is enough to let the compiler know that there is an `Engine` class in the `Foo` namespace. The forward declaration satisfies the compiler enough to let us declare a pointer or reference to the `Engine`. Before we actually use the `Engine` in a `.cpp` file, we will have to include its header. (Another solution to circular dependencies is [`unique_ptr`](https://ortogonal.github.io/cpp/forward-declaration-and-smart-pointers/) and yet another is the so-called "pimpl pattern" (see [here](https://www.fluentcpp.com/2017/09/22/make-pimpl-using-unique_ptr/) or [here](pimpl/house.h)).)

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

This is just pseudocode. Alternatively, your engine's `RunGameLoop()` could call `Startup()` at the beginning of the function and `Shutdown()` at the end. In that case, `RunGameLoop()` would need to take another callback function to let the user of your engine perform game setup between `Startup()` and the game loop itself.

You will also need to consider how the managers can access each other. Eventually, we will have even more managers. For this next checkpoint, you don't need to create an input manager. Fortunately, if the managers can access the engine, they can access each other. One possibility is to make an engine global variable. In C++17, you can declare it right in the engine header as an [inline variable](https://stackoverflow.com/a/47502744). For example, if your class were named `Foo`, you could declare `inline Foo gFoo`, where the `g` prefix is [Hungarian notation](https://en.wikipedia.org/wiki/Hungarian_notation).) See [here](globals/global.h) for an example. If you are strongly allergic to global variables, the engine can instead pass a reference (or pointer) to itself to all the managers so that they can access each other. (References can never be stored uninitialized. If your managers will store a reference to the `Engine`, `Engine`'s constructor will have to pass `*this` to the constructors of all the managers in an [initializer list](https://en.cppreference.com/w/cpp/language/constructor), as in `Engine() : graphics( *this ) {}`. The managers in turn will need an initializer list to initialize their `Engine&`, as in `GraphicsManager( Engine& e ) : engine( e ) {}`. You can see an example in [here](demo/constructor_reference.cpp). If you want to wait to pass the `Engine&` until a startup method, you will have to use the "pimpl pattern" (see [here](https://www.fluentcpp.com/2017/09/22/make-pimpl-using-unique_ptr/) or [here](pimpl/house.h)). It's easier to store an `Engine*`, but then the compiler won't raise an error if you forget to set it.)

Managing the time step means making sure that your game loop runs 60 times per second. The code inside the loop should take less than 1/60 of a second, so your engine needs to sleep until the next iteration ([tick](https://gamedev.stackexchange.com/questions/81608/what-is-a-tick-in-the-context-of-game-development)) should start. (Handling slow downs is an [interesting topic](https://gafferongames.com/post/fix_your_timestep/).) You can manage the timestep using C++'s `std::this_thread::sleep_for()` and passing it a C++ `std::chrono::duration<>`. You can get a double-valued timer by calling `glfwGetTime()` (which you can subtract and create a `std::chrono::duration<double>` from). See below for how to include `GLFW`. You don't need GLFW to get the current time. You can instead use the C++ standard library directly by subtracting two `std::chrono::time_point`s, which you can get via `std::chrono::steady_clock::now()`. For example, `const auto t1 = std::chrono::steady_clock::now()` stores the current time in a variable `t1`. For example, you can create a 0.1 second duration via `const auto one_tenth_of_a_second = std::chrono::duration<real>( 1./10. )`. You will need to `#include <thread>` and `#include <chrono>` to access the C++ standard library's functionality. As another alternative, you could use [`sokol_time`](https://github.com/floooh/sokol/blob/master/sokol_time.h). You can `add_requires("sokol")` and `add_packages("sokol")` to access this header.

## The `GraphicsManager`

At this point, all we want from the graphics manager is to create a window. We will use [GLFW](https://www.glfw.org/) for this. It's an extremely lightweight graphics and input handling library. Declare your need for it at the top of your `xmake.lua` (just below the `add_rules()` line:

```
add_requires("glfw")
```

Inside `target("illengine")`, add it as a package:

```
    add_packages("glfw")
```

In your graphics manager C++ file, you will need to include the GLFW headers. You can do this in the implementation file only (the `.cpp` file). Users of your engine don't need to know that it uses GLFW to create a window. The engine is abstracting this detail! Since we are using `WebGPU` as our GPU API, we don't want GLFW to include any OpenGL headers directly for us. This is how to do it:

```
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
```

Here is a magic bit of code for your graphics manager's startup method to create a window and lock its aspect ratio:

```
glfwInit();
// We don't want GLFW to set up a graphics API.
glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
// Create the window.
GLFWwindow* window = glfwCreateWindow( window_width, window_height, window_name, window_fullscreen ? glfwGetPrimaryMonitor() : 0, 0 );
glfwSetWindowAspectRatio( window, window_width, window_height );
if( !window )
{
    std::cerr << "Failed to create a window." << std::endl;
    glfwTerminate();
}
```

This code expects a few parameters: the `window_width` and `window_height`, a name for the window (`window_name`), and a boolean to specify whether the window should be fullscreen (`window_fullscreen`). You can hard-code some reasonable values or you can find a way to take parameters. You might store the parameters in a configuration `struct` in the `Engine`. The user can set them directly or pass it in to your engine's startup method. Your graphics manager's shutdown only needs to call `glfwTerminate();`.

Finally, this is a good time to mention that I recommend a proper logging library rather than outputting to `std::cout` and `std::cerr`. You have a lot of options. I used [spdlog](https://github.com/gabime/spdlog). That way I can do things like: `spdlog::error( "Failed to create a window." );` or `spdlog::info( "Calling UpdateCallback()." );`. To use spdlog, you have to include it (`#include "spdlog/spdlog.h"`) and add it to your `xmake.lua` (`add_requires("spdlog")` near the top and `add_packages("spdlog")` in your `illengine` target).

Modify `demo/helloworld.cpp` to start up your engine, run its main loop, and shut it down when the main loop terminates.

**You have reached the second checkpoint.** Commit your code to your git repository. Upload it for grading. Run `xmake clean` and then zip your entire directory. This checkpoint should contain:

* A graphics manager class that creates a window.
* A game engine class that starts up the graphics manager and runs a game loop 60 times per second. Your game engine won't take a user update callback yet.
* The modified `demo/helloworld.cpp`.

## Detecting input

It's time to create an input manager. The input manager provides an interface to the various input hardware state, such as which keyboard/mouse/joystick buttons are pressed and where the mouse or joystick is. Each time the game loop runs, the engine gives the input manager a chance to process input events (`input.Update()` in the pseudocode above). A fancy input manager lets users map physical actions (e.g. pressing a hardware button) to game actions. We will start with a simple input manager that just makes GLFW's input state available for querying by users of our engine. The input manager's `Update()` method only needs to call `glfwPollEvents();`. We will create a method called something like `KeyIsPressed` that takes a key name as a parameter and returns a boolean. It will be a thin wrapped around [`glfwGetKey()`](https://www.glfw.org/docs/3.3/group__input.html#gadd341da06bc8d418b4dc3a3518af9ad2). You can make the key name an `int` and just pass it along directly to GLFW, or you can create your own `enum class` for key names that you expose to users. Your enum can choose the same numbers for keys as GLFW (see [here](https://www.glfw.org/docs/3.3/group__keys.html); you can cast your `enum class` to an `int`). It is fine to simply ask users to use GLFW key names (e.g. `GLFW_KEY_LEFT`). In that case, your input manager header should also include the GLFW header, and your `xmake.lua` should change `add_packages("glfw")` to `add_packages("glfw", {public = true})`.

There isn't a perfect separation between the graphics manager and the input manager. The input manager needs to call GLFW functions and pass them the `GLFWwindow*` created by the graphics manager. This is why your Engine stores all the managers—so they can access each other. Modify your graphics manager to store the `GLFWwindow*` as an instance variable. You can make it `private` and still let the input manager access it by writing `friend class InputManager;` anywhere inside your graphics manager class.

It's time to add a user update callback to your game engine. In C++, a reasonable type to use for the user's update callback is: `std::function<void(Engine&)>`. That is, the user's callback is a function that takes an `Engine&` as a parameter and doesn't return anything. If you have an `Engine` global variable, then you can just use `std::function<void()>`, which is a function that takes no parameters and returns nothing. You can typedef that and then use it as a parameter to your game loop. Example syntax:

```
typedef std::function<void()> UpdateCallback;
void RunGameLoop( const UpdateCallback& callback );
```

You will need to `#include <functional>` to access `std::function`. It's a general type that can take any kind of function a C++ user might want to pass. The user of your engine can create a [lambda](https://en.cppreference.com/w/cpp/language/lambda) function to pass, as in:

```
engine.RunGameLoop( [&]( Engine& ) {
    // code that will run inside the game loop
    } );
```

Modify your `helloworld` function to pass a callback when running your engine's main loop. The callback should print or log a message when at least one key of your choosing is pressed.

You may have noticed that there's no way to quit your program (without asking your operating system to terminate it, e.g., `^C`). Quitting is easy; you just break out of the main loop. The `RunGameLoop()` function will then return. You can do that in response to a key press like escape or Q. If you want to check if someone clicked the window's close box, call the GLFW function `glfwWindowShouldClose(window)`. You can set this programmatically with `glfwSetWindowShouldClose( window, true );`. You can design your own way to encapsulate those. It could be something like `GraphicsManager::ShouldQuit()` and `GraphicsManager::SetShouldQuit(true)` methods.

**You have reached the third checkpoint.** Upload your code. Run `xmake clean` and then zip your entire directory. For this checkpoint:

* Your engine should have an input manager that lets users detect key presses.
* Your engine should take a user callback to execute in its main loop.
* Your `demo/helloworld.cpp` should pass a callback that prints or logs a message when a key is pressed.
* Your program should terminate when someone clicks the close box.

## To be continued...

---

## ChangeLog

* 2022-08-22: First week.
* 2022-08-23: First two checkpoints.
* 2022-08-23: `Types.h` example is more complete. It now has a `#pragma once` and an `#include`.
* 2022-08-23: Added links for pragma once and header guards.
* 2022-08-25: Added `xmake` commands to switch to debug mode and run with a debugger. Simplified Engine pseudocode. Added clearer checkpoint guidelines.
* 2022-08-28: Clarified the relationship of the Engine pseudocode to the checkpoints. Clarified checkpoint.
* 2022-08-28: Described input manager and its checkpoint.
* 2022-08-29: More details about making the engine a global variable. Fixed a typo.
* 2022-08-29: Re-ordered the engine as a global variable discussion. More about engine configuration parameters. Described an alternative to the RunGameLoop pseudocode which itself calls Startup and Shutdown. Added the glfw call for the graphics manager's shutdown. Discussed the input manager accessing the graphics manager's window. Discussed exiting the game loop.
* 2022-08-29: Described the new `xmake watch` command.
* 2022-08-30: Mentioned ticks and linked to Fix Your Timestep.
* 2022-08-31: Added compiler warnings.
* 2022-08-31: Mentioned `unique_ptr` as a solution to forward declarations.
* 2022-08-31: Added checkpoint 4, sound and resource managers.
* 2022-09-01: Resource manager is now before sound manager.
* 2022-09-07: Checkpoint 5: Graphics manager. Also mentioned `sokol_time` in the main loop discussion and std::async for background resource loading.
* 2022-09-07: Mentioned `soloud_wav.h` header. Fixed an anachronism that mentioned the sound manager in detail while describing the resource manager.
* 2022-09-07: Showed an example of `std::filesystem::path` /. Fixed typos in graphics manager.
* 2022-09-08: A fix for transparent pixels when not depth sorting.
* 2022-09-08: Explaining how to depth sort for those who want.
* 2022-09-09: Mentioned initializer lists. Updated pimpl example.
* 2022-09-16: Warned not to use operator= for SoLoud::Wav.
* 2022-09-17: Clarified which parts of the graphics manager go in setup and which in the draw method.
* 2022-09-20: Mentioned `xmake project` command.
* 2022-09-23: Checkpoint 6: Entity Component System
* 2022-09-23: Fixed graphics manager texture coordinates. V was flipped.
* 2022-09-23: Changed the description of vertex attributes to make it clearer how to send more.
* 2022-09-23: Described extra uniforms and preparing uniforms for the fragment shader.
* 2022-09-23: Described a file watcher resource manager extension. Clarified that managers should declare their own components.
* 2022-09-24: Checkpoint 7: Script Manager.
* 2022-09-27: Reminder about lambdas in ECS example.
* 2022-09-28: Reminder that templates have to live in headers, so ECS will be header only.
* 2022-10-02: Added an example of how to replace a `for` loop with `ForEach` using the ECS.
* 2022-10-02: Added gotcha about copying a unique_ptr.
* 2022-10-04: Added inline global example.
* 2022-10-04: Added EntityID.Get<>() example.
* 2022-10-04: Mentioned `xmake -vwD` for diagnostic output.
* 2022-10-06: Added `demo/lua_parameters.cpp` example. Clarified that you really type the `...` for lua parameters.
* 2022-10-07: Added a note about explicitly declaring lambda return value to be by reference for scripting.
* 2022-10-12: Mention that running in a debugger should compile in debug mode.
* 2022-10-12: Mention how to new a user type in Lua.
* 2022-10-19: Straightened curly quotes.
* 2022-10-19: Recommend setting the working directory instead of copying assets.
* 2022-10-21: Suggest that components that are simple subclasses should provide a constructor that takes the superclass.
* 2022-10-26: Mentioned gcc slow compile with sol in debug mode.
* 2022-10-26: Added What's Next? discussion of text rendering and debug GUI.
* 2022-10-28: More information about Dear ImGui integration.
* 2022-11-10: Mentioned some networking libraries.
* 2022-11-26: Clear the global compiler detection cache with `xmake g -c`
* 2023-08-21: GLFW doesn't initialize an OpenGL context since we'll use WebGPU. Deleted remaining content in anticipation of changes.
* 2023-08-23: Fixed missing ) in the text.
