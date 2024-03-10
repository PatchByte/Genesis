add_rules("mode.debug", "mode.release")

set_languages("c++23")
set_arch("x64")

target("genesis-bootstrap")
    set_kind("binary")
    set_pcxxheader("src/stdafx.h")

    add_files("src/**.cpp")
    add_headerfiles("src/**.h", "deps/shared/src/**.h")

target("genesis")
    set_kind("binary")
    set_pcxxheader("src/stdafx.h")

    add_files("src/**.cpp")
    add_headerfiles("src/**.h", "deps/shared/src/**.h")
    add_defines("GENESIS")
