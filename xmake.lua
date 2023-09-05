add_rules("mode.debug", "mode.release")

set_policy("build.warning", true) -- show warnings
set_warnings("all") -- warn about many things

add_requires("sol2", "lua")

target("helloworld")
    set_kind("binary")
    set_languages("cxx17")
    
    add_files("demo/helloworld.cpp")

target("templates")
    set_kind("binary")
    set_languages("cxx17")
    
    add_files("demo/templates.cpp")


target("callbacks")
    set_kind("binary")
    set_languages("cxx17")
    
    add_files("demo/callbacks.cpp")

target("callbacks_with_parameters")
    set_kind("binary")
    set_languages("cxx17")
    
    add_files("demo/callbacks_with_parameters.cpp")

target("paths")
    set_kind("binary")
    set_languages("cxx17")
    
    -- Set the working directory so the binary sees `data`
    set_rundir("$(projectdir)")
    
    add_files("demo/paths.cpp")

target("chrono")
    set_kind("binary")
    set_languages("cxx17")
    
    add_files("demo/chrono_sleep_for.cpp")

target("constructor_reference")
    set_kind("binary")
    set_languages("cxx17")
    
    add_files("demo/constructor_reference.cpp")

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

target("globals")
    set_kind("binary")
    set_languages("cxx17")
    
    add_files("globals/*.cpp")

target("entity_get")
    set_kind("binary")
    set_languages("cxx17")
    
    add_files("demo/entity_get.cpp")

target("lua_parameters")
    set_kind("binary")
    set_languages("cxx17")
    
    add_packages("sol2", "lua")
    add_files("demo/lua_parameters.cpp")

target("lua_globals")
    set_kind("binary")
    set_languages("cxx17")
    
    add_packages("sol2", "lua")
    add_files("demo/lua_globals.cpp")
