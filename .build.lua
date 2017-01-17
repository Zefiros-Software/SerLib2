project "SerLib2"
    kind "StaticLib"

    files "serialisation/src/**.cpp"   
    zpm.export [[
        includedirs "serialisation/include/"
        flags "C++11"
    ]]