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

* `xmake run -d helloworld` launches the program in a debugger.
* `xmake f --menu` opens a command-line menu system you can use to switch between debug and release modes.
* `xmake f -m debug` and `xmake f -m release` directly switch between debug and release mode.
* `xmake watch` will re-run `xmake` automatically when any code changes. `xmake watch -r -t helloworld` will do the same and then run the `helloworld` target.

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

This header is also a good place for "forward declarations". Because C++ uses file-based compilation units rather than some kind of module system (that is, until the very recent C++20 standard), we need to be careful to avoid including the same header file multiple times. The easiest way to do this is by putting the line `#pragma once` at the top of every header file. While technically not standard, it's [supported by all the major C++ compilers](https://en.wikipedia.org/wiki/Pragma_once#Portability) and a lot less typing and redundancy than the [header guards](https://en.wikipedia.org/wiki/Include_guard) you may have seen in C or in C++ in the past. However, `#pragma once` won't solve circular dependency problems, where, for example, the `GraphicsManager` needs to know about the `ResourceManager` and vice versa. The solution is to partially declare a class in advance. Writing `class Engine;` in our `Types.h` file is enough to let the compiler know that there is an `Engine` class in the `Foo` namespace. The forward declaration satisfies the compiler enough to let us declare a pointer or reference to the `Engine`. Before we actually use the `Engine` in a `.cpp` file, we will have to include its header. (Another solution to circular dependencies is [`unique_ptr`](https://ortogonal.github.io/cpp/forward-declaration-and-smart-pointers/) and yet another is the so-called ["pimpl pattern"](https://www.fluentcpp.com/2017/09/22/make-pimpl-using-unique_ptr/).)

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

You will also need to consider how the managers can access each other. Eventually, we will have even more managers. For this next checkpoint, you don't need to create an input manager. Fortunately, if the managers can access the engine, they can access each other. One possibility is to make an engine global variable. In C++17, you can declare it right in the engine header as an [inline variable](https://stackoverflow.com/a/47502744). For example, if your class were named `Foo`, you could declare `inline Foo gFoo`, where the `g` prefix is [Hungarian notation](https://en.wikipedia.org/wiki/Hungarian_notation).) If you are strongly allergic to global variables, the engine can instead pass a reference (or pointer) to itself to all the managers so that they can access each other. (You can do this in the constructor of Engine or in the Startup methods.)

Managing the time step means making sure that your game loop runs 60 times per second. The code inside the loop should take less than 1/60 of a second, so your engine needs to sleep until the next iteration ([tick](https://gamedev.stackexchange.com/questions/81608/what-is-a-tick-in-the-context-of-game-development) should start. (Handling slow downs is an [interesting topic](https://gafferongames.com/post/fix_your_timestep/).) You can manage the timestep using C++'s `std::this_thread::sleep_for()` and passing it a C++ `std::chrono::duration<>`. You can get a double-valued timer by calling `glfwGetTime()` (which you can subtract and create a `std::chrono::duration<double>` from). See below for how to include `GLFW`. You don't need GLFW to get the current time. You can instead use the C++ standard library directly by subtracting two `std::chrono::time_point`s, which you can get via `std::chrono::steady_clock::now()`. For example, `const auto t1 = std::chrono::steady_clock::now()` stores the current time in a variable `t1`. For example, you can create a 0.1 second duration via `const auto one_tenth_of_a_second = std::chrono::duration<real>( 1./10. )`. You will need to `#include <thread>` and `#include <chrono>` to access the C++ standard library's functionality. As another alternative, you could use [`sokol_time`](https://github.com/floooh/sokol/blob/master/sokol_time.h). We'll use `sokol_gfx` for our graphics manager; you can `add_requires("sokol")` and `add_packages("sokol")` already in this checkpoint to access both headers.

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

This code expects a few parameters: the `window_width` and `window_height`, a name for the window (`window_name`), and a boolean to specify whether the window should be fullscreen (`window_fullscreen`). You can hard-code some reasonable values or you can find a way to take parameters. You might store the parameters in a configuration `struct` in the `Engine`. The user can set them directly or pass it in to your engine's startup method. You may notice that this code snippet is asking for an OpenGL-compatible window. That's because we will use `sokol_gfx`'s OpenGL backend for cross-platform simplicity. Your graphics manager's shutdown only needs to call `glfwTerminate();`.

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

## Resources and the Resource Manager

Our resource management needs are pretty basic. We will create an `assets` directory to organize all the things that we'll want to access from our game. Modern build systems (like cmake and xmake) perform out of source builds, which means that the executable will be created in a funky location in some separate build directory. Since we'll want to load files from the `assets` directory, we'll need to tell xmake to copy `assets` next to the executable. Add the following snippet to `xmake.lua` somewhere inside the `target("helloworld")`:

```
    -- Copy assets
    after_build(function (target)
        cprint("Copying assets")
        os.cp("$(projectdir)/assets", path.directory(target:targetfile()))
    end)
```

At this point, reading a file from a path like `assets/sounds/coin.wav` should just work from your engine. This is what we assumed above for the sound manager. For the most straightforward cases, we could skip creating a resource manager altogether. However, it's a good idea to centralize our path handling. For that, you can make a resource manager that simply provides a method that resolves paths. The method would take in a partial path and return a path to a real file. Our `xmake.lua` will copy assets so that paths like `assets/sounds/coin.wav` just work, so the most basic resource manager would have a resolve path method that simply returns its input. You should create at least this resource manager now, and then use it to resolve paths in your sound manager (and any manager that loads resources). A better resource manager would have a method to set the path root (it could default to `assets` or not) which could let users resolve `sounds/coin.wav`. Use [`std::filesystem::path`](https://en.cppreference.com/w/cpp/filesystem/path) to append `path` to the root path. `std::filesystem::path` is neat. It overloads `/` so that you can do things like `relative_to_foo = std::filesystem::path("foo") / relative_path`. You can create an `std::filesystem::path` from an `std::string` and vice versa.

Even fancier resource managers could do more. Some possibilities:

* Take in a URI, download the linked file, and then return the path to the download file or the file loaded into memory directly. Our sound and graphics and scripting libraries can load from files on disk (via a path) or from files already in memory.
* Load files to memory asynchronously. Either provide a way for the user to check later if the data is ready, or else let them provide a callback when it is. This would allow resources to be loaded in parallel and could drastically speed up game launch. (It's easy to run a function—even a lambda—in a background thread with [`std::thread`](https://en.cppreference.com/w/cpp/thread/thread/thread) or [`std::async`](https://en.cppreference.com/w/cpp/thread/async). It will be harder to make sure the main thread actually starts loading resources in parallel rather than blocking on the first one. You might also want a thread pool.)

## The Sound Manager

It's easier (much less code) to play sounds than draw graphics to the screen, so we'll warm up with a sound manager. We will use [SoLoud](https://sol.gfxile.net/soloud/) as our sound library. It has a really nice C++ API. (Click the link! Look at the sample code!) Unfortunately, it's not in the `xmake` package repository `xrepo`, but it's easy enough to create a one-off xmake package for an external library. Download this [`external/xmake_soloud.lua`](external/xmake_soloud.lua) file and put it in an `external` directory. Load it into your `xmake.lua` file by adding

```
includes("external/xmake_soloud.lua")
```

near the top. We can then write `add_requires("soloud")` after that line and `add_packages("soloud")` in our engine target. (You will need to do `add_packages("soloud", {public = true})` if your sound manager header includes a SoLoud header.)

Let's wrap SoLoud in a sound manager class. The sound manager will contain an instance of `SoLoud::Soloud`. (Yes, the capitalization is different for the namespace and the class.) You'll need to `#include "soloud.h"`. The startup method must call `.init()` and the shutdown method must call `.deinit()` on the `SoLoud::Soloud` instance. More interesting, let's add a method to load, destroy, and play a sound. We will follow a similar pattern for all of our resources (sounds, images, scripts). The load function will always have a signature like:

```
bool LoadSound( const string& name, const string& path );
```

This lets our engine's users load a sound and then access it by a convenient name in the destroy and play methods. Loading a sound in SoLoud is easy. We need to instantiate an instance of `SoLoud::Wav`, and then we can call the method `.load( path.c_str() );` on it. (`SoLoud::Wav` is declared in the `soloud_wav.h` header, so `#include` that.) Don't forget to resolve `path` with your resource manager. Keeping track of the `name` is easy, too. Let's use an [`std::unordered_map< string, SoLoud::Wav >`](https://en.cppreference.com/w/cpp/container/unordered_map) as our name-to-sound map. With a map like that (I'll call it `m`, but you should call it something better), we can write `m[ name ].load( path.c_str() );`. That's it! An `std::unordered_map` will instantiate the `SoLoud::Wav` if it doesn't already exist when looking up the value for a key. (If you'd like to know in advance, you can use `m.count( name ) == 0` to check if `m` already has a sound by that name. In C++20, that will shorten to `m.contains( name )`.) Destroying the sound is also easy: `m.erase( name );`. Finally, playing a sound is as simple as telling our `SoLoud::Soloud` instance to `.play( m[ name ] )`.

Go ahead and load a sound and play it in response to a key changing from not pressed to pressed. (If you play it **if** the key is pressed, you'll play the sound many times—every 1/60 of a second—until the key is released.)

**You have reached the fourth checkpoint.** Upload your code. Run `xmake clean` and then zip your entire directory. For this checkpoint:

* Your engine should have a sound manager that lets users load and play sounds.
* Your engine should have a resource manager you use to resolve paths, even if it returns those paths unchanged.
* Your `demo/helloworld.cpp` should pass a callback that plays a sound when a key is pressed.

## Graphics

It's time to draw images. Images are the workhorse of 2D engines. Sprites are images, the background is an image, and so on. We want to do our sprite drawing with a GPU so that it's fast. (In fact, almost everything we will do could be used just as easily to draw 3D shapes.) How do we use the hardware-accelerated GPU pipeline to draw an image? We draw a rectangle (two triangles) covered by the image (as a texture map).

The modern way to program GPUs is to describe all the state involved in the GPU's graphics pipeline (shaders, the layout of vertex data, various flags for fixed functionality to turn on and off). Here is the entire pipeline state for the `sokol_gfx` graphics API we're using [[source]](https://github.com/floooh/sokol/blob/8c4337a15c3edc98626a34fa926f42498d62acac/sokol_gfx.h/#L1960):

```
/*
    sg_pipeline_desc
    The sg_pipeline_desc struct defines all creation parameters for an
    sg_pipeline object, used as argument to the sg_make_pipeline() function:
    - the vertex layout for all input vertex buffers
    - a shader object
    - the 3D primitive type (points, lines, triangles, ...)
    - the index type (none, 16- or 32-bit)
    - all the fixed-function-pipeline state (depth-, stencil-, blend-state, etc...)
    If the vertex data has no gaps between vertex components, you can omit
    the .layout.buffers[].stride and layout.attrs[].offset items (leave them
    default-initialized to 0), sokol-gfx will then compute the offsets and
    strides from the vertex component formats (.layout.attrs[].format).
    Please note that ALL vertex attribute offsets must be 0 in order for the
    automatic offset computation to kick in.
    The default configuration is as follows:
    .shader:            0 (must be initialized with a valid sg_shader id!)
    .layout:
        .buffers[]:         vertex buffer layouts
            .stride:        0 (if no stride is given it will be computed)
            .step_func      SG_VERTEXSTEP_PER_VERTEX
            .step_rate      1
        .attrs[]:           vertex attribute declarations
            .buffer_index   0 the vertex buffer bind slot
            .offset         0 (offsets can be omitted if the vertex layout has no gaps)
            .format         SG_VERTEXFORMAT_INVALID (must be initialized!)
    .depth:
        .pixel_format:      sg_desc.context.depth_format
        .compare:           SG_COMPAREFUNC_ALWAYS
        .write_enabled:     false
        .bias:              0.0f
        .bias_slope_scale:  0.0f
        .bias_clamp:        0.0f
    .stencil:
        .enabled:           false
        .front/back:
            .compare:       SG_COMPAREFUNC_ALWAYS
            .depth_fail_op: SG_STENCILOP_KEEP
            .pass_op:       SG_STENCILOP_KEEP
            .compare:       SG_COMPAREFUNC_ALWAYS
        .read_mask:         0
        .write_mask:        0
        .ref:               0
    .color_count            1
    .colors[0..color_count]
        .pixel_format       sg_desc.context.color_format
        .write_mask:        SG_COLORMASK_RGBA
        .blend:
            .enabled:           false
            .src_factor_rgb:    SG_BLENDFACTOR_ONE
            .dst_factor_rgb:    SG_BLENDFACTOR_ZERO
            .op_rgb:            SG_BLENDOP_ADD
            .src_factor_alpha:  SG_BLENDFACTOR_ONE
            .dst_factor_alpha:  SG_BLENDFACTOR_ZERO
            .op_alpha:          SG_BLENDOP_ADD
    .primitive_type:            SG_PRIMITIVETYPE_TRIANGLES
    .index_type:                SG_INDEXTYPE_NONE
    .cull_mode:                 SG_CULLMODE_NONE
    .face_winding:              SG_FACEWINDING_CW
    .sample_count:              sg_desc.context.sample_count
    .blend_color:               (sg_color) { 0.0f, 0.0f, 0.0f, 0.0f }
    .alpha_to_coverage_enabled: false
    .label  0       (optional string label for trace hooks)
*/
```

We won't have to worry about setting many of these fields. The defaults shown above are fine. We will only be concerned with `.shader`, `.layout`, `.primitive_type`, `.depth` (to enable z sorting), and `.colors` (to enable alpha blending). We will create a pipeline state when our graphics manager starts up. We will also create the vertex data we need. Later, when it's time to draw sprites, we will tell our GPU to activate the pipeline and then apply it to our vertex data and each sprite's image.

`sokol_gfx`'s documentation lives directly [in its header](https://github.com/floooh/sokol/blob/master/sokol_gfx.h). The [samples](https://floooh.github.io/sokol-html5/) are also helpful. There is some backstory in [this blog post](https://floooh.github.io/2017/07/29/sokol-gfx-tour.html). We're following the steps to initialize and use `sokol_gfx` described in its [header file](https://github.com/floooh/sokol/blob/8c4337a15c3edc98626a34fa926f42498d62acac/sokol_gfx.h/#L85) (but there are C++ versions of the functions that take references rather than pointers to the structs, so we can avoid a bunch of noisy `&`s).

First things first. Let's add `sokol` to our `xmake.lua` with `add_requires("sokol")` near the top and `add_packages("sokol")` inside `target("illengine")`. Include the `sokol_gfx.h` header file in your graphics manager `.cpp` file. Since `sokol_gfx` is a header-only library, we need to tell it to include the implementation in exactly one `.cpp` file (our graphics manager `.cpp` file). We'll also tell it we want the OpenGL 3.3 Core Profile backend:

```
#define SOKOL_IMPL
#define SOKOL_GLCORE33
#include "sokol_gfx.h"
```

To initialize `sokol_gfx`, we need to call `sg_setup(sg_desc{});`. We must do this after setting up `GLFW` (in our graphics manager's startup function). The curly-braces are C++ for initialize all members to zero if they don't have constructors. (It's called [aggregate initialization](https://en.cppreference.com/w/cpp/language/aggregate_initialization). `sokol_gfx` examples written in C99 use a more convenient initialization syntax; that syntax became part of C++20, where it is called [designated initializers](https://www.cppstories.com/2021/designated-init-cpp20/).) `sokol_gfx` uses zeros to mean default values. `sokol_gfx` is also primarily a C API, so initializing the structs to zero is our responsibility. Let's be responsible and add `sg_shutdown();` to our graphics manager's shutdown function.

Before we describe our pipeline, let's describe the data we will use. We will draw every rectangle as a scaled and translated unit square. The vertex data we need for a unit square is:

```
// A vertex buffer containing a textured square.
const float vertices[] = {
    // positions      // texcoords
    -1.0f,  -1.0f,    0.0f,  0.0f,
     1.0f,  -1.0f,    1.0f,  0.0f,
    -1.0f,   1.0f,    0.0f,  1.0f,
     1.0f,   1.0f,    1.0f,  1.0f,
};
```

Notice that this is just a 1D array of floats. Later we will tell the GPU pipeline about the layout of this data: each vertex has two floats for the position (x and y) and two floats for the texture coordinates (u and v). For now, let's copy this data to GPU memory. We will do that with `sg_make_buffer()`, which takes an `sg_buffer_desc` struct. The only field we need to fill is `.data`, which stores a pointer to our data and the number of bytes. Our `vertices` data was declared on the stack, so we can just give it `vertices` and `sizeof(vertices)`. Let's create one, zero-initialized, and then set its `.data` field.

```
sg_buffer_desc buffer_desc{};
buffer_desc.data = SG_RANGE(vertices);
```

We used the `SG_RANGE` macro, which creates an `sg_range` struct containing exactly `vertices` and `sizeof(vertices)`. Now we can upload the data to the GPU:

```
sg_buffer vertex_buffer = sg_make_buffer(buffer_desc);
```

The GPU now has a copy of `vertices`, so we are fine lettings its memory become automatically de-allocated when the enclosing scope of our graphics manager's startup function terminates.

Now let's create a pipeline description struct (`sg_pipeline_desc`). It will completely describes the GPU state we need for drawing. Let's create one, zero-initialized:

```
sg_pipeline_desc pipeline_desc{};
```

To start, let's tell the pipeline what our vertex data represents. We want to make a rectangle out of two triangles. The four vertices in our buffer do this when interpreted as a [triangle strip](https://en.wikipedia.org/wiki/Triangle_strip):

```
pipeline_desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP;
```

Alpha blending is off by default. The images we want to draw may have transparency, so let's turn on alpha blending with over compositing (ɑ⋅foreground + (1-ɑ)⋅background):

```
pipeline_desc.colors[0].blend.enabled = true;
pipeline_desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
pipeline_desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
```

The depth-buffer (aka z-buffer) is off by default. We could leave it this way and make sure to draw our images back-to-front. But it's easier to use GPU hardware. Let's turn on the depth buffer.

```
pipeline_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
pipeline_desc.depth.write_enabled = true;
```

(*N.B.*: If you have images with non-trivial transparency—pixels whose alpha is not 0 or 1—you'll get wrong colors if you *don't* draw back to front. You will notice this if, for example, your images have feathered or anti-aliased borders. You can instead sort in your graphics manager's `Draw()` function using the `std::sort()` function available when you `#include <algorithm>`. You can sort on an arbitrary field using a lambda. For example, an `std::vector< Sprite > sprites;` could be sorted on the `.z` property via `std::sort( sprites.begin(), sprites.end(), []( const Sprite& lhs, const Sprite& rhs ) { return lhs.z > rhs.z; } );`. This example performs a reverse sort that puts larger z values corresponding to farther `Sprite`s first.)

Now let's tell our pipeline about our vertex data layout. For each vertex, the first two floating point numbers should be grouped into one 2D vector pair and the second two floating point numbers should be grouped into a second 2D vector. We don't need to name them (i.e. position and texcoords). Our shader will do that.

```
pipeline_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT2;
pipeline_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT2;
```

Let's define our shader. We'll need to make a shader description struct:

```
sg_shader_desc shader_desc{};
```

We're using `sokol_gfx`'s OpenGL 3.3 Core Profile backend, so we'll write our shaders in OpenGL's shading language (GLSL). (`sokol_gfx` does have a shader cross-compiler we could use to become graphics API agnostic. It operates as a stage in the build system. You can go there as an extension.)

Let's start with our vertex shader. This will run on each vertex of the triangle (its 2D position and texture coordinates). It will also have access to some global variables (uniforms) we define. The output will be interpolated and passed to the fragment shader, which runs for each pixel of the triangles. Here is a simple vertex shader to get us started:

```
shader_desc.vs.source = R"(
    #version 330
    uniform mat4 projection;
    uniform mat4 transform;
    layout(location=0) in vec2 position;
    layout(location=1) in vec2 texcoords0;
    out vec2 texcoords;
    void main() {
        gl_Position = projection*transform*vec4( position, 0.0, 1.0 );
        texcoords = texcoords0;
    })";
```

The `location=0` and `location=1` match what we put in our `pipeline_desc.layout.attrs[0]` and `pipeline_desc.layout.attrs[1]`. It passes the texture coordinates directly along to the fragment shader. The uniforms are:

* A per-sprite `transform` matrix that transforms the unit square to an appropriately sized and positioned rectangle in our world.
* A global `projection` matrix that transforms from world coordinates to OpenGL's normalized device coordinates that cover the window. OpenGL's normalized device coordinates run from [-1,1] in x and y and z. That's inconvenient and a non-uniform scale if the window itself isn't square. You can choose anything you like for your world coordinates, so long as you handle the aspect ratio properly. One possibility is what [microStudio](https://microstudio.dev/) does. A square with dimensions [-100,100] is centered inside the window. The short edge of the window will run from -100 to 100. The long edge will run from -N to N, where N ≥ 100.

We need to tell our pipeline about the uniforms. Let's make a struct. Since this `Uniforms` struct won't be seen outside the graphics manager, we can put it in an unnamed namespace:

```
namespace {
struct Uniforms {
    mat4 projection;
    mat4 transform;
};
}
```

That 4x4 matrix type comes from the [glm](https://github.com/g-truc/glm) library, which is like a C++ implementation of the vector math in GLSL. To use it, insert `add_requires("glm")` near the top of the `xmake.lua` and `add_packages("glm")` inside `target("illengine")`. Then we can `#include "glm/glm.hpp"` and use its namespace in our `.cpp` file (`using namespace glm`). You might want to make this a public package and `typedef glm::vec2 vec2;` in your `Types.h` (and possibly also `glm::vec3` and `glm::vec4`). Then your engine's users will have access to a full-featured 2D vector type for specifying positions and velocities (and possibly a 3D or 4D vector type for specifying colors).

Let's tell the shader about these uniforms:

```
shader_desc.vs.uniform_blocks[0].size = sizeof(Uniforms);
// The order of `.uniforms[0]` and `.uniforms[1]` must match the order in `Uniforms`
shader_desc.vs.uniform_blocks[0].uniforms[0].name = "projection";
shader_desc.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_MAT4;
shader_desc.vs.uniform_blocks[0].uniforms[1].name = "transform";
shader_desc.vs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_MAT4;
```

When it's time to draw each sprite, we will create an instance of `Uniforms`, fill out the `projection` and `transform` fields, and then upload it to the GPU. What should `projection` be so that a centered square in the image has dimensions [-100,100]^2? It's a pure scaling matrix. We want to scale the short edge of this square down by 1/100 so that it fits inside OpenGL's [-1,1] normalized device coordinates. The long edge is longer. How much longer? long/short longer. So we want to scale it by (1/(100*long.short)). We can write that in code as:

```
// Start with an identity matrix.
uniforms.projection = mat4{1};
// Scale x and y by 1/100.
uniforms.projection[0][0] = uniforms.projection[1][1] = 1./100.;
// Scale the long edge by an additional 1/(long/short) = short/long.
if( width < height ) {
    uniforms.projection[1][1] *= width;
    uniforms.projection[1][1] /= height;
} else {
    uniforms.projection[0][0] *= height;
    uniforms.projection[0][0] /= width;
}
```

What should `transform` be? It depends what you want to support with your engine. You could allow people to scale, translate, and rotate sprites. You could allow them to specify arbitrary anchor points inside your sprite. You could allow them to only scale and translate sprites and assume that sprites are anchored from their center. We'll start with that, since it's simple. We'll also assume sprites have a z value between 0 and 1 that determines their drawing order. Whenever we transform, we always want to scale first, then rotate, and then translate. The glm library has some convenient functions for creating such matrices in another header (`#include "glm/gtc/matrix_transform.hpp"`).

```
uniforms.transform = translate( mat4{1}, vec3( position, z ) ) * scale( mat4{1}, vec3( scale ) );
```

We have one other order of business. The image itself may not be square. Let's make sure to scale the quad down (so it always fits inside the square) so that the image draws with the appropriate aspect ratio:

```
if( image_width < image_height ) {
    uniforms.transform = uniforms.transform * scale( mat4{1}, vec3( real(image_width)/image_height, 1.0, 1.0 ) );
} else {
    uniforms.transform = uniforms.transform * scale( mat4{1}, vec3( 1.0, real(image_height)/image_width, 1.0 ) );
}
```

That's all for our vertex shader. The fragment shader is also very simple. It is given texture coordinates `texcoords` and expects a 2D image `tex` to have been uploaded to the GPU. It uses the texture coordinate to look up a color from the texture:

```
shader_desc.fs.source = R"(
    #version 330
    uniform sampler2D tex;
    in vec2 texcoords;
    out vec4 frag_color;
    void main() {
        frag_color = texture( tex, texcoords );
        // If we're not drawing back to front, discard very transparent pixels so we
        // don't write to the depth buffer and prevent farther sprites from drawing.
        if( frag_color.a < 0.1 ) discard;
    }
)";
shader_desc.fs.images[0].name = "tex"; // The name should match the shader source code.
shader_desc.fs.images[0].image_type = SG_IMAGETYPE_2D;
```

We're now ready to make our shader program, add it to the pipeline, and then make the entire pipeline:

```
pipeline_desc.shader = sg_make_shader( shader_desc );
sg_pipeline pipeline = sg_make_pipeline( pipeline_desc );
```

We now have a graphics pipeline capable of drawing sprites. Whenever we begin drawing a frame, we have to tell `sokol_gfx` what to do with what's already in the frame buffer. We do this with a `sg_pass_action` struct. Let's make one and choose a clear color:

```
sg_pass_action pass_action{};
pass_action.colors[0].action = SG_ACTION_CLEAR;
pass_action.colors[0].value = { /* red, green, blue, alpha floating point values for a color to fill the frame buffer with */ };
```

We want to attach ("bind") our `vertex_buffer` data to our pipeline for each draw call. We do this with an `sg_bindings` struct:

```
sg_bindings bindings{};
bindings.vertex_buffers[0] = vertex_buffer;
```

We also want to attach the sprite's image to the fragment shader. We will do that with

```
bindings.fs_images[0] = image;
```

What is `image`? We haven't loaded any images yet. Let's make a function to load images, just like we did for our sound manager. The load function should have a signature like:

```
bool LoadImage( const string& name, const string& path );
```

We'll use the wonderful `std_image` image loader. The documentation is [the header](https://github.com/nothings/stb/blob/master/stb_image.h). Add it to our `xmake.lua` with `add_requires("stb")` near the top and `add_packages("stb")` inside `target("illengine")`. `stb_image` is header only, but requires us to `#define STB_IMAGE_IMPLEMENTATION` in one compilation unit before `#include "stb_image.h"`. We'll only use it here, so:

```
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
```

Loading an image is easy with `stb_image`:

```
int width, height, channels;
unsigned char* data = stbi_load( path.c_str(), &width, &height, &channels, 4 );
```

The output parameters `width` and `height` store the image's dimensions. We'll need to keep them around for our aspect ratio calculations. We don't need `channels`, which stores the number of color channels (RGB or greyscale) stored in the file, since we asked `stb_image` to guarantee `4`-channel RGBA output. We'll create an `sokol_gfx` image description struct with this data:

```
sg_image_desc image_desc{};
image_desc.width = width;
image_desc.height = height;
image_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
image_desc.min_filter = SG_FILTER_LINEAR;
image_desc.mag_filter = SG_FILTER_LINEAR;
image_desc.wrap_u = SG_WRAP_CLAMP_TO_EDGE;
image_desc.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
image_desc.data.subimage[0][0].ptr = data;
image_desc.data.subimage[0][0].size = (size_t)(width * height * 4);
```

We'll upload the image to the GPU with:

```
sg_image image = sg_make_image( image_desc );
```

Once it's uploaded, we're done with the data returned by `stbi_load()`. Free that memory with:

```
stbi_image_free( data );
```

Just like with our sound manager, we can create an `std::unordered_map` that maps from the image's `name` to the data we want to store about the image, a little struct containing the `sg_image`, its width, and its height. You can add a call to destroy an image resource. Call `sg_destroy_image( image );` and then erase it from the `std::unordered_map`.

When it's time to draw:

1. Get the current frame buffer size with `glfwGetFramebufferSize()`.
2. Call `sg_begin_default_pass( pass_action, width, height );` to clear the default frame buffer.
3. Compute `uniforms.projection` from `width` and `height`.
4. Apply the pipeline with `sg_apply_pipeline( pipeline );`.
5. Draw each sprite.
    1. Update `uniforms.transform` for the sprite and call `sg_apply_uniforms( SG_SHADERSTAGE_VS, 0, SG_RANGE(uniforms) );`.
    2. Apply the sprite's bindings. Update the image in `bindings.fs_images[0]` and then call `sg_apply_bindings(bindings);` You can skip this step if the correct image is already bound (because the previous sprite also used it).
    3. Draw one (`1`) instance of our quad. That is, draw four (`4`) vertices starting at vertex `0` in our vertex buffer. We do this with `sg_draw(0, 4, 1);`
6. Finish drawing with `sg_end_pass();` followed by `sg_commit();` followed by `glfwSwapBuffers(window);`.

As you can see, we'll need to use the same `pass_action`, `pipeline`, and `bindings` each time we draw. Initialize them when the graphics manager starts up. Store them in instance variables.

Add a `Draw()` method to your to your graphics manager that takes a parameter something like `const std::vector< Sprite >& sprites` and draws them. What should a `Sprite` be? It could be a small struct containing an image name, position, scale, and z value. Since we don't yet have an entity manager, the engine itself can't call `Draw()`, since no one is tracking drawable entities. For now, you can call `Draw()` in your main loop's callback function. We'll revisit this later, once we have an entity manager.

Extensions:

* Sprites could have a color parameter that tints the image by multiplying `color * texture( tex, texcoords );`.
* If you really want to draw a lot of sprites efficiently, you must do it with one draw call that tells `sokol_gfx` to draw many instances of the sprite. We won't have a chance to change the uniform data between instances. Instead, we can submit the uniform information as additional vertex attributes that changes once per instance.
* If sprites use different images, then we have to change our pipeline's image binding. This defeats our attempts to use instanced rendering. The solution is to use a sprite sheet (also called a texture atlas) that packs multiple images into one image. You will need to store the names and locations of all sub-images in the sprite sheet so that you can pass appropriate texture coordinates as vertex attributes.

**You have reached the fifth checkpoint.** Upload your code. Run `xmake clean` and then zip your entire directory. For this checkpoint:

* Your engine should have a graphics manager that lets users draw images to the screen.
* Your `demo/helloworld.cpp` should load one or more images during startup and draw one or more sprites inside the main loop.

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
