project "SerLib2"
    kind "StaticLib"

    zpm.export [[
        includedirs "bsp/include/"
        flags "C++11"
    ]]