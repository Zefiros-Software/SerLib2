project "SerLib2"
    kind "StaticLib"

    files "serialisation/src/**.cpp"   
    zpm.export [[
        includedirs "serialisation/include/"
        cppdialect "C++11"
    ]]