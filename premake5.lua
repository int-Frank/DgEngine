workspace "DgEngine"
  architecture "x64"
  
  configurations
  {
    "Debug",
    "Release"
  }
  
include("premake-proj-DgEngine.lua")
include("Vendor/DgLib/premake-proj-DgLib.lua")