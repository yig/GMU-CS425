-- Examples:
-- https://github.com/xmake-io/xmake-repo/tree/master/packages/n/newtondynamics
-- https://github.com/tboox/benchbox

package("soloud")
    set_description("SoLoud is an easy to use, free, portable c/c++ audio engine for games.")
    
    add_urls("https://sol.gfxile.net/soloud/soloud_20200207_lite.zip")
    add_versions("20200207", "a6fa8e5b7d26b03e21947307d45869e33f0d233639258bd5bf4bea73f88e709d")
    
    set_homepage("https://sol.gfxile.net/soloud/")
    set_license("zlib")
    
    on_install(function (package)
        local configs = {}
        configs.kind = package:config("shared") and "shared" or "static"
        configs.mode = package:debug() and "debug" or "release"
        
        io.writefile("xmake.lua", [[
            add_rules("mode.debug", "mode.release")
            
            target("soloud")
                set_kind("$(kind)")
                set_languages("cxx11")
                
                add_includedirs("include", {defines = "WITH_MINIAUDIO", public = true})
                
                -- skip `tools` and `backend`
                add_files("src/**.cpp|tools/**.cpp|backend/**.cpp", {defines = "WITH_MINIAUDIO"})
                add_files("src/**.c|tools/**.c|backend/**.c", {defines = "WITH_MINIAUDIO"})
                -- compile the miniaudio backend
                add_files("src/backend/miniaudio/*.c*", {defines = "WITH_MINIAUDIO"})
                
                before_install(function (package)
                    os.cp("include", package:installdir())
                end)
        ]])
        
        import("package.tools.xmake").install(package, configs)
        -- Copy headers
        -- os.cp("include", package:installdir())
    end)
    
    on_test(function (package)
        -- check includes and interfaces
        assert(package:has_cxxincludes("soloud.h"))
        assert(package:has_cxxtypes("SoLoud::Soloud", {includes = "soloud.h"}))
        assert(package:check_cxxsnippets({test = [[
            void test(int args, char** argv) {
                SoLoud::Soloud soloud;
            }
        ]]}, {includes = "soloud.h"}))
    end)
package_end()
