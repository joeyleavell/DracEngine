Type = "Runtime"

Modules = ["Core", "Math", "AssetCore"]

# Only add shader conductor in non distribute builds
# This is because shader conductor won't compile under MinGW atm
# TODO: Shader compilation should not be done in the abstract rendering module!!
if not Distribute:
	Extern = ["ShaderConductor"]
else:
	Extern = []