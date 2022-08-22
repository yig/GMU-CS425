add_rules("mode.debug", "mode.release")

target("helloworld")
    set_kind("binary")
    set_languages("cxx17")
    
    add_files("demo/helloworld.cpp")
