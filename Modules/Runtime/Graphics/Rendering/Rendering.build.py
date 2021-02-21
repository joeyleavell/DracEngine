Type = "Runtime"

# Only add shader conductor in non distribute builds
# This is because shader conductor won't compile under MinGW atm
if not Distribute:
	Extern = ["ShaderConductor"]
else:
	Extern = []

Modules = ["Core", "Math", "AssetCore", "Physics", "Profiling"]