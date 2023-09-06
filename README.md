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

```c++
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

## Some useful `xmake` commands:

* `xmake` compiles all targets.
* `xmake run <target>` runs the given target.
* `xmake clean --all` clears xmake's build cache.
* `xmake run -d helloworld` launches the program in a debugger. If you are using a debugger, you will want to compile in debug mode.
* `xmake config --menu` opens a command-line menu system you can use to switch between debug and release modes.
* `xmake config -m debug` and `xmake f -m release` directly switch between debug and release mode.
* `xmake project -k xcode -m debug` or `xmake project -k vsxmake -m debug` to generate an Xcode project on macOS or a Visual Studio solution on Windows. You make prefer debugging directly in the IDE. [[docs](https://xmake.io/#/plugin/builtin_plugins)]
* `xmake watch` will re-run `xmake` automatically when any code changes. `xmake watch -r -t helloworld` will do the same and then run the `helloworld` target.
* If you are having trouble with `xmake`, run with the flags `-vwD` to print a lot of diagnostic output.
* If you install or remove a compiler, run `xmake global --clean` to clear the global compiler detection cache. `xmake config --clean` clears the cached user configuration and compiler detection cache.
* `xmake run -w <path> <target>` runs the target with path as the working directory.
* There are some other helpful questions and answers in the [`xmake` FAQ](https://xmake.io/#/guide/faq).
* The following error when `xmake` tries to download a package, `error: curl: (60) SSL certificate problem: unable to get local issuer certificate`, can be solved with [this workaround](https://xmake.io/#/guide/faq?id=what-should-i-do-if-the-download-package-failed-to-get-the-local-issuer-certificate): `xmake g --insecure-ssl=y`
* To update the packages `xmake` knows about from its repository, run `xmake repo -u`.

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

**You have reached the first checkpoint.** Upload your code. Run `xmake clean --all` and then zip your entire directory. Your directly tree should look like:

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

```c++
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

```python
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

You will also need to consider how the managers can access each other. Eventually, we will have even more managers. For this next checkpoint, you don't need to create an input manager. Fortunately, if the managers can access the engine, they can access each other. One possibility is to make an engine global variable. In C++17, you can declare it right in the engine header as an [inline variable](https://stackoverflow.com/a/47502744). For example, if your class were named `Foo`, you could declare `inline Foo gFoo`, where the `g` prefix is [Hungarian notation](https://en.wikipedia.org/wiki/Hungarian_notation).) See [here](demo/globals/globals.h) for an example. If you are strongly allergic to global variables, the engine can instead pass a reference (or pointer) to itself to all the managers so that they can access each other. (References can never be stored uninitialized. If your managers will store a reference to the `Engine`, `Engine`'s constructor will have to pass `*this` to the constructors of all the managers in an [initializer list](https://en.cppreference.com/w/cpp/language/constructor), as in `Engine() : graphics( *this ) {}`. The managers in turn will need an initializer list to initialize their `Engine&`, as in `GraphicsManager( Engine& e ) : engine( e ) {}`. You can see an example in [here](demo/constructor_reference.cpp). If you want to wait to pass the `Engine&` until a startup method, you will have to use the "pimpl pattern" (see [here](https://www.fluentcpp.com/2017/09/22/make-pimpl-using-unique_ptr/) or [here](pimpl/house.h)). It's easier to store an `Engine*`, but then the compiler won't raise an error if you forget to set it.)

Managing the time step means making sure that your game loop runs 60 times per second. The code inside the loop should take less than 1/60 of a second, so your engine needs to sleep until the next iteration ([tick](https://gamedev.stackexchange.com/questions/81608/what-is-a-tick-in-the-context-of-game-development)) should start. (Handling slow downs is an [interesting topic](https://gafferongames.com/post/fix_your_timestep/).) You can manage the timestep using C++'s `std::this_thread::sleep_for()` and passing it a C++ `std::chrono::duration<>`. You can get a double-valued timer by calling `glfwGetTime()` (which you can subtract and create a `std::chrono::duration<double>` from). See below for how to include `GLFW`. You don't need GLFW to get the current time. You can instead use the C++ standard library directly by subtracting two `std::chrono::time_point`s, which you can get via `std::chrono::steady_clock::now()`. For example, `const auto t1 = std::chrono::steady_clock::now()` stores the current time in a variable `t1`. For example, you can create a 0.1 second duration via `const auto one_tenth_of_a_second = std::chrono::duration<real>( 1./10. )`. You will need to `#include <thread>` and `#include <chrono>` to access the C++ standard library's functionality. You can see an example [here](demo/chrono_sleep_for.cpp). As another alternative, you could use [`sokol_time`](https://github.com/floooh/sokol/blob/master/sokol_time.h). You can `add_requires("sokol")` and `add_packages("sokol")` to access this header.

## The `GraphicsManager`

At this point, all we want from the graphics manager is to create a window. We will use [GLFW](https://www.glfw.org/) for this. It's an extremely lightweight graphics and input handling library. Declare your need for it at the top of your `xmake.lua` (just below the `add_rules()` line:

```
add_requires("glfw")
```

Inside `target("illengine")`, add it as a package:

```
    add_packages("glfw")
```

In your graphics manager C++ file, you will need to include the GLFW headers. You can do this in the implementation file only (the `.cpp` file). Users of your engine don't need to know that it uses GLFW to create a window. The engine is abstracting this detail! (If you include the GLFW headers in one of your `.h` files, you would need to write `add_packages("glfw", {public = true})` instead.)

Since we are using `WebGPU` as our GPU API, we don't want GLFW to include any OpenGL headers directly for us. This is how to do it:

```c++
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
```

Here is a magic bit of code for your graphics manager's startup method to create a window and lock its aspect ratio:

```c++
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

**You have reached the second checkpoint.** Commit your code to your git repository. Upload it for grading. Run `xmake clean --all` and then zip your entire directory. This checkpoint should contain:

* A graphics manager class that creates a window.
* A game engine class that starts up the graphics manager and runs a game loop 60 times per second. Your game engine won't take a user update callback yet.
* The modified `demo/helloworld.cpp`.

## Detecting input

It's time to create an input manager. The input manager provides an interface to the various input hardware state, such as which keyboard/mouse/joystick buttons are pressed and where the mouse or joystick is. Each time the game loop runs, the engine gives the input manager a chance to process input events (`input.Update()` in the pseudocode above). A fancy input manager lets users map physical actions (e.g. pressing a hardware button) to game actions. We will start with a simple input manager that just makes GLFW's input state available for querying by users of our engine. The input manager's `Update()` method only needs to call `glfwPollEvents();`. We will create a method called something like `KeyIsPressed` that takes a key name as a parameter and returns a boolean. It will be a thin wrapped around [`glfwGetKey()`](https://www.glfw.org/docs/3.3/group__input.html#gadd341da06bc8d418b4dc3a3518af9ad2). You can make the key name an `int` and just pass it along directly to GLFW, or you can create your own `enum class` for key names that you expose to users. Your enum can choose the same numbers for keys as GLFW (see [here](https://www.glfw.org/docs/3.3/group__keys.html); you can cast your `enum class` to an `int`). It is fine to simply ask users to use GLFW key names (e.g. `GLFW_KEY_LEFT`). (In that case, you will want to include the GLFW headers in your `.h` file and use `add_packages("glfw", {public = true})` in your `xmake.lua`.)

There isn't a perfect separation between the graphics manager and the input manager. The input manager needs to call GLFW functions and pass them the `GLFWwindow*` created by the graphics manager. This is why your Engine stores all the managers—so they can access each other. Modify your graphics manager to store the `GLFWwindow*` as an instance variable. You can make it `public` or `private`. If you make it `private`, you will need to add an accessor method or else have the graphics manager class announce that the input manager class is a `friend` (by writing `friend class InputManager;` inside your graphics manager class declaration). (`friend`s can access `private` fields and methods.)

It's time to add a user update callback to your game engine. In C++, a reasonable type to use for the user's update callback is: `std::function<void(Engine&)>`. That is, the user's callback is a function that takes an `Engine&` as a parameter and doesn't return anything. If you have an `Engine` global variable, then you can just use `std::function<void()>`, which is a function that takes no parameters and returns nothing. You can typedef that and then use it as a parameter to your game loop. Example syntax:

```c++
typedef std::function<void()> UpdateCallback;
void RunGameLoop( const UpdateCallback& callback );
```

You will need to `#include <functional>` to access `std::function`. It's a general type that can take any kind of function a C++ user might want to pass. The user of your engine can create a [lambda](https://en.cppreference.com/w/cpp/language/lambda) function to pass, as in:

```c++
engine.RunGameLoop( [&]( Engine& ) {
    // code that will run inside the game loop
    } );
```

Modify your `helloworld` function to pass a callback when running your engine's main loop. The callback should print or log a message when at least one key of your choosing is pressed.

You may have noticed that there's no way to quit your program (without asking your operating system to terminate it, e.g., `^C`). Quitting is easy; you just break out of the main loop. The `RunGameLoop()` function will then return. You can do that in response to a key press like escape or Q. If you want to check if someone clicked the window's close box, call the GLFW function `glfwWindowShouldClose(window)`. You can set this programmatically with `glfwSetWindowShouldClose( window, true );`. You can design your own way to encapsulate those. It could be something like `GraphicsManager::ShouldQuit()` and `GraphicsManager::SetShouldQuit(true)` methods.

**You have reached the third checkpoint.** Upload your code. Run `xmake clean --all` and then zip your entire directory. For this checkpoint:

* Your engine should have an input manager that lets users detect key presses.
* Your engine should take a user callback to execute in its main loop.
* Your `demo/helloworld.cpp` should pass a callback that prints or logs a message when a key is pressed.
* Your program should terminate when someone clicks the close box.

## Resources and the Resource Manager

Our resource management needs are pretty basic. We will create an `assets` directory to organize all the things that we'll want to access from our game. Modern build systems (like cmake and xmake) perform out of source builds, which means that the executable will be created in a funky location in some separate build directory. Since we'll want to load files from the `assets` directory, let's tell xmake to run the executable with the working directory set to the project directory, rather than the build directory. Add the following snippet to `xmake.lua` somewhere inside the `target("helloworld")`:

```
    set_rundir("$(projectdir)")
```

(I used to instead recommend the following snippet to copy `assets` next to the executable. I no longer do. It duplicates disk space, requires you to run `xmake` each time you change an asset, and makes it harder to implement live reloading. I'm leaving the snippet here in case you want or need it for scenarios in which you don't run using `xmake run` and can't set the working directory.)

```
    -- Copy assets
    after_build(function (target)
        cprint("Copying assets")
        os.cp("$(projectdir)/assets", path.directory(target:targetfile()))
    end)
```

At this point, reading a file from a path like `assets/sounds/coin.wav` or `assets/textures/coin.png` should just work from your engine. (*N.B.* Do **not** change the `/` to `\` on Windows. `/` is cross platform. Windows handles `/` just fine.) For the most straightforward cases, we could skip creating a resource manager altogether. However, it's a good idea to centralize our path handling. For that, you can make a resource manager that simply provides a method that resolves paths. The method would take in a partial path and return a path to a real file. Our `xmake.lua` sets the working directory so paths like `assets/textures/coin.png` just work, so the most basic resource manager would have a resolve path method that simply returns its input. You should create at least this resource manager now, and then use it to resolve paths in your sound or graphics manager (and any manager that loads resources). A better resource manager would have a method to set the path root (it could default to `assets` or not) which could let users resolve `textures/coin.png`. Use [`std::filesystem::path`](https://en.cppreference.com/w/cpp/filesystem/path) to append `path` to the root path. `std::filesystem::path` is neat. It overloads `/` so that you can do things like `relative_to_foo = std::filesystem::path("foo") / relative_path`. You can create an `std::filesystem::path` from an `std::string` and vice versa.

### Extensions

Even fancier resource managers could do more. Some possibilities:

* Watch for changes to the file on disk. When there's a change, reload the resource (image, sound, script, etc.). Your users will love this hot loading feature when working on their games. You could implement this with a `WatchPath()` method that looks for changes to the last modification time of the file. `WatchPath()` could take a callback parameter to call when a modification is detected. Your engine could give your resource manager a chance to check for changes in the game loop.
* Take in a URI, download the linked file, and then return the path to the download file or the file loaded into memory directly. Our sound and graphics and scripting libraries can load from files on disk (via a path) or from files already in memory.
* Load files to memory asynchronously. Either provide a way for the user to check later if the data is ready, or else let them provide a callback when it is. This would allow resources to be loaded in parallel and could drastically speed up game launch. (It's easy to run a function—even a lambda—in a background thread with [`std::thread`](https://en.cppreference.com/w/cpp/thread/thread/thread) or [`std::async`](https://en.cppreference.com/w/cpp/thread/async). It will be harder to make sure the main thread actually starts loading resources in parallel rather than blocking on the first one. You might also want a thread pool.)

## Sound Manager (optional, bonus)

It's easier (much less code) to play sounds than draw graphics to the screen, so you can warm up with a sound manager or come back to this later. We will use [SoLoud](https://sol.gfxile.net/soloud/) as our sound library. It has a really nice C++ API. (Click the link! Look at the sample code!) I added it to the `xmake` package repository, so we can write `add_requires("soloud")` near the top of our `xmake.lua` and `add_packages("soloud")` in our engine target. (You will need to do `add_packages("soloud", {public = true})` if your sound manager header includes a SoLoud header.)

Let's wrap SoLoud in a sound manager class. The sound manager will contain an instance of `SoLoud::Soloud`. (Yes, the capitalization is different for the namespace and the class.) You'll need to `#include "soloud.h"`. The startup method must call `.init()` and the shutdown method must call `.deinit()` on the `SoLoud::Soloud` instance. More interesting, let's add a method to load, destroy, and play a sound. We will follow a similar pattern for all of our resources (sounds, images, scripts). The load function will always have a signature like:

```c++
bool LoadSound( const string& name, const string& path );
```

This lets our engine's users load a sound and then access it by a convenient name in the destroy and play methods. Loading a sound in SoLoud is easy. We need to instantiate an instance of `SoLoud::Wav`, and then we can call the method `.load( path.c_str() );` on it. (`SoLoud::Wav` is declared in the `soloud_wav.h` header, so `#include` that.) Don't forget to resolve `path` with your resource manager. Keeping track of the `name` is easy, too. Let's use an [`std::unordered_map< string, SoLoud::Wav >`](https://en.cppreference.com/w/cpp/container/unordered_map) as our name-to-sound map. With a map like that (I'll call it `m`, but you should call it something better), we can write `m[ name ].load( path.c_str() );`. That's it! An `std::unordered_map` will instantiate the `SoLoud::Wav` if it doesn't already exist when looking up the value for a key. (If you'd like to know in advance, you can use `m.count( name ) == 0` to check if `m` already has a sound by that name. In C++20, that will shorten to `m.contains( name )`.) Destroying the sound is also easy: `m.erase( name );`. Finally, playing a sound is as simple as telling our `SoLoud::Soloud` instance to `.play( m[ name ] )`. (Don't attempt to copy a `SoLoud::Wav` by assigning it with `=`. Both copies will access and delete the same chunk of memory holding the audio data. When either one is deleted, the other may try to access freed memory. The `SoLoud::Wav` class should have deleted or made `operator=` private.)

Go ahead and load a sound and play it in response to a key changing from not pressed to pressed. (If you play it **if** the key is pressed, you'll play the sound many times—every 1/60 of a second—until the key is released.)

**You have reached the end of the optional sound manager component.**

## Graphics

It's time to draw images. Images are the workhorse of 2D engines. Sprites are images, the background is an image, and so on. We want to do our sprite drawing with a GPU so that it's fast. (In fact, almost everything we will do could be used just as easily to draw 3D shapes.) How do we use the hardware-accelerated GPU pipeline to draw an image? We draw a rectangle (two triangles) covered by the image (as a texture map).

The modern way to program GPUs is to describe all the state involved in the GPU's graphics pipeline (shaders, the layout of vertex data, various flags for fixed functionality to turn on and off), upload data to the GPU memory, and then run the pipeline on that data. The graphics API we're using for our engine is called WebGPU. It is a standard designed to unify access to all the major API's (Vulkan, Metal, DirectX) while being high-performance and not overly verbose. The standard was created simultaneously for the web (in JavaScript) and native access (`webgpu.h`). WebGPU is extremely new, but there are good resources out there already. Here is a list of resources you may wish to consult while working on your graphics manager (as I did):

* [Learn WebGPU (in C++)](https://eliemichel.github.io/LearnWebGPU/): A C++ tutorial. This was my primary source for learning WebGPU. It covers more topics than we need. If you are looking for more information about something, this is a good place to start.
* [WebGPU Fundamentals](https://webgpufundamentals.org/): A JavaScript tutorial. This tutorial has nice diagrams.
* [WebGPU Specification](https://www.w3.org/TR/webgpu/): The API spec. If you are wondering what a parameter is in excruciating detail, this is the place to look. I use it by searching for symbols.
* [webgpu.h](https://github.com/webgpu-native/webgpu-headers/blob/main/webgpu.h): The official C header developed along with the spec. It's more succinct than the specification, and shows the data structures precisely. I use it by searching for symbols.
* [WGSL WebGPU Shading Language Specification](https://www.w3.org/TR/WGSL/): This is the specification for the shading language.
* [I want to talk about WebGPU](https://cohost.org/mcc/post/1406157-i-want-to-talk-about-webgpu): The history of graphics API's and why WebGPU is exciting. At the end, there are excellent overview diagrams that explain using WebGPU.
* [WebGPU Bind Group best practices](https://toji.dev/webgpu-best-practices/bind-groups.html): An explanation of why there are bind groups.
* [How to write a renderer for modern graphics APIs](https://blog.mecheye.net/2023/09/how-to-write-a-renderer-for-modern-apis/): An explanation of why modern API's are designed the way they are, and how to manage the fast path.
* There are two active Discord servers ("Graphics Programming"'s `#webgpu` channel, "WebGPU C++") and a "WebGPU" matrix.org room.

**To be continued...** The rest of this checkpoint is being written. If you are trying to work ahead, skip to the next checkpoint. [Here](https://github.com/yig/GMU-CS425-2022#graphics) is last year's version of this checkpoint, which used the excellent but non-standard [`sokol_gfx` graphics API](https://github.com/floooh/sokol).

**You have reached the fourth checkpoint.** Upload your code. Run `xmake clean --all` and then zip your entire directory. For this checkpoint:

* Your engine should have a resource manager you use to resolve paths, even if it returns those paths unchanged.
* Your engine should have a graphics manager that lets users draw images to the screen.
* Your `demo/helloworld.cpp` should load one or more images during startup and draw one or more sprites inside the main loop.

## Game Objects (Entity Component System)

Your engine is now capable of playing sounds, drawing graphics, and reading input. It's time to add a notion of game objects. This will allow users of your engine to populate their games with objects and update them in response to user input and the passage of time (such as physics). This will also give your graphics manager something to draw every frame. Rather than creating a game object base class that all user classes derive from, we will implement a modern game architecture design called [Entity Component System (ECS)](https://github.com/SanderMertens/ecs-faq). The key idea is to separate data (Components) from game Entities (integers IDs) and algorithms (Systems). This has several advantages:

1. You avoid awkward OOP hierarchies (do drawable objects subclass physics body objects or vice versa).
2. When a game's requirements change, you don't have to refactor your OOP classes.
3. It makes efficient use of our computing hardware (data locality and opportunities for parallelism).
4. It's easy to save and load your entities and components to and from a file, and to inspect or edit them with a GUI.

### Components

Components are simple structs; they contain data and no methods. In fact, any struct your engine's users create can be used as Components. You may declare some common structs in your `Types.h` or in your managers. Some examples:

```c++
struct Position { real x, y; }; // or: struct Position : public vec2 {};
struct Velocity { real x, y; }; // or: struct Velocity : public vec2 {};
struct Gravity { real meters_per_second; }
struct Sprite { string image; real size; };
struct Health { real percent; };
struct Script { string name; };
```

An ECS stores components together in memory. This data locality leads to good cache efficiency.
(If you make components that are simply subclasses of other types, you may need to define a constructor that takes the superclass as a parameter. For example, if your `Position` is simply a subclass of `vec2`, you may want to define a constructor that takes a `vec2`, as in `Position( const vec2& v ) : vec2( v ) {}`. If you do that, tell the compiler that you still want the default constructor, too: `Position() = default;`)

### Entities

Every entity in the game—what the user would have made an object for if using object oriented programming—will be identified with a unique ID (an integer). Users of your ECS will be able to create an entity by asking the ECS for an unused unique ID. (Some implementations require the programmer to declare all components associated with an entity when creating the entity. Others, including the implementation described below, allow users to dynamically add Components to an entity. Users don't need to "register" their components in advance.) For example:

```c++
ECS.Get<Position>( entity ) = Position{0,0};
ECS.Get<Position>( entity ).x += 10;
ECS.Get<Health>( entity ) = Health{100};
```

### Systems

Systems are the algorithms. They iterate over all entities with a given set of components. For example, here is a physics system that iterates over entities with `Position` and `Velocity`:

```c++
ECS.ForEach<Position,Velocity>( [&]( EntityID e, Position& p, Velocity& v ) {
    p.x += v.x * dt;
    p.y += v.y * dt;
} );
```

This code tells the ECS to execute the callback function `[&]( EntityID e, Position& p, Velocity& v ) { p.x += v.x * dt; p.y += v.y * dt; }` for each entity with the components. Since the for each function is in one place, it would be trivial to parallelize it on multiple threads. This callback function is a lambda (local function) that has access to all the variables in the enclosing scope by reference (because of the `[&]`). The body of the lambda (`{ ... }`) gets executed as if it had been in the body of a for loop like this:
```c++
for( EntityID e, Position& p, Velocity& v : ECS.EntitiesWithComponents<Position,Velocity> ) {
    p.x += v.x * dt;
    p.y += v.y * dt;
}
```

In this checkpoint, modify your graphics manager's draw method to iterate over all entities with a `Sprite` (or a `Sprite` and a `Position`, depending on what you put in your `Sprite` component) and draw them. Do this instead of iterating over a vector of `Sprite`s passed as a parameter, which is what you did for checkpoint 5. In the next checkpoint, your script manager will iterate over all entities with a `Script` component and run the named script. In general, your managers should declare the components (`struct`s) that they will iterate over in their headers (or in `Types.h`).

### Requirements

The basic functionality you must implement is a class for your entity component system that has at least the following functionality:

* A method to **create** a new entity (returning its entity ID) or to return an unused entity ID: `EntityID Create();` or `EntityID UnusedEntity();`. It is fine if your `Create()` requires the user to specify all components for the entity.
* A method to **destroy** an entity, removing all of its components: `void Destroy( EntityID e );`
* A method to **get** a given component for an entity. By returning a reference `&`, callers can also set the component: `template< typename T > T& Get( EntityID e );`
* A method to iterate over all entities with a given set of entities and call a callback function once **for each** entity: `template< typename... EntitiesThatHaveTheseComponents > void ForEach( callback );`

### Implementation Possibility

You are free to implement your ECS any way you like so long as it implements the above methods. It makes for a fun and challenging puzzle. Here is a sketch for a sparse set implementation of an ECS. You are free to use it. I am leaving the implementation of `Create()` or `UnusedEntity()` as an exercise for you to solve on your own. This implementation makes heavy use of templated functions. There will only be a `.h` file for this implementation. Templated functions have to be defined—not just declared—in header (`.h`) files. This is because the compiler uses the template to generate code customized for the types used when it is called.

First, we need to define the `EntityID` type:

```c++
typedef long EntityID;
```

We could have used a `uint64_t` or `int64_t` instead of a `long` by including `<cstdint>`.

We need a sparse set to store each component. The simplest (but not best performing) option is `std::unordered_map< EntityID, T >`, where `T` is the type of the component. We will have many such maps, one per type. We need some way for users to access a component with type `T` for a given `EntityID`:

```c++
template< typename T >
T& Get( EntityID entity ) {
    return GetAppropriateSparseSet<T>()[ entity ];
}
```

This declares a function `Get()` that takes an `EntityID` parameter. The function is templated on a type `T`, which means the compiler will generate code for it for each type `T` that anyone calls it with. By returning a reference (`T&`), the caller can do:
```c++
Get<Position>(entity).x // Get access to the position's x component.
Get<Position>(entity) = p; // Set the component given another Position p.
```

How will we store all of these sparse sets? In an `std::vector`. C++ is statically typed. We can't directly create a vector of heterogeneous types, such as `{ SparseSet<Position>, SparseSet<Health> }`. Instead, we will create a `SparseSetHolder` superclass and store a vector of pointers to the superclasses in our ECS:

```c++
std::vector< std::unique_ptr< SparseSetHolder > > m_components;
```

> 💣 Gotcha: If you try to make a copy of a `unique_ptr`, such as by returning a copy of this vector or passing your entity component system class by value, you will get an obtuse error message. You almost certainly want to return or pass by reference instead. (We don't want multiple instances reading and writing and freeing the same memory.)

How will we know which element in the vector stores the component we are looking for? We will create a function that returns a deterministic index for each unique type it sees. It uses static variables to always returns the same index for the same type.

```c++
typedef int ComponentIndex;
template<typename T> ComponentIndex GetComponentIndex() {
    static ComponentIndex index = GetNextIndex(); // Only calls GetNextIndex() the first time this function is called.
    return index;
}
ComponentIndex GetNextIndex() {
    static ComponentIndex index = -1; // Returns the sequence 0, 1, 2, … every time this is called.
    index += 1;
    return index;
}
```

(If you allow the user to create multiple instances of your ECS (multiple worlds), be aware that `GetComponentIndex()` values are global across all instances. So if ECS one has Position components and ECS two doesn't, there will be a gap in ECS two's `m_components` vector. This is fine, so long as your `GetAppropriateSparseSet()` can handle it. Alternatively, `m_components` could be an `std::unordered_map` that uses `std::type_index(typeid(T))` to get a unique integer for each type. You don't have to worry about any of this if your engine stores one global ECS instance.)

We need a `SparseSetHolder` subclass for each type, and a superclass for operations we'll need without knowing about the type being held by the subclass.
```c++
class SparseSetHolder {
public:
    // A virtual destructor, since we store pointers to this superclass yet have subclasses with destructors that need to run.
    virtual ~SparseSetHolder() = default;
    virtual bool Has( EntityID ) const = 0;
    virtual void Drop( EntityID ) = 0;
};
// Subclasses are templated on the component type they hold.
template< typename T > class SparseSet : public SparseSetHolder {
public:
    std::unordered_map< EntityID, T > data;
    bool Has( EntityID e ) const override { return data.count( e ) > 0; };
    void Drop( EntityID e ) override { data.erase( e ); };
};
```

Now we can write a templated function to return a reference to the underlying `unordered_map`:
```c++
template< typename T >
std::unordered_map< EntityID, T >&
GetAppropriateSparseSet() {
    // Get the index for T's SparseSet
    const ComponentIndex index = GetComponentIndex<T>();
    // If we haven't seen it yet, it must be past the end.
    // Since component indices are shared by all ECS instances,
    // it could happen that index is more than one past the end.
    if( index >= m_components.size() ) { m_components.resize( index+1 ); }
    assert( index < m_components.size() );
    // Create the actual sparse set if needed.
    if( m_components[ index ] == nullptr ) m_components[index] = std::make_unique< SparseSet<T> >();
    // It's safe to cast the SparseSetHolder to its subclass and return the std::unordered_map< EntityID, T > inside.
    return static_cast< SparseSet<T>& >( *m_components[ index ] ).data;
}
```

Now we can write the rest of our user-facing (public) functions:
```c++
// Destroy the entity by removing all components.
void Destroy( EntityID e ) {
    for( const auto& comps : m_components ) { comps->Drop( e ); }
}
```

The `ForEach` function can be used like: `ForEach<Position,Velocity,Health>( callback )`. It takes multiple templated types. The idea behind this implementation is to find the container for the first component. That gives us candidate `EntityID`s. We will iterate over those `EntityID`s and call the callback for entities that have all the other components.
```c++
typedef std::function<void( EntityID )> ForEachCallback;
template<typename EntitiesThatHaveThisComponent, typename... AndAlsoTheseComponents>
void ForEach( const ForEachCallback& callback ) {
    // Iterate over elements of the first container.
    auto& container = GetAppropriateSparseSet<EntitiesThatHaveThisComponent>();
    for( const auto& [entity, value] : container ) {
        // We know it has the first component.
        // Skip the entity if it doesn't have the remaining components.
        // This `if constexpr` is evaluated at compile time. It is needed when AndAlsoTheseComponents is empty.
        // https://stackoverflow.com/questions/48405482/variadic-template-no-matching-function-for-call/48405556#48405556
        if constexpr (sizeof...(AndAlsoTheseComponents) > 0) {
            if( !HasAll<AndAlsoTheseComponents...>( entity ) ) {
                continue;
            }
        }
        callback( entity );
    }
}
```
The `HasAll<Position,Velocity,Health>()` helper method is easier to write:
```c++
// Returns true if the entity has all types.
template <typename T, typename... Rest>
bool HasAll( EntityID entity ) {
    bool result = true;
    // https://stackoverflow.com/questions/48405482/variadic-template-no-matching-function-for-call/48405556#48405556
    if constexpr (sizeof...(Rest) > 0) { result = HasAll<Rest...>(entity); }
    return result && GetAppropriateSparseSet<T>().count( entity ) > 0;
}
```

That's it!

Note that this `ForEach()` function doesn't pass the components by reference to the callback. Use `Get<Component>(entity)` in the callback. For example, you could replace a for loop over an `std::vector<Sprite> sprites`:
```c++
for( Sprite& sprite : sprites ) {
    // inside the loop
}
```
with
```c++
ECS.ForEach<Sprite>( [&]( EntityID entity ) {
    Sprite& sprite = ECS.Get<Sprite>(entity);
    // inside the loop
} );
```

If your ECS is global (or lives inside a global variable), you can make your `EntityID` type support component access directly, as in `entity.Get<Sprite>()`. See [here](demo/entity_get.cpp) for an example.

### Extensions

* `ForEach` whose inner for loop iterates over the smallest sparse set among all the given components.
* A `ParallelForEach` that uses a thread pool.
* A more efficient sparse set that stores its component data in a single, dense, contiguous chunk of memory with no gaps. `unordered_map` doesn't actually do that.
* Support for [range-based for loops](https://stackoverflow.com/questions/8164567/how-to-make-my-custom-type-to-work-with-range-based-for-loops).

**You have reached the fifth checkpoint.** Upload your code. Run `xmake clean --all` and then zip your entire directory. For this checkpoint:

* Your engine should have an entity component system that lets users create entities, attach components to them, and run algorithms over the components.
* Your graphics manager's draw method should iterate over your ECS entities rather than taking a parameter.
* Your `demo/helloworld.cpp` should create entities with a `Sprite` component. You can re-use the `Sprite` class you made for the previous checkpoint as the component.

## Scripting

At this point, you have a working game engine. Someone can use your engine to make a game! They would startup your engine, setup their game (create entities and so on), and then tell the engine to run the main loop, passing a callback for updating the game every frame. Even more fun? Adding scripting support to your engine, so users can write much or all of the game logic in a scripting language without recompiling.

[Lua](https://www.lua.org/) is a popular choice. To quote from the [about](https://www.lua.org/about.html) page:

> Lua is a powerful, efficient, lightweight, embeddable scripting language. It supports procedural programming, object-oriented programming, functional programming, data-driven programming, and data description.

> Lua combines simple procedural syntax with powerful data description constructs based on associative arrays and extensible semantics. Lua is dynamically typed, runs by interpreting bytecode with a register-based virtual machine, and has automatic memory management with incremental garbage collection, making it ideal for configuration, scripting, and rapid prototyping.

Official documentation for the Lua programming language is its [reference manual](https://www.lua.org/manual/). There is also an official book, *Programming in Lua*. The [first edition](https://www.lua.org/pil/contents.html) is freely available online.

To embed Lua in our engines, we will use the [`sol`](https://sol2.readthedocs.io/en/latest/) C++ binding library. (You will sometimes see it referred to as `sol2` and `sol3`.) `sol` makes it as easy as possible to interface C++ code with Lua code and vice versa. (It really is as easy as possible! The [developer](https://thephd.dev/) is involved in the C++ (and C) language standards.) You'll be able to declare Lua functions that are written in C++ and expose C++ types to Lua (for use in Lua scripts). You'll be able to call Lua code from C++ (for executing the scripts). The `sol` documentation has a lot of examples. Look over [the tutorial](https://sol2.readthedocs.io/en/latest/tutorial/tutorial-top.html) to get a feel for how to use it. (If you are using the `gcc` compiler, and it gets very slow during this checkpoint, try switching to release mode. `gcc` has a [known bug](https://stackoverflow.com/questions/33443626/gcc-compilation-very-slow-large-file/33456247#33456247).)

You will have a lot of architectural freedom in this checkpoint. Should the scripting manager startup first, and then other managers (sound, input, ECS) make calls to the scripting manager to expose their functionality to Lua? Or should the scripting manager startup last and take on the task of exposing the other managers' functionality itself? You are free to make these architectural choices (and discuss them with each other). I will describe scripting functionality that our engines must support. I will also give examples of how to use the `sol` library.

First things first. Let's add `lua` and `sol` to our `xmake.lua` with `add_requires("lua", "sol2")` near the top and, inside `target("illengine")`, `add_packages("lua")` and `add_packages("sol2", {public = true})`. We will want to add the `sol` package publicly so that users can make their game's data structures accessible from inside Lua. Next let's make a script manager. The script manager should include the `sol` header.

```c++
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
```

The entirety of the Lua environment is contained in a single class called `sol::state`. Your script manager must store an instance of this class. For the purposes of this document, let's assume it's declared as `sol::state lua`. When your script manager starts up, I recommend telling `lua` to load some convenient built-in libraries:
```c++
lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table);
```
This makes these standard libraries ([manual](https://www.lua.org/manual/5.4/manual.html#6), [book](https://www.lua.org/pil/contents.html#P3)) available to scripts. For debugging, I like to make the math random seed always the same. You can do that with `lua.script("math.randomseed(0)");`. You just executed your first Lua code! It's that easy to call Lua from C++.

The script manager should provide a way to load scripts from files with something like:
```c++
bool LoadScript( const string& name, const string& path );
```
You can load a script with `lua.load_file( path );`, which returns an object of type `sol::load_result`. You can use an `unordered_map` from names to `sol::load_result`. The `sol::load_result` type can be called like a function. Any parameters will be passed to the Lua script. See [this tutorial example](https://sol2.readthedocs.io/en/latest/tutorial/all-the-things.html#passing-arguments-to-scripts) or the small example [here](demo/lua_parameters.cpp).

You should provide a way for users to run a script they've loaded. You could simply give them access to the `sol::load_result` by returning a reference to it. If you expose enough of your engine's functionality to Lua, the C++ code your users will need to write could be limited to loading a setup script and running it.

The script manager should also declare a `Script` struct with a `name` field as a component for your entity component system. The script manager should have an update method to be called every frame that uses your entity component system to iterate over each entity with a `Script` component and run the script with the entity as the parameter. That way, any script beginning `local entity = ...` will run on that entity. (You really type the `...`. See the examples above.) You will want to decide if these scripts run before or after the user's C++ callback, whether you want an additional C++ callback so that one runs before and one after, or whether the user's C++ callback is responsible for calling an update method of the script manager.

You should expose your **input manager** functionality to Lua. For example, your key down function could be exposed as simply as `lua.set_function( "KeyIsDown", [&]( const int keycode ) { return input.KeyIsDown( keycode ); } );`. You can expose your keycodes via [`lua.new_enum`](https://sol2.readthedocs.io/en/latest/api/table.html?highlight=new_enum#new-enum):
```c++
lua.new_enum( "KEYBOARD",
    "SPACE", KEY_SPACE,
    ...
    );
```
Then a Lua script can call `KeyIsDown( KEYBOARD.SPACE )` as needed. You should also expose a function to quit your game.

You should expose your **sound manager**'s ability to play a sound.

You should expose your **entity component systems**'s functionality for creating an entity, destroying an entity, and getting built-in components. Exposing the functions to create and destroy entities is straightforward. However, you can't directly expose your ECS's templated get-a-component function. You will have to expose a different getter for each component. For example, expose a `"GetSprite"` Lua function that returns your ECS's `Get<Sprite>(e)` by reference (or by pointer). `sol` will make the struct available to Lua by reference. (If you are using a C++ lambda to wrap your `Get<Sprite>(e)`, you'll need to explicitly specify its return value to be a reference, as in `[&]( EntityID e ) -> Sprite& { return ecs.Get<Sprite>(e); }` or else return a pointer.) This means that Lua scripts will be able to directly modify the struct members, as in `GetPosition(e).x = 10`. (The only things `sol` doesn't pass by reference are primitive Lua type like an integer, float, or string.) You will need to register `struct`s like `Sprite` and our hypothetical `Position` with `sol`. To do that, use the [`new_usertype` function](https://sol2.readthedocs.io/en/latest/tutorial/cxx-in-lua.html). For example, you can expose a `struct Person { string name; int age; };` via:
```c++
lua.new_usertype<Person>("Person",
    sol::constructors<Person()>(), 
    "name", &Person::name,
    "age", &Person::age
    );
```
You can access the `.name` and `.age` members of a `Person` in Lua. You can create a new `Person` inside a Lua script via `Person.new()`.

You may wish to expose your `vec2` and `vec3` types. Here is a fancy/gnarly version that exposes their different constructors and also the `+`, `-`, and `*` functionality that lets us multiply a `vec3` by a scalar or add or subtract two `vec3`s:
```c++
lua.new_usertype<glm::vec3>("vec3",
    sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
    "x", &glm::vec3::x,
    "y", &glm::vec3::y,
    "z", &glm::vec3::z,
    // optional and fancy: operator overloading. see: https://github.com/ThePhD/sol2/issues/547
    sol::meta_function::addition, sol::overload( [](const glm::vec3& v1, const glm::vec3& v2) -> glm::vec3 { return v1+v2; } ),
    sol::meta_function::subtraction, sol::overload( [](const glm::vec3& v1, const glm::vec3& v2) -> glm::vec3 { return v1-v2; } ),
    sol::meta_function::multiplication, sol::overload(
        [](const glm::vec3& v1, const glm::vec3& v2) -> glm::vec3 { return v1*v2; },
        [](const glm::vec3& v1, float f) -> glm::vec3 { return v1*f; },
        [](float f, const glm::vec3& v1) -> glm::vec3 { return f*v1; }
    )
);
```

Users should have a way to access the `sol::state lua` so they can call `lua.new_usertype()` for their own structs and `lua.set_function()` to expose their own functions. That way users can expose their component structs, `Get<Component>()` functions, and even `ForEach<Components...>()` for specific sets of components. (`sol` seamlessly supports Lua functions as callbacks in C++.) Alternatively, if users don't expose their own components, Lua scripts would only be able to call the functions involving the engine's built-in components (to create, destroy, and access components the engine will look for). In that case, a script could manage additional entity properties on its own via Lua's convenient [table data structure](https://www.lua.org/pil/2.5.html).

If you wish, you can organize the functionality you expose to Lua with [something like namespaces](https://sol2.readthedocs.io/en/latest/tutorial/all-the-things.html?highlight=get_or_create#namespacing).

### Extensions

* Expose more of your engine's functionality. For example, if you want your users to be able to write all of their setup code in Lua, you should expose asset loading functionality.

**You have reached the sixth checkpoint.** Upload your code. Run `xmake clean --all` and then zip your entire directory. For this checkpoint:

* Your engine should have a script manager that lets users run Lua scripts on demand or attach a `Script` component to entities that will be run automatically in the game loop.
* Your engine should expose to Lua scripts the ability to query input state, quit the game, play sounds, and manipulate sprites.

## What's Next?

You don't need anything else. You might want:

* Text rendering. Some options:
  * [`text2image`](https://github.com/justinmeiners/text2image) draws image data into a buffer and then saves it to a file. You could use that directly with no code changes in your graphics manager. With small modifications, you could avoid the round trip to and from the filesystem. Modify `text2image` to return the image data, and modify your graphics manager to load an image from memory. The overhead of rendering text to an image (on the CPU and then uploading it to the GPU) is irrelevant if your text doesn't change often.
* A GUI for inspecting and editing your game state. [Dear ImGui](https://github.com/ocornut/imgui) and [Nuklear](https://github.com/Immediate-Mode-UI/Nuklear) are good for that.
  * To avoid the hooks making your graphics manager code look ugly, I recommend making a GUI manager with its own draw method. (It could be entirely owned and managed by the graphics manager.) Your graphics manager can call it at the appropriate times.
  * For Dear ImGui, you can `add_requires("imgui", {configs = {glfw = true, wgpu = true}})` and `add_packages("imgui")`. You can then include `<imgui.h>`, `<backends/imgui_impl_wgpu.h>`, and `<backends/imgui_impl_glfw.h>`. For an example, see [Learn WebGPU's Simple GUI example](https://eliemichel.github.io/LearnWebGPU/basic-3d-rendering/some-interaction/simple-gui.html). It boils down to: (1) Call `ImGui::CreateContext();` followed by `ImGui_ImplGlfw_InitForOther()` and `ImGui_ImplWGPU_Init()` on startup. (2) Call `ImGui_ImplGlfw_Shutdown()` followed by `ImGui::DestroyContext()` at shutdown. (3) Call `ImGui_ImplWGPU_NewFrame()`, `ImGui_ImplGlfw_NewFrame()`, and `ImGui::NewFrame()` at the beginning of the GUI manager's draw function and `ImGui::EndFrame()`, `ImGui::Render()`, and `ImGui_ImplWGPU_RenderDrawData()` at the end. Put your GUI drawing commands in between.
* Networking. This is a big topic. Some options (in `xrepo`):
  * [ENet](http://enet.bespin.org/): The tutorial is quite easy to follow and mentions how you would incorporate this into a game loop.
  * [Asio](http://think-async.com/Asio/): This is a very general library. There isn't a tutorial as well-documented as ENet.
  * [brynet](https://github.com/IronsDu/brynet): The examples aren't well documented.

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
* 2023-08-24: xmake clean --all
* 2023-08-30: mentioned `add_packages("glfw", {public = true})` in checkpoint 2.
* 2023-08-30: fixed link to `globals.h` example
* 2023-08-30: added a link to the xmake FAQ and a workaround for curl certificate issues
* 2023-08-31: mentioned `xmake repo -u` to update the known packages
* 2023-09-04: resource manager and optional sound manager added back and revised to use xrepo.
* 2023-09-04: added `chrono` example.
* 2023-09-04: added back in Graphics (under construction), Game Objects, Scripting, and What Next sections.
* 2023-09-05: added `friend` example
* 2023-09-05: added `xmake clean --all` and `xmake config --clean` and some other minor cleanups to "Some useful `xmake` commands" section.
* 2023-09-06: added language hints to fenced code blocks
* 2023-09-06: clarified `friend class`
