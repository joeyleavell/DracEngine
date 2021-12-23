# Declare module dependencies
Modules = ["Core", "Math", "AssetCore", "Rendering", "Profiling", "ARI"]

Extern = ["Stb"]

# Declare module type
Type = "Runtime"

if TargetArch == "x86_64":
	if TargetOS == "Windows":
		if BuildToolset == "MSVC":
			Libraries = ["Advapi32.lib"]