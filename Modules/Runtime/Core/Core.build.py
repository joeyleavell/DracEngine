# Declare module dependencies
Modules = []

# Declare module type
Type = "Runtime"

# Declare external dependencies
Extern = ["Glfw"]

if TargetOS == "Linux":
	if BuildToolset == "GCC":
		Libraries = ["stdc++fs"]