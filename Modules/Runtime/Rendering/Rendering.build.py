Type = "Runtime"

Modules = ["Core", "Math", "AssetCore", "Physics", "Profiling", "ARI"]

if TargetOS == "Linux":
	if BuildToolset == "GCC":
		Libraries = ["stdc++fs"]