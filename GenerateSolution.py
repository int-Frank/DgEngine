import subprocess
import sys
sys.path.insert(0, 'D:\\dev\\GlobalPythonModules')

import pyPremake

pyPremake.Make_vpaths("./Vendor/DgLib/DgLib/src/", "./DgLib_vpaths.lua")
pyPremake.Make_vpaths("./Engine/src/", "./Engine_vpaths.lua")

subprocess.call("Vendor\premake\premake5.exe vs2019")
wait = input("PRESS ENTER TO CONTINUE.")
