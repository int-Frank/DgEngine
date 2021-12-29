project "Demo"
  kind "ConsoleApp"
  targetdir (projOutput)
  objdir (projOutputInt)
  systemversion "latest"
  language "C++"
  cppdialect "C++17"
    
  files 
  {
    "Demo/src/**.h",
    "Demo/src/**.cpp",
  }
    
  includedirs
  {
    "%{wks.location}/Engine/src",
    "%{IncludeDir.spdlog}",
    "%{IncludeDir.DgLib}"
  }
    
  links
  {
    "Engine",
    "DgLib",
    "%{wks.location}/Vendor/freetype2/lib/x64/freetype.lib",
    "Glad",
    "%{wks.location}/Vendor/SDL2/lib/x64/SDL2.lib",
    "%{wks.location}/Vendor/SDL2/lib/x64/SDL2main.lib"
  }
    
  postbuildcommands 
  {
    "{COPY} %{wks.location}/Vendor/SDL2/lib/x64/SDL2.dll %{cfg.targetdir}",
    "{COPY} %{wks.location}/Vendor/FreeType2/lib/x64/freetype.dll %{cfg.targetdir}"
  }
    
  filter "configurations:Debug"
    defines "DGE_DEBUG"
    runtime "Debug"
    symbols "on"

  filter "configurations:Release"
    defines "DGE_RELEASE"
    runtime "Release"
    optimize "on"