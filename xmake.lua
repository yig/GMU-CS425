add_rules("mode.debug", "mode.release")

set_policy("build.warning", true) -- show warnings
set_warnings("all") -- warn about many things

target("helloworld")
    set_kind("binary")
    set_languages("cxx17")
    
    add_files("demo/helloworld.cpp")

target("callbacks")
    set_kind("binary")
    set_languages("cxx17")
    
    add_files("demo/callbacks.cpp")

target("callbacks_with_parameters")
    set_kind("binary")
    set_languages("cxx17")
    
    add_files("demo/callbacks_with_parameters.cpp")

target("pimpl")
    set_kind("binary")
    set_languages("cxx17")
    
    add_files("pimpl/*.cpp")

target("disco")
    set_kind("static")
    set_languages("cxx17")
    
    add_includedirs("disco", {public = true})
    add_files("disco/*.cpp")

target("disco_demo")
    set_kind("binary")
    set_languages("cxx17")
    add_deps("disco")
    add_files("demo/disco_demo.cpp")
