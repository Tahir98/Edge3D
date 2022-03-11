workspace "Edge3D" 
    configurations { "Debug", "Release" , "Dist"}
    platforms "Win64"

IncludeDir = {}
IncludeDir ["GLFW"] = "Dependencies/GLFW/include"
   
include "Dependencies/GLFW"

project "Edge3D"
    location "Edge3D"
    kind "StaticLib"
    language "C++"

    targetdir "bin/output/%{prj.name}/%{cfg.buildcfg}"
    objdir "bin/int/%{prj.name}/%{cfg.buildcfg}"

    includedirs {
        "Dependencies/GLFW/include",
        "Dependencies/GLAD/include",
        "Dependencies/spdlog/include",
        "Dependencies/STB",
        --"Dependencies/PhysX/include",
        "%{IncludeDir.GLFW}",
        "Edge3D/vendor/ImGui",
        "Edge3D/src"
    }

    links {
        "GLFW"
    }

    files { "%{prj.name}/**.h", "%{prj.name}/**.c", "%{prj.name}/**.hpp", "%{prj.name}/**.cpp" }

    filter "configurations:Debug"
        defines {"DEBUG"}
        symbols "On"

    filter "configurations:Release"
        defines {"RELEASE"}
        optimize "On"

    filter "configurations:Dist"
        defines {"DIST"}
        optimize "On"

    filter { "platforms:Win64" }
        system "Windows"
        cppdialect "C++17"
        systemversion "latest"
        architecture "x86_64"    
        
        
project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    
    targetdir "bin/output/%{prj.name}/%{cfg.buildcfg}"
    objdir "bin/int/%{prj.name}/%{cfg.buildcfg}"
    
    includedirs {
        "Dependencies/GLFW/include",
        "Dependencies/GLAD/include",
        "Dependencies/spdlog/include",
        "Dependencies/STB",
        --"Dependencies/PhysX/include",
        "Edge3D",
        "Edge3D/src",
        "%{IncludeDir.GLFW}",
        "Edge3D/vendor/ImGui"
    }
    
    links {
        "Edge3D"
    }
    
    files { "%{prj.name}/**.h", "%{prj.name}/**.c", "%{prj.name}/**.hpp", "%{prj.name}/**.cpp"}
    
    filter "configurations:Debug"
        defines {"DEBUG"}
        symbols "On"
    
    filter "configurations:Release"
        defines {"RELEASE"}
        optimize "On"
    
    filter "configurations:Dist"
        defines {"DIST"}
        optimize "On"
    
    filter { "platforms:Win64" }
        system "Windows"
        cppdialect "C++17"
        systemversion "latest"
        architecture "x86_64"
                    

project "Editor"
    location "Editor"
    kind "ConsoleApp"
    language "C++"

    targetdir "bin/output/%{prj.name}/%{cfg.buildcfg}"
    objdir "bin/int/%{prj.name}/%{cfg.buildcfg}"

    includedirs {
        "Dependencies/GLFW/include",
        "Dependencies/GLAD/include",
        "Dependencies/spdlog/include",
        "Dependencies/STB",
        --"Dependencies/PhysX/include",
        "Edge3D",
        "Edge3D/src",
        "%{IncludeDir.GLFW}",
        "Edge3D/vendor/ImGui"
    }

    links {
        "Edge3D"
    }

    files { "%{prj.name}/**.h", "%{prj.name}/**.c", "%{prj.name}/**.hpp", "%{prj.name}/**.cpp"}

    filter "configurations:Debug"
        defines {"DEBUG"}
        symbols "On"

    filter "configurations:Release"
        defines {"RELEASE"}
        optimize "On"

    filter "configurations:Dist"
        defines {"DIST"}
        optimize "On"

    filter { "platforms:Win64" }
        system "Windows"
        cppdialect "C++17"
        systemversion "latest"
        architecture "x86_64"
        
        postbuildcommands {
            --"{COPY} %{wks.location}/Dependencies/PhysX/PhysXDevice64.dll %{cfg.targetdir}",
            --"{COPY} %{wks.location}/Dependencies/PhysX/PhysXGpu_64.dll %{cfg.targetdir}",
            --"{COPY} %{wks.location}/Dependencies/PhysX/glut32.dll %{cfg.targetdir}"
          }
  

          