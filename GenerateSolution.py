import subprocess
import sys
sys.path.insert(0, '.\\Vendor\\BuildScripts')

import DgBuild

DgBuild.Make_vpaths("./Vendor/DgLib/src/", "./DgLib_vpaths.lua")
DgBuild.Make_vpaths("./Engine/src/", "./Engine_vpaths.lua")

subprocess.call("Vendor\premake\premake5.exe vs2022")
