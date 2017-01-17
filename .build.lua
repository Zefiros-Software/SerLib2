project "SerLib2"
    kind "StaticLib"

    zpm.export [[
        includedirs "serialisation/include/"
        flags "C++11"
    ]]