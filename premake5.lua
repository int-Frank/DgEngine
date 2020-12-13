workspace "BlakeStoneRemake"
  location ""
  architecture "x64"
  
  startproject "Game"
  
  configurations
  {
    "Debug",
    "Release",
    "Dist"
  }
  
projOutput = "%{wks.location}/build/%{prj.name}-%{cfg.buildcfg}"
projOutputInt = "%{wks.location}/build/intermediate/%{prj.name}-%{cfg.buildcfg}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["Glad"] = "%{wks.location}/Vendor/GLAD/include"
IncludeDir["SDL2"] = "%{wks.location}/Vendor/SDL2/include"
IncludeDir["spdlog"] = "%{wks.location}/Vendor/spdlog/include"
IncludeDir["DgLib"] = "%{wks.location}/Vendor/DgLib/src"
IncludeDir["FreeType2"] = "%{wks.location}/Vendor/freetype2/include"

group "Depenencies"

  project "Glad"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    location "Vendor/GLAD"
    
    targetdir (projOutput)
    objdir (projOutputInt)
    
    files
    {
      "Vendor/GLAD/include/glad/glad.h",
      "Vendor/GLAD/include/KHR/khrplatform.h",
      "Vendor/GLAD/src/glad.c"
    }

    includedirs
    {
      "Vendor/GLAD/include"
    }
    
    filter "system:windows"
      systemversion "latest"

    filter "configurations:Debug"
	  	runtime "Debug"
	  	symbols "on"

	  filter "configurations:Release"
	  	runtime "Release"
	  	optimize "on"

	  filter "configurations:Dist"
	  	runtime "Release"
	  	optimize "on"
    
  project "DgLib"
    location "Vendor/DgLib"
    kind "StaticLib"
    targetdir (projOutput)
    objdir (projOutputInt)
    systemversion "latest"
    language "C++"
    cppdialect "C++17"
  
    files 
    {
      "Vendor/DgLib/src/**.h",
      "Vendor/DgLib/src/**.cpp",
      "Vendor/DgLib/src/impl/**.h",
      "Vendor/DgLib/src/impl**.cpp"
    }
  
    include "./DgLib_vpaths.lua"
    
    filter "configurations:Debug"
		  runtime "Debug"
		  symbols "on"

	  filter "configurations:Release"
		  runtime "Release"
		  optimize "on"

	  filter "configurations:Dist"
		  runtime "Release"
		  optimize "on"
group ""

project "Engine"
  location "Engine"
  kind "StaticLib"
  targetdir (projOutput)
  objdir (projOutputInt)
  systemversion "latest"
  language "C++"
  cppdialect "C++17"
  
  files 
  {
    "Engine/src/**.h",
    "Engine/src/**.cpp",
  }

  links
  {
    "DgLib",
    "Glad",
    "Vendor/SDL2/lib/x64/SDL2.lib",
    "Vendor/SDL2/lib/x64/SDL2main.lib",
    "Vendor/freetype2/lib/x64/freetype.lib"
  }

  includedirs
  {
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.DgLib}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.SDL2}",
		"%{IncludeDir.FreeType2}"
  }
  
  include "./Engine_vpaths.lua"
  
  filter "configurations:Debug"
		defines "BSR_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "BSR_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "BSR_DIST"
		runtime "Release"
		optimize "on"

  project "Game"
    location "Game"
    kind "ConsoleApp"
    targetdir (projOutput)
    objdir (projOutputInt)
    systemversion "latest"
    language "C++"
    cppdialect "C++17"
    
    files 
    {
      "Game/src/**.h",
      "Game/src/**.cpp",
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
      "DgLib"
    }
    
    postbuildcommands {
      "{COPY} %{wks.location}/Vendor/SDL2/lib/x64/SDL2.dll %{cfg.targetdir}",
      "{COPY} %{wks.location}/Vendor/FreeType2/lib/x64/freetype.dll %{cfg.targetdir}"
    }
    
    filter "configurations:Debug"
      defines "BSR_DEBUG"
      runtime "Debug"
      symbols "on"

    filter "configurations:Release"
      defines "BSR_RELEASE"
      runtime "Release"
      optimize "on"

    filter "configurations:Dist"
      defines "BSR_DIST"
      runtime "Release"
      optimize "on"
  
 