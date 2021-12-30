import subprocess
import sys
sys.path.insert(0, '.\\Vendor\\BuildScripts')

import DgBuild

DgBuild.Make_vpaths("Vendor/DgLib/src", "Vendor/DgLib/DgLib_vpaths.lua")
DgBuild.Make_vpaths("src", "DgEngine_vpaths.lua")

subprocess.call("Vendor/DgLib/3rdParty/premake/premake5.exe vs2022")
