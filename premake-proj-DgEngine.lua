project "DgEngine"
  location ""
  kind "StaticLib"
  targetdir ("%{wks.location}/build/%{prj.name}-%{cfg.buildcfg}")
  objdir ("%{wks.location}/build/intermediate/%{prj.name}-%{cfg.buildcfg}")
  systemversion "latest"
  language "C++"
  cppdialect "C++17"
  flags {"FatalWarnings"}
  
  files 
  {
    "src/**.h",
    "src/**.cpp",
  }

  include "DgEngine_vpaths.lua"
  
  includedirs
  {
		"Vendor/spdlog/include",
		"Vendor/DgLib/src",
		"Vendor/GLAD/include",
		"Vendor/SDL2/include",
		"Vendor/freetype2/include"
  }
  
  filter "configurations:Debug"
		defines "DGE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "DGE_RELEASE"
		runtime "Release"
		optimize "on"
    
  project "Glad"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    location "Vendor/GLAD"
    
    targetdir ("%{wks.location}/build/%{prj.name}-%{cfg.buildcfg}")
    objdir ("%{wks.location}/build/intermediate/%{prj.name}-%{cfg.buildcfg}")
    
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